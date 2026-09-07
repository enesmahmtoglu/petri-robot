# Firmware Mimarisi

Hedef: XMC4200 üzerinde, sıfırdan yazılmış, 4 eksenli, ivme profilli bir hareket kontrolcüsü ve
görev durum makinesi.

---

## 1. Neden sıfırdan yazıyoruz

İki bağımsız sebep var ve ikisi de aynı sonuca çıkıyor.

**Sebep 1 — hazır bir şey yok.** Yaygın açık kaynak CNC/3D yazıcı firmware'lerinin hiçbirinin
Infineon portu yok:

| Firmware | XMC desteği | Desteklenen mimariler |
|---|---|---|
| **grblHAL** | ❌ yok | iMXRT1062, ESP32, RP2040, STM32F1/F3/F4/F7/H7/U5, SAM3X8E, SAMD21, LPC176x, MSP432, TM4C, PSoC5, HC32F460, GD32 |
| **FluidNC** | ❌ yok | ESP32, ESP32-S3, RP2040, RP2350 |
| **Marlin** | ❌ yok | AVR, DUE, ESP32, GD32, HC32, LPC1768, RP2040, SAMD, STM32, Teensy |
| **Klipper** | ❌ yok | AVR, ATSAM, ATSAMD, LPC176X, STM32, HC32F460, RPXXXX, PRU, Linux |

**DAVE 4.5'te de step motor APP'i yok.** 106 APP'in tamamı tarandı — `STEPPER`, `STEP_DIR` veya
hareket profili APP'i yok. "Motor Control" kategorisi tamamen üç fazlı BLDC/PMSM/ACIM. Infineon'un
XMC4000 için step motor uygulama notu da yok (sadece PMSM FOC, BLDC ve POSIF notları var).

> Not: Infineon'un tek step motor ürünü XMC1300 tabanlı IFX9201 shield'ı — farklı aile, ve orada
> XMC *akım kontrolcüsünün kendisi*, STEP/DIR sürücüsü değil.

**Sebep 2 — zaten öğrenmek istediğiniz bu.** Projenin belirtilen amacı "motorları sürmeyi
öğrenmek". Hazır firmware bunu tam olarak gizlerdi.

> **İleride bakılabilecek bir yol:** Zephyr RTOS'un hem bir Infineon XMC4xxx SoC portu
> (`soc/infineon/cat3/xmc4xxx`) hem de gerçek bir step motor alt sistemi
> (`drivers/stepper/step_dir`, `gpio_stepper`, `adi_tmc`) var. Ama `soc.yml` yalnızca
> xmc4500/4700/4800 tanımlıyor — **XMC4200 listede yok.** Çalışan bir XMC4xxx portuna XMC4200
> eklemek, grblHAL sürücüsü yazmaktan çok daha küçük bir iştir. Bu projenin kapsamı dışında ama
> not düşülmeye değer.

---

## 2. Çekirdek mimari kararı: tek ana zamanlayıcı + Bresenham

### Neden eksen başına donanım PWM değil

XMC4200'ün zamanlayıcı kaynakları cömert (CCU4 × 2 modül = 8 dilim, CCU8 × 1 modül = 4 dilim,
80 MHz'e kadar). İlk akla gelen "her eksene bir dilim ver, donanım STEP darbelerini üretsin"
oluyor. **Bu yaklaşım senkron hareket veremez.**

X ve Y birlikte eğik bir doğru çizecekse, ikisinin adımları **birbirine göre oranlanmalıdır** —
X 100 adım atarken Y 37 adım atmalı, ve bu 37 adım 100'ün içine düzgün dağılmalı. Bağımsız
periyotlu iki PWM bunu yapamaz; iki eksen birbirinden habersiz sürüklenir ve çizilen şekil doğru
olmaz.

Ayrıca pratik bir engel de var: bu kartta **D4 (Z-STEP) ve D12 (G-STEP) aynı CCU8 dilimini
paylaşıyor** (`CCU80.OUT21` ve `OUT22` bir dilimin iki çıkışı), yani aynı periyoda mahkumlar.

### Seçilen mimari

**Tek bir yüksek frekanslı ana zamanlayıcı kesmesi**, içinde **Bresenham/DDA** algoritması. Her
tikte hangi eksenlerin adım atacağına karar verilir ve o pinler birlikte tetiklenir.

GRBL, grblHAL, Marlin — hepsi bu yapıyı kullanır. Sebebi tam olarak yukarıdaki senkronizasyon
ihtiyacıdır.

```
        ┌──────────────────────────────────────────┐
        │  Ana zamanlayıcı kesmesi (CCU4, ~30 kHz) │
        │                                          │
        │  1. Aktif segmentten adım aralığını al   │
        │  2. Bresenham sayaçlarını güncelle       │
        │  3. Adım atacak eksenlerin STEP pinini ↑ │
        │  4. ~2 µs sonra hepsini ↓                │
        │  5. Segment bittiyse sıradakini al       │
        └──────────────────────────────────────────┘
                          ▲
                          │ segment kuyruğu
                          │
        ┌─────────────────┴────────────────────────┐
        │  Hareket planlayıcı (ana döngü)          │
        │  Hedef konum → trapez hız profili →      │
        │  sabit hızlı küçük segmentlere böl       │
        └──────────────────────────────────────────┘
                          ▲
        ┌─────────────────┴────────────────────────┐
        │  Görev durum makinesi (ana döngü)        │
        └──────────────────────────────────────────┘
```

**Periyodu uçuşta değiştirmek destekleniyor.** Gölge registerlara (`CC4yPRS`/`CC4yCRS`) yazıp
`GCSS.SySE` bitini kurarsanız, transfer bir sonraki periyot eşleşmesinde **glitch'siz**
gerçekleşir. İvme profili için gereken tam olarak budur — donanım desteği var, yazılımla
kıvırmaya gerek yok.

### Alternatif: adım aralığı başına yeniden programlama

İkinci bir geçerli yaklaşım: sabit frekanslı bir tik yerine, **her adımdan sonra zamanlayıcıyı bir
sonraki adım aralığına göre yeniden programlamak**. Kesme yükü daha düşük (yalnızca gerçek adım
başına kesme), ama çok eksenli senkronizasyon daha karmaşık. Marlin ve grbl bunun bir karışımını
kullanır.

**Bu proje için öneri: sabit frekanslı tik ile başlayın.** Anlaması ve hata ayıklaması çok daha
kolay. Kesme yükü sorun olursa sonra optimize edin.

---

## 3. Donanım kaynakları

| Kaynak | XMC4200 | Kullanım |
|---|---|---|
| Çekirdek | ARM Cortex-M4, **80 MHz**, FPU, DSP/MAC | Planlayıcı matematiği |
| Flash | **256 KB** (80 MHz'de 1 bekleme durumu, prefetch tamponu var) | Kod |
| SRAM | **40 KB** (16 KB PSRAM + 24 KB DSRAM1) | Segment kuyruğu, durum |
| CCU4 | 2 modül × 4 dilim = **8** dilim, 16 bit | Ana zamanlayıcı |
| CCU8 | 1 modül × 4 dilim, dilim başına 2 karşılaştırma | Yedek |
| NVIC | 112 düğüm, **64 öncelik seviyesi** | Kesme önceliklendirme |
| HRPWM | 4 kanal, 150 ps | Bu projede gereksiz |

**Flash bekleme durumu:** 80 MHz'de `WSPFLASH ≥ 2` olmalı (flash erişim süresi 20 ns, çevrim
12.5 ns). Prefetch birimi (1 KB, 2 yollu) çoğu isabeti tek çevrime indiriyor, ama kesme
işleyicisinin sıcak yolu için bu bir dikkat noktası. Gerekirse ISR'ı RAM'e taşıyın.

### Gerçekçi adım hızı

> ⚠️ **Bunlar mühendislik tahminidir, datasheet değeri değildir.** Infineon kesme gecikmesini
> çevrim cinsinden yayınlamıyor. Ölçün.

80 MHz Cortex-M4'te, flash'tan çalışan sıkı bir 4 eksenli Bresenham ISR'ının giriş/çıkış dahil
kabaca 150-400 çevrim tutması beklenir:

| | Tahmin |
|---|---|
| Teorik tavan | ~200-500 kHz |
| **Makul tasarım noktası (%30-50 CPU)** | **~40-80 kHz toplam adım olayı** |
| Karşılaştırma: 16 MHz AVR'de GRBL | ~30 kHz |

Yani en kötü senaryoda bile klasik bir Arduino CNC kontrolcüsünden hızlısınız. Adım hızı bu
projede darboğaz olmayacak — TMC2208'in kendi sınırı 23.4 kfullstep/s (≈7000 rpm) zaten çok
yüksek.

---

## 4. Modül yapısı

```
firmware/
├── ortak/                     Paylaşılan katman (F1'den itibaren büyür)
│   ├── board.h/.c             Pin tanımları, saat kurulumu, GPIO init
│   ├── stepper.h/.c           STEP/DIR sürme, EN kontrolü, mikroadım sabitleri
│   ├── timer.h/.c             CCU4 kurulumu, ana tik, gölge register yazımı
│   ├── planner.h/.c           Trapez profil, segment kuyruğu
│   ├── kinematics.h/.c        mm ↔ adım dönüşümü, eksen parametreleri
│   ├── limits.h/.c            Limit anahtarı okuma, debounce, homing
│   ├── io.h/.c                Vakum valfi, acil stop, sensörler
│   ├── fsm.h/.c               Görev durum makinesi
│   └── config.h               ⭐ TÜM ayarlanabilir parametreler burada
├── F1-tek-eksen-gpio/
├── F2-zamanlayici/
├── ...
└── F7-tam-cevrim/
```

**`config.h` kuralı:** `adım/mm`, hız, ivme, yön bitleri, pin atamaları — hepsi **tek bir dosyada**
sabit olarak. Kod içine dağılmış sihirli sayı olmayacak. Ölçümler geldiğinde tek dosya
güncellenecek.

### Eksen soyutlaması

```c
typedef struct {
    volatile uint32_t *step_port;   // STEP pininin portu
    uint8_t            step_pin;
    volatile uint32_t *dir_port;
    uint8_t            dir_pin;
    bool               invert_dir;  // TMC2208 ters polarite → hepsi true
    float              steps_per_mm;
    float              max_rate_mm_min;
    float              accel_mm_s2;
    float              max_travel_mm;
    uint8_t            limit_pin;
    bool               homing_dir_negative;
} axis_cfg_t;
```

Dört eksen aynı yapıyı kullanır. Tutucu ekseni (G) de bir eksendir — sadece koordineli harekete
katılmaz, tek başına hareket eder.

---

## 5. F1-F7 kademeli yol haritası

Her aşama **tek başına gösterilebilir bir demo** ve bir öncekinin üstüne inşa edilir.

### F1 — Tek eksen, GPIO ile, sabit hız

**Ne yazılır:** Ana döngüde `STEP` pinini yükselt, kısa bekle, indir, bekle. `DIR` sabit.
Zamanlayıcı yok, kesme yok.

**Ne öğrenilir:** STEP/DIR mantığı, mikroadım ne demek, TMC2208'in yön polaritesinin ters olduğu,
akım ayarının motor davranışına etkisi, "kaç adım = kaç mm".

**Kabul kriteri:** Motor düzgün dönüyor, ses tiz/tırmalayıcı değil, ısınma normal.
`1600 adım = 1 tam tur` doğrulandı.

**Tuzaklar:** DIR'i STEP'ten en az 20 ns önce kurun. Adım darbesi en az 100 ns yüksek kalmalı.
Yön ters dönüyorsa bu **normaldir**, `invert_dir` ile düzeltin.

---

### F2 — Zamanlayıcı kesmesiyle adım üretimi

**Ne yazılır:** CCU4 diliminden periyodik kesme. Kesme içinde STEP pinini tetikle. Hız artık
`while` döngüsünün hızıyla değil, periyot registerıyla belirleniyor.

**Ne öğrenilir:** CCU4 kurulumu, NVIC, kesme önceliği, periyot hesabı
(`periyot = fCCU / (prescaler × istenen_adım_hz)`), gölge register mantığı.

**Kabul kriteri:** Adım frekansı ölçülebilir ve komut edilen değerle uyuşuyor (osiloskop veya
"N adımda geçen süre" ile).

---

### F3 — Trapez ivme profili, tek eksen

**Ne yazılır:** Hedef mesafe → hızlanma / sabit hız / yavaşlama üç fazı. Her fazda adım aralığı
kademeli değişir. Gölge registerlar ile glitch'siz periyot güncellemesi.

**Ne öğrenilir:** Hareket planlamanın matematiği. Neden ani hız değişimi adım kaçırtır. Kısa
mesafede trapezin üçgene dönüşmesi (sabit hıza hiç ulaşamama).

**Kabul kriteri:** Motor yumuşak hızlanıp yavaşlıyor, başlangıç ve bitişte "zıplama" yok. İvme
değeri artırıldıkça adım kaçırma başlıyor — bu sınır bulundu ve yarısında kalındı.

**Bu, projenin en öğretici aşaması.** Aceleye getirmeyin.

---

### F4 — Homing ve limit anahtarları

**Ne yazılır:** Limit pinini oku, debounce et. Homing dizisi: hızlı arama → anahtara değince dur →
geri çekil → yavaş yaklaş → sıfır kabul et.

**Ne öğrenilir:** Referanslama, debounce (donanım RC + yazılım), güvenlik mantığı, "homing
yapılmadan hareket yok" kuralı.

**Kabul kriteri:** Homing 10 kez üst üste yapıldığında sıfır noktası **tekrarlanabilir** (aynı
yerde). Fark 1-2 adımdan fazlaysa debounce veya yaklaşma hızı sorunludur.

**Ek olarak:** StealthChop'un otomatik kalibrasyonu bu hareketle tamamlanıyor — homing'i her
açılışta yapma alışkanlığı buradan geliyor.

---

### F5 — Üç eksen senkron hareket

**Ne yazılır:** Bresenham/DDA. Tek ana tik, birden fazla eksenin adımlarını oranla. Segment
kuyruğu.

**Ne öğrenilir:** Koordineli hareketin nasıl çalıştığı, neden bağımsız PWM'lerin yetmediği,
kuyruk yönetimi ve kesme ile ana döngü arasında veri paylaşımı (`volatile`, kritik bölge).

**Kabul kriteri:** X ve Y ile çizilen bir kare gerçekten kare; köşeler dik, kenarlar eşit.
Çapraz hareket düz bir doğru.

---

### F6 — Tutucu ekseni ve vakum

**Ne yazılır:** G ekseni bağımsız hareket (koordineli değil). MOSFET üzerinden valf kontrolü.
Referans anahtarı ile tutucu sıfırlama.

**Ne öğrenilir:** IO kontrolü, indüktif yükü sürme, zamanlama (vakum kurulması için bekleme),
opsiyonel olarak vakum sensörü okuma.

**Kabul kriteri:** Tutucu kabı ezmeden kavrıyor. Vakum kapağı kaldırıyor ve bırakıyor.

---

### F7 — Tam çevrim

**Ne yazılır:** Görev durum makinesi (bkz. [görev akışı](09-gorev-akisi.md)), hata durumları,
optik inceleme için tetik/bekle arayüzü (şimdilik boş bırakılacak).

**Kabul kriteri:** Kabı al → kapağı aç → taşı → bırak → başa dön, elle müdahale olmadan.

---

## 6. Toolchain

**DAVE 4.5 XMC4200'ü tam destekliyor.** Cihaz destek paketinde `XMC4200_series/` var
(`XMC4200.h`, `system_XMC4200.c`, `startup_XMC4200.S`, `XMC4200x256.ld`, `XMC4200.svd`) ve kart
paketleri arasında **`XMC42_Platform2Go`** bulunuyor. Sizde zaten kurulu
(`C:\Infineon\DAVE-IDE-4.5.0`).

### Kullanılabilecek DAVE APP'leri

`PWM_CCU4`, `PWM_CCU8`, `CCU4_SLICE_CONFIG`, `GLOBAL_CCU4`, `TIMER`, `SYSTIMER`, `INTERRUPT`,
`PIN_INTERRUPT`, `DIGITAL_IO`, `DMA_CH`.

**Ama bu proje için bare-metal XMCLib öneriliyor.** Gerekçe: öğrenme hedefi. APP'ler register
erişimini gizler; sizin öğrenmek istediğiniz tam olarak o katman. Ayrıca sıcak ISR yolunda
üretilen APP kodu gereksiz çevrim harcar.

DAVE'i **sadece IDE ve hata ayıklayıcı** olarak kullanabilirsiniz: "Empty Main Project" (XMCLib,
APP'siz) açın.

### ARM GCC + Makefile alternatifi

- **XMCLib:** https://github.com/Infineon/mtb-xmclib-cat3 → `CMSIS/Infineon/COMPONENT_XMC4200/`
- **CMSIS paketi:** `Infineon::XMC4000_DFP@2.14.0` — bu sürüm özellikle XMC4200 Platform2Go kart
  desteği ekliyor
- **Linker script:** hazır gelen `XMC4200x256.ld` kullanın, elle yazmayın — flash'ın önbellekli
  (`0x0800_0000`) / önbelleksiz (`0x0C00_0000`) takma adlarını ve ECC'li SRAM'i yönetiyor

### Hata ayıklama

Kart üstündeki debugger **ikinci bir XMC4200 üzerinde çalışan J-Link OB**. SWD + SWO/SWV ve
UART-USB köprüsü (sanal COM port) veriyor.

| Yöntem | Not |
|---|---|
| **SWD + kesme noktası** | Standart. J-Link sürücüsü DAVE ile geliyor |
| **VCOM `printf`** | Hedefin P2.14 (TX) / P2.15 (RX) = Arduino D1/D0 pinleri |
| **SEGGER RTT** | ⭐ Gerçek zamanlı yol için **en iyisi.** SWD üzerinden RAM'deki halka tamponu okur — pin harcamaz, ISR'ı bloklamaz |

> 🔴 **ISR içinde `printf` kullanmayın.** 115200 baud'da tek karakter ~87 µs sürer; adım
> zamanlamanız mikrosaniye mertebesinde. Gerçek zamanlı yolda ya RTT kullanın ya da bir GPIO'yu
> osiloskopla izleyin.

**Boot modu** reset anında TMS/TCK'dan alınır; varsayılan (TMS yüksek, TCK düşük) flash'tan
başlatır. Değiştirmeye gerek yok.

---

## 7. Güvenlik gereksinimleri (firmware tarafı)

Bunlar öneri değil, **kabul kriteridir**:

1. **Homing yapılmadan hiçbir hareket komutu kabul edilmez.** Açık çevrim sistemde konum
   bilinmiyorsa hareket etmek tanım gereği güvensizdir.
2. **Yumuşak limitler**, strok değerleri girildikten sonra etkin olmalı. Hedef konum stroğun
   dışındaysa komut reddedilir (hareketin ortasında durmak değil).
3. **Limit anahtarı hareket sırasında tetiklenirse** derhal durdur, `HATA` durumuna geç, yeniden
   homing iste.
4. **Acil stop girişi izlenir.** Basılıyken komut kabul edilmez; bırakıldığında **otomatik devam
   edilmez**, yeniden homing istenir.
5. **Vakum tutarken hata oluşursa vakumu bırakma.** Kapak tutulmaya devam etmeli — fail-safe
   davranış budur.
6. **Watchdog** etkin olmalı ve zaman aşımında motorları devre dışı bırakmalı (EN yüksek).

---

## 8. Açık teknik sorular

| # | Soru | Nasıl kapanır |
|---|---|---|
| 1 | Gerçek ISR maliyeti ve maksimum adım hızı | F2'de GPIO'yu osiloskopla ölçün |
| 2 | ISR'ın RAM'e taşınması gerekli mi | F5'te CPU yükü sıkışırsa ölçüp karar verin |
| 3 | Z ekseni kayışlı ve dikey — enerjisiz düşüyor mu | F1'de test edin ([AS-06](11-acik-sorular.md)) |
| 4 | Tutucu kavrama kuvveti nasıl sınırlanır | F6'da: referans anahtarı + adım sayma mı, akım sınırıyla stall mı |
| 5 | StealthChop yüksek hızda adım kaçırıyor mu | F3'te hız sınırını ararken görülecek |

---

## Kaynaklar

- XMC4100/XMC4200 datasheet V1.5:
  https://www.infineon.com/dgdl/Infineon-XMC4100_XMC4200_DS-DS-v01_04-EN.pdf?fileId=5546d462696dbf120169817056f938ff
- XMC4000 referans kılavuzu (CCU4 §18, GPIO §22): Infineon dokümantasyon portalı
- XMCLib CAT3: https://github.com/Infineon/mtb-xmclib-cat3
- XMC4 API referansı: https://infineon.github.io/mtb-xmclib-cat3/xmc4_api_reference_manual/html/index.html
- CMSIS paketi: https://www.keil.arm.com/packs/xmc4000_dfp-infineon/versions/
- DAVE IDE: https://softwaretools.infineon.com/tools/com.ifx.tb.tool.daveide
- grblHAL sürücü listesi (XMC yok, teyit için): https://github.com/grblHAL/drivers
- Zephyr XMC4xxx portu (ileride bakılabilir): `soc/infineon/cat3/xmc4xxx`

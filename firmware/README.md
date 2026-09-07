# Firmware

XMC4200 Platform2Go üzerinde, sıfırdan yazılan 4 eksenli hareket kontrolcüsü.

Mimari ve gerekçeler: [`../docs/08-firmware-mimarisi.md`](../docs/08-firmware-mimarisi.md)

---

## Klasör yapısı

```
ortak/                     Paylaşılan katman, F1'den itibaren büyür
├── config.h               ⭐ TÜM parametreler burada
├── board.h/.c             (F2'de) saat kurulumu, GPIO init
├── stepper.h/.c           (F2'de) STEP/DIR sürme
├── timer.h/.c             (F2'de) CCU4 kurulumu
├── planner.h/.c           (F3'te) trapez profil, segment kuyruğu
├── limits.h/.c            (F4'te) limit okuma, homing
├── io.h/.c                (F6'da) valf, sensörler
└── fsm.h/.c               (F7'de) görev durum makinesi

F1-tek-eksen-gpio/         ✅ İskelet hazır
F2-zamanlayici/            Boş
F3-ivme-profili/           Boş
F4-homing/                 Boş
F5-senkron-hareket/        Boş
F6-tutucu-vakum/           Boş
F7-tam-cevrim/             Boş
```

Her aşama kendi `main.c`'sine sahiptir ve `ortak/` katmanını kullanır. Bir öncekini silmeyin —
sorun çıktığında geri dönüp çalışan bir noktaya inmek en hızlı hata ayıklama yöntemidir.

---

## Derleme ortamı

**Seçenek A — DAVE 4.5 (önerilen başlangıç).** Sizde zaten kurulu.

1. DAVE'i açın → `File > New > DAVE Project`
2. **"Empty Main Project"** seçin (DAVE CE Project *değil*) — bu, XMCLib'i verir ama APP
   üretmez. Amacımız register katmanını öğrenmek olduğu için doğru seçim budur.
3. Cihaz: **XMC4200-F64K256**
4. `main.c`'yi ilgili F klasöründen kopyalayın, `ortak/` dosyalarını projeye ekleyin
5. J-Link ile flash'layın

> 🔴 **DAVE'deki "BMI Get/Set" aracını ASLA kullanmayın.** Infineon'un kendi uyarısı: yanlış
> kullanımı kartı kalıcı olarak çalışmaz hale getirebilir.

**Seçenek B — ARM GCC + Makefile + J-Link.**

- XMCLib: https://github.com/Infineon/mtb-xmclib-cat3 → `CMSIS/Infineon/COMPONENT_XMC4200/`
- CMSIS paketi: `cpackget add Infineon::XMC4000_DFP@2.14.0`
- Linker script: hazır gelen `XMC4200x256.ld` — **elle yazmayın**, flash'ın önbellekli
  (`0x0800_0000`) / önbelleksiz (`0x0C00_0000`) takma adlarını ve ECC'li SRAM'i yönetiyor
- Derleyici: `arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfpu=fpv4-sp-d16 -mfloat-abi=hard`

---

## Hata ayıklama

| Yöntem | Ne zaman |
|---|---|
| SWD + kesme noktası | Genel amaçlı |
| VCOM `printf` (P2.14/P2.15 = D1/D0) | Yavaş yolda durum yazdırma |
| **SEGGER RTT** | ⭐ Gerçek zamanlı yol — pin harcamaz, ISR'ı bloklamaz |
| GPIO + osiloskop | Adım zamanlaması ölçmek |

> 🔴 **ISR içinde `printf` kullanmayın.** 115200 baud'da tek karakter ~87 µs; adım zamanlamanız
> mikrosaniye mertebesinde. Zamanlamayı bozar ve aradığınız hatayı gizler.

---

## Başlamadan önce

```
☐ docs/07-guvenlik.md okundu
☐ docs/10-test-plani.md T0 adımları tamamlandı
☐ Shield ile kart arasında fiziksel bağlantı yapıldı (erkek-erkek sorunu çözüldü)
☐ Shield'ın 5V pininde 3.3 V ölçüldü
☐ TMC2208 Vref'leri ayarlandı (T1) — motorlar SÖKÜLÜYKEN
☐ Motorlar bağlandı, motor gücü kapalıyken
☐ Acil stop bağlı ve test edildi
```

---

## `config.h` hakkında

**Tüm ayarlanabilir parametreler tek dosyada.** Koda dağılmış sihirli sayı yok.

Şu anda `steps_per_mm`, `max_rate` ve `max_travel` değerleri **0.0f**. Bu kasıtlıdır —
[ölçüm formu](../formlar/olcum-formu.md) doldurulmadan bu sayılar bilinmiyor ve **uydurma bir
sayı, eksik bir sayıdan daha tehlikelidir.** F3'ten itibaren firmware bu değerler sıfırken
çalışmayı reddetmelidir.

---

## Aşamalar

| Aşama | Ne yazılır | Ne öğrenilir | Test |
|---|---|---|---|
| **F1** | GPIO ile sabit hızda tek eksen | STEP/DIR, mikroadım, yön polaritesi | T2 |
| **F2** | CCU4 kesmesiyle adım üretimi | Zamanlayıcı, NVIC, periyot hesabı | T4.1 |
| **F3** | Trapez ivme profili | Hareket planlama matematiği | T4.2-T4.5 |
| **F4** | Homing + limit anahtarları | Referanslama, debounce, güvenlik | T5 |
| **F5** | Bresenham ile senkron hareket | Koordineli hareket, kuyruk yönetimi | T6 |
| **F6** | Tutucu ekseni + vakum | IO kontrolü, indüktif yük | T7 |
| **F7** | Görev durum makinesi | Sistem entegrasyonu | T8 |

**F3 projenin en öğretici aşamasıdır.** Aceleye getirmeyin.

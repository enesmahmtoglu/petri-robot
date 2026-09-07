# Elektrik ve Bağlantılar

XMC4200 Platform2Go ↔ Protoneer CNC Shield V3 arayüzü, pin haritası, seviye uyumu ve güç dağıtımı.

---

## 1. 🔴 Shield karta TAKILMIYOR — önce bunu okuyun

**XMC4200 Platform2Go'nun Arduino konnektörleri erkek pin başlığıdır, dişi soket değil.**

Infineon'un kendi malzeme listesinde (kullanım kılavuzu Tablo 11) X300-X304 konnektörleri **Samtec
TSW** serisi olarak geçiyor — bu seri, tanımı gereği *erkek kare pimli terminal şerididir* ve
dişi SSW/SSQ soketlerle eşleşir. Arduino shield'ının altında da erkek pinler vardır.
**Erkek-erkek eşleşmez.**

> Infineon ürün sayfasında "Arduino Uno uyumlu" yazması **pin dizilimini** kastediyor, mekanik
> takılabilirliği değil. Pin haritası gerçekten UNO R3 ile birebir uyumlu — yani bu **elektriksel
> değil, mekanik** bir problem.

### Üç çözüm

| Çözüm | Nasıl | Artı / Eksi |
|---|---|---|
| **A — Dişi-dişi jumper kablo** | 36 adet dişi-dişi dupont kablo, iki kart arasında | En hızlı, lehimsiz. Ama kablo karmaşası ve gevşeme riski. STEP hatlarını kısa tutun. |
| **B — Şerit kablo adaptörü** | İki uçta erkek başlıklı bir şerit kablo veya delikli plaka | Derli toplu, tekrarlanabilir. Biraz yapım işi. |
| **C — Başlıkları değiştirmek** ⭐ | X300-X304'ü sökup yerine **Arduino istifleme soketi** (dişi) lehimlemek | En temiz ve kalıcı; shield doğrudan takılır. Lehim sökme işi ister, kart hasarı riski var. |

**Öneri:** Kalıcı bir makine kuruyorsanız **C**, hızlı prototip için **A**. A'yı seçerseniz
en az **12 hattı** (4 STEP + 4 DIR + EN + 3 limit) mümkün olduğunca kısa ve toprak dönüşüyle
birlikte çekin — STEP sinyalleri onlarca kHz'de keskin kenarlı darbelerdir.

> Bu, projenin fiziksel olarak çözülmesi gereken ilk maddesidir. Kablo/soket siparişi vermeden
> firmware yazmaya başlamak sorun değil, ama test edemezsiniz.

---

## 2. 🟠 Kart varyantınızı belirleyin

**KIT_XMC_PLT2GO_XMC4200'ün dört montaj varyantı var**, hepsinin silkscreen'i aynı. Sadece takılı
parçalardan ayırt ediliyorlar ve **aralarındaki fark bu projede kritik**.

| Varyant | TXS0108E seviye çevirici (IC300/IC301) | 0 Ω köprü dizisi | JP300 jumper | CAN |
|---|---|---|---|---|
| **A — 3.3 V** | ✗ yok | ✓ var | ✓ | ✓ |
| **B — 5 V** | ✓ **var** | ✗ | ✓ | ✓ |
| **C — 3.3 V Lite** | ✗ yok | ✓ var | ✗ | ✗ |
| **D — 5 V Lite** | ✓ **var** | ✗ | ✗ | ✗ |

### Nasıl bakılır

Arduino dijital başlığının iki yanına bakın (silkscreen `IC300` / `IC301`, kılavuzda *"Voltage
Levelshifter"* diye işaretli):

- **İki adet TSSOP-20 çip, üzerinde `TXS0108E` yazıyor** → **5 V varyantı** (B veya D)
- **Onun yerine direnç dizileri var** → **3.3 V varyantı** (A veya C)

Bunu [ölçüm formuna](../formlar/olcum-formu.md) **M19** olarak ekleyin ve fotoğraflayın.

> ⚠️ **DOĞRULANMADI:** Infineon tek sipariş kodu altında hangi varyantı gönderiyor, ürün
> sayfasında yazmıyor ve kılavuz dördünü birden belgeliyor.

### 🔴 5 V varyantındaysanız EN hattı çalışmayacak

Bu somut bir arıza, teorik bir risk değil.

TXS0108E, çift yönlü otomatik yön algılamalı bir çeviricidir ve hatları **düşükken 40 kΩ, yüksekken
4 kΩ** iç dirençlerle tutar. TI'ın kendi uygulama notu, TXS hatlarına bağlanacak harici dirençlerin
**≥50 kΩ** olmasını istiyor.

**CNC Shield V3'ün EN hattında (D8) 10 kΩ'luk bir pull-up direnci var** (şemada `R1`, silkscreen'de
*"Disable Drivers"*). Bu, TXS'in tavsiye ettiği değerin **beşte biri**.

Sonuç: mikrodenetleyici EN'i düşüğe çektiğinde, TXS'in 40 kΩ'luk tutma direnci ile shield'ın
10 kΩ'luk pull-up'ı bir gerilim bölücü oluşturur:

```
V_düşük ≈ 3.3 V × 40kΩ / (40kΩ + 10kΩ) ≈ 2.64 V
```

TMC2208'in geçerli düşük seviye eşiği 3.3 V VIO'da **0.99 V**. 2.64 V bunun çok üstünde — yani
**EN hiçbir zaman etkinleşmez, sürücüler hiç açılmaz.** Ve bu, multimetreyle bakınca "sinyal var
ama motor dönmüyor" şeklinde görünür.

**Çözümler:**

1. **Shield'ın R1 direncini sökün** veya 100 kΩ ile değiştirin. Sürücülerin açılışta kapalı kalması
   için pull-up'a hâlâ ihtiyacınız var; 100 kΩ hem bunu sağlar hem TXS'i rahatsız etmez.
2. EN'i shield başlığından değil, kartın X1/X2 delikli genişleme alanından ayrı bir pinle sürün
   (TXS'i baypas eder).
3. 3.3 V varyantı temin edin — orada hatlar sadece 0 Ω köprü, sorun hiç yok.

> Aynı analiz diğer hatlar için geçerli değil: STEP/DIR/limit hatlarında shield tarafında pull-up
> yok. Sadece EN'de var.

---

## 3. Lojik seviyesi: 3.3 V

**Karar: tüm sistem 3.3 V lojikte çalışacak.**

### Gerekçe

- **XMC4200 5 V toleranslı DEĞİL.** Datasheet Tablo 14 (mutlak maksimum): giriş gerilimi
  `VDDP + 1.0 V` veya **4.3 V**, hangisi düşükse. Metin açık: *"giriş gerilimi 4.3 V'u
  aşmamalıdır."* 5 V uygulamak mutlak maksimum sınırı ihlal eder.
- **CNC Shield'ın +5V hattı, rayı 3.3 V yapmaya elverişli.** Arşiv şemasından doğrulandı: bu hat
  yalnızca dört sürücünün VIO pinini, mikroadım jumper başlıklarını, EN pull-up'ını, K6 limit
  seçim jumper'ını ve iki breakout pinini besliyor. **Shield üzerinde 5 V gerektiren aktif hiçbir
  bileşen yok.**
- **TMC2208'in VIO aralığı 3.00-5.25 V.** 3.3 V spec içinde.
- **Bonus:** EN pull-up'ı artık 5 V yerine 3.3 V'a çeker — MCU pini için tehlike olmaktan çıkar.

### Uygulama

| Adım | Ne yapılır |
|---|---|
| 1 | Platform2Go'nun **3V3** pinini shield'ın **5V** pinine bağlayın |
| 2 | **GND**'leri bağlayın (en az iki hat) |
| 3 | 5 V varyantındaysanız **JP300 jumper'ını 1-2 konumuna** alın (IOREF = 3.3 V) |
| 4 | Shield'ın 5V pinine **başka hiçbir yerden 5 V vermeyin** — kartı etiketleyin |

> ⚠️ **TMC2208'in VIO düşük gerilim reset eşiği en kötü durumda 3.0 V.** 3.3 V rayda sadece
> **0.3 V** payımız var. Jumper kablolarındaki gerilim düşümü bu payı yiyebilir. Kabloları kısa
> tutun ve shield'ın 5V/GND ucuna yakın bir **100 µF + 100 nF** dekuplaj koyun. Sürücüler rastgele
> resetleniyorsa ilk bakılacak yer burasıdır.

---

## 4. Pin haritası

Kaynak: XMC4200 Platform2Go kullanım kılavuzu Şekil 9 (s.15), kart silkscreen'i ve şema (s.20) ile
çapraz doğrulandı.

### Ana bağlantı tablosu

| Arduino | XMC4200 | CNC Shield V3 | Bu projede | CCU çıkışı (ALT3) |
|---|---|---|---|---|
| D0 | P2.15 | — | *(hata ayıklama VCOM RX)* | CCU80.OUT11 |
| D1 | P2.14 | — | *(hata ayıklama VCOM TX)* | CCU80.OUT21 |
| **D2** | **P1.0** | X-STEP | **X STEP** | CCU40 dilim 3 |
| **D3** | **P2.2** | Y-STEP | **Y STEP** | CCU41 dilim 3 |
| **D4** | **P2.9** | Z-STEP | **Z STEP** | CCU80 dilim 2 |
| **D5** | **P2.3** | X-DIR | **X DIR** | CCU41 dilim 2 |
| **D6** | **P2.4** | Y-DIR | **Y DIR** | CCU41 dilim 1 |
| **D7** | **P2.8** | Z-DIR | **Z DIR** | CCU80 dilim 3 |
| **D8** | **P2.6** | EN (tüm sürücüler) | **EN** (aktif düşük) | CCU80 dilim 1 |
| **D9** | **P0.11** | X-EndStop | **X limit** | CCU80 dilim 3 |
| **D10** | **P1.7** | Y-EndStop | **Y limit** | *(yok)* |
| **D11** | **P1.9** | Z-EndStop | **Z limit** | *(yok)* |
| **D12** | **P0.0** | SpinEnable → A-STEP | **G STEP** (tutucu) | CCU80 dilim 2 |
| **D13** | **P1.8** | SpinDir → A-DIR | **G DIR** (tutucu) | *(yok)* |
| SDA | P2.5 | — | *(yedek)* | CCU41 dilim 0 |
| **SCL** | **P3.0** | — | **Vakum valfi** (MOSFET gate) | *(yok)* |
| A0 | P14.0 | Abort | ⚠️ kart potuna bağlı, kullanma | — |
| **A1** | **P14.6** | Hold | **Tutucu referans anahtarı** | — |
| **A2** | **P14.7** | Resume | **Acil stop izleme** | — |
| **A3** | **P14.8** | Coolant | **Vakum sensörü (analog)** | — |

### Önemli notlar

**GRBL'in D11/D12 karmaşası bizde YOK.** GRBL 1.1'de Z-limit pini D12'ye taşınmıştı ve shield'ın
silkscreen'i ile çelişiyordu. Kendi firmware'imizi yazdığımız için bu sorun **tamamen ortadan
kalkıyor** — shield'ın silkscreen'i olduğu gibi doğru: `Z-` klemensi D11'e gider.

**4. eksen (tutucu) jumper'ları.** Shield'daki `P7` ve `P8` jumper bloklarını **4. sütuna** alın —
bu konum A-STEP'i D12'ye, A-DIR'i D13'e bağlar (bağımsız 4. eksen konumu). Diğer sütunlar A'yı
X/Y/Z'nin klonu yapar, bizim istediğimiz bu değil.

**A0 kullanılamaz.** Kart üzerindeki 10 kΩ potansiyometre (silkscreen `R7`) P14.0'a bağlı ve hattı
yüklüyor. Kullanmanız gerekirse **R7'yi sökün** (0 Ω köprü, sökülebilir).

**A1/A2/A3 pinleri (P14.x) sadece GİRİŞ.** Çıkış sürücüleri yok. Bu bizim için sorun değil — zaten
üçünü de giriş olarak kullanıyoruz. Ama şuna dikkat: **P14 pinlerinde dahili pull-up yoktur**
(analog giriş sınıfı). Tutucu referans anahtarı ve acil stop girişi için **harici 10 kΩ pull-up**
kullanın.

**D0/D1 hata ayıklama konsolu.** Kart üstündeki J-Link OB'nin sanal COM portu P2.14/P2.15'e bağlı.
`printf` çıktısı buradan gelir. Shield bu hatları sürmesin.

---

## 5. Zamanlayıcı kaynakları ve adım üretimi

XMC4200'de **CCU4 × 2 modül (8 dilim)** ve **CCU8 × 1 modül (4 dilim)** var. Zamanlayıcı saati
`fCCU` maksimum **80 MHz** (12.5 ns çözünürlük).

### Donanımla adım üretme sınırı

Dört STEP pinini bağımsız zamanlayıcı dilimlerine dağıtmaya çalışırsak:

| STEP pini | Dilim |
|---|---|
| D2 (X) | CCU40 dilim 3 ✔ |
| D3 (Y) | CCU41 dilim 3 ✔ |
| D4 (Z) | CCU80 dilim 2 ✔ |
| D12 (G) | **CCU80 dilim 2 ✘ — Z ile aynı dilim** |

`CCU80.OUT21` ve `CCU80.OUT22` aynı dilimin iki çıkışıdır, yani **aynı periyodu paylaşırlar**.
Dört eksenden üçü bağımsız donanım zamanlamalı olabilir, dördüncüsü çakışır.

### Ama bu bizim için sorun değil — ve nedeni önemli

**Eksen başına donanım PWM'i, senkron çok eksenli hareket vermez.** X ve Y birlikte bir doğru
çizecekse ikisinin adımları birbirine göre *oranlanmalıdır*; bağımsız periyotlu iki PWM bunu
yapamaz.

Bu yüzden CNC firmware'lerinin standart çözümü **tek bir ana zamanlayıcı kesmesi** ve içinde
**Bresenham/DDA** algoritmasıdır: tek bir yüksek frekanslı kesme, hangi eksenlerin bu tikte adım
atacağına karar verir ve o pinleri birlikte tetikler. GRBL, grblHAL, Marlin — hepsi böyle çalışır.

**Kararımız:** tek ana zamanlayıcı + Bresenham. Böylece D4/D12 dilim çakışması anlamsızlaşır,
ve senkron hareket doğal olarak elde edilir. Detaylar:
[firmware mimarisi](08-firmware-mimarisi.md).

**Periyodu uçuşta değiştirme desteklenir.** Referans kılavuz §18.2.5.1: gölge registerlara
(`CC4yPRS`/`CC4yCRS`) yazıp `GCSS.SySE` bitini kurarsanız transfer bir sonraki periyot eşleşmesinde
**glitch'siz** gerçekleşir. İvme profili için gereken tam olarak budur.

**D10, D11, D13'te hiç CCU çıkışı yok** — sadece GPIO. DIR ve limit için sorun değil.

---

## 6. Limit anahtarı bağlantısı

> ⚠️ **XMC4200 5 V toleranslı değil (mutlak maks. 4.3 V).** Limit hatlarında asla 5 V oluşmamalı.

### Reçete

1. **Shield'ın `K6` (ES_Select) jumper'ını GND'ye alın** veya tamamen çıkarıp anahtar ortak
   uçlarını bir GND pinine bağlayın. 5 V'a alınırsa anahtar kapandığında MCU pinine doğrudan
   5 V gider ve pin ölür.
2. Anahtarları **GND'ye** bağlayın. Boşta pin yüksek, tetiklenince düşük (aktif düşük).
3. **X/Y/Z limitleri (D9/D10/D11 → P0.11/P1.7/P1.9):** normal GPIO, **dahili pull-up** kullanın.
4. **Tutucu referans anahtarı (A1 → P14.6):** P14 pinlerinde dahili pull-up yok →
   **harici 10 kΩ pull-up** 3.3 V'a.
5. Her pine **1 kΩ seri direnç** koyun. Bir gün yanlışlıkla 5 V gelirse akımı sınırlar
   (datasheet ±5 mA aşırı yük payı verir). Bedava sigorta.
6. Her pine **10-100 nF kondansatör** GND'ye. 1 kΩ ile birlikte 10-100 µs RC → donanımsal
   debounce. Yazılımda da debounce yapın.

```
   3.3 V
     │
    [10k]  ← sadece P14 pinlerinde (dahili pull-up yok)
     │
     ├──[1k]──● MCU pini
     │        │
   Anahtar  [100nF]
     │        │
    GND      GND
```

> CNC Shield V3'te limit hatlarında **pull-up direnci ve filtre yoktur** — GRBL, AVR'nin dahili
> pull-up'ına güvenirdi. V3.51'de filtre eklenmiş ama o varyantın şeması hiç yayınlanmamış.

---

## 7. Güç dağıtımı

### Kartın kendi beslemesi

| | |
|---|---|
| Giriş | **Yalnızca micro-USB** (X100 = hedef, X101 = debugger). **Barrel jack yok, VIN pini yok.** |
| Regülatör | IFX1117ME V33, **1.0 A** 3.3 V LDO |
| "5V" hattı | USB VBUS'ın ters akım koruma diyodundan geçmiş hâli — **regülatör değil** |
| Dış devreye verilebilecek akım | Pratikte **~500 mA** (USB host sınırı). Daha fazlası için harici USB güç adaptörü |

> ⚠️ **Kart USB ile beslenirken 5V header pininden ayrı bir besleme UYGULAMAYIN.** Kılavuzun kendi
> uyarısı: *harici güç kaynağına ters akım için koruma yoktur.*

### Sistem güç dağıtımı

```
   220 V şebeke ──→ [LCM-40DA] ──→ LED aydınlatma paneli   (izole, ayrı, sonraki faz)

   24 V kaynak ──┬──→ CNC Shield EXT-V klemensi ──→ 4× TMC2208 motor gücü
                 │      ⚠️ acil stop bu hattın üzerinde, seri
                 │
                 └──→ [DC-DC 24→12 V] ──→ vakum pompası + solenoid valf
                                              (ayrı besleme tercih edilir)

   USB (PC veya adaptör) ──→ Platform2Go ──→ 3V3 ──→ Shield lojik rayı (5V pinine)
```

**Kritik kurallar:**

1. **Motor gücü asla Platform2Go üzerinden geçmez.** 24 V doğrudan shield'ın kendi vidalı
   klemensine gider. Kart bunu ne taşır ne görür.
2. **Ortak toprak zorunlu.** MCU GND'si ile motor besleme GND'si bağlı olmalı. Ama güç akımları
   ile lojik akımları aynı kablodan dönmemeli — **yıldız topraklama**.
3. **Valf bobinini MCU regülatöründen beslemeyin.** 261 mA'e kadar çekebilir, 3.3 V LDO'yu
   çökertir. Ayrı ray + MOSFET (bkz. [vakum dokümanı](06-vakum-sistemi.md)).
4. Shield giriş aralığı 12-36 V; TMC2208 için **24 V önerilir** (12 V çalışır ama yüksek hızda
   tork kaybı verir).

Ayrıntılı akım hesabı: [güç bütçesi formu](../formlar/guc-butcesi.md).

---

## 8. GPIO sürme kabiliyeti

XMC4200'de (XMC1100'ün aksine) **sürücü gücü seçilebilir** — `Pn_PDR0`/`Pn_PDR1` registerları.

| Mod | `VOL` ≤ 0.4 V | `VOH` ≥ VDDP−0.4 V | Kenar süresi |
|---|---|---|---|
| Zayıf | 500 µA | −400 µA | ≤150 ns @ 20 pF |
| Orta | 2 mA | −1.4 mA | ≤50 ns @ 50 pF |
| Güçlü, yavaş kenar | 2 mA | −1.4 mA | ≤28 ns @ 50 pF |
| Güçlü, yumuşak kenar | 2 mA | −1.4 mA | ≤16 ns @ 50 pF |

> **Önemli yanlış anlama:** "güçlü" mod **DC akım değil, kenar hızı** kazandırır. Garanti edilen
> DC sürme kapasitesi orta ve güçlü modda aynı (~2 mA).

**Akım sınırları:** tavsiye edilen **±5 mA/pin**, 20 mA/pin grubu, 100 mA/cihaz. Mutlak maksimum
±10 / ±25 / ±100 mA.

🔴 **Solenoid valfi asla doğrudan pinden sürmeyin.** MOSFET şart.

**STEP hatları için:** başlıktaki tüm pinler A1+ sınıfı (25 MHz / 50 pF). Bizim ihtiyacımızın çok
üstünde. Jumper kablo kullanıyorsanız **güçlü + yumuşak kenar** modu ve kaynak ucunda **33-100 Ω
seri direnç** ile çınlamayı bastırın.

---

## 9. Bağlantı kontrol listesi

```
☐ Kart varyantı belirlendi (TXS0108E var mı?) → M19
☐ 5 V varyantıysa: shield R1 (10k EN pull-up) söküldü veya 100k yapıldı
☐ 5 V varyantıysa: JP300 → 1-2 konumu (IOREF = 3.3 V)
☐ Fiziksel bağlantı yöntemi seçildi (jumper / adaptör / soket değişimi)
☐ 3V3 → shield 5V pini, GND → GND (en az 2 hat)
☐ Shield 5V pinine başka besleme YOK, kart etiketlendi
☐ Shield 5V/GND ucuna 100 µF + 100 nF dekuplaj
☐ MS jumper'ları TAKILMADI (1/8 mikroadım)
☐ P7/P8 jumper'ları 4. sütunda (bağımsız A ekseni)
☐ K6 (ES_Select) → GND
☐ Limit anahtarları GND'ye, seri 1 kΩ + 100 nF
☐ A1 (P14.6) için harici 10 kΩ pull-up
☐ Acil stop 24 V hattına seri
☐ Ortak toprak (yıldız), motor gücü karttan geçmiyor
```

---

## 10. Bilinmeyenler ve kılavuz hataları

**Bu projeye özel doğrulanmamışlar:**
- ⚠️ Hangi kart varyantına sahip olduğunuz — **M19 ile belirlenecek**
- ⚠️ `WSPFLASH` değerini DAVE/XMCLib `SystemInit()`'in ne yaptığı; 80 MHz'de ≥2 olmalı
- ⚠️ D10/P1.7'nin SPI chip-select alternatif fonksiyonu (kılavuz `U0C0.SELO0` diyor, datasheet
  `U1C1.SELO2` işaret ediyor). Bizim kullanımımızda önemsiz — o pini GPIO limit girişi yapıyoruz.

**Kart kılavuzundaki hatalar** (peşine düşmeyin):
- Kılavuz §1, kartın **Ethernet** özelliği olduğunu ve tarayıcıdan kontrol edilebileceğini
  söylüyor. **Bu yanlış** — XMC4200'de Ethernet MAC yok, malzeme listesinde PHY yok. XMC4400
  kılavuzundan kopyalanmış.
- Tablo 9 `CAN_RX`'i P14.0 gösteriyor; Tablo 4 ve Şekil 7 **P14.3** diyor. Doğrusu P14.3
  (P14.0 potansiyometre).

---

## Kaynaklar

- XMC4200 Platform2Go kullanım kılavuzu Rev 1.1 (şema Bölüm 3.1, s.19-24; BOM Bölüm 3.2):
  https://www.infineon.com/dgdl/Infineon-XMC4200_Platform2Go-UserManual-v01_00-EN.pdf?fileId=5546d4626f229553016f8fca76c12c96
- XMC4100/XMC4200 datasheet V1.5 (Tablo 13 port fonksiyonları, Tablo 14 mutlak maksimum):
  https://www.infineon.com/dgdl/Infineon-XMC4100_XMC4200_DS-DS-v01_04-EN.pdf?fileId=5546d462696dbf120169817056f938ff
- Ürün sayfası: https://www.infineon.com/evaluation-board/KIT-XMC-PLT2GO-XMC4200
- ModusToolbox BSP: https://github.com/Infineon/TARGET_KIT_XMC_PLT2GO_XMC4200
- TI TXS0108E datasheet: https://www.ti.com/lit/ds/symlink/txs0108e.pdf
- TI SCEA054 (TXS ile harici direnç kullanımı): https://www.ti.com/lit/pdf/scea054
- Samtec TSW serisi (erkek başlık teyidi): https://www.samtec.com/products/tsw
- CNC Shield V3.XX şeması (yerel kopya):
  [`../datasheets/cnc-shield/cnc_shield_v3xx_schematic.jpg`](../datasheets/cnc-shield/cnc_shield_v3xx_schematic.jpg)

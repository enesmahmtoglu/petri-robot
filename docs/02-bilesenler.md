# Bileşenler

Her bileşenin doğrulanmış künyesi ve kaynağı. Datasheet'lerin yerel kopyaları
[`datasheets/`](../datasheets/INDEX.md) klasöründe.

**İşaretleme:** ⚠️ = doğrulanamadı · 🔴 = tehlike veya ölçüm gerekli · `___` = henüz ölçülmedi

---

## 1. Protoneer Arduino CNC Shield V3

Arduino UNO form faktöründe, 4 adet Pololu-format sürücü yuvası taşıyan kart.

| Parametre | Değer |
|---|---|
| Motor besleme girişi | **12-36 V** (vidalı klemens, `Ext-V`) |
| Sürücü yuvası | 4 adet (X, Y, Z, A) |
| Lojik besleme | Arduino'dan gelir; kartın kendi regülatörü **yok** |
| Sürücü başına akım | Sürücü modülü belirler, kart değil |
| Endstop klemensleri | Eksen başına 2 konum (Top/+ ve Bottom/−), **ikisi de aynı pine bağlı** |
| EN pull-up | 10 kΩ, 5 V rayına — açılışta sürücüleri **devre dışı tutar** |

### +5V rayının beslediği her şey (şemadan doğrulanmış)

1. Dört sürücü soketinin **VDD (pin 2) = TMC2208'in VIO** pini
2. Mikroadım jumper başlıkları (üç pini de raya bağlı)
3. `R1` 10 kΩ **EN pull-up** direnci ("Disable Drivers" silkscreen'i)
4. `K6` endstop `ES_Select` jumper'ının 3. pini
5. `P11` breakout başlığının 6. pini
6. `RX/TX1` breakout başlığının 2. pini

**Kartta 5 V gerektiren aktif hiçbir bileşen yok** — regülatör yok, lojik entegre yok, op-amp yok,
seviye çevirici yok. Bu, [rayı 3.3 V'ta çalıştırma kararının](03-elektrik-baglanti.md) temelidir.

### Motor konnektörü pin sırası

Sürücü soketi pinleri `2B / 2A / 1A / 1B` → shield başlık konumları 1/2/3/4.
Yani **bobin A = pin 1+2, bobin B = pin 3+4**.

### 4. eksen jumper blokları

İki adet 4 konumlu başlık (`P7` = STEP, `P8` = DIR). Her bloktan **aynı sütunda** birer jumper
takılır:

| Sütun | A ekseni neyi klonlar |
|---|---|
| 1 | X |
| 2 | Y |
| 3 | Z |
| 4 | **Bağımsız** (SpinEnable / SpinDir pinlerinden sürülür) |

Bu projede tutucu ekseni bağımsız olduğu için **4. sütun** kullanılacak.

### Mikroadım jumper'ları

Her yuvanın MS1/MS2/MS3 pinleri birer 3 pinli jumper başlığına, karşı taraf ise **+5V rayına**
sabit bağlı. Arduino'ya **hiç bağlı değiller** — mikroadım tamamen donanımsaldır.

- Jumper takılı = pin raya çekilir (**HIGH**)
- Jumper yok = pin, sürücünün kendi iç pull-down'ına düşer (**LOW**)

> ⚠️ Kartın üzerindeki tablo **A4988 içindir, TMC2208'de geçerli değildir.**
> Bkz. [TMC2208 kalibrasyonu §2](04-tmc2208-kalibrasyon.md).

### 🔴 Uyarılar

- **Enerji varken motor takmayın/çıkarmayın.** Üreticinin kendi kılavuzu bu konuda kesin.
- **36 V sınırı sürücüye bağlıdır.** A4988 24 V üstünde patlar; TMC2208'in aralığı 5.5-36 V.
- **Soğutucu takarken pinleri kısa devre ettirmeyin** — ortaya yerleştirin.

**Kaynaklar:** [`datasheets/cnc-shield/`](../datasheets/cnc-shield/) · resmi site **ölü**, sadece
Wayback kopyaları var.

---

## 2. TMC2208 v1.2 Step Motor Sürücü × 4

Trinamic TMC2208 çipini taşıyan StepStick formatlı modül (BigTreeTech / FYSETC / Watterott tipi).

| Parametre | Değer |
|---|---|
| Bobin başına RMS akım (tasarım kılavuzu) | **1.2 A** |
| Bobin başına tepe akım | 2.0 A |
| Besleme gerilimi VS | **5.5 – 36 V** |
| I/O besleme VIO | **3.00 – 5.25 V** |
| VIO düşük gerilim reset eşiği | 🔴 **maks. 3.0 V** |
| Giriş eşikleri | VIH = 0.7 × VIO, VIL = 0.3 × VIO |
| Rsense (v1.2 kartlarda) | **0.11 Ω** |
| Ayarlanabilir akım tavanı | 1.64 A RMS |
| Maks. tam adım frekansı | 23.4 kfullstep/s |
| Mikroadım (standalone) | 1/2, 1/4, 1/8, 1/16 — hepsi 1/256 interpolasyonlu |
| Chopper modu (standalone) | **StealthChop2'ye kilitli** (SPREAD pini yok) |

**Akım formülü:** `Vref = 1.524 × I_RMS` — internetteki ×1.41 / ×1.77 kuralları bu kart için
**yanlıştır**.

### Bu sürücünün A4988'den farkları (özet)

| Konu | Fark |
|---|---|
| DIR polaritesi | **Ters** — motor ters yönde döner |
| Mikroadım tablosu | Tamamen farklı; jumper yokluğu tam adım değil **1/8** |
| Vref ölçümü | Motor besleme gerilimi **bağlıyken** ölçülür |
| Vref anlamı | **RMS akım** (A4988'de tepe/kesme akımı) |
| MS3 pini | **Yok** — o soket konumunda ne olduğu belirsiz, jumper takmayın |
| RESET / SLEEP pinleri | **Yok** |
| EN mantığı | Aynı (aktif düşük) ✅ |

Detaylı açıklama: [TMC2208 kalibrasyonu](04-tmc2208-kalibrasyon.md)

**Kaynak:** [`datasheets/tmc2208/`](../datasheets/tmc2208/)

---

## 3. NEMA 8 Bipolar Step Motor — SY20STH30-0604A

Tutucu (G) ekseninde. Üretici: Changzhou Songyang ("Soyo").

| Parametre | Değer |
|---|---|
| Adım açısı / tur başına adım | **1.8°** (±%5) / **200** |
| Faz sayısı | 2 (bipolar) |
| **Anma faz akımı** | **0.6 A** |
| Anma gerilimi | 3.9 V |
| Faz direnci (25 °C) | **6.5 Ω** ±%10 |
| Faz endüktansı | **1.7 mH** ±%20 |
| **Tutma torku** | **17.65 mN·m** (180 g·cm / 2.5 oz·in) |
| Detent torku | ⚠️ **DOĞRULANMADI** — tabloda alan var, değer basılmamış |
| Rotor ataleti | 2.0 g·cm² (2.0×10⁻⁷ kg·m²) |
| Kablo sayısı | 4 |
| **Kablo renkleri → bobinler** | **siyah + yeşil** \| **kırmızı + mavi** |
| Kablo | 300 ±10 mm, UL 1007 AWG28 |
| Mil | ∅4 mm, "D" düzlemli, 15 mm |
| Gövde | **20 × 20 × 30 mm** (NEMA 8) |
| Ağırlık | 60 g |
| Yalıtım sınıfı | B |
| Maks. sıcaklık artışı | 80 °C (anma akımı, 2 faz enerjili) |
| Ortam sıcaklığı | −20 … +50 °C |
| Maks. radyal / eksenel kuvvet | 28 N (flanştan 20 mm) / 10 N |

**Elektriksel zaman sabiti:** τ = L/R = **262 µs**.

> ⚠️ 17.65 mN·m küçük bir torktur. Hareket hâlinde bunun yarısından azını hesaba katın.
> Tork-kuvvet dönüşümü: [Mekanik parametreler §2](05-mekanik-parametreler.md).

**Kaynak:** [`datasheets/nema8/`](../datasheets/nema8/)

---

## 4. NEMA 17 İnce (Pancake) Step Motor × 3

X, Y ve Z eksenlerinde. Model bilinmiyor.

| Parametre | Değer |
|---|---|
| Gövde | 42 × 42 mm kare, ~23 mm derinlik (ince/pancake tip) |
| Adım açısı | 1.8° varsayılıyor — ⚠️ **teyit edilmedi** |
| Anma faz akımı | 🔴 `___` — [M4](../formlar/olcum-formu.md) ile etiketten okunacak |
| Faz direnci | `___` |
| Tutma torku | `___` (ince NEMA 17'lerde tipik 0.1-0.2 N·m) |

**Z ekseni GT2 kayış** ile tahrik ediliyor, X ve Y **vidalı mil**.

> ⚠️ Bu motorların akım değeri bilinmeden sürücü Vref'i ayarlanamaz. Etiket okunamıyorsa faz
> direncini ölçüp motorun ısınmasına bakarak kademeli artırın (0.5 A'den başlayın).

---

## 5. Zonhen ZHV-0518 Mikro Solenoid Valf

Vakum hattının aç/kapa elemanı.

| Parametre | Değer |
|---|---|
| Üretici | Shenzhen Zonhen Electric Appliances |
| 🔴 **Bobin gerilimi** | **Model numarasında YOK** — [M15](../formlar/olcum-formu.md) ile ölçülecek |
| Sürekli güç | **1.6 W** (05 gövde sınıfı) |
| Çalışma basıncı | 0-375 mmHg (**0.5 bar**) |
| Vakum servisi | Uygun |
| Delik çapı | 1.2-1.8 mm |
| Hortum ucu | ∅3 mm dış → 2-3 mm iç çaplı hortum |
| Ömür | 1.000.000 çevrim |
| Yalıtım sınıfı | B |
| Tepki süresi | ⚠️ **DOĞRULANMADI** — üretici yayınlamıyor |
| Çalışma sıcaklığı | ⚠️ **DOĞRULANMADI** |
| Normalde açık mı kapalı mı | 🔴 **BİLİNMİYOR** — [M17](../formlar/olcum-formu.md) |

> 🔴 **"ZHV-0518" üreticinin kataloğunda yok.** Ya silkscreen `ZHV-0519` okunmalı, ya da OEM'e özel
> bir varyant. Detay: [Vakum sistemi §6](06-vakum-sistemi.md).

**Kaynak:** [`datasheets/valf/`](../datasheets/valf/)

---

## 6. Mean Well LCM-40DA — LED Sürücü

Optik inceleme istasyonunun aydınlatması. **Sonraki faz**, şu anki kapsamda kullanılmıyor.

| Parametre | Değer |
|---|---|
| 🔴 Giriş | **180-295 VAC** — şebeke gerilimi |
| Çıkış tipi | **Sabit AKIM** (constant current) |
| Seçilebilir akımlar | 350 / 500 / 600 / **700 (fabrika)** / 900 / 1050 mA — DIP ile |
| Çıkış gerilim aralığı | 2-100 V (akım ayarına göre) |
| 🔴 Boşta çıkış gerilimi | **110 V'a kadar** |
| Anma gücü | 42 W |
| Verim | %91 |
| Güç katsayısı | 0.975 @ 230 VAC |
| Açılış ani akımı | **20 A** (260 µs) |
| 🔴 Yalıtım sınıfı | **Sınıf II — çift yalıtımlı, TOPRAKLAMA UCU YOK** |
| Koruma sınıfı | **IP20** — kapalı, kuru ortam |
| Boyut / ağırlık | 123.5 × 81.5 × 23 mm / 0.24 kg |
| Korumalar | Kısa devre (otomatik toparlar), aşırı gerilim ve aşırı sıcaklık (**AC güç kesip açmayı gerektirir**) |

### DIP anahtar tablosu (datasheet'ten birebir)

| Çıkış | SW1 | SW2 | SW3 | SW4 | SW5 | SW6 |
|---|---|---|---|---|---|---|
| 350 mA | – | – | – | – | – | – |
| 500 mA | ON | – | – | – | – | – |
| 600 mA | ON | ON | – | – | – | – |
| **700 mA (fabrika)** | ON | ON | ON | – | – | **ON** |
| 900 mA | ON | ON | ON | ON | – | **ON** |
| 1050 mA | ON | ON | ON | ON | ON | ON |

> SW6'nın düzensiz görünen deseni **yazım hatası değildir**, iki bağımsız kaynakta doğrulandı.
> Aynen basıldığı gibi ayarlayın.

### Klemens pinout

**TB1 (5 pin):** 1 = AC/L · 2 = AC/N · 3 = **PUSH** · 4 = DA+ · 5 = DA−
**TB5 (2 pin):** 1 = +V · 2 = −V (LED çıkışı)

> Model ayırt etme: **DA = 5 pinli TB1** (PUSH var), **DA2 = 4 pinli TB1** (PUSH yok).

### 🔴 İki kritik uyarı

1. **Panel etiketi okunmadan bağlamayın.** Sabit akım sürücüsü, sabit **gerilim** tipi bir paneli
   (12 V / 24 V) yakar. [M11](../formlar/olcum-formu.md).
2. **PUSH ucu şebeke fazı taşır.** Dimleme butonu PUSH ile AC/L arasına bağlanır — nötre
   bağlanırsa kısa devre olur. **Mikrodenetleyiciyi buraya bağlamayın.** DA+/DA− de şebeke
   referanslıdır, SELV değildir.

**Kaynak:** [`datasheets/led-surucu/`](../datasheets/led-surucu/) · Güvenlik:
[07-guvenlik.md §2](07-guvenlik.md)

---

## 7. MCH-305A — Laboratuvar Güç Kaynağı

| Parametre | Değer |
|---|---|
| Üretici | Shenzhen Meichuang Instrument |
| Çıkış | **0-30 V, 0-5 A** (150 W) |
| Topoloji | **Lineer** (anahtarlamalı değil) |
| Çıkış sayısı | **Tek**, sadece ayarlanabilir |
| CV/CC | Otomatik geçiş, mod göstergeli |
| Dalgalanma | ≤1 mV ⚠️ (RMS mi tepe-tepe mi belirtilmemiş) |
| Şebeke girişi | 220 V |
| Boyut / ağırlık | 130 × 165 × 330 mm / 6.4 kg |
| Sabit 5 V çıkışı | **Yok** (305**B** modelinde var: 5 V / 2 A) |

⚠️ **Kamuya açık PDF kılavuz bulunamadı.** Üreticinin sitesi HTTPS reddediyor; veriler üreticinin
mağaza listesinden.

### Akım limitini ayarlama

Çıkış kapalı/bağlantısızken: gerilim düğmesini hedefe getirin (24.0 V). Akım düğmesini **tamamen
kısın**. Çıkış uçlarını kalın bir kabloyla **kısa devre edin** (CC kaynakta bu güvenli ve
normaldir). Cihaz CC'ye girer, gerilim göstergesi sıfıra düşer. Akım düğmesini istediğiniz sınıra
kadar açın. Kısa devreyi kaldırın.

**Riskler ve öneriler:** [Güvenlik §5](07-guvenlik.md)

---

## 8. Infineon XMC4200 Platform2Go

Sistemin tek kontrolcüsü.

> **Araştırma sürüyor** — künye, pin haritası ve Arduino shield uyumluluğu
> [elektrik dokümanına](03-elektrik-baglanti.md) eklenecek.

---

## 9. Sonraki faz bileşenleri (henüz yok)

| Bileşen | Rol | Not |
|---|---|---|
| Raspberry Pi + kamera | Optik inceleme | Firmware'de arayüz hazır bırakılacak: "incele" çıkışı + "bitti" girişi |
| Vakum sensörü | Kavrama doğrulama | Önerilen: NXP MPXV6115V. [Vakum §7](06-vakum-sistemi.md) |
| Acil stop butonu | Güvenlik | 🔴 **Eklenecek.** [Güvenlik §3](07-guvenlik.md) |
| Limit anahtarları | Homing | 🔴 **Eklenecek**, eksen başına en az 1 adet |
| 24 V SMPS | Makine beslemesi | MCH-305A'yı tezgâh aleti olarak serbest bırakmak için |

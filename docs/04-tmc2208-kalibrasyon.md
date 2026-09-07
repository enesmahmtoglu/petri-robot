# TMC2208 Kalibrasyonu ve Bilinen Tuzaklar

TMC2208, A4988'in birebir yerine takılan bir parça **değildir**. İnternetteki CNC Shield
rehberlerinin çoğu A4988'e göre yazılmıştır ve bu sürücüde **yanlış sonuç verir**. Bu doküman
farkları tek tek sayar.

---

## 1. Akım ayarı — internetteki formül bu kart için yanlış

### Doğru formül

v1.2 kartlarda **Rsense = 0.11 Ω**. Datasheet'in kendi formülü uygulandığında:

```
I_RMS = 0.656 × Vref          →          Vref = 1.524 × I_RMS
```

Tam skala (Vref = 2.5 V) → 1.64 A RMS.

### Neden yaygın formül yanlış

İnternetteki rehberlerin çoğu `I_RMS = Vref × 1.77` veya `× 1.41` der. Bu değerler ya
**Rsense = 0.10 Ω** varsayar, ya da TMC220x'in +30 mΩ parazitik terimi yerine **TMC2130'un
+20 mΩ** terimini kullanır. İkisi de 0.11 Ω'luk bir TMC2208 v1.2 için geçerli değildir.

Pratik etkisi: `×1.41` kuralıyla 0.6 A hedefleyip 0.85 V ayarlarsanız gerçekte **0.56 A** akar —
%7-8 eksik tork. Tehlikeli değil ama motorunuz zayıf kalır ve sebebini aramakla vakit
kaybedersiniz.

### Ayar değerleri

**NEMA 8 (tutucu ekseni, SY20STH30-0604A, 0.6 A/faz):**

| Hedef I_RMS | Ayarlanacak Vref | Tam skalanın yüzdesi |
|---|---|---|
| 0.6 A (tavan) | **0.915 V** | %37 |
| **0.5 A (önerilen)** | **0.762 V** | %30 |
| 0.4 A (soğuk/sessiz) | 0.610 V | %24 |

0.6 A'i tavan olarak görün, hedef olarak değil — küçük gövdeli motor, 80 °C sıcaklık artışı sınırı
ve hava akımı yok.

**NEMA 17'ler (X, Y, Z):** anma akımı henüz bilinmiyor, [M4 ölçümü](../formlar/olcum-formu.md) ile
etiketten okunacak. Formül aynı: `Vref = 1.524 × I_RMS`. Genel kural: `I_RMS ≈ anma akımı × 0.7`
ile başlayın, motor ısınmıyorsa yükseltin.

| Eksen | Anma akımı (M4) | Hedef I_RMS | **Hesaplanan Vref** | Ayarlandı ✓ |
|---|---|---|---|---|
| X | ___ | ___ | ___ | ☐ |
| Y | ___ | ___ | ___ | ☐ |
| Z | ___ | ___ | ___ | ☐ |
| G | 0.6 A | 0.5 A | **0.762 V** | ☐ |

### Ölçüm prosedürü

> ⚠️ **Motor besleme gerilimi (12-24 V) BAĞLI olmalı, motor kablosu SÖKÜLÜ olmalı.**
> Detaylı gerekçe: [güvenlik dokümanı §4](07-guvenlik.md).

1. Sürücüler shield'a takılı, motorlar sökülü.
2. Motor gücünü aç (24 V).
3. Multimetre DC gerilim kademesinde: **siyah prob GND**, **kırmızı prob potansiyometrenin metal
   gövdesi** (veya kartta işaretli Vref test noktası).
4. Potu çevirerek hedef değere getirin. BigTreeTech kartlarda **saat yönünün tersi Vref'i
   artırır** ve pot aralığı yaklaşık 0-2 V'tur.
5. Motor gücünü kapat, motorları bağla.

**Not:** Datasheet, 0.5 V'un altındaki Vref değerlerinde chopper hassasiyetinin düştüğünü söylüyor.
Bizim değerlerimiz (0.6-0.9 V) bu sınırın üstünde ama kartın aralığının sadece ~üçte birini
kullanıyoruz — bu, hassasiyetin ideal olmadığı bir bölge. UART moduna geçilirse akım doğrudan
sayısal olarak ayarlanabilir (bkz. §4).

---

## 2. Mikroadım — shield'ın tablosu bu sürücüde geçersiz

CNC Shield'ın üzerindeki jumper tablosu A4988 içindir. **TMC2208 sadece MS1 ve MS2'yi okur** ve
tablosu tamamen farklıdır:

| MS2 | MS1 | Çözünürlük |
|---|---|---|
| GND | GND | **1/8** |
| GND | VIO | 1/2 |
| VIO | GND | 1/4 |
| VIO | VIO | **1/16** |

Shield'da jumper takmak ilgili pini besleme rayına (VIO) çeker, jumper yokluğu ise pini sürücünün
kendi iç pull-down'ına bırakır (GND).

| Jumper durumu | A4988'de olurdu | **TMC2208'de gerçekte** |
|---|---|---|
| **Hiç jumper yok** | Tam adım | **1/8** |
| Üç jumper da takılı | 1/16 | **1/16** |

> 🔴 **En sık yapılan hata:** "jumper takmazsam tam adım olur" varsayımı. Gerçekte 1/8 olur, yani
> `adım/mm` değeriniz **8 kat** yanlış hesaplanır ve robot her mesafeyi 8'e bölerek gider.

Shield'ın 8 kombinasyonundan yalnızca 4'üne (1/2, 1/4, 1/8, 1/16) erişilebilir. Tam adım ve 1/32
mümkün değildir.

### Bu projede: **1/8, yani hiçbir MS jumper'ı takılmayacak**

Gerekçe: TMC2208 içeride **MicroPlyer** ile her giriş adımını 1/256'ya böler. Yani 1/8'de sürseniz
bile motor 1/256 pürüzsüzlüğünde döner. 1/16'ya çıkmak pürüzsüzlük kazandırmaz, sadece adım
frekansını iki katına çıkarıp hız tavanınızı yarıya indirir.

MicroPlyer konum çözünürlüğünü artırmaz — komut çözünürlüğünüz 1/8'de kalır ve sabit hızda bir
giriş adımı kadar gecikme ekler.

> ⚠️ **MS3 jumper'ını takmayın.** TMC2208'de MS3 pini yoktur; o soket konumunda klon kartlarda
> hangi sinyalin olduğu belirsizdir. Eğer oraya `CLK` denk gelirse ve siz onu 5 V'a çekerseniz
> sürücüyü bozarsınız. **DOĞRULANMADI:** kendi kartlarınızda o konumun ne olduğunu silkscreen'den
> kontrol edin.

---

## 3. Yön ters dönüyor — bu normal

**TMC2208'in DIR pini A4988'e göre terstir.** Üreticinin (Watterott) kendi ifadesi: dönüş yönü
TMC2xxx SilentStepStick'lerde terstir ve ya yazılımdan ya da motor konnektörünü 180° çevirerek
düzeltilir.

**Bu projede çözüm: firmware'de yön bitini çevirin.** Kabloyu sökmeyin, konnektörü ters takmayın —
her ikisi de sonradan kafa karıştırır. Firmware'de her eksen için bir `invert_dir` bayrağı olacak.

---

## 4. En ciddi risk: StealthChop kilidi

**Standalone modda TMC2208 StealthChop2'ye kilitlidir.** SpreadCycle'a geçmek için kullanılan
`SPREAD` pini **TMC2208'de yoktur** — o TMC2209'da var. Datasheet'teki SPREAD tablosu açıkça
"sadece TMC222x" der.

**Neden önemli:** StealthChop gerilim modlu bir chopper'dır. Hız arttıkça motorun ters EMK'sı
akımı düşürür ve tork, SpreadCycle'a göre çok daha erken çöker. Trinamic'in kendi tavsiyesi belirli
bir hızın üstünde SpreadCycle'a geçmektir — ki standalone TMC2208'de bunu **yapamazsınız**.

**Ve bu, sizin özel donanımınız için somut bir risk:** NEMA 8'iniz 6.5 Ω yüksek direnç, 1.7 mH
düşük endüktans ve 0.6 A düşük akım ile StealthChop'un regülasyon penceresinin **tam dibinde**
oturuyor. Datasheet bu bölgede sürücünün Vref değişimlerine tepki yeteneğinin sınırlandığını
söylüyor.

### Çıkış yolları

| Seçenek | Artı | Eksi |
|---|---|---|
| **Hızı düşük tutup StealthChop ile yaşamak** | Ek iş yok | Hız tavanı düşük |
| **PDN_UART köprüsünü lehimleyip UART moduna geçmek** | Akım ve chopper modu yazılımdan ayarlanır; en doğru çözüm | 4 sürücüde 4 lehim işi |
| **TMC2209 ile değiştirmek** | SPREAD pini standalone'da kullanılabilir | Parça değişimi |

**Bu proje için öneri:** StealthChop ile başlayın. Petri kabı taşımak hızlı bir iş değil; adım
kaçırma görülürse önce hızı ve ivmeyi düşürün, çözülmezse UART'a geçin.

### Zorunlu alışkanlık: her açılışta homing

StealthChop2 ilk hareketinde motoru **otomatik olarak öğrenir** ("automatic tuning"). Datasheet:
bir homing dizisi bu öğrenme için yeterlidir. Kritik uyarı: **Vref'i veya besleme gerilimini
değiştirmek öğrenme sonucunu geçersiz kılar** — kartı yeniden başlatmanız gerekir.

---

## 5. UART modu (isteğe bağlı, ileride)

- `PDN_UART` pini standalone modda otomatik akım düşürme (standstill reduction) sağlar; UART
  modunda tek telli çift yönlü arayüz olur.
- Çoğu modülde bu pin varsayılan olarak header'a **çıkmaz**, bir lehim köprüsü kapatmak gerekir.
- ⚠️ **FYSETC notu:** v1.0 kartlar standalone gelir, **v1.2 kartlar UART için yapılandırılmış
  gelebilir.** Yani "v1.2" yazması hangi modda olduğunu söylemez — **köprüyü gözle kontrol edin**
  ([M5 ölçümü](../formlar/olcum-formu.md)).
- UART açarsanız: akımı (IRUN/IHOLD) doğrudan sayısal olarak, mikroadımı 1/1-1/256 arası,
  StealthChop↔SpreadCycle geçişini ve teşhis bilgilerini elde edersiniz.

---

## 6. Elektriksel sınırlar

| Parametre | Değer |
|---|---|
| Bobin başına RMS akım (tasarım kılavuzu) | **1.2 A** |
| Bobin başına tepe akım | 2.0 A |
| Besleme gerilimi VS | **5.5 – 36 V** |
| I/O besleme VIO | **3.00 – 5.25 V** |
| VIO düşük gerilim reset eşiği | **maks. 3.0 V** ⚠️ |
| Giriş yüksek seviye eşiği | 0.7 × VIO |
| Giriş düşük seviye eşiği | 0.3 × VIO |
| Kartın (0.11 Ω) ayarlanabilir tavanı | 1.64 A RMS |

> ⚠️ **VIO = 3.3 V ile çalışacağız** (bkz. [elektrik dokümanı](03-elektrik-baglanti.md)).
> Reset eşiği en kötü durumda 3.0 V olduğu için **sadece 0.3 V payımız var**. Kabloları kısa
> tutun, shield'a yakın dekuplaj koyun. Sürücüler rastgele resetleniyorsa sebebi budur.

**Soğutma:** Datasheet 1 A RMS ve üzeri için PCB ısıl tasarımına dikkat çekiyor. Bizim akımlarımız
(0.5-1 A) bu sınırın altında, muhtemelen soğutucu gerekmeyecek. Yine de sürücüye elle
dokunulamayacak kadar ısınıyorsa akımı düşürün. Soğutucu takarsanız **pinleri kısa devre
ettirmeyin** — ortaya yerleştirin.

---

## 7. Adım/yön sinyali zamanlaması

| Parametre | Değer |
|---|---|
| Maks. tam adım frekansı | **23.4 kfullstep/s** (≈7000 rpm) |
| STEP min. düşük/yüksek süre | tipik **100 ns** |
| DIR → STEP kurulum süresi | 20 ns |
| STEP sonrası DIR tutma süresi | 20 ns |
| STEP/DIR ani darbe filtresi | tipik 20 ns |

Sadece **yükselen kenar** aktiftir. Bu değerler pratikte kısıt oluşturmaz — darboğaz her zaman
mikrodenetleyici tarafındadır.

> Datasheet dipnotu önemli: bu değerler **tam lojik seviye salınımı** ile geçerlidir. Asimetrik
> seviyeler (ör. 5 V VIO'lu bir sürücüye 3.3 V sürmek) iç RC filtresi yüzünden filtreleme
> gecikmesini artırır. Bu, rayı 3.3 V'ta çalıştırma kararımızın bir başka gerekçesi.

---

## 8. Kalibrasyon kontrol listesi

```
☐ Sürücü yönleri doğru (EN pini hizalı, potu referans alma)
☐ MS jumper'ları TAKILMADI  → 1/8 mikroadım
☐ MS3 konumu kontrol edildi, jumper takılmadı
☐ Motorlar sökülü, motor gücü açık
☐ X  Vref ayarlandı: ____ V
☐ Y  Vref ayarlandı: ____ V
☐ Z  Vref ayarlandı: ____ V
☐ G  Vref ayarlandı: 0.762 V
☐ Motor gücü kapatıldı, motorlar bağlandı
☐ Firmware'de yön bitleri çevrildi (TMC2208 ters polarite)
☐ İlk homing yapıldı (StealthChop otomatik kalibrasyonu için)
```

---

## Kaynaklar

- TMC2202/2208/2224 datasheet Rev 1.14 (Analog Devices/Trinamic):
  https://www.analog.com/media/en/technical-documentation/data-sheets/TMC2202_TMC2208_TMC2224_datasheet_rev1.14.pdf
- Watterott SilentStepStick SSS (yön tersliği, soğutma, hot-plug):
  https://learn.watterott.com/silentstepstick/faq/
- Watterott TMC2208 pin yapılandırması:
  https://learn.watterott.com/silentstepstick/pinconfig/tmc2208/
- BigTreeTech TMC2208 dokümanı: https://global.bttwiki.com/TMC2208.html
- FYSETC TMC2208 wiki (v1.0 vs v1.2 mod farkı): https://wiki.fysetc.com/docs/TMC2208
- Yerel kopya: [`../datasheets/tmc2208/`](../datasheets/tmc2208/)

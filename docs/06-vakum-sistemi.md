# Vakum Sistemi

Petri kabının **kapağını** iki vantuzlu bir vakum kafası kaldırıyor. Kabın kendisini ayrı bir
motorlu çatal tutucu kavrıyor.

> **Durum:** Düzenek fiziksel olarak **mevcut ama belgelenmemiş.** Bu doküman kısmen tersine
> mühendislik görevi tanımlar, kısmen de tasarım referansı verir. Ölçümler
> ([M15-M18](../formlar/olcum-formu.md)) yapılmadan pnömatik şema kesinleşemez.

---

## 1. Bilinenler

- Kafada **iki adet körüklü vantuz** var, iki ayrı mavi hortumla besleniyor (hızlı geçme rakorlar).
- Hatta bir **Zonhen ZHV-0518** mikro solenoid valf var (kırmızı silikon hortum bir ucunda, metal
  rakor diğer ucunda, iki telli siyah kablo).
- Kullanıcı beyanına göre sistemde bir **SPDT röle** var ve enerji kesildiğinde vakumun
  tutulmaya devam ettiği söyleniyor. **Bu doğrulanmadı** — bkz. §3.
- Boru ucu bir dönem açık bırakılmış; vakum kaynağının (pompa mı, ejektör mü) ne olduğu **bilinmiyor**.

## 2. İki vantuz kullanmanın sonuçları

İki vantuz tek vantuza göre daha iyi bir tasarım: tek vantuz kapağı eğik kaldırıp sıyırabilir, iki
vantuz kapağı düz tutar. Ama iki sonucu var:

1. **Vakum hattı ikiye bölünüyor** — birinde kaçak olursa **ikisi birden** düşer. Ortak hatta bir
   kaçak, tüm kavramayı bitirir.
2. **İki vantuzun aynı düzlemde olması gerekiyor.** Yükseklik farkı varsa biri temas edip
   sızdırmaya başlar, diğeri havada kalır. Montaj prosedürüne bir hizalama adımı girmeli.

---

## 3. 🔴 Fail-safe davranışı — çelişki var, doğrulanmalı

### İddia

Enerji kesildiğinde SPDT röle sayesinde vakum tutulmaya devam ediyor.

### Çelişen kanıt

Zonhen'in ZHV serisi valfleri vakum uygulamalarına ağırlıklı olarak **"S" (normalde açık)** tipiyle
satılıyor: ZHV-0520S göğüs pompası ve gıda vakumlama için, ZHV-0420S tansiyon aleti için. Bu
**kasıtlıdır** — o cihazlarda enerji kesildiğinde basıncın/vakumun boşalması bir güvenlik
gereğidir.

Sizin uygulamanızda **aynı davranış ters sonuç verir**: enerji kesilir → valf açılır → vakum
boşalır → kapak düşer, muhtemelen açık petri kabının içine.

Sizin gövdenizde suffix harfi (L/S) basılı olmadığı için hangi tip olduğu **belirsiz**.

### Doğrulama testleri

**[M16 — Enerji kesme testi](../formlar/olcum-formu.md):** kapağı vantuzla tutturun (altına
yumuşak bir şey koyun), sistemin elektriğini kesin, kapak düşüyor mu bakın.

**[M17 — Valf yönü testi](../formlar/olcum-formu.md):** valf sökülü ve enerjisizken her iki
ucundan sırayla hafifçe üfleyin; sonra kısa süre enerji verip tekrarlayın. Enerjisizken hava
geçiyorsa **normalde açık**, geçmiyorsa **normalde kapalı**.

### Sonuca göre karar

| Test sonucu | Yapılacak |
|---|---|
| Kapak tutuluyor | Mevcut düzenek doğru. Sadece [M18](../formlar/olcum-formu.md) ile şemayı çıkarıp belgeleyin. |
| Kapak düşüyor | **Tasarım değişikliği gerekli** — bkz. §4. |

---

## 4. Fail-safe kuralı ve referans devre

### Tek cümlelik kural

> **Valfin enerjisiz hâli, tutan hâl olmalıdır.**

Bırakmak için enerji verirsiniz. Böylece elektrik kesilmesi, acil stop, sigorta atması, gevşek
kablo, MCU çökmesi, watchdog reseti — **hepsi kapağı tutar**. Tehlikeli durum, enerji gerektiren
durum olmalıdır.

Ters bağlanırsa bunların hepsi kapağı düşürür.

### Bunu sağlayan üç standart desen

**A — Yaylı geri dönüşlü 3/2 valf (en yaygın).**
3/2 valf, SPDT anahtarın pnömatik karşılığıdır: bir ortak port (vantuza giden) ya vakum kaynağına
ya atmosfere bağlanır.

- Enerjisiz (yay konumu): vantuz ↔ vakum kaynağı → **tutar**
- Enerjili: vantuz ↔ atmosfer → **bırakır**

**B — Çek valf + vakum haznesi.**
Vakum üretecinin ile hazne/vantuz arasına bir **çek valf** konur; hava pompaya doğru çıkabilir ama
geri dönemez. Pompa durunca hazne + vantuz + hortumda hapsolan vakum, kaçak hızına bağlı olarak
saniyeler ila dakikalar boyunca parçayı tutar.

**C — Enerji tasarruflu ejektör.**
Venturi/ejektör üreteçleri (ör. SMC ZK2) hedef vakuma ulaşınca basınçlı hava beslemesini kesen bir
basınç anahtarı ve tutan bir çek valf içerir. Basınçlı hava varsa elektrik kesintisini atlatır.

Gerçek makineler genellikle **A + B** birlikte kullanır. Sizinkinin de bu olması muhtemel.

### SPDT rölenin doğru bağlanışı

```
Röle COM  ──→ valf bobini
Röle NO   ──→ +12 V (valf beslemesi)
Röle NC   ──→ boşta

MCU röleyi çeker  → NO kapanır → valf enerjilenir → vantuz havalanır → BIRAKIR
MCU bırakır / güç gider / MCU çöker → NC'ye düşer → valf enerjisiz → yay konumu → TUTAR
```

### Zorunlu ek: manuel tahliye

Fail-safe tutma, **enerji yokken bırakamazsınız** demektir. Sıkışan bir kapağı elle kurtarmak için
bir yol olmalı:

- Küçük bir el valfi (bleed valve), veya
- Sökülebilir bir hortum noktası (rakoru elle ayırabileceğiniz bir yer)

**Kapağı zorla çekmeyin** — vantuzu yırtarsınız. Bu prosedür makinenin yanında yazılı olmalı.

---

## 5. Vantuz seçimi — kuvvet kısıt değil

Sezgiye aykırı ama önemli: **kuvvet bu uygulamada hiç kısıt değil.**

Tutma kuvveti formülü (Schmalz yöntemi):

```
F = ΔP × A = ΔP × (π D² / 4)

D = √( 4·m·g·S / (π·ΔP·n) )
```

`m` = kütle, `g` = 9.81, `S` = güvenlik katsayısı, `ΔP` = atmosfer altı vakum (Pa), `n` = vantuz sayısı.

Güvenlik katsayısı (Schmalz/UVV): pürüzsüz, gözeneksiz parçalar için **en az 1.5**; kritik veya
gözenekli parçalar için ≥2.0; sert ivmelenen parçalar için ≥2.5.

### Örnek hesap — 90 mm polistiren petri kapağı, ~8 g

S = 2 ile gereken kuvvet: `F = 0.008 × 9.81 × 2 = 0.157 N`

Mütevazı bir hobi pompası varsayımıyla ΔP = 40 kPa:

```
D = √( 4 × 0.008 × 9.81 × 2 / (π × 40000 × 1) ) = 0.00224 m = 2.2 mm
```

**2.2 mm çapında bir vantuz teoride yeterli.** Buna karşılık 20 mm'lik bir vantuz 40 kPa'da
**12.6 N** verir — kapağın ağırlığının yaklaşık **160 katı**.

### O halde neye göre seçilir

| Kriter | Açıklama |
|---|---|
| **Sızdırmazlık** | İnce, hafif kubbeli, esnek polistiren yüzeyde conta tutturmak asıl zorluk |
| **Kapağı deforme etmemek** | Küçük vantuz yüksek vakumda ince PS'i göçürür ve contayı bozar; büyük vantuz yükü yayar |
| **Uyum (compliance)** | Kapak gripper'a göre tam düzlemde olmayabilir |

**Öneri: 15-20 mm çapında, körüklü (1-1.5 kıvrımlı) silikon vantuz.** Körük dikey uyumu
kendiliğinden sağlar, yaylı montaj gerekmez.

> ⚠️ **Nitril (NBR) kullanmayın.** Açık renkli plastikte iz bırakır — ve o iz tam da kamerayla
> bakacağınız yüzeydedir. Silikon hem yumuşak (düşük vakumda iyi sızdırmazlık) hem de iz bırakmaz.

Ayrıca: **tutucu kütlesini düşük tutun.** Z ekseni sınırlı torkla çalışıyor.

---

## 6. Valfin sürülmesi

### Bobin verileri (Zonhen 05 gövde sınıfı, doğrulanmış)

| Görev döngüsü | %100 (sürekli) | ≤%50 | ≤%25 | ≤%10 |
|---|---|---|---|---|
| Maks. açık kalma | ∞ | 55 s | 19 s | 3 s |
| Güç (20 °C) | **1.6 W** | 3.2 W | 5.4 W | 16 W |

| Direnç ±%10 | Sürekli çalışma gerilimi | Akım |
|---|---|---|
| 23 Ω | 6 V | 261 mA |
| 90 Ω | 12 V | 133 mA |
| 380 Ω | 24 V | 63 mA |
| 1440 Ω | 48 V | 33 mA |

> 🔴 **Yanma mekanizması:** 23 Ω'luk (6 V) bir bobine 12 V vermek 6.3 W demektir — sürekli 1.6 W'a
> dayanıklı bir parçada. Tablo buna yalnızca **≤%25 görev döngüsü, maks. 19 s** izin veriyor.
> Sürekli uygulanırsa sarım pişer. **[M15](../formlar/olcum-formu.md) yapılmadan gerilim vermeyin.**

### Sürme devresi

En kötü durum (6 V / 23 Ω = 261 mA) baz alınarak:

```
                        +6V veya +12V   ← valfin kendi rayı, MCU regülatöründen DEĞİL
                              │
              ┌───────────────┼──────────────┐
              │               │              │
            [C1]            [C2]          ┌──┴──┐
          100µF/25V        100nF          │Bobin│  ~23Ω (6V) / ~90Ω (12V)
          elektrolitik     seramik        └──┬──┘
              │               │              │
              │               │       D1 ────┤   1N5819 (1A Schottky) veya SS14
              │               │  (katot +V'ye, anot drain'e)
              │               │              │
              │               │              ● drain
              │               │              │
   MCU GPIO ──┼──[R1 100Ω]────┼───────●──────┤ gate    Q1 = AO3400A (SOT-23)
   (3.3 V)    │               │       │      │         veya IRLML2502 / SI2302
              │               │     [R2]     ● source
              │               │    100kΩ     │
              │               │       │      │
             GND─────────────GND─────GND────GND    ← yıldız topraklama, MCU ile ortak
```

**Üç madde, üçü de "atlanırsa yanar" kategorisinde:**

| Bileşen | Neden |
|---|---|
| **Q1 lojik seviyeli ve ≥261 mA olmalı** | `AO3400A` (30 V, 5.7 A, Vgs=2.5 V'ta ~50 mΩ) doğru seçim. **`2N7002` (115 mA) ve `BSS138` (200 mA) yetersizdir** — bu devrede en sık yapılan hata. `IRF540N` ve `2N7000` lojik seviyeli değildir, 3.3 V ile tam açılmaz ve ısınır. |
| **D1 flyback diyot zorunlu** | Bobinin manyetik alanı çökerken yüzlerce volt üretir; diyot yoksa MOSFET ilk kapatmada ölür. **`1N4148` (200 mA) yetersiz**, 1 A'lik parça kullanın. *(İnce ayar: D1 ile seri ~24 V zener/TVS koymak kapanma süresini kısaltır.)* |
| **R2 gate pull-down (100 kΩ)** | MCU reset hâlindeyken veya açılışta GPIO yüksek empedanstadır. Bu direnç olmazsa valf rastgele çekebilir. **Sistemin "enerjisiz = güvenli" mantığının elektriksel garantisi budur.** |

**C1/C2** valfe fiziksel olarak yakın konur; açılış akımını yutar ve MCU rayını temiz tutar.

**Opto-izolasyon gerekmiyor** — düşük gerilimli DC bobin, ortak toprak, flyback diyot varken
güvenli. Ama **bobini MCU'nun regülatöründen beslemeyin**, 261 mA çoğu 3.3 V LDO'yu çökertir.

Alternatif: hazır **opto-izoleli röle modülü** — daha basit ve gerçek izolasyon verir, ama röleler
yavaştır (~5-10 ms) ve aşınır. Petri kabı çevrim hızlarında sorun değil.

### Valfin diğer verileri

| | |
|---|---|
| Çalışma basıncı | **0-375 mmHg (0.5 bar)** |
| Vakum servisi | Uygun |
| Delik çapı | 1.2-1.8 mm |
| Hortum ucu | ∅3 mm dış → **2-3 mm iç çaplı silikon hortum** |
| Ömür | **1.000.000 çevrim** |
| Yalıtım sınıfı | B |
| Tepki süresi | ⚠️ **DOĞRULANMADI** — üretici yayınlamıyor (tipik ~10-30 ms tahmini) |
| Çalışma sıcaklığı | ⚠️ **DOĞRULANMADI** |

> ⚠️ **0.5 bar (≈50 kPa) çalışma basıncı sınırı**, vantuz hesabında kullandığımız 40 kPa ile aynı
> mertebede. İyi bir diyafram pompa 60-80 kPa çekebilir; bu valf o seviyede sınırda kalır ve
> sızdırabilir.

### Model numarası uyarısı

**"ZHV-0518" Zonhen'in kataloğunda yok.** Yayınlanan liste: 0414, 0416, 0419, 0420, **0519**, 0520,
0525, 0530, 0626, 0627, 0628, 0829, 0830, 0832, 0837, 0840, 1032, 1035, 1165, 1434, 4036.

İki ihtimal: silkscreen **ZHV-0519** olarak okunmalı (0519'un genişliği 12 mm, fotoğraftakiyle
uyuyor), ya da OEM'e özel bir varyant.

Sipariş kodu şöyle çalışıyor — **gerilim ve direnç gövdeye basılmayan ayrı alanlardır**:

```
ZHV-0519 L  - 12 -  A  - 180
   │     │     │     │     └─ Bobin direnci (Ω)
   │     │     │     └─────── Seri no
   │     │     └───────────── ÇALIŞMA GERİLİMİ (V)
   │     └─────────────────── (boş)=3 yollu, L=normalde kapalı, S=normalde açık
   └───────────────────────── Gövde/model indeksi
```

Kesinlik isterseniz üreticiye fotoğrafla e-posta atın; OEM varyantını silkscreen'den
tanımlayabilirler.

---

## 7. Kavrama doğrulama sensörü (önerilir, opsiyonel)

**Açık çevrim "vakumu açtım, tutmuştur" varsayımı, bu projede kapağın yere düşmesinin tek sebebi
olacaktır.**

Hatta valfin **çıkışına** (pompaya değil, **vantuza yakın**) bir vakum sensörü koyun — vantuzun
gerçekte ne gördüğünü ölçmek istiyorsunuz.

| Seçenek | Parça | Not |
|---|---|---|
| **En iyi analog** | **NXP MPXV6115V** | −115...0 kPa, analog çıkış, 38.26 mV/kPa, doğrudan ADC'ye. Hazır kart: MikroElektronika Vacuum Click (MIKROE-3195) |
| Alternatif | MPX5100DP | Diferansiyel; bir portu atmosfere referanslamak gerekir |
| Bütçe | XGZP6847 / MPS20N0040D | I²C, çok ucuz, eşik kontrolü için yeterli. ⚠️ Uzun vadeli kararlılığı DOĞRULANMADI |
| Endüstriyel | SMC ZSE30A | Ayarlanabilir eşikli dijital vakum anahtarı; bu proje için pahalı |

### Firmware deseni

1. Vakumu komut et (valfi tutma durumuna al).
2. Sensörü örnekle. Basıncın **kavrama eşiğinin** (ör. −35 kPa) altına inmesini bir **zaman aşımı**
   içinde (ör. 500 ms) bekle.
3. Eşiğe ulaşılmazsa **kaldırma**. Küçük bir Z hareketiyle vantuzu yeniden oturtup bir kez daha
   dene, sonra hata ver.
4. **Hareket boyunca izlemeye devam et.** Basınç **bırakma eşiğinin** (ör. −20 kPa) üstüne
   çıkarsa hareketi kes ve kapağı en yakın güvenli yere bırak.
5. Bırakırken basıncın 0 kPa'ya döndüğünü doğrula — bu, sıkışmış valfi de yakalar.

İki eşik arasında **histerezis** bırakın, yoksa sınırdaki bir conta durum makinesini titretir.

**İlk sürümde sensörsüz de başlayabilirsiniz** (vakumu aç, 300 ms bekle, kaldır). Sensör,
güvenilirlik istediğinizde eklenecek bir iyileştirmedir.

---

## 8. Minimum malzeme listesi

| Bileşen | Amaç | Not |
|---|---|---|
| Vakum üreteci | Vakum kaynağı | Küçük **12 V diyafram pompa** (kompresör gerektirmez, tezgâh için doğru seçim) veya basınçlı hava varsa **venturi ejektör** |
| **3/2 yaylı geri dönüşlü valf** | Tut/bırak, enerjisiz = tut | Fail-safe'in kalbi |
| Çek valf | Üreteç durunca vakumu hapseder | Ucuz, güç kesintisi tutması için şart |
| Vakum haznesi | Vakumu tamponlar, tutma süresini uzatır | 50-100 mL bile çok fayda eder; sert bir boru parçası olabilir |
| Vakum sensörü | Kavrama doğrulama | §7 |
| Vantuz | Kapak arayüzü | 15-20 mm körüklü silikon, ×2 |
| **Filtre** | Agar/toz pompaya ve valfe girmesin | 40 µm inline, vantuza yakın. Biyoloji laboratuvarında ucuz sigorta |
| MOSFET devresi | Valf sürme | §6 |

---

## Kaynaklar

- Zonhen ZHV-0519 datasheet: http://www.zonhen.com/pdf/ZHV-0519.pdf
- Zonhen ZHV-0520 datasheet: http://www.zonhen.com/pdf/ZHV-0520.pdf
- Zonhen ürün sitesi: http://www.zonhen.com
- Schmalz — vantuz tutma kuvveti hesabı:
  https://www.schmalz.com/en/support/know-how/vacuum-knowledge/the-vacuum-system-and-its-components/system-design-calculation-example/theoretical-holding-force-of-a-suction-cup
- Schmalz — vantuz seçimi:
  https://www.schmalz.com/en/support/know-how/vacuum-knowledge/the-vacuum-system-and-its-components/vacuum-suction-cups/design-of-the-suction-cup
- SMC ZK2 enerji tasarruflu ejektör (güç kesintisinde tutma):
  https://www.smcworld.com/catalog/New-products-en/pdf/es100-102-zk2.pdf
- NXP MPXV6115V: https://www.nxp.com/docs/en/data-sheet/MPXV6115V.pdf
- Yerel kopyalar: [`../datasheets/valf/`](../datasheets/valf/)

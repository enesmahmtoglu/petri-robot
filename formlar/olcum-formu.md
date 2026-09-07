# Ölçüm Formu

Bu formdaki değerler olmadan firmware doğru çalışamaz. Yaklaşık 1 saatlik iş.
Doldurdukça `___` yerine değeri yazın ve tarihi not edin.

**Gereken aletler:** cetvel veya kumpas, multimetre, keçeli kalem, mutfak terazisi (varsa), telefon
kamerası.

**Doldurma tarihi:** ___
**Dolduran:** ___

---

## Öncelik sırası

| Öncelik | Ölçüm | Neden acil |
|---|---|---|
| 🔴 1 | **M11** — LED panel tipi | Yanlışsa panel yanar |
| ~~🔴 2~~ | ~~**M15** — Valf bobin direnci~~ | ✅ **TAMAM: 25 Ω → 6 V bobin, 240 mA** |
| 🟠 3 | **M1** — Vida hatvesi / kasnak dişi | `adım/mm` bunsuz hesaplanamaz |
| 🟠 4 | **M4** — Motor akımları ve faz çiftleri | Sürücü akım ayarı bunsuz yapılamaz |
| 🟡 5 | Kalanı | |

---

## A. Mekanik

### M1 — İletim oranı (her eksen için ayrı)

**Vidalı mil eksenleri (X, Y) için yöntem:**
1. Kafayı eksenin ortasına getirin.
2. Vidalı mile keçeli kalemle bir işaret koyun; kafanın yanına sabit bir referans işareti koyun.
3. Mili **elle tam 10 tur** çevirin. (Tek tur ölçmeyin — hata payı 10 kat büyür.)
4. Kafanın gittiği mesafeyi ölçün, 10'a bölün.

**Kayışlı eksen (Z) için ölçmeye gerek yok, sayın:**
Motorun üzerindeki kasnağın **diş sayısını** sayın. GT2 kayışta diş aralığı 2 mm olduğundan
`mm/tur = diş sayısı × 2`. (Tipik 20 diş → 40 mm/tur.)

| Eksen | Tahrik tipi | 10 turda gidilen (mm) | **mm/tur** | Kasnak dişi (kayışlıysa) |
|---|---|---|---|---|
| X | Vidalı mil | ___ | ___ | — |
| Y | Vidalı mil | ___ | ___ | — |
| Z | GT2 kayış | — | ___ | ___ |
| G (tutucu) | ___ | ___ | ___ | ___ |

> Vidalı millerde beklenen değerler: T8 tek ağızlı → 2 mm/tur, dört ağızlı → 8 mm/tur.
> Eksenler arasında 5-20 kat fark çıkması **normaldir**, kayış ve vida farklı oranlar verir.

### M2 — Eksen strokları

Her ekseni elle bir uçtan diğerine sürün, gidebildiği toplam mesafeyi ölçün.
Mekanik olarak sertçe durduğu noktaları da not edin — limit anahtarları oraya yakın gelecek.

| Eksen | Toplam strok (mm) | Mekanik son nerede? |
|---|---|---|
| X | ___ | ___ |
| Y | ___ | ___ |
| Z | ___ | ___ |
| G | ___ (açıklık aralığı) | ___ |

### M8 — Hareketli kütle

Z eksenine asılı olan her şeyin toplam ağırlığı (kafa + vantuzlar + çatal + kablolar).
Sökmeden kabaca tahmin yeterli.

**Z ekseninin taşıdığı kütle:** ___ g

### M10 — İstasyon konumları

Kabaca, birbirlerine göre X/Y mesafeleri:

| İstasyon | X (mm) | Y (mm) |
|---|---|---|
| Petri kabı alma yuvası | ___ | ___ |
| Kapak bırakma noktası | ___ | ___ |
| Optik inceleme (beyaz yuvarlak) | ___ | ___ |
| Atık kutusu | ___ | ___ |

### M7 — Petri kabı

| Ölçü | Değer |
|---|---|
| Dış çap | ___ mm |
| Kapak takılıyken toplam yükseklik | ___ mm |
| Sadece kapağın yüksekliği | ___ mm |
| Boş kabın ağırlığı | ___ g |
| Kapağın ağırlığı | ___ g |

---

## B. Motorlar

### M3 — Motor gövde ölçüleri

Kare gövdenin kenar uzunluğu: **20 mm → NEMA 8**, **42 mm → NEMA 17**.

| Eksen | Kenar (mm) | Gövde derinliği (mm) | Mil çapı (mm) |
|---|---|---|---|
| X | ___ | ___ | ___ |
| Y | ___ | ___ | ___ |
| Z | ___ | ___ | ___ |
| G | ___ | ___ | ___ |

### M4 — Motor etiketi ve faz çiftleri

**Etiketten okuyun:** her motorun anma faz akımı (A/faz). NEMA 17'lerinki bilinmiyor, bu değer
sürücü akım ayarının girdisi.

**Faz çiftlerini multimetreyle bulun:** ohm kademesinde kablo çiftlerini yoklayın.
**Düşük direnç gösteren ikili aynı fazdır**; farklı fazlar arası direnç sonsuz çıkar.

| Eksen | Kablo sayısı | Anma akımı (A/faz) | Ölçülen faz direnci (Ω) | Faz A çifti | Faz B çifti |
|---|---|---|---|---|---|
| X | ___ | ___ | ___ | ___ | ___ |
| Y | ___ | ___ | ___ | ___ | ___ |
| Z | ___ | ___ | ___ | ___ | ___ |
| G (NEMA 8) | 4 *(datasheet)* | **0.6** *(datasheet)* | ~6.5 bekleniyor | siyah+yeşil | kırmızı+mavi |

> NEMA 8'in değerleri datasheet'ten geliyor, yine de teyit edin — klon motorlarda renk kodu
> tutmayabiliyor.

---

## C. Elektrik

### M5 — TMC2208 Vref (mevcut durum)

> ⚠️ **Vref yalnızca motor besleme gerilimi (12-24 V) bağlıyken ölçülebilir.** Vref bölücüsü
> çipin içindeki 5 V regülatörden beslenir, o da ancak VM varken çalışır. Sadece USB ile
> ölçerseniz potu nasıl çevirirseniz çevirin 0-0.2 V görürsünüz.
>
> ⚠️ **Motor kablosu takılı OLMAYACAK.** Enerjiliyken motor takıp çıkarmak sürücüyü öldürür.

Siyah prob GND'ye, kırmızı prob potansiyometrenin metal gövdesine (veya kartta işaretli Vref test
noktasına).

| Sürücü | Kart markası (BTT / FYSETC / diğer) | Ölçülen Vref (V) |
|---|---|---|
| X | ___ | ___ |
| Y | ___ | ___ |
| Z | ___ | ___ |
| G | ___ | ___ |

**PDN_UART köprüsü lehimli mi?** (v1.2 kartlar bazen UART modunda geliyor) ___

### M19 — 🟠 XMC4200 Platform2Go kart varyantı

> Kartın dört montaj varyantı var, hepsinin silkscreen'i aynı. **5 V varyantındaysanız EN hattı
> çalışmayacak** ve sürücüler hiç açılmayacak — bkz.
> [elektrik dokümanı §2](../docs/03-elektrik-baglanti.md).

Arduino dijital başlığının iki yanına bakın (silkscreen `IC300` / `IC301`):

| Gördüğünüz | Varyant |
|---|---|
| İki adet TSSOP-20 çip, üzerinde **TXS0108E** | **5 V** (B veya D) → R1 sorunu var |
| Onun yerine **direnç dizileri** | **3.3 V** (A veya C) → sorun yok |

**Sonuç:** ☐ 5 V varyantı ☐ 3.3 V varyantı — fotoğraflayın.

**JP300 jumper'ı var mı?** (D-Sub yakınında, 3 pinli) ___
**Konnektörler erkek pin mi, dişi soket mi?** ___ *(erkekse shield takılmaz)*

### M6 — Güç kaynağı

| | Değer |
|---|---|
| Model (ön panelden) | ___ |
| Çıkış gerilim aralığı | ___ |
| Çıkış akım aralığı | ___ |
| **Sabit 5 V çıkışı var mı?** (varsa MCH-305**B**'dir) | ___ |

### M13 — Vakum pompası

| | Değer |
|---|---|
| Çalışma gerilimi (etiket) | ___ V |
| Çalışma akımı (etiket veya ampermetre) | ___ A |
| Tip (diyafram pompa / venturi ejektör) | ___ |

### M15 — ✅ Valf bobin gerilimi (TAMAMLANDI)

> ✅ **Ölçüm yapıldı: 25 Ω → 6 V bobin.** Aşağıdaki yöntem kayıt için bırakılmıştır.
> Model numarası ("ZHV-0518") gerilimi
> içermiyor; bobin 6 V için sarılmışsa 12 V verdiğinizde 6.3 W harcanır, oysa parça sürekli
> 1.6 W'a dayanıklıdır.

Valfi söküp iki ucu arasındaki **DC direnci** ölçün. Önce probları birbirine değdirip kablo
direncini okuyun ve çıkarın (23 Ω'da 0.3 Ω fark eder).

**Ölçülen direnç:** ✅ **25 Ω**

| Direnç | Bobin gerilimi | Akım |
|---|---|---|
| 15-20 Ω | 5-6 V | ~300 mA |
| **23-30 Ω** | **6 V** | 261 mA |
| ~50 Ω | 9 V | |
| **85-120 Ω** | **12 V** | 133 mA |
| ~180 Ω | 12 V (düşük güçlü sarım) | 67 mA |
| 380-480 Ω | 24 V | |

Kontrol formülü: **V ≈ √(1.6 × R)**. Bulduğunuz gerilim için `V²/R` hesabı **1.2-2.4 W** bandına
düşmüyorsa tahmin yanlıştır.

**Doğrulama (MCH-305A ile):** akım sınırını beklenen değerin biraz üstüne koyun (ör. 350 mA),
**2 V'tan başlayıp yavaşça yükseltin**, "klik" sesini dinleyin. Valfler anma geriliminin
%70-80'inde çeker → `anma ≈ klik gerilimi ÷ 0.75`, en yakın standart raya yuvarlayın.

**Sonuç — valf bobin gerilimi:** ✅ **6 V** · akım **240 mA** · güç **1.44 W** (sürekli sınır 1.6 W, içeride)

> Kontrol: `V ≈ √(1.6 × 25) = 6.32 V` → 6 V. Üretici tablosundaki 23 Ω ±%10 bandı 20.7-25.3 Ω;
> 25 Ω bandın üst ucunda ve tutarlı. **Sistemde bir 6 V rayı gerekiyor** — 24→6 V buck ya da
> 12 V + seri 25 Ω/3 W direnç. Bkz. [Vakum sistemi §6](../docs/06-vakum-sistemi.md).

> 🔴 **12 V vermeyin:** 25 Ω bobinde 5.76 W olur, sürekli sınırın 3.6 katı.

---

## D. Vakum sistemi (tersine mühendislik)

### M16 — 🔴 Fail-safe testi

Sistemin gerçekten enerji kesildiğinde vakumu tutup tutmadığını belirler. **Kapağın altına
yumuşak bir şey koyun.**

1. Kapağı vantuzlarla tutturun.
2. Sistemin elektriğini kesin.
3. Kapak düşüyor mu?

**Sonuç:** ☐ Kapak tutuluyor (fail-safe ÇALIŞIYOR) ☐ Kapak düşüyor (fail-safe YOK)

### M17 — Valf yönü (normalde açık / normalde kapalı)

Valf sökülü ve **enerjisizken**, her iki ucundan sırayla hafifçe üfleyin. Sonra kısa süre enerji
verip tekrarlayın.

| Durum | Hava geçiyor mu? |
|---|---|
| Enerjisiz | ___ |
| Enerjili | ___ |

**Sonuç:** ☐ Normalde AÇIK (NO) ☐ Normalde KAPALI (NC)

> Zonhen'in vakum uygulamalarına sattığı valfler ağırlıklı olarak **normalde açık**tır — o
> cihazlarda (tansiyon aleti, göğüs pompası) elektrik kesilince basıncın boşalması bir güvenlik
> gereğidir. Bizim uygulamamızda aynı davranış kapağı düşürür. Sonuç "normalde açık" çıkarsa
> [vakum dokümanındaki](../docs/06-vakum-sistemi.md) tasarım değişikliği gerekir.

### M18 — Pnömatik hat izleme

Mavi hortumları uçtan uca izleyip çizin veya fotoğraflayın:

- Vakum kaynağı nedir, nerede? ___
- SPDT röle nerede, kaç V ile sürülüyor? ___
- Çek valf var mı? ___
- Vakum haznesi var mı? ___
- Vantuz çapı ve malzemesi: ___
- Hortum iç çapı: ___ mm

---

## E. Aydınlatma (sonraki faz)

### M11 — 🔴 LED panel tipi

> **Panel etiketi okunmadan LCM-40DA'ya BAĞLAMAYIN.** LCM-40DA sabit **akım** sürücüsüdür ve
> boşta 110 V'a kadar çıkar. Panel sabit **gerilim** tipiyse (12 V / 24 V) bağladığınız anda yanar.

**Panel etiketinde ne yazıyor:** ___

| Etikette görürseniz | Anlamı |
|---|---|
| "12V" veya "24V" | Sabit gerilim → **LCM-40DA yanlış sürücü, BAĞLAMAYIN** |
| "350mA" / "700mA" / "DC 30-40V" gibi | Sabit akım → LCM-40DA uygun |

### M14 — LCM-40DA mevcut ayarları

| | Değer |
|---|---|
| DIP anahtar konumları (fotoğraflayın) | ___ |
| TB1 klemens uç sayısı (**5 uç → DA**, 4 uç → DA2) | ___ |
| Etiketteki tam model adı | ___ |

---

## Ölçüm sonrası

Form dolunca şu dokümanlar güncellenmeli:

- [`docs/05-mekanik-parametreler.md`](../docs/05-mekanik-parametreler.md) — `adım/mm` tablosu
- [`docs/03-elektrik-baglanti.md`](../docs/03-elektrik-baglanti.md) — güç dağıtımı
- [`docs/04-tmc2208-kalibrasyon.md`](../docs/04-tmc2208-kalibrasyon.md) — hedef Vref değerleri
- [`docs/06-vakum-sistemi.md`](../docs/06-vakum-sistemi.md) — pnömatik şema ve fail-safe kararı
- [`formlar/guc-butcesi.md`](guc-butcesi.md) — akım hesabı

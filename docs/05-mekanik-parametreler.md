# Mekanik Parametreler

Bu dokümandaki formüller parametriktir; sayılar [ölçüm formu](../formlar/olcum-formu.md)
doldurulunca yerine oturur. **Uydurma sayı yazmayın** — `adım/mm` yanlışsa robot her mesafeyi
sabit bir katsayıyla yanlış gider ve bu, hata ayıklaması en can sıkıcı arıza türüdür.

---

## 1. `adım/mm` hesabı

Temel formül:

```
adım/mm = (tur başına tam adım × mikroadım) / (tur başına ilerleme mm)
```

- **Tur başına tam adım:** 1.8° motorlarda **200**. (Dört motorun da 1.8° olduğu varsayılıyor;
  M3/M4 ile teyit edin.)
- **Mikroadım:** bu projede **8** (bkz. [TMC2208 kalibrasyonu](04-tmc2208-kalibrasyon.md)).
- **Tur başına ilerleme:** tahrik tipine göre değişir, aşağıya bakın.

### Vidalı mil eksenleri (X, Y)

`mm/tur` = milin **hatvesi** (lead), yani bir tam turda kafanın gittiği mesafe. Bu, dişler arası
mesafe (pitch) **değildir** — "T8" adı taşıyan miller hem 2 mm hem 8 mm hatveli olabilir ve
aradaki fark 4 kattır.

### Kayışlı eksen (Z)

```
mm/tur = kasnak diş sayısı × kayış diş aralığı
```

GT2 kayışta diş aralığı **2 mm**. Yani 20 dişli kasnak → 40 mm/tur.

### Hazır referans tablosu (1.8° motor, 1/8 mikroadım)

| Tahrik | mm/tur | **adım/mm** | Mikroadım başına çözünürlük |
|---|---|---|---|
| T8 vida, hatve 2 mm | 2 | **800.0** | 1.25 µm |
| T8 vida, hatve 8 mm | 8 | **200.0** | 5.00 µm |
| GT2 kayış, 16 diş | 32 | **50.0** | 20.0 µm |
| GT2 kayış, 20 diş | 40 | **40.0** | 25.0 µm |
| GT2 kayış, 24 diş | 48 | ~33.3 | 30.0 µm |

> Vidalı mil eksenleri ile kayışlı eksen arasında **5-20 kat** fark çıkması normaldir. Kayış hızlı
> ama kaba, vida yavaş ama hassas ve yüksek kuvvetli.

### Bu makinenin değerleri

| Eksen | Tahrik | mm/tur (M1) | Mikroadım | **adım/mm** |
|---|---|---|---|---|
| X | Vidalı mil | ___ | 8 | ___ |
| Y | Vidalı mil | ___ | 8 | ___ |
| Z | GT2 kayış | ___ | 8 | ___ |
| G | ___ | ___ | 8 | ___ |

### Deneysel doğrulama (ölçüm yapamıyorsanız veya teyit için)

1. Ekseni ortaya getirin, kafanın konumunu işaretleyin.
2. Firmware'den **tam olarak 1600 adım** gönderin (= 1/8 mikroadımda 1 tam motor turu).
3. Gidilen mesafeyi ölçün. Bu doğrudan `mm/tur` değeridir.
4. `adım/mm = 1600 / ölçülen mesafe`.

Daha hassas sonuç için 10 tur (16000 adım) gönderip 10'a bölün.

---

## 2. Tork bütçesi

### NEMA 8 (tutucu ekseni) — doğrulanmış veriler

| | |
|---|---|
| Tutma torku | **17.65 mN·m** (180 g·cm) |
| Anma akımı | 0.6 A/faz |
| Faz direnci / endüktansı | 6.5 Ω / 1.7 mH |
| Anma gerilimi | 3.9 V |
| Rotor ataleti | 2.0 g·cm² |
| Ağırlık | 60 g |

### Vidalı milde kuvvete çevirme

```
F ≈ (2π × T × η) / hatve
```

`T` = tork (N·m), `η` = verim (trapez vida için tipik **0.3**), hatve metre cinsinden.

NEMA 8'in 17.65 mN·m torkuyla:

| Hatve | Elde edilen kuvvet | Kaldırabildiği kütle |
|---|---|---|
| 2 mm | ~16.6 N | ~1.7 kg |
| 8 mm | ~4.2 N | ~0.42 kg |

> Bu **tutma torku** üzerinden hesaptır. Hareket hâlinde kullanılabilir tork belirgin biçimde
> düşer — pratik tasarımda bu değerin **yarısından azını** hedefleyin.

### Kayışta kuvvete çevirme

```
F = T / r        (r = kasnak yarıçapı = diş sayısı × 2 mm / 2π)
```

20 dişli kasnak → r ≈ 6.37 mm. NEMA 17'nin torku M4'ten sonra bilinecek.

> ⚠️ **Z ekseni kayışlı ve dikey.** Kayış, vidalı milin aksine **kendini tutmaz** (self-locking
> değildir). Enerji kesildiğinde kafa yerçekimiyle düşebilir. İki sonuç:
> 1. Motorun standstill akımı kesilmemeli, ya da
> 2. Mekanik bir denge/fren olmalı, ya da
> 3. Düşüş kabul edilebilir olmalı (kafa zaten en altta duruyorsa).
>
> [Açık sorulardan biri](11-acik-sorular.md) budur — Z'nin enerjisiz davranışı test edilmeli.

---

## 3. Hız ve ivme

### Hız tavanı

```
maks. mm/dk = (mikrodenetleyicinin adım/s tavanı / adım-mm) × 60
```

XMC4200'ün gerçekçi adım üretme kapasitesi [firmware dokümanında](08-firmware-mimarisi.md)
verilecek. Örnek olarak 50 000 adım/s ile:

| adım/mm | Teorik maks. hız |
|---|---|
| 40 (kayış) | 75 000 mm/dk |
| 200 (vida h=8) | 15 000 mm/dk |
| 800 (vida h=2) | 3 750 mm/dk |

Bunlar **teorik tavanlar**; mekanik ve tork sınırları çok daha önce devreye girer. Gerçek çalışma
hızını deneyle bulacaksınız: hızı kademeli artırın, motor adım kaçırmaya (tiz ses, konum kayması)
başladığı değerin **%50'sinde** kalın.

### İvme

Açık çevrim sistemde ivme, adım kaçırmanın **birinci sebebidir**. Motor duran bir kütleyi aniden
hızlandıramaz; komut ile gerçek konum arasında fark oluşur ve bu fark bir daha kapanmaz.

Başlangıç için düşük değerlerle başlayın (ör. 200-500 mm/s²) ve kademeli artırın.

Gereken ivmelendirme torku:

```
T_gerekli = J_toplam × α + T_sürtünme + T_yerçekimi(dikey eksenlerde)
```

Pratikte bunu hesaplamak yerine deneyle bulmak daha hızlı: ivmeyi artır, kaçırma başlayınca yarıya
düş.

### Bu makinenin değerleri

| Eksen | Maks. hız (mm/dk) | İvme (mm/s²) | Strok (mm, M2) |
|---|---|---|---|
| X | ___ | ___ | ___ |
| Y | ___ | ___ | ___ |
| Z | ___ | ___ | ___ |
| G | ___ | ___ | ___ |

---

## 4. Neden düşük endüktanslı küçük motor farklı davranır

NEMA 8'in verilerinden çıkan mühendislik analizi:

**Elektriksel zaman sabiti:** τ = L/R = 1.7 mH / 6.5 Ω = **262 µs**. Sargı akımı hızla kurulur —
düşük endüktansın iyi tarafı budur, motor yüksek adım frekanslarına kadar tork tutabilir.

**Gerilim aşırı sürüşü:** Anma gerilimi sadece 3.9 V (= 0.6 A × 6.5 Ω). 24 V baraya bağlandığında
bu **6.2 kat** aşırı sürüştür. Bu normaldir ve chopper için gereklidir — bara gerilimi *ne kadar
akacağını* değil, akımın *ne kadar hızlı kurulacağını* belirler.

**Kötü tarafı — dalgalanma (ripple):** Chopper dalgalanması ΔI ≈ V·t_on/L. L sadece 1.7 mH
olduğundan di/dt çok yüksektir (24 V / 1.7 mH ≈ 14 A/ms). Sabit chopper frekansında dalgalanma,
0.6 A'lik ayar noktasının büyük bir kesri olur. Sonuç: fazladan demir ve bakır kaybı, duyulabilir
uğultu, ve düşük akım ayarlarında sürücünün minimum açık kalma süresinin altına düşüp kötü
regülasyon yapması.

**Bara gerilimi kuralı (topluluk kaynağı, Geckodrive kökenli):**
`V_besleme ≈ 32 × √(L mH)` = 32 × √1.7 ≈ **42 V** üst sınır. 24 V rahat içeride; 12 V çalışır ama
yüksek hızda belirgin tork kaybı verir. **36 V'a çıkmayın** — kazanç az, dalgalanma ısınması çok.

---

## 5. Ölçüm sonrası kontrol

Form dolup değerler girildikten sonra:

```
☐ Her eksen için adım/mm hesaplandı ve tabloya yazıldı
☐ 1600 adım testi ile en az bir eksende doğrulandı
☐ Strok değerleri girildi (yumuşak limitler için gerekli)
☐ Z ekseninin enerjisiz davranışı test edildi (düşüyor mu?)
☐ Başlangıç hız/ivme değerleri belirlendi
```

---

## Kaynaklar

- SY20STH30-0604A üretici datasheet'i (Changzhou Songyang, Pololu barındırıyor):
  https://www.pololu.com/file/0J635/SY20STH30-0604A.pdf
- SOYO step motor aile datasheet'i:
  https://www.otomasyonline.com/download/Soyo/SOYO%20STEP%20MOTOR%20DATASHEET.pdf
- Pololu ürün sayfası: https://www.pololu.com/product/1204
- Yerel kopya: [`../datasheets/nema8/`](../datasheets/nema8/)

> ⚠️ **DOĞRULANMADI:** NEMA 8 datasheet'inde bir tork-hız (pull-out) eğrisi var (24 VDC, sabit
> 0.6 A, yarım adım koşullarında) ama sayısal eksen değerleri raster görüntü olarak basılmış ve
> metin olarak çıkarılamıyor. İhtiyaç duyarsanız PDF'i açıp grafiği gözle okuyun.
> **Detent (tutucu) torku** datasheet tablosunda alan olarak var ama değeri basılmamış.

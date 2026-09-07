# Datasheet Arşivi

Bu klasördeki dosyalar **kasıtlı olarak yerel kopyalardır**, link listesi değil.

Sebebi somut: **Protoneer'ın resmi sitesi ölü.** `blog.protoneer.co.nz` alakasız bir domaine
yönleniyor, `wiki.protoneer.co.nz` ve `forum.protoneer.co.nz` DNS'te hiç yok. CNC Shield'ın resmi
şeması artık yalnızca Wayback Machine'de bir JPEG olarak duruyor ve **V3.51'in şeması hiç
yayınlanmamış** — üretici, Çin klonları yüzünden tasarım dosyalarını yayınlamayı bıraktığını
yazmış. Aynı şey diğer bileşenler için de olabilir.

---

## İçindekiler

### `tmc2208/`

| Dosya | Ne |
|---|---|
| `TMC2202-2208-2224-datasheet-rev1.13.pdf` | Trinamic/Analog Devices resmi datasheet. Akım formülü, mikroadım tablosu, StealthChop, zamanlama, elektriksel sınırlar. |

**Çevrimiçi kaynaklar:**
- Rev 1.14: https://www.analog.com/media/en/technical-documentation/data-sheets/TMC2202_TMC2208_TMC2224_datasheet_rev1.14.pdf
- Watterott SilentStepStick SSS (pratik tuzaklar): https://learn.watterott.com/silentstepstick/faq/
- Watterott TMC2208 pin yapılandırması: https://learn.watterott.com/silentstepstick/pinconfig/tmc2208/
- BigTreeTech: https://global.bttwiki.com/TMC2208.html
- FYSETC wiki: https://wiki.fysetc.com/docs/TMC2208

> İlgili doküman: [TMC2208 kalibrasyonu](../docs/04-tmc2208-kalibrasyon.md)

---

### `nema8/`

| Dosya | Ne |
|---|---|
| `SY20STH30-0604A-datasheet.pdf` | Üretici çizimi (Changzhou Songyang, 2012-05-13). Tam künye + tork-hız eğrisi. |
| `SOYO-step-motor-aile-datasheet.pdf` | SOYO aile datasheet'i, çok modelli tablolar. Rotor ataleti ve ağırlık burada. |

**Çevrimiçi kaynaklar:**
- Pololu ürün sayfası: https://www.pololu.com/product/1204
- TME: https://www.tme.com/us/en-us/details/pololu-1204/stepper-motors/pololu/sy20sth30-0604a/

> ⚠️ Tork-hız (pull-out) eğrisi PDF'in 1. sayfasında **raster görüntü** olarak var (test koşulları:
> 24 VDC, sabit 0.6 A, yarım adım) ama sayısal eksen değerleri metin olarak çıkarılamıyor — gözle
> okuyun. **Detent torku** tabloda alan olarak var ama değeri basılmamış.

> İlgili doküman: [Mekanik parametreler](../docs/05-mekanik-parametreler.md)

---

### `cnc-shield/`

| Dosya | Ne |
|---|---|
| `cnc_shield_v3xx_schematic.jpg` | **Resmi V3.XX şeması.** Tek hayatta kalan birincil kaynak (Wayback Machine kopyası). Pin haritası, jumper blokları, EN pull-up, endstop devresi. |
| `CNC-Shield-V3-kullanim-kilavuzu-Bulkman.pdf` | Bulkman 3D kullanım kılavuzu v1.0 — pratik montaj ve uyarılar. |
| `protoneer-urun-sayfasi-arsiv.txt` | Arşivlenmiş resmi ürün sayfası metni. |
| `protoneer-montaj-kilavuzu-arsiv.txt` | Arşivlenmiş resmi montaj kılavuzu metni — mikroadım tabloları, 4. eksen jumper açıklaması, uyarılar. |

**Çevrimiçi kaynaklar (hepsi arşiv):**
- Ürün sayfası: https://web.archive.org/web/2020/https://blog.protoneer.co.nz/arduino-cnc-shield/
- Montaj kılavuzu: https://web.archive.org/web/2020/https://blog.protoneer.co.nz/arduino-cnc-shield-v3-00-assembly-guide/
- Elecrow wiki (canlı): https://www.elecrow.com/wiki/arduino-cnc-shield.html

> ⚠️ **V3.51 şeması yok ve elde edilemiyor.** Kartınız V3.51 ise bazı detaylar (endstop filtreleme
> devresi, 4. eksen pinleri) V3.XX'ten farklı olabilir.

> İlgili doküman: [Elektrik ve bağlantılar](../docs/03-elektrik-baglanti.md)

---

### `valf/`

Zonhen ZHV serisi mikro solenoid valf datasheet'leri.

| Dosya | Neden burada |
|---|---|
| `ZHV-0519.pdf` | **En olası eşleşme.** Genişlik 12 mm, fotoğraftaki parçayla uyuyor. |
| `ZHV-0520.pdf` | İkinci olasılık — 2/2, vakum uygulamalarına satılıyor, metal + plastik uçlu. |
| `ZHV-0414/0419/0420/0626/0627/0628.pdf` | Aynı ailenin diğer gövde sınıfları — bobin güç/direnç tablolarını karşılaştırmak için. |

> 🔴 **"ZHV-0518" Zonhen'in kataloğunda yok.** Gövdedeki numara gerilimi **içermiyor**; sipariş
> kodunda gerilim ve direnç ayrı alanlardır. Gerilim ölçümle bulunmalı:
> [M15](../formlar/olcum-formu.md).

**Çevrimiçi kaynaklar:** http://www.zonhen.com (sadece HTTP, HTTPS reddediliyor)

> İlgili doküman: [Vakum sistemi](../docs/06-vakum-sistemi.md)

---

### `led-surucu/`

| Dosya | Ne |
|---|---|
| `LCM-40DA-datasheet.pdf` | Mean Well resmi datasheet. DIP akım tablosu, klemens pinout, DALI/push dim, güvenlik. |
| `lcm-40da-datasheet-metin.txt` | Aynı belgenin metin çıkarımı (arama kolaylığı için). |

**Çevrimiçi kaynaklar:**
- Resmi: https://www.meanwell.com/Upload/PDF/LCM-40DA/LCM-40DA-SPEC.PDF
- Kurulum kılavuzu: https://manuals.plus/mean-well/lcm-40da-led-driver-manual

> 🔴 Bu cihaz **220 V şebekeye** bağlanır ve sabit **akım** sürücüsüdür (boşta 110 V'a kadar).
> Bağlamadan önce [güvenlik dokümanını](../docs/07-guvenlik.md) ve [M11 ölçümünü](../formlar/olcum-formu.md) okuyun.

---

### `guc-kaynagi/`

**MCH-305A için kamuya açık PDF kılavuz bulunamadı.** Üreticinin sitesi (`china-mch.com`) HTTPS
bağlantısını reddediyor. Teknik veriler üreticinin kendi mağaza listesinden alınmıştır:

- https://mch1689.en.made-in-china.com/product/hBLJZUAlbYVs/China-150W-30V-5A-Portable-DC-Power-Supply-for-Laboratory-Power-Supply-Unit-Benchtop-Supply-305A.html
- Üretici: Shenzhen Meichuang Instrument Co., Ltd.

Doğrulanmış künye: 0-30 V / 0-5 A, 150 W, **lineer** (anahtarlamalı değil), tek çıkış, otomatik
CV/CC geçişi, 220 V giriş, 6.4 kg.

> ⚠️ Ön panelde sabit 5 V çıkış ucu varsa elinizdeki **MCH-305B**'dir (5 V / 2 A ek çıkışlı).
> **MCH-K305D / K305DN** ise farklı bir aile — onlar anahtarlamalı.

---

### `xmc4200/`

Infineon XMC4200 Platform2Go belgeleri. *(Araştırma sürüyor, dosyalar eklenecek.)*

---

## Arşive dosya eklerken

1. Dosya adını **anlaşılır** yapın: `TMC2202-2208-2224-datasheet-rev1.13.pdf`, `ds.pdf` değil.
2. Bu INDEX'e bir satır ekleyin: ne olduğu, hangi dokümanla ilgili olduğu.
3. **Kaynak URL'yi mutlaka yazın** — dosya bozulursa veya sürüm sorusu çıkarsa gerekir.
4. Sürüm/revizyon numarası varsa dosya adına koyun.

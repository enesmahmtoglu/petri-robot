# Test Planı

Her aşama bir öncekine dayanır. **Sırayı atlamayın** — atlanırsa dört bilinmeyeni aynı anda hata
ayıklamaya çalışırsınız, ki bu pratikte imkânsızdır.

Her testin bir **kabul kriteri** var: geçtiğini nasıl anlayacağınız. "Çalışıyor gibi görünüyor"
kabul kriteri değildir.

---

## T0 — Enerjilendirme öncesi

**Amaç:** Hiçbir şeyi yakmadan başlamak.

| # | Adım | Kabul kriteri |
|---|---|---|
| T0.1 | [Güvenlik dokümanı](07-guvenlik.md) okundu | — |
| T0.2 | Kart varyantı belirlendi (TXS0108E var mı) | M19 dolduruldu |
| T0.3 | 5 V varyantıysa shield R1 söküldü/100k yapıldı | Ohmmetre ile teyit |
| T0.4 | Fiziksel bağlantı yapıldı (jumper/adaptör/soket) | Süreklilik testi: her hat uçtan uca geçiyor |
| T0.5 | 3V3 → shield 5V, GND → GND | Multimetre: shield 5V pininde **3.3 V** |
| T0.6 | Kısa devre kontrolü | 3V3-GND arası direnç > 1 kΩ |
| T0.7 | MS jumper'ları takılı **değil** | Gözle |
| T0.8 | P7/P8 jumper'ları 4. sütunda | Gözle |
| T0.9 | K6 (ES_Select) → GND | Gözle |
| T0.10 | Acil stop 24 V hattına seri bağlandı | Basılıyken shield EXT-V'de 0 V |

> 🔴 **T0.5 geçmeden motor gücü vermeyin.** Shield'ın 5V pininde 5 V görüyorsanız bir yerde yanlış
> bağlantı var ve XMC pinleri risk altında.

---

## T1 — TMC2208 akım kalibrasyonu

**Amaç:** Sürücüleri doğru akıma ayarlamak. Motorlar henüz bağlı değil.

**Ön koşul:** T0 tamam, [M4 ölçümü](../formlar/olcum-formu.md) yapılmış (motor anma akımları
biliniyor).

| # | Adım | Kabul kriteri |
|---|---|---|
| T1.1 | Motorlar **sökülü**, motor gücü (24 V) **açık** | — |
| T1.2 | Her sürücünün Vref'i ölçüldü | 0 V değil (0 V ise motor gücü yok demektir) |
| T1.3 | Vref hedef değere ayarlandı | `Vref = 1.524 × I_RMS`, ±%5 |
| T1.4 | Motor gücü kapatıldı, motorlar bağlandı | — |

Hedef değerler: [TMC2208 kalibrasyonu](04-tmc2208-kalibrasyon.md).

---

## T2 — F1: Tek eksen döner

**Amaç:** İlk hareket. Zamanlayıcı yok, kesme yok, sabit hız.

| # | Adım | Kabul kriteri |
|---|---|---|
| T2.1 | X ekseni bir yöne dönüyor | Motor düzgün dönüyor, tırmalama/titreme yok |
| T2.2 | Yön değişiyor | `invert_dir` ile istenen yön elde edildi |
| T2.3 | Ses ve ısınma normal | Sürücüye 30 s sonra elle dokunulabiliyor |
| T2.4 | EN kontrolü çalışıyor | EN yüksekken mil elle serbestçe dönüyor |

> ⚠️ **T2.4 önemli:** 5 V kart varyantında EN hattı çalışmayabilir (TXS0108E + shield'ın 10 kΩ
> pull-up'ı sorunu). Motor sürekli tutuyorsa veya hiç tutmuyorsa
> [elektrik dokümanı §2](03-elektrik-baglanti.md) bölümüne bakın.

**Ek gözlem:** Z eksenini elle en üste getirip EN'i devre dışı bırakın. Kafa düşüyor mu?
Sonucu [AS-06](11-acik-sorular.md) altına yazın.

---

## T3 — Adım/mm kalibrasyonu

**Amaç:** Komut edilen mesafe ile gerçek mesafeyi eşitlemek.

**Ön koşul:** [M1 ölçümü](../formlar/olcum-formu.md) yapılmış.

| # | Adım | Kabul kriteri |
|---|---|---|
| T3.1 | 1600 adım gönder (= 1 tam tur) | Gidilen mesafe M1'deki `mm/tur` ile uyuşuyor (±%2) |
| T3.2 | `config.h`'a `steps_per_mm` girildi | — |
| T3.3 | "100 mm git" komutu | **Gerçekten 100 mm gidiyor** (±0.5 mm) |
| T3.4 | Aynı test diğer üç eksende | Her eksende ±%2 |

> Sapma sabit bir katsayı kadarsa (ör. hep 8 kat fazla) sebep **mikroadım varsayımıdır** —
> MS jumper'larını kontrol edin.

---

## T4 — F2/F3: Zamanlayıcı ve ivme profili

| # | Adım | Kabul kriteri |
|---|---|---|
| T4.1 | Adım frekansı komut edilen değerle uyuşuyor | Osiloskop veya "N adımda geçen süre" ölçümü, ±%1 |
| T4.2 | Trapez profil çalışıyor | Motor yumuşak hızlanıp yavaşlıyor, başlangıçta/bitişte zıplama yok |
| T4.3 | Hız sınırı bulundu | Hızı artırırken adım kaçırma başladığı değer kaydedildi |
| T4.4 | İvme sınırı bulundu | Aynı şekilde |
| T4.5 | Çalışma değerleri sınırın **%50'sine** ayarlandı | `config.h` güncellendi |

**Adım kaçırmayı nasıl anlarsınız:** ekseni 100 kez ileri-geri gönderin; başlangıç noktasına
dönmüyorsa adım kaçırıyor. Ayrıca kaçırma anında ses belirgin şekilde değişir.

> T4.3/T4.4 sırasında StealthChop kilidinin etkisini göreceksiniz — yüksek hızda tork erken çöker.
> Bu beklenen davranış, arıza değil. Bkz. [TMC2208 §4](04-tmc2208-kalibrasyon.md).

---

## T5 — F4: Homing

| # | Adım | Kabul kriteri |
|---|---|---|
| T5.1 | Limit anahtarı okunuyor | Elle bastırınca firmware görüyor |
| T5.2 | Debounce çalışıyor | Anahtar titrerken tek tetikleme sayılıyor |
| T5.3 | Homing dizisi tamamlanıyor | Hızlı ara → dur → geri çekil → yavaş yaklaş → sıfırla |
| T5.4 | **Tekrarlanabilirlik** | 10 kez üst üste homing → sıfır noktası aynı yerde (±2 adım) |
| T5.5 | Homing öncesi hareket reddediliyor | Firmware hata veriyor |
| T5.6 | Yumuşak limitler çalışıyor | Strok dışı hedef reddediliyor |

> **T5.4 bu testin kalbidir.** Tekrarlanabilir değilse robot "hep aynı rotayı izle" görevini
> yapamaz. Fark büyükse: yaklaşma hızını düşürün, debounce'u artırın, anahtarın mekanik
> gevşekliğine bakın.

---

## T6 — F5: Senkron çok eksenli hareket

| # | Adım | Kabul kriteri |
|---|---|---|
| T6.1 | X-Y ile kare çiziliyor | Köşeler dik, kenarlar eşit uzunlukta (±1 mm) |
| T6.2 | Çapraz hareket düz | Kalem/işaretleyici ile çizdirin, doğru düz olmalı |
| T6.3 | Üç eksen birlikte | Z de dahil, hedef konuma varılıyor |
| T6.4 | Ardışık hareketlerde birikimli hata yok | 20 kez kare çiz, başlangıca dön, sapma < 1 mm |

---

## T7 — F6: Tutucu ve vakum

**Ön koşul:** [M15, M16, M17](../formlar/olcum-formu.md) yapılmış (valf gerilimi ve yönü biliniyor).

| # | Adım | Kabul kriteri |
|---|---|---|
| T7.1 | Tutucu ekseni hareket ediyor | Çatal açılıp kapanıyor |
| T7.2 | Tutucu referans anahtarı çalışıyor | Sıfırlama tekrarlanabilir |
| T7.3 | Kabı kavrıyor | Kap kaymıyor **ve ezilmiyor** |
| T7.4 | Valf kontrol ediliyor | MOSFET ile aç/kapa, bobin ısınmıyor |
| T7.5 | Vakum kapağı kaldırıyor | Kapak düz kalkıyor, eğilmiyor |
| T7.6 | Vakum bırakıyor | Kapak istenen noktaya düşüyor |
| T7.7 | **Fail-safe doğrulandı** | Enerji kesildiğinde kapak tutuluyor (veya davranış belgelendi) |

> T7.5 başarısızsa: iki vantuzun **aynı düzlemde** olduğunu kontrol edin. Yükseklik farkı varsa
> biri sızdırır ve ikisi birden düşer.

---

## T8 — F7: Tam çevrim

| # | Adım | Kabul kriteri |
|---|---|---|
| T8.1 | Tek çevrim elle müdahalesiz tamamlanıyor | Kabı al → kapağı aç → taşı → bırak → başa dön |
| T8.2 | **10 çevrim üst üste** | Hepsi başarılı, birikimli kayma yok |
| T8.3 | Hata durumları çalışıyor | Limit tetikleme, acil stop, vakum kaybı → `HATA` durumu |
| T8.4 | Acil stop sonrası davranış | Bırakıldığında **otomatik devam etmiyor**, homing istiyor |
| T8.5 | Optik inceleme arayüzü | Tetik sinyali çıkıyor, "bitti" bekleniyor (sahte sinyalle test) |

---

## Test kayıt tablosu

| Test | Tarih | Sonuç | Not |
|---|---|---|---|
| T0 | ___ | ☐ | |
| T1 | ___ | ☐ | |
| T2 | ___ | ☐ | |
| T3 | ___ | ☐ | |
| T4 | ___ | ☐ | |
| T5 | ___ | ☐ | |
| T6 | ___ | ☐ | |
| T7 | ___ | ☐ | |
| T8 | ___ | ☐ | |

---

## Hata ayıklama ipuçları

**Motor hiç dönmüyor:**
1. EN hattı — 5 V varyantı sorunu mu? (elektrik §2)
2. Vref 0 mı? Motor gücü var mı?
3. Faz çiftleri doğru mu? (yanlış eşleşmede motor sadece titrer)
4. STEP darbesi gerçekten çıkıyor mu? Osiloskop/LED ile bakın

**Motor titriyor ama dönmüyor:** faz çiftleri yanlış eşleşmiş. M4'e dönün.

**Mesafeler sabit bir katsayı kadar yanlış:** mikroadım varsayımı hatalı. MS jumper'larına bakın.

**Rastgele adım kaçırma:**
1. İvme çok yüksek
2. Hız StealthChop'un tork bölgesinin dışında
3. Güç kaynağı CC moduna giriyor (gerilim çöküyor) — dekuplaj ve akım sınırı
4. Vref çok düşük

**Sürücüler rastgele resetleniyor:** 3.3 V rayında gerilim düşümü. TMC2208'in VIO reset eşiği
en kötü 3.0 V, payımız 0.3 V. Kabloları kısaltın, dekuplaj ekleyin.

**Homing tekrarlanabilir değil:** yaklaşma hızı yüksek, debounce yetersiz, veya anahtarda mekanik
boşluk var.

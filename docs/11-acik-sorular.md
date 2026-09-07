# Açık Sorular

Bilinmeyenlerin kaydı. **Boş bırakmak yerine ne bilinmediğini yazmak**, sonradan uydurma bir
varsayımın sisteme sızmasını engeller.

**Durum kodları:** 🔴 kritik (donanım riski veya ilerlemeyi bloke ediyor) · 🟠 önemli ·
🟡 sonraki faz · ✅ kapandı

---

## 🔴 Kritik

### AS-01 — LED paneli sabit gerilim mi, sabit akım mı?

**Neden kritik:** LCM-40DA sabit **akım** sürücüsüdür ve boşta 110 V'a kadar çıkar. Panel sabit
gerilim tipiyse (12 V / 24 V) bağlandığı anda yanar.

**Nasıl kapanır:** [M11](../formlar/olcum-formu.md) — panelin etiketini okuyun.
**Kapanana kadar:** LCM-40DA'yı panele **bağlamayın**.

---

### ~~AS-02 — Solenoid valfin bobin gerilimi kaç volt?~~ ✅ KAPANDI

**Ölçüm (M15):** bobin omik direnci **25 Ω**.

**Sonuç: 6 V bobin.**

| Kontrol | Hesap | Sonuç |
|---|---|---|
| Gerilim tahmini | `V ≈ √(1.6 × 25)` = 6.32 V | En yakın standart ray → **6 V** |
| Sürekli güç | `6² / 25` = **1.44 W** | Sürekli sınır 1.6 W → **içeride** ✔ |
| Çalışma akımı | `6 / 25` = **240 mA** | |
| Üretici tablosu | 23 Ω ±%10 = 20.7-25.3 Ω → 6 V | 25 Ω bandın üst ucunda, tutarlı ✔ |

**Sonucu:** Sistemde bir **6 V rayı** gerekiyor. Ayrıntı ve iki uygulama seçeneği:
[Vakum sistemi §6](06-vakum-sistemi.md).

> ⚠️ **12 V vermeyin.** 25 Ω bobinde 12 V = 5.76 W; üretici tablosu buna yalnızca **≤%25 görev
> döngüsü, maksimum 19 s** izin veriyor. Sürekli uygulanırsa sarım pişer.

---

### AS-03 — Vakum sistemi gerçekten fail-safe mi?

**Neden kritik:** Zonhen'in vakum uygulamalarına sattığı valfler ağırlıklı olarak **normalde
açık**tır — o durumda enerji kesildiğinde vakum boşalır ve kapak, muhtemelen açık petri kabının
içine düşer. Kullanıcı beyanı ("SPDT röle sayesinde tutuyor") bununla çelişiyor.

**Nasıl kapanır:** [M16 + M17](../formlar/olcum-formu.md) — enerji kesme testi ve valf yönü testi.
**Sonuca göre:** Kapak düşüyorsa tasarım değişikliği gerekli
([Vakum §4](06-vakum-sistemi.md)).

---

### AS-04 — NEMA 17'lerin anma akımı kaç amper?

**Neden kritik:** Sürücü Vref'i bu değer olmadan ayarlanamaz. Yanlış ayar ya motoru yakar ya da
tork yetersizliğinden adım kaçırtır.

**Nasıl kapanır:** [M4](../formlar/olcum-formu.md) — motor etiketinden okuyun.
**Etiket okunamıyorsa:** Faz direncini ölçün, 0.5 A'den başlayıp motorun ısınmasına bakarak
kademeli artırın.

---

## 🟠 Önemli

### AS-05 — XMC4200 Platform2Go'da Arduino shield başlığı var mı?

**Neden önemli:** Varsa CNC Shield doğrudan takılır ve projenin en kırılgan fiziksel parçası
(elle çekilecek ~14 jumper kablosu) tamamen ortadan kalkar. Yoksa pin pin elle bağlanacak ve
bağlantı şeması çok daha kritik hale gelir.

**Durum:** Araştırma sürüyor.

---

### AS-06 — Z ekseni enerjisiz kaldığında düşüyor mu?

**Neden önemli:** Z ekseni **kayışlı ve dikey**. Kayış, vidalı milin aksine kendini tutmaz
(self-locking değildir). Enerji kesildiğinde kafa yerçekimiyle düşebilir — ve o anda vantuzda
kapak varsa kapak da düşer.

**Nasıl kapanır:** Kafayı ortaya getirin, motor gücünü kesin, düşüyor mu bakın.

**Sonuca göre seçenekler:**
1. Motorun standstill (duruş) akımı hiç kesilmez — sürekli ısınma pahasına
2. Mekanik denge ağırlığı veya fren eklenir
3. Düşüş kabul edilir (kafa park pozisyonunda zaten en alttaysa)

---

### AS-07 — Mekanik parametrelerin hiçbiri ölçülmedi

**Neden önemli:** `adım/mm` bilinmeden hiçbir mesafe doğru gidilemez.

**Nasıl kapanır:** [M1, M2, M3, M8, M10](../formlar/olcum-formu.md).
**Ara çözüm:** Firmware'de değerler yapılandırma dosyasından okunacak şekilde yazılıyor; ölçüm
gelince tek dosya değişecek.

---

### AS-08 — Tutucu (G) ekseni nasıl referanslanacak?

**Neden önemli:** Çatalın kabı ezmeden kavraması lazım. Açık çevrimde "ne kadar kapandığını"
bilmenin bir yolu olmalı.

**Seçenekler:**

| Seçenek | Artı | Eksi |
|---|---|---|
| **Mikro switch ile "tam açık" referansı** (önerilen) | Deterministik, güvenli | Bir anahtar ve bir pin |
| Akımı düşürüp mekanik sona dayama (stall) | Ek donanım yok | Öngörülemez, kabı ezebilir |
| Kuvvet sensörü | En doğru kavrama | Aşırı karmaşık |

**Durum:** Mikro switch öneriliyor, karar bekliyor.

---

### AS-09 — Limit anahtarları henüz yok

**Neden önemli:** Homing olmadan tekrarlanabilir hareket **fiziksel olarak imkânsız**. Her
açılışta koordinat sistemi kayar.

**Nasıl kapanır:** X, Y, Z için birer mekanik mikro switch alınıp mekanik sonlara yakın monte
edilecek. Bağlantı reçetesi: [elektrik dokümanı](03-elektrik-baglanti.md).

---

### AS-10 — Güç dağıtımı çözülmedi

**Neden önemli:** MCH-305A tek çıkışlı, ama sistem üç ray istiyor: ~24 V motorlar, ~12 V (veya 6 V)
pompa/valf, 3.3-5 V lojik.

**Seçenekler:**
1. Makineye ayrı 24 V SMPS + DC-DC dönüştürücüler (**önerilen**)
2. MCH-305A 24 V verir, pompa ayrı 12 V adaptörden, valf 24→6 V buck ile (iyi uzlaşma)
3. Her şey MCH-305A'dan + DC-DC (kabul edilebilir, ama CC foldback riski en yüksek)

**Durum:** Karar bekliyor. Detay: [Güvenlik §5](07-guvenlik.md).

---

### AS-11 — Acil stop henüz yok

**Karar verildi: eklenecek.** Montaj bekliyor. [Güvenlik §3](07-guvenlik.md).

---

## 🟡 Sonraki faz

### AS-12 — Optik inceleme nasıl tetiklenecek?

Raspberry Pi henüz yok. Karar: robot **tetikleyip bekleyecek**, sonuca göre davranış
değiştirmeyecek (basit arayüz: bir çıkış "incele", bir giriş "bitti").

Sonuca göre dallanma (ör. kusurluysa farklı kutu) ileride tek bir `if` ile eklenebilir.

---

### AS-13 — Kavrama doğrulama sensörü eklenecek mi?

Önerilir ama zorunlu değil. İlk sürüm zamanlama ile çalışabilir (vakumu aç, 300 ms bekle, kaldır).
[Vakum §7](06-vakum-sistemi.md).

---

### AS-14 — Petri kabı sayısı artacak mı?

Şu an: tek kap, sabit yuva, sabit kapak bırakma noktası. Çoklu kaba geçmek yazılımda bir döngü;
magazin (üst üste dizili kaplar) mekanik olarak zor, en sona bırakılmalı.

---

## ✅ Kapananlar

| # | Soru | Cevap |
|---|---|---|
| ✅ | Kaç eksen var, hangi motor nerede? | X, Y (vidalı mil) + Z (kayış) + G tutucu (NEMA 8). Ayrı Z kulesi motorsuz. |
| ✅ | Kontrolcü ne olacak? | XMC4200 Platform2Go, tek kart. Arduino UNO projeden çıkarıldı. |
| ✅ | GRBL kullanılacak mı? | Hayır — AVR firmware'i, XMC'de çalışmaz. Firmware sıfırdan yazılacak. |
| ✅ | Hareketler eşzamanlı mı sıralı mı? | Sıralı. Tutucu ekseni diğerleriyle interpolasyon yapmıyor. |
| ✅ | Kabı ne tutuyor, kapağı ne kaldırıyor? | Kabı motorlu çatal, kapağı iki vantuzlu vakum kafası. |
| ✅ | Shield lojik rayı 5 V mı 3.3 V mı? | 3.3 V. Şemadan doğrulandı: rayda 5 V gerektiren aktif bileşen yok. |
| ✅ | Mikroadım kaç olacak? | 1/8 — hiçbir MS jumper'ı takılmayacak. |
| ✅ | Acil stop olacak mı? | Evet. |
| ✅ | LCM-40DA nedir? | Mean Well 40 W sabit akım LED sürücü, DALI dimlemeli. 220 V. |
| ✅ | Aydınlatma dimlenecek mi? | Hayır — DIP ile sabitlenecek. |

---

## Bu dosyanın kullanımı

- Yeni bir bilinmeyen çıktığında **buraya yazın**, kafanızda tutmayın.
- Bir soru kapandığında satırı "Kapananlar" tablosuna taşıyın ve **cevabı yazın** — sadece
  silmeyin, sonra "bunu neden böyle yapmıştık" diye sorulacak.
- 🔴 işaretli bir soru açıkken ilgili donanıma **enerji vermeyin**.

# Güç Bütçesi

Ölçümler gelince doldurulacak. Hesap yöntemi ve tahminler aşağıda.

---

## 1. Neden chopper sürücüde giriş akımı ≠ faz akımı

Sezgiye aykırı ama önemli: **TMC2208'in kaynaktan çektiği akım, motorun faz akımı değildir.**

Chopper sürücü, her faz için bir anahtarlamalı düşürücü (buck) regülatör gibi çalışır. Güç korunur:

```
Giriş gücü ≈ Çıkış gücü / verim
Giriş akımı ≈ (bakır kaybı + demir kaybı + mekanik güç) / (V_bara × η)
```

Yani bara gerilimi ne kadar yüksekse **giriş akımı o kadar düşük** olur. 24 V bu açıdan 12 V'tan
avantajlıdır.

**Örnek — NEMA 8 (0.6 A/faz tepe, 6.5 Ω):**
- Sinüzoidal mikroadımda faz başına RMS = 0.6 / √2 = 0.424 A
- İki fazın bakır kaybı = 2 × 0.424² × 6.5 = **2.34 W**
- 24 V'ta, %85 sürücü verimiyle → **~0.12 A giriş akımı**

Faz akımı 0.6 A ama kaynaktan çekilen 0.12 A. Beş kat fark.

---

## 2. Motor akım bütçesi

**Ölçüm gerekiyor:** NEMA 17'lerin anma akımı ([M4](olcum-formu.md)). Aşağıdaki tahminler
1.0 A/faz, 3.4 Ω varsayımıyla — **doğrulanmadı**.

| Motor | Faz akımı | Faz direnci | Bakır kaybı | 24 V'ta giriş akımı |
|---|---|---|---|---|
| X (NEMA 17) | ___ A | ___ Ω | ___ W | ___ A |
| Y (NEMA 17) | ___ A | ___ Ω | ___ W | ___ A |
| Z (NEMA 17) | ___ A | ___ Ω | ___ W | ___ A |
| G (NEMA 8) | 0.6 A | 6.5 Ω | 2.34 W | ~0.12 A |
| **Toplam** | | | ___ W | **___ A** |

### Tahmini toplam (NEMA 17'ler 1.0 A/faz varsayımıyla)

| Durum | Tahmini güç | 24 V'ta akım |
|---|---|---|
| Dört motor duruyor (tutma) | ~13 W | **0.55-0.65 A** |
| Normal koordineli hareket | ~25-40 W | **1.0-1.7 A** |
| En kötü: hepsi aynı anda hızlanıyor | | **~2 A** |

**Sonuç:** Motorlar tek başına 24 V'ta 2 A'in altında (≈48 W). MCH-305A'nın 5 A'i **kapasite
olarak yeterli**, ~2.5 kat pay var.

---

## 3. Diğer yükler

| Yük | Gerilim | Akım | Not |
|---|---|---|---|
| Vakum pompası | ___ V | ___ A | M13. Kalkışta 3-6 kat ani akım |
| Solenoid valf | **6 V** | **240 mA** | ✅ M15 ölçüldü (25 Ω). 1.44 W sürekli. **6 V rayı gerekiyor** |
| XMC4200 Platform2Go | 5 V (USB) | ~___ mA | USB'den, ayrı |
| LED sürücü (LCM-40DA) | **220 V AC** | 0.23 A @ 230 V | **DC bütçeye dahil değil** — şebekeden besleniyor |

> LCM-40DA'nın DC güç kaynağına **hiç yük bindirmemesi** bir avantajdır. Böyle kalsın.

---

## 4. Güç kaynağı yapılandırması

MCH-305A: **0-30 V / 0-5 A, tek çıkışlı, lineer.** Sabit 5 V çıkışı yok (varsa elinizdeki
**305B**'dir).

### Sorun: tek çıkış, üç ray gerekiyor

| Ray | Kim kullanıyor |
|---|---|
| 24 V | 4 × TMC2208 motor gücü |
| ~12 V | Vakum pompası |
| **6 V** | Solenoid valf (M15: 25 Ω → 6 V) |
| 5 V | XMC4200 (USB'den) |
| 3.3 V | Shield lojik rayı (karttan) |

Tek ayarlanabilir çıkışla bunları aynı anda besleyemezsiniz. **DC-DC dönüştürücü gerekli.**

### Üç yapılandırma, tercih sırasıyla

**1. En iyi — tezgâh kaynağını makineden çıkarın**

Makineye ayrı bir **24 V / ≥5 A kapalı tip SMPS** koyun (ör. Mean Well LRS-100-24 veya
LRS-150-24). MCH-305A'yı **tezgâh aleti** olarak saklayın.

Gerekçe: ayarlanabilir akım sınırı, yeni bir devreyi **100 mA sınırla ilk kez enerjilendirip
hiçbir şey yakmadan** test etmek için paha biçilmez bir teşhis aracıdır. Sabit bir makine rayı
olarak kullanmak bu yeteneği israf eder. Ayrıca muz fişli kablolar çalışma sırasında gevşer.

**2. İyi ödünleşme — gürültülü yükü ayırın**

- Motorlar: MCH-305A, **24.0 V, akım sınırı 4.5 A**
- Pompa: **ayrı bir 12 V / 2-3 A adaptör**
- Valf: **6 V** — 24→6 V buck, ya da 12 V + seri 25 Ω/3 W direnç

En kötü geçici akımı sürücülerden tamamen ayırır. Tezgâh kaynağını kullanmaya devam etmek
istiyorsanız **tek başına en değerli iyileştirme budur.**

**3. Kabul edilebilir — sadece MCH-305A**

- 24.0 V, akım sınırı **4.5 A** (2 A değil — pay bırakın)
- Sürücü V_M ucuna **1000-2200 µF düşük ESR + 100 nF**
- 24 V hattına **TVS diyot** (SMBJ30A)
- Pompa 24→12 V buck üzerinden (buck'ın kendi akım sınırı ani akımı yumuşatır)
- Valf 24→6 V buck üzerinden (240 mA, en küçük modül yeter)
- Firmware'de: **pompa çalıştırma asla hareketle çakışmasın**

---

## 5. Riskler

| Risk | Mekanizma | Önlem |
|---|---|---|
| 🔴 **CC foldback** | Akım sınırına değince kaynak gerilim kaynağı olmaktan çıkar; ray çöker, sürücüler UVLO'ya girip resetlenir. **Konum ve StealthChop kalibrasyonu hareketin ortasında sessizce kaybolur.** | Sınırı cömert ayarla, dekuplaj, firmware'de sıralama |
| **Rejeneratif gerilim** | Lineer kaynak akım çekemez; frenleyen motorlar rayı yukarı iter | TVS diyot. TMC2208 mutlak maks. ~40 V |
| **Pompa ani akımı** | Küçük diyafram pompalar kalkışta 3-6 kat çeker | Ayrı besleme, veya buck arkasına al |
| **Komütasyon gürültüsü** | Fırçalı pompa motoru V_M'ye gürültü bindirir | Ayrı besleme, yıldız topraklama |

---

## 6. Akım sınırı ayarlama prosedürü (MCH-305A)

1. Çıkış **kapalı / bağlantısız**
2. Gerilim düğmesini hedefe getir (24.0 V)
3. Akım düğmesini **tamamen kıs**
4. Çıkış uçlarını **kalın bir kabloyla kısa devre et** — CC kaynakta bu güvenli ve normaldir
5. Cihaz CC moduna girer, gerilim göstergesi sıfıra düşer
6. Akım düğmesini istediğiniz sınıra kadar yükselt (4.5 A)
7. Kısa devreyi kaldır

Artık kaynak 24 V verir ve ayarladığınız akımın üstünde CC'ye katlanır.

---

## 7. Doldurulacak özet

| | Değer |
|---|---|
| Seçilen yapılandırma (1/2/3) | ___ |
| Motor bara gerilimi | ___ V |
| Ayarlanan akım sınırı | ___ A |
| Ölçülen boşta akım (motorlar tutuyor) | ___ A |
| Ölçülen hareket akımı | ___ A |
| Pompa beslemesi | ___ |
| Valf 6 V rayı nasıl elde edildi (buck / seri direnç) | ___ |
| Dekuplaj kondansatörü takıldı | ☐ |
| TVS diyot takıldı | ☐ |

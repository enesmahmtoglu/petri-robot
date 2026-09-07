# Sistem Genel Bakış

---

## 1. Ne yapıyor

Petri kaplarını otomatik işleyen bir laboratuvar robotu. Görev döngüsü:

```
1. Petri kabını sabit yuvadan al          (motorlu çatal tutucu kavrar)
2. Kapağını kaldır                        (iki vantuzlu vakum kafası)
3. Kapağı sabit bırakma noktasına koy
4. Kabı optik inceleme istasyonuna götür
5. İncelemeyi tetikle ve bitmesini bekle  (sonraki faz)
6. Kabı atık kutusuna bırak
7. Başa dön
```

**Şu anki kapsam: 1-4 ve 6.** Optik inceleme (5) sonraki fazda Raspberry Pi + kamera ile
eklenecek; firmware'de arayüzü hazır bırakılacak ama içi boş olacak.

---

## 2. Kinematik

Kartezyen X-Y-Z + bağımsız bir tutucu ekseni.

| Eksen | Motor | Tahrik | Görevi |
|---|---|---|---|
| **X** | NEMA 17 ince | Vidalı mil | Öndeki uzun ray boyunca yatay |
| **Y** | NEMA 17 ince | Vidalı mil | Diğer yönde yatay |
| **Z** | NEMA 17 ince | **GT2 kayış** | Vakum kafası + çatal, dikey |
| **G** | NEMA 8 | *(ölçülecek)* | Çatal açıklığı — kavrama |

**Uzun dikey kule motorsuzdur.** İleride kamera ve bilgisayarın montaj yeri olacak.

**Hareketler sıralıdır, eşzamanlı değil.** Tutucu ekseni (G) X/Y/Z ile interpolasyon yapmaz —
"konuma git, dur, kavra, devam et" şeklinde çalışır. Bu, firmware'in zorluğunu belirgin biçimde
azaltan bir tasarım kararıdır.

> ⚠️ **Z kayışlıdır ve dikeydir.** Kayış, vidalı milin aksine kendini tutmaz. Enerji kesildiğinde
> kafa düşebilir — [AS-06](11-acik-sorular.md) olarak açık, test edilmeli.

---

## 3. Katman mimarisi

Tek kart, üç yazılım katmanı:

```
┌───────────────────────────────────────────────────────┐
│  GÖREV KATMANI            durum makinesi              │
│  "kabı al, kapağı kaldır, taşı, bırak"                │
└───────────────────────┬───────────────────────────────┘
                        │ hedef konumlar, IO komutları
┌───────────────────────▼───────────────────────────────┐
│  HAREKET KATMANI        planlayıcı + Bresenham ISR    │
│  trapez hız profili, senkron çok eksenli hareket      │
└───────────────────────┬───────────────────────────────┘
                        │ STEP/DIR darbeleri
┌───────────────────────▼───────────────────────────────┐
│  DONANIM KATMANI        CNC Shield + 4× TMC2208       │
└───────────────────────────────────────────────────────┘

     ⋯ sonraki faz ⋯
┌───────────────────────────────────────────────────────┐
│  GÖRÜ KATMANI (Raspberry Pi)   iki sinyal ile bağlanır│
│  "incele" çıkışı → ... → "bitti" girişi               │
└───────────────────────────────────────────────────────┘
```

Görü katmanının **iki sinyalle** bağlanması bilinçli bir sadeleştirmedir: robot tetikler, bekler,
sonuca göre dallanmaz. Böylece görüntü işleme tamamen ayrı bir alt proje olarak gelişebilir ve bu
reponun kapsamını büyütmez.

---

## 4. Donanım

| Bileşen | Model | Not |
|---|---|---|
| Mikrodenetleyici | **XMC4200 Platform2Go** | Cortex-M4 80 MHz, 256 KB flash, 40 KB SRAM |
| Sürücü kartı | Protoneer CNC Shield V3 | ⚠️ Karta **takılmıyor**, bkz. [elektrik](03-elektrik-baglanti.md) |
| Motor sürücü | TMC2208 v1.2 × 4 | Standalone, 1/8 mikroadım |
| Motorlar | NEMA 17 × 3, NEMA 8 × 1 | |
| Vakum valfi | Zonhen ZHV-0518 | Gerilimi **bilinmiyor** |
| Vakum kafası | 2 × körüklü vantuz | Mevcut, belgelenmemiş |
| LED sürücü | Mean Well LCM-40DA | 220 V, sonraki faz |
| Güç kaynağı | MCH-305A | 0-30 V / 0-5 A, tek çıkış |

Ayrıntılı künyeler: [bileşenler](02-bilesenler.md).

---

## 5. Temel tasarım kararları ve gerekçeleri

| Karar | Gerekçe | Alternatif neden seçilmedi |
|---|---|---|
| **Tek kart (XMC4200)** | Kart arası protokol yok, tek hata ayıklama ortamı, bol pin ve zamanlayıcı | UNO+XMC2Go hibriti: iki firmware, bir protokol, daha çok iş |
| **Sıfırdan firmware** | Belirtilen amaç "motor sürmeyi öğrenmek"; ayrıca XMC4000'i hedefleyen hazır firmware **yok** | GRBL: AVR'ye özel, taşınamaz. grblHAL/FluidNC/Marlin/Klipper: XMC portu yok |
| **Tek ana zamanlayıcı + Bresenham** | Senkron çok eksenli hareketin tek doğru yolu | Eksen başına donanım PWM: eksenleri oranlayamaz, ayrıca D4/D12 aynı dilimi paylaşıyor |
| **1/8 mikroadım** | TMC2208 içeride MicroPlyer ile 1/256'ya bölüyor; 1/16'ya çıkmak pürüzsüzlük kazandırmaz, hız tavanını yarıya indirir | 1/16: gereksiz maliyet |
| **Lojik rayı 3.3 V** | XMC4200 5 V toleranslı değil (mutlak maks. 4.3 V); shield rayında 5 V gerektiren aktif bileşen yok | 5 V + seviye çevirici: fazladan devre, TMC2208 girişine 3.3 V sürmek spec dışı |
| **Sıralı hareket** | Kapak kaldırma doğası gereği sıralı | Eşzamanlı 4 eksen interpolasyonu: gereksiz karmaşıklık |
| **Vakum fail-safe = tutan** | Enerji kesilince kapak düşmemeli, numune kontamine olmamalı | Ters kurgu: her arıza kapağı düşürür |
| **Görü ayrı, iki sinyalle** | Kapsam kontrolü; görüntü işleme bağımsız gelişebilir | Sonuca göre dallanma: protokolü şimdiden karmaşıklaştırır |

---

## 6. Bilinen riskler

| Risk | Etki | Durum |
|---|---|---|
| **Shield karta takılmıyor** (erkek-erkek başlık) | Fiziksel montaj engeli | Çözüm yolları [elektrik dokümanında](03-elektrik-baglanti.md); parça gerekiyor |
| **5 V kart varyantında EN hattı çalışmayabilir** | Sürücüler hiç açılmaz | Varyant belirlenmeli (M19); çözüm: shield R1'i sök/büyüt |
| **StealthChop kilidi** | Yüksek hızda adım kaçırma | Hız düşük tutulacak; gerekirse UART moduna geçilecek |
| **Açık çevrim** | Adım kaçırma sessizce olur | Homing zorunlu; ivme düşük tutulacak |
| **Z kayışlı ve dikey** | Enerji kesilince düşebilir | Test edilmeli (AS-06) |
| **Valf normalde açık olabilir** | Enerji kesilince kapak düşer | M16/M17 testleriyle doğrulanacak |
| **LED panel tipi bilinmiyor** | Yanlış sürücü paneli yakar | M11 ölçümü yapılmadan bağlanmayacak |
| **Tek çıkışlı güç kaynağı, CC foldback** | Hareket ortasında sürücü reseti | Sınır cömert ayarlanacak, dekuplaj eklenecek |

Tam liste ve kapatma yolları: [açık sorular](11-acik-sorular.md).

---

## 7. Mevcut durum

**Yapılmış:** Mimari kararlar, bileşen araştırması, dokümantasyon.

**Yapılmamış:** Ölçümler ([form](../formlar/olcum-formu.md) boş), fiziksel kart-shield bağlantısı,
firmware'in tamamı.

**Sıradaki adım:** [Ölçüm formunu](../formlar/olcum-formu.md) doldurmak ve F1 firmware'ini
çalıştırmak.

Ayrıntı: [`../DURUM.md`](../DURUM.md)

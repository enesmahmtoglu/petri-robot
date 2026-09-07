# Durum — Nerede Kaldık

**Son güncelleme:** 2026-09-07
**Durum:** Dokümantasyon tamamlandı. Sıradaki iş sizde: ölçümler ve fiziksel bağlantı.

---

## Kararlaştırılmış olanlar (tartışma kapandı)

| Konu | Karar |
|---|---|
| **Kontrolcü** | **Infineon XMC4200 Platform2Go**, tek kart. Arduino UNO ve XMC 2Go projeden çıkarıldı. |
| **Firmware** | Sıfırdan yazılacak. GRBL/grblHAL/FluidNC/Marlin/Klipper — hiçbirinin XMC portu yok. |
| **Eksenler** | X, Y (vidalı mil, NEMA 17) · Z (GT2 kayış, NEMA 17) · G tutucu (NEMA 8). Uzun kule motorsuz. |
| **X tanımı** | Öndeki uzun ray = X. (Keyfi ama sabitlendi.) |
| **Adım üretimi** | Tek ana zamanlayıcı + Bresenham. Eksen başına donanım PWM **değil**. |
| **Mikroadım** | 1/8 — hiçbir MS jumper'ı takılmayacak. |
| **Shield lojik rayı** | 3.3 V (şemadan doğrulandı, rayda 5 V gerektiren aktif bileşen yok). |
| **Hareket tipi** | Sıralı, eşzamanlı değil. Tutucu ekseni interpolasyon yapmıyor. |
| **Kapsam** | Sadece otonom hareket + motor sürmeyi öğrenme. Optik inceleme sonraki faz. |
| **Optik arayüz** | Robot tetikleyip bekler, sonuca göre dallanmaz. |
| **Acil stop** | Eklenecek. |
| **Aydınlatma** | DIP ile sabit parlaklık, dimleme yok. |

---

## Repo tamamlandı

```
README.md                          ✅
DURUM.md                           ✅ (bu dosya)
docs/01-sistem-genel-bakis.md      ✅
docs/02-bilesenler.md              ✅
docs/03-elektrik-baglanti.md       ✅  ← pin haritası, erkek başlık sorunu
docs/04-tmc2208-kalibrasyon.md     ✅
docs/05-mekanik-parametreler.md    ✅
docs/06-vakum-sistemi.md           ✅
docs/07-guvenlik.md                ✅
docs/08-firmware-mimarisi.md       ✅  ← Bresenham kararı, F1-F7
docs/09-gorev-akisi.md             ✅
docs/10-test-plani.md              ✅  ← T0-T8 kabul kriterleri
docs/11-acik-sorular.md            ✅
formlar/olcum-formu.md             ✅  ← M1-M19, DOLDURULMAYI BEKLİYOR
formlar/guc-butcesi.md             ✅
firmware/README.md                 ✅
firmware/ortak/config.h            ✅  ← tüm parametreler, ölçümler 0.0f
firmware/F1-tek-eksen-gpio/main.c  ✅  ← ilk çalışan kod
arastirma/kaynaklar.md             ✅  ← tüm URL'ler + doğrulanmamışlar listesi
datasheets/**                      ✅  17 dosya, ~11 MB
```

---

## 🔴 Sıradaki iş — sizde

### 1. Fiziksel engel: shield karta takılmıyor

XMC4200 Platform2Go'nun Arduino konnektörleri **erkek pin başlığı** (Samtec TSW serisi), dişi
soket değil. Arduino shield'ının altında da erkek pinler var. **Erkek-erkek eşleşmez.**

Pin haritası elektriksel olarak tam uyumlu — bu **mekanik** bir problem. Üç çözüm
([elektrik §1](docs/03-elektrik-baglanti.md)): dişi-dişi jumper kablo (36 adet), şerit kablo
adaptörü, veya başlıkları söküp Arduino istifleme soketi lehimlemek (en temiz).

**Parça siparişi gerekiyor.** Firmware yazmaya bu olmadan başlanabilir ama test edilemez.

### 2. Ölçümler — [`formlar/olcum-formu.md`](formlar/olcum-formu.md)

Öncelikli beş madde:

| # | Ne | Neden acil |
|---|---|---|
| **M11** | LED panel etiketi: sabit gerilim mi akım mı | Yanlışsa LCM-40DA paneli yakar (boşta 110 V) |
| **M15** | Valf bobin direnci | Model numarası gerilimi içermiyor; yanlış gerilim bobini yakar |
| **M19** | Kart varyantı: TXS0108E çipleri var mı | 5 V varyantındaysa **EN hattı hiç çalışmayacak** |
| **M1** | Vida hatvesi / kasnak dişi | `adım/mm` bunsuz hesaplanamaz |
| **M4** | NEMA 17 anma akımları | Sürücü akım ayarı bunsuz yapılamaz |

### 3. Kalibrasyon ve ilk demo

Ölçümler gelince: [test planı](docs/10-test-plani.md) T0 → T1 → T2 sırasıyla.

---

## Devam ederken bilinmesi gerekenler

**`config.h`'daki `0.0f` değerleri kasıtlıdır.** Ölçümler yapılmadan bu sayılar bilinmiyor ve
uydurma bir sayı, eksik bir sayıdan daha tehlikelidir. Ölçümler geldiğinde tek dosya güncellenecek.

**En riskli iki bulgu:**
1. **5 V kart varyantında EN hattı sorunu.** Shield'ın EN hattındaki 10 kΩ pull-up, TXS0108E'nin
   40 kΩ tutma direnciyle bölücü oluşturur → hat ~2.64 V'ta kalır, TMC2208'in geçerli düşük eşiği
   0.99 V. Sürücüler hiç açılmaz. Çözüm: shield R1'i sök veya 100 kΩ yap.
2. **Valf muhtemelen normalde açık.** Zonhen'in vakum uygulamalarına sattığı valfler ağırlıklı
   olarak NO tipi. Öyleyse enerji kesilince kapak düşer — Q18'deki fail-safe iddiasının tersi.
   M16/M17 testleri karar verecek.

**Doğrulanmamışların tam listesi:** [`arastirma/kaynaklar.md`](arastirma/kaynaklar.md) sonundaki
12 maddelik tablo.

---

## Yazılacak firmware (F2-F7)

F1 iskeleti hazır. Kalanlar [firmware mimarisi](docs/08-firmware-mimarisi.md) ve
[firmware/README.md](firmware/README.md) içinde tanımlı:

| Aşama | İçerik | Bağımlılık |
|---|---|---|
| F2 | CCU4 kesmesiyle adım üretimi | F1 çalışmalı |
| F3 | Trapez ivme profili ⭐ en öğretici | M1 ölçümü |
| F4 | Homing + limit anahtarları | Limit anahtarları takılmalı |
| F5 | Bresenham senkron hareket | F3 |
| F6 | Tutucu + vakum | M15/M16/M17 |
| F7 | Görev durum makinesi | Hepsi |

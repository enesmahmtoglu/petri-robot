# Petri Kabı Otomasyon Robotu

3 eksenli (X-Y-Z) + tutuculu, vakumla petri kabı kapağı kaldıran, otonom rota izleyen laboratuvar
otomasyon robotunun teknik dokümantasyonu ve firmware'i.

> **Durum:** Tasarım aşaması. Mekanik yapı ve vakum düzeneği fiziksel olarak mevcut; kontrol
> elektroniği ve firmware yazılacak. Mekanik parametreler henüz ölçülmedi — bkz.
> [`formlar/olcum-formu.md`](formlar/olcum-formu.md).

---

## Robot ne yapıyor

Hedeflenen görev döngüsü:

```
Petri kabını al  →  kapağını kaldır  →  kapağı bırakma noktasına koy
      →  kabı optik inceleme istasyonuna götür  →  incelemeyi tetikle
      →  bitince kabı atık kutusuna bırak  →  başa dön
```

Kabı **motorlu bir çatal tutucu** kavrıyor, kapağı **iki vantuzlu bir vakum kafası** kaldırıyor.
Optik inceleme sonraki fazda (Raspberry Pi + kamera) eklenecek; şu anki kapsam **otonom hareket**.

---

## Eksenler ve motorlar

| Eksen | Motor | Tahrik | Görevi |
|---|---|---|---|
| **X** | NEMA 17 (ince, 23 mm) | Vidalı mil | Öndeki uzun ray boyunca yatay hareket |
| **Y** | NEMA 17 (ince, 23 mm) | Vidalı mil | Diğer yönde yatay hareket |
| **Z** | NEMA 17 (ince, 23 mm) | GT2 kayış | Vakum kafası + çatal, yukarı/aşağı |
| **G** | NEMA 8 (SY20STH30-0604A) | *(ölçülecek)* | Tutucu çatalın açıklığı (kavrama) |

Uzun dikey kule **motorsuzdur** — ileride kamera ve bilgisayarın montaj yeri olacak.

> ⚠️ X ve Y'nin hangi fiziksel rayı temsil ettiği bu dokümanda **tanım gereği** sabitlenmiştir:
> **X = öndeki uzun ray.** Motorların yönü ters çıkarsa firmware'de yön biti çevrilir, kablo
> sökülmez.

---

## Donanım envanteri

| Bileşen | Model | Rol | Durum |
|---|---|---|---|
| Mikrodenetleyici | Infineon **XMC4200 Platform2Go** | Tüm kontrol | Kullanılacak |
| Sürücü kartı | **Protoneer CNC Shield V3** | 4 sürücü yuvası, klemensler | Mevcut |
| Motor sürücü | **TMC2208 v1.2** × 4 | Adım motoru sürme | Mevcut |
| Motor | **NEMA 17** ince × 3 | X, Y, Z | Mevcut |
| Motor | **NEMA 8 / SY20STH30-0604A** × 1 | Tutucu | Mevcut |
| Solenoid valf | **Zonhen ZHV-0518** | Vakum aç/kapa | Mevcut, **gerilimi bilinmiyor** |
| LED sürücü | **Mean Well LCM-40DA** | İnceleme aydınlatması | Mevcut, sonraki faz |
| Güç kaynağı | **MCH-305A** (0-30 V / 0-5 A) | Besleme | Mevcut |
| Görü | Raspberry Pi + kamera | Optik inceleme | **Henüz yok** |

Arduino UNO projeden çıkarılmıştır.

---

## Dokümanlar

| # | Doküman | İçerik |
|---|---|---|
| 01 | [Sistem genel bakış](docs/01-sistem-genel-bakis.md) | Mimari, katmanlar, kinematik, tasarım kararları |
| 02 | [Bileşenler](docs/02-bilesenler.md) | Her bileşenin doğrulanmış künyesi ve kaynakları |
| 03 | [Elektrik ve bağlantılar](docs/03-elektrik-baglanti.md) | Pin haritası, güç dağıtımı, seviye uyumu |
| 04 | [TMC2208 kalibrasyonu](docs/04-tmc2208-kalibrasyon.md) | Akım ayarı, mikroadım, bilinen tuzaklar |
| 05 | [Mekanik parametreler](docs/05-mekanik-parametreler.md) | `adım/mm` hesabı, hız ve tork bütçesi |
| 06 | [Vakum sistemi](docs/06-vakum-sistemi.md) | Pnömatik şema, valf sürme, fail-safe |
| 07 | [Güvenlik](docs/07-guvenlik.md) | Acil stop, 220 V, çalışma kuralları |
| 08 | [Firmware mimarisi](docs/08-firmware-mimarisi.md) | Adım üretimi, hareket planlayıcı, F1-F7 yol haritası |
| 09 | [Görev akışı](docs/09-gorev-akisi.md) | Durum makinesi, hata durumları |
| 10 | [Test planı](docs/10-test-plani.md) | T0-T8 kabul kriterleri |
| 11 | [Açık sorular](docs/11-acik-sorular.md) | Bilinmeyenler ve nasıl kapatılacakları |

**Formlar:**
- [Ölçüm formu](formlar/olcum-formu.md) — M1-M17, doldurulmayı bekliyor
- [Güç bütçesi](formlar/guc-butcesi.md) — akım hesabı tablosu

**Arşiv:**
- [`datasheets/`](datasheets/) — bileşen datasheet'lerinin yerel kopyaları
- [`arastirma/`](arastirma/) — ham araştırma notları ve kaynak listeleri

---

## Nereden başlamalı

Sırayla:

1. **[Güvenlik](docs/07-guvenlik.md) dokümanını okuyun.** Projede 220 V'a bağlı bir bileşen var
   ve motor sürücüleri yanlış işlemle anında ölüyor. Bu doküman "sonra okurum" kategorisinde değil.
2. **[Ölçüm formunu](formlar/olcum-formu.md) doldurun.** Mekanik parametreler olmadan `adım/mm`
   hesaplanamaz, yani robot hiçbir mesafeyi doğru gidemez. Yaklaşık 1 saatlik iş.
3. **[TMC2208 kalibrasyonunu](docs/04-tmc2208-kalibrasyon.md) yapın.** Motorlar bağlanmadan önce
   akım ayarlanır. Ters sırada yapılırsa motor ısınır veya adım kaçırır.
4. **[F1 firmware'ini](firmware/F1-tek-eksen-gpio/)** çalıştırın — tek eksen, sabit hız. İlk demo.
5. Oradan [firmware yol haritasını](docs/08-firmware-mimarisi.md) takip edin.

---

## Bu dokümantasyonun kuralları

- **Doğrulanmış her veri kaynağıyla birlikte verilir.** Datasheet'ten okunmuş bir sayı ile
  tahmin arasındaki fark her zaman belirtilir.
- **Doğrulanmamış bilgi `⚠️ DOĞRULANMADI` ile işaretlenir.** Boş bırakmak yerine ne bilinmediği
  yazılır.
- **Ölçülmemiş parametreler `___` olarak bırakılır.** Uydurma bir sayı, eksik bir sayıdan daha
  tehlikelidir.
- Türkçe yazılmıştır; sektörde İngilizce kullanılan terimler ilk geçtiği yerde parantez içinde
  verilir.

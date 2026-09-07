# Görev Akışı ve Durum Makinesi

Robotun otonom çevrimi. Firmware'in kalbi bu durum makinesidir.

---

## 1. Hedef çevrim

```
Petri kabını al  →  kapağını kaldır  →  kapağı bırakma noktasına koy
      →  kabı optik inceleme istasyonuna götür  →  incelemeyi tetikle ve bekle
      →  bitince kabı atık kutusuna bırak  →  başa dön
```

Tüm istasyonlar **sabit konumda**. Tek kap ile çalışır; çoklu kap ileride bir döngüyle eklenir.

---

## 2. Durum diyagramı

```
                    ┌─────────┐
                    │  BOOT   │  güç geldi, donanım başlatılıyor
                    └────┬────┘
                         ▼
                    ┌─────────┐
              ┌────►│ HOMING  │  tüm eksenler referansa gidiyor
              │     └────┬────┘
              │          ▼
              │     ┌─────────┐
              │     │  HAZIR  │◄──────────────────────┐
              │     └────┬────┘                       │
              │          │ başlat                     │
              │          ▼                            │
              │  ┌───────────────┐                    │
              │  │ KAP_KONUMUNA  │  XY → alma yuvası  │
              │  └───────┬───────┘                    │
              │          ▼                            │
              │  ┌───────────────┐                    │
              │  │ TUTUCU_KAPAT  │  G ekseni kavrar   │
              │  └───────┬───────┘                    │
              │          ▼                            │
              │  ┌───────────────┐                    │
              │  │ KAPAK_INDIR   │  Z aşağı, vantuz   │
              │  └───────┬───────┘   kapağa değer     │
              │          ▼                            │
              │  ┌───────────────┐                    │
              │  │  VAKUM_AC     │  valf tutma konumu │
              │  └───────┬───────┘  + doğrulama       │
              │          ▼                            │
              │  ┌───────────────┐                    │
              │  │ KAPAK_KALDIR  │  Z yukarı          │
              │  └───────┬───────┘                    │
              │          ▼                            │
              │  ┌───────────────┐                    │
              │  │ KAPAK_TASI    │  XY → bırakma nok. │
              │  └───────┬───────┘                    │
              │          ▼                            │
              │  ┌───────────────┐                    │
              │  │ KAPAK_BIRAK   │  Z aşağı, vakum    │
              │  └───────┬───────┘  kapat, Z yukarı   │
              │          ▼                            │
              │  ┌───────────────┐                    │
              │  │ INCELEMEYE_GIT│  XY → beyaz yuvarlak│
              │  └───────┬───────┘                    │
              │          ▼                            │
              │  ┌───────────────┐                    │
              │  │ INCELEME_BEKLE│  "incele" tetikle, │
              │  └───────┬───────┘  "bitti" bekle     │
              │          ▼                            │
              │  ┌───────────────┐                    │
              │  │  ATIGA_GIT    │  XY → atık kutusu  │
              │  └───────┬───────┘                    │
              │          ▼                            │
              │  ┌───────────────┐                    │
              │  │ TUTUCU_AC     │  kabı bırak        │
              │  └───────┬───────┘                    │
              │          └────────────────────────────┘
              │
              │     ┌─────────┐
              └─────┤  HATA   │◄── her durumdan girilebilir
                    └─────────┘    çıkış: elle onay + yeniden HOMING
```

---

## 3. Durumların sözleşmesi

Her durum üç şeyi tanımlar: **girişte ne yapılır**, **çıkış koşulu nedir**, **ne kadar sürede
olmazsa hata**.

| Durum | Giriş eylemi | Çıkış koşulu | Zaman aşımı | Hata durumunda |
|---|---|---|---|---|
| `BOOT` | Donanım init, pinler güvenli konuma | Init tamam | 1 s | `HATA` |
| `HOMING` | Sırayla Z↑ → X → Y → G referansa | Tüm eksenler referanslandı | eksen başına 30 s | `HATA` |
| `HAZIR` | Motorlar enerjili, bekleme | Başlat komutu | — | — |
| `KAP_KONUMUNA` | XY hareketi | Hedefe varıldı | 20 s | `HATA` |
| `TUTUCU_KAPAT` | G ekseni kavrama konumuna | Hedef adım sayısına ulaşıldı | 5 s | `HATA` |
| `KAPAK_INDIR` | Z aşağı, temas yüksekliğine | Hedefe varıldı | 10 s | `HATA` |
| `VAKUM_AC` | Valfi tutma konumuna al | Vakum eşiği aşıldı **veya** sabit gecikme | 500 ms | 1 kez yeniden dene, sonra `HATA` |
| `KAPAK_KALDIR` | Z yukarı, güvenli yüksekliğe | Hedefe varıldı | 10 s | `HATA` |
| `KAPAK_TASI` | XY → bırakma noktası | Hedefe varıldı | 20 s | vakum izleniyor; kaybolursa **derhal dur** |
| `KAPAK_BIRAK` | Z aşağı, vakumu bırak, Z yukarı | Basınç 0'a döndü | 3 s | `HATA` (sıkışmış valf) |
| `INCELEMEYE_GIT` | XY → inceleme istasyonu | Hedefe varıldı | 20 s | `HATA` |
| `INCELEME_BEKLE` | "incele" çıkışını tetikle | "bitti" girişi geldi | 120 s | `HATA` |
| `ATIGA_GIT` | XY → atık kutusu | Hedefe varıldı | 20 s | `HATA` |
| `TUTUCU_AC` | G ekseni açık konuma | Hedefe varıldı | 5 s | `HATA` |
| `HATA` | **Tüm hareketi durdur.** Vakumu **DEĞİŞTİRME.** Hata kodunu göster. | Elle onay | — | → `HOMING` |

---

## 4. Tasarım kuralları

### 4.1 Homing sırası önemli

**Önce Z yukarı çıkmalı.** Z aşağıdayken X veya Y hareket ederse kafa petri kabına, tutucuya veya
tezgâha çarpar. Sıra: **Z↑ → X → Y → G**.

### 4.2 Homing yapılmadan hareket yok

Firmware, `HOMING` tamamlanmadan hiçbir konum komutunu kabul etmemeli. Açık çevrim bir sistemde
referanssız hareket, mekanik sona dayanmak demektir.

### 4.3 Hata durumunda vakuma dokunulmaz

`HATA` durumuna girildiğinde motorlar durur ama **vakum olduğu gibi kalır**. Kapak tutuluyorsa
tutulmaya devam eder. Hata anında kapağı bırakmak, kapağı yere veya açık kabın içine düşürmek
demektir.

### 4.4 Taşıma sırasında vakum izlenir

`KAPAK_TASI` durumunda, vakum sensörü varsa sürekli izlenmeli. Basınç bırakma eşiğinin üstüne
çıkarsa hareket derhal kesilir ve kapak en yakın güvenli yere indirilir — hareketin devam etmesi
kapağı savurur.

### 4.5 Her hareketin zaman aşımı olmalı

Zaman aşımı olmayan bir bekleme, bir gün sistemi sonsuza kadar kilitler. Motor adım kaçırdıysa,
limit anahtarı bozulduysa veya kablo çıktıysa firmware bunu **fark edip hata vermeli**, sessizce
beklememeli.

### 4.6 Acil stop her durumu keser

Acil stop girişi aktifken hiçbir hareket başlatılmaz. Bırakıldığında **otomatik devam edilmez** —
`HOMING`'e dönülür. Aksi halde robot beklenmedik anda hareket eder.

---

## 5. Konum tablosu

Tüm istasyon koordinatları tek bir yapılandırma yapısında toplanacak; ölçümler
([M10](../formlar/olcum-formu.md)) gelince doldurulacak.

| İstasyon | X (mm) | Y (mm) | Z_yaklaşma | Z_temas |
|---|---|---|---|---|
| Park / güvenli | `___` | `___` | — | — |
| Petri kabı alma yuvası | `___` | `___` | `___` | `___` |
| Kapak bırakma noktası | `___` | `___` | `___` | `___` |
| Optik inceleme | `___` | `___` | `___` | — |
| Atık kutusu | `___` | `___` | `___` | — |

**Tutucu (G) konumları:**

| Konum | Adım |
|---|---|
| Tam açık (referans) | 0 |
| Kavrama | `___` |

---

## 6. Hata kodları

| Kod | Anlam | Muhtemel sebep |
|---|---|---|
| `E01` | Homing zaman aşımı | Limit anahtarı bağlı değil, kablo kopuk, motor dönmüyor |
| `E02` | Hareket zaman aşımı | Adım kaçırma, mekanik sıkışma |
| `E03` | Limit anahtarı beklenmedik anda tetiklendi | Yumuşak limit yanlış, konum kaymış |
| `E04` | Vakum kurulamadı | Conta oturmadı, kaçak, pompa çalışmıyor, valf ters |
| `E05` | Vakum taşıma sırasında kayboldu | Conta sıyrıldı, hortum çıktı |
| `E06` | Vakum bırakılamadı | Valf sıkışmış |
| `E07` | İnceleme zaman aşımı | Pi cevap vermiyor |
| `E08` | Acil stop aktif | Buton basılı |

Hata kodu en az kart üzerindeki LED ile (yanıp sönme sayısı) gösterilmeli; UART varsa metin olarak
da basılmalı.

---

## 7. Kademeli uygulama

Durum makinesinin tamamını bir seferde yazmayın. [Firmware yol haritasındaki](08-firmware-mimarisi.md)
F6-F7 aşamalarında şu sırayla ekleyin:

1. `BOOT → HOMING → HAZIR` (hareket yok, sadece referanslama)
2. `+ KAP_KONUMUNA → HAZIR` (tek bir XY hareketi, geri dön)
3. `+ TUTUCU_KAPAT / TUTUCU_AC`
4. `+ KAPAK_INDIR / VAKUM_AC / KAPAK_KALDIR`
5. `+ KAPAK_TASI / KAPAK_BIRAK`
6. `+ INCELEMEYE_GIT / INCELEME_BEKLE / ATIGA_GIT` (tam çevrim)

Her adımda `HATA` durumu ve zaman aşımları **baştan** yerinde olsun — sonradan eklenmez, unutulur.

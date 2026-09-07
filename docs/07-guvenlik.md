# Güvenlik

Bu doküman "sonra okurum" kategorisinde değil. Projede iki farklı tehlike sınıfı var: **220 V
şebeke gerilimi** (insan için tehlikeli) ve **yanlış işlemle anında ölen elektronik** (donanım
için tehlikeli). İkisi de kaza eseri değil, kural bilinmediği için oluyor.

---

## 1. Asla yapılmayacaklar

Bu listedeki her madde, ya bir bileşeni anında öldürür ya da insanı yaralar.

| # | Yapma | Neden |
|---|---|---|
| 1 | **Enerji varken motor kablosunu takma/çıkarma** | Motorun ters EMK'sı sürücünün çıkış katını anında öldürür. TMC2208, A4988, hepsi. |
| 2 | **Motor bağlı değilken sürücüyü enerjilendirip test etme** | Akımı tüketecek bir yük olmadığı için sürücü zarar görebilir. *(İstisna: Vref ölçümü — orada motor **sökülü olmalı**, bkz. §4.)* |
| 3 | **Sürücüyü ters takma** | Yanar. Yön için potansiyometreyi referans alma — TMC2208'in potu A4988'in **ters** tarafındadır. **EN pinini** shield'ın EN işaretiyle hizala. |
| 4 | **LED panelini etiketini okumadan LCM-40DA'ya bağlama** | Sürücü sabit **akım** tipidir, boşta 110 V'a kadar çıkar. Panel sabit gerilim tipiyse yanar. Bkz. §2. |
| 5 | **Valfe 6 V'tan fazla verme** | ✅ Ölçüldü: 25 Ω → **6 V bobin**. 12 V vermek 5.76 W demektir, sürekli sınır 1.6 W. Sarım pişer. |
| 6 | **220 V'a enerjiliyken dokunma** | LCM-40DA'nın PFC kondansatörleri fişten çektikten sonra da yük tutar. **En az 60 saniye bekleyin.** |
| 7 | **DAVE'deki "BMI Get/Set" aracını kullanma** | Infineon kart kılavuzunun kendi uyarısı: yanlış kullanımı kartı kalıcı olarak çalışmaz hale getirebilir. |
| 8 | **XMC pinine 5 V verme** | XMC pinleri 5 V toleranslı değil. Bkz. [elektrik dokümanı](03-elektrik-baglanti.md). |
| 9 | **Homing yapmadan hareket komutu verme** | Açık çevrim sistemde konum bilinmez; eksen mekanik sona dayanır. Firmware bunu zaten engelleyecek. |

---

## 2. 220 V — Mean Well LCM-40DA

Projedeki tek şebeke gerilimli bileşen. Step motorlardan **çok daha tehlikeli**; masaüstünde
açıkta duran bir 220 V bağlantısı bu projenin en ciddi riski.

### Cihazın kendi özellikleri (datasheet'ten doğrulanmış)

| | |
|---|---|
| Giriş | **180-295 VAC** (Türkiye'de sorun yok; 110 V şebekede çalışmaz) |
| Yalıtım sınıfı | **Sınıf II — çift yalıtımlı, TOPRAKLAMA UCU YOK** |
| Koruma sınıfı | **IP20** — sadece kapalı, kuru ortam |
| Açılış ani akımı | **20 A** (260 µs) |
| Çıkış | Sabit akım, 350/500/600/700/900/1050 mA (DIP ile seçilir), 2-100 V |
| Boşta çıkış gerilimi | **110 V'a kadar** |

### Kurallar

1. **Topraklamayı aramayın — yok.** Cihaz Sınıf II, gövdesi tamamen yalıtımlı plastik. Metal bir
   muhafazaya koyarsanız *muhafazayı* topraklarsınız, sürücüyü değil.
2. **Kapalı muhafaza zorunlu.** TB1 klemensi açıkta 220 V taşır. Klemens koruyucusu veya kapalı bir
   kutu kullanın. Açıkta bırakılmaz.
3. **Gecikmeli (T tipi) sigorta kullanın.** 20 A ani akım hızlı sigortayı her açılışta attırır.
4. **Kaçak akım rölesi (RCD/GFCI, 30 mA)** tüm makinenin beslemesinde olmalı. Sürücünün kendi
   kaçağı <0.5 mA, yani gereksiz atmaz.
5. **Kablo çekme emniyeti (strain relief)** — kabloyu çektiğinizde iletken klemensden fırlamamalı.
6. **PUSH ve DA+/DA− uçları da şebeke referanslıdır.** Bunlar SELV değildir. Mikrodenetleyiciyi
   buraya bağlamayın; bu hatlara da şebeke sınıfı kablo kullanın ve 24 V/lojik kablolarından
   fiziksel olarak ayırın.
7. **Çok telli iletkenlere yüksük (ferrule)** takın; kaçak tel bırakmayın.
8. **Canlıyken çalışmayın.** Fişi çekin, LED'lerin sönmesini bekleyin, **60 saniye daha bekleyin**.
9. **Deneyimli birinin gözetiminde bağlayın.**

### Dimleme kararı

**Dimlemeyin.** Parlaklığı DIP anahtarlarıyla sabitleyin. Gerekçe:

- Optik inceleme zaten **sabit ve tekrarlanabilir** aydınlatma ister; dimleme bu işe zarar verir.
- "Push dim" ucu şebeke fazı taşır ve **durum tutan, göreli** bir arayüzdür — "%50'ye ayarla"
  diyemezsiniz, sadece "biraz artır/azalt" diyebilirsiniz. Geri besleme yok.
- DALI ile mutlak seviye kontrolü mümkün ama ayrı bir 16 V veri yolu beslemesi ve izoleli bir
  arayüz gerektirir. Bu projenin kapsamı dışında.

Açma/kapama gerekiyorsa **şebeke sınıfı bir SSR** kullanın (hobi röle modülü değil — kaçak
mesafeleri genellikle yetersiz).

---

## 3. Acil stop

**Ne:** Büyük, kırmızı, mantar başlıklı buton. Elektrikçiden "acil stop butonu" diye alınır,
100-150 TL bandında.

**Nasıl bağlanır:** Motor besleme hattının (24 V artı ucunun) **üzerine seri olarak**. Basınca hat
kopar, dört motor da anında gücünü kaybeder.

**Neden gerekli:** Yazılım kilitlenirse, yön biti yanlışsa veya bir eksen mekanik sona dayanıp
zorlamaya başlarsa bilgisayardan yardım gelmez — kod donmuş, seri port cevapsız olabilir. Acil stop
**yazılımın iş birliğine ihtiyaç duymayan tek durdurma yolunuzdur.**

**Bu makineye özel not:** Acil stop motorları durdurur ama **vakumu etkilemez, etkilememeli**.
Kapağın tutulmaya devam etmesi doğru davranıştır — bu yüzden vakumun fail-safe kurulması önemli
(bkz. [vakum dokümanı](06-vakum-sistemi.md)).

**Firmware tarafı:** Acil stop durumu bir giriş pininden izlenmeli. Buton basılıyken firmware
hiçbir hareket komutu kabul etmemeli ve bırakıldığında **otomatik devam etmemeli** — yeniden
homing istemeli. Aksi halde robot beklenmedik anda hareket eder.

---

## 4. Motor sürücüleri — ölümcül işlem sırası

Sürücülerin çoğu yanlış **sıra** yüzünden ölüyor, yanlış bağlantı yüzünden değil.

### Doğru sıra

```
1. Sürücüler shield'a takılı, motorlar SÖKÜLÜ, motor gücü KAPALI
2. Sürücü yönlerini kontrol et (EN pini hizası)
3. Motor gücünü AÇ  (24 V)
4. Vref'i ölç ve ayarla         ← motor hâlâ sökülü olmalı
5. Motor gücünü KAPAT
6. Motorları BAĞLA
7. Motor gücünü AÇ
8. Test et
```

> **4. adımdaki çelişki bilinçlidir.** Genel kural "motor bağlı olmadan sürücüyü çalıştırma"dır,
> ama Vref ölçümü bunun tek istisnasıdır: pot ayarlanırken motorun bağlı olmaması gerekir, ve bu
> işlem yükü olmayan kısa bir ölçümdür. Ayarı yapıp gücü kesin, motoru sonra bağlayın.

### Neden Vref motor gücü olmadan ölçülemez

TMC2208'in Vref bölücüsü, çipin içindeki 5 V regülatörden beslenir. O regülatör ancak VM (12-24 V)
varken ayağa kalkar. Sadece USB ile ölçerseniz **potu nasıl çevirirseniz çevirin 0-0.2 V**
görürsünüz ve "pot bozuk" sanırsınız.

---

## 5. Güç kaynağı riskleri

**MCH-305A** (0-30 V / 0-5 A, tek çıkışlı, lineer).

| Risk | Açıklama | Önlem |
|---|---|---|
| **CC foldback** | Akım sınırına değince kaynak gerilim kaynağı olmaktan çıkar, gerilimi düşürür. Sürücüler resetlenir, konum ve StealthChop kalibrasyonu **hareketin ortasında sessizce** kaybolur. | Sınırı cömert ayarla (4-4.5 A). Sürücü V_M ucuna **1000-2200 µF + 100 nF**. Pompayı hareket sırasında çalıştırma. |
| **Rejeneratif gerilim** | Lineer kaynak akım çekemez; frenleyen motorlar rayı yukarı iter. | 24 V hattına **TVS diyot** (SMBJ30A). TMC2208 mutlak maks. ~40 V. |
| **Tek çıkış, dört ray gerekli** | 24 V motor, ~12 V pompa, **6 V valf**, 3.3-5 V lojik. | DC-DC dönüştürücüler, veya pompayı ayrı bir 12 V adaptöre al. |
| **Pompa ani akımı** | Küçük diyafram pompalar çalışırken 0.5-1.5 A, kalkışta 3-6 katı çeker. | Pompayı ayrı beslemeye al — en yüksek değerli tek iyileştirme. |

**Öneri:** MCH-305A'yı makineye sabitlemek yerine **tezgâh aleti olarak saklayın**. Ayarlanabilir
akım sınırı, yeni bir devreyi 100 mA sınırla ilk kez enerjilendirip hiçbir şey yakmadan test etmek
için paha biçilmez bir teşhis aracıdır. Makineye ayrı bir 24 V / 5 A kapalı tip SMPS koyun.

**Ortak toprak kuralı:** Mikrodenetleyicinin GND'si ile motor besleme GND'si **ortak olmalı**.
Atlanırsa sürücüler garip davranır ve sebebi bulunamaz. Ama güç akımları ile lojik akımları aynı
kablodan dönmemeli — **yıldız topraklama** kullanın.

---

## 6. Çalışma sırasında kurallar

1. **İlk enerjilendirmede elinizi acil stopun üzerinde tutun.**
2. **Homing'i her açılışta yapın.** Hem konum referansı için, hem de TMC2208'in StealthChop
   otomatik kalibrasyonunun tamamlanması için. Vref veya besleme gerilimi değiştiyse kalibrasyon
   geçersizdir, kartı yeniden başlatın.
3. **Yeni bir hareket komutunu önce düşük hızda deneyin.**
4. **Vakum tutarken enerji kesilirse kapak vantuzda kalır.** Manuel tahliye yolunu bilin
   (bkz. [vakum dokümanı](06-vakum-sistemi.md)) — kapağı zorla çekmeyin, vantuzu yırtarsınız.
5. **Sürücüler ısınır.** TMC2208 1 A üzerinde soğutucu ister. Bizim akımlarımız düşük ama yine de
   elle dokunulamayacak kadar ısınıyorsa akım ayarını gözden geçirin.

---

## Kaynaklar

- Mean Well LCM-40DA datasheet: https://www.meanwell.com/Upload/PDF/LCM-40DA/LCM-40DA-SPEC.PDF
- LCM-40DA kurulum kılavuzu: https://manuals.plus/mean-well/lcm-40da-led-driver-manual
- TMC2208 datasheet (Analog Devices/Trinamic Rev 1.14):
  https://www.analog.com/media/en/technical-documentation/data-sheets/TMC2202_TMC2208_TMC2224_datasheet_rev1.14.pdf
- Watterott SilentStepStick SSS: https://learn.watterott.com/silentstepstick/faq/
- Protoneer CNC Shield montaj kılavuzu (arşiv):
  https://web.archive.org/web/2020/https://blog.protoneer.co.nz/arduino-cnc-shield-v3-00-assembly-guide/

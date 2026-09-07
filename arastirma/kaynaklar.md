# Kaynak Dizini

Bu repodaki teknik iddiaların dayandığı birincil kaynaklar. Dokümanlardaki her sayı buraya
kadar izlenebilir.

**Kural:** üretici birincil dokümanı > resmi wiki/GitHub > topluluk kaynağı. Topluluk kaynakları
etiketlenmiştir.

---

## XMC4200 Platform2Go

| Kaynak | URL |
|---|---|
| Kart kullanım kılavuzu Rev 1.1 (2025-01-23) — **şema Bölüm 3.1 s.19-24, BOM Bölüm 3.2** | https://www.infineon.com/dgdl/Infineon-XMC4200_Platform2Go-UserManual-v01_00-EN.pdf?fileId=5546d4626f229553016f8fca76c12c96 |
| Aynı belge (assets aynası) | https://www.infineon.com/assets/row/public/documents/30/44/infineon-xmc4200-platform2go-usermanual-en.pdf?fileId=5546d4626f229553016f8fca76c12c96 |
| Ürün sayfası | https://www.infineon.com/evaluation-board/KIT-XMC-PLT2GO-XMC4200 |
| XMC4100/XMC4200 datasheet V1.5 — **Tablo 13 port fonksiyonları, Tablo 14 mutlak maksimum** | https://www.infineon.com/dgdl/Infineon-XMC4100_XMC4200_DS-DS-v01_04-EN.pdf?fileId=5546d462696dbf120169817056f938ff |
| ModusToolbox BSP | https://github.com/Infineon/TARGET_KIT_XMC_PLT2GO_XMC4200 |
| XMCLib CAT3 | https://github.com/Infineon/mtb-xmclib-cat3 |
| XMC4 API referansı | https://infineon.github.io/mtb-xmclib-cat3/xmc4_api_reference_manual/html/index.html |
| CMSIS paketi XMC4000_DFP 2.14.0 | https://www.keil.arm.com/packs/xmc4000_dfp-infineon/versions/ |
| DAVE IDE | https://softwaretools.infineon.com/tools/com.ifx.tb.tool.daveide |
| Samtec TSW serisi (erkek başlık teyidi) | https://www.samtec.com/products/tsw |

> ⚠️ **Ayrı yayınlanmış şema PDF'i yok.** Şema kullanım kılavuzunun içinde. "PCB design data"
> ZIP'i (11.38 MB) ürün kaydı arkasında.

**Kılavuzdaki hatalar:** §1 kartın Ethernet'i olduğunu söylüyor — **yanlış**, XMC4200'de Ethernet
MAC yok, BOM'da PHY yok (XMC4400 kılavuzundan kopyalanmış). Tablo 9 `CAN_RX`'i P14.0 gösteriyor,
Tablo 4 ve Şekil 7 **P14.3** diyor (doğrusu bu).

---

## TMC2208

| Kaynak | URL |
|---|---|
| TMC2202/2208/2224 datasheet Rev 1.14 (Analog Devices/Trinamic) | https://www.analog.com/media/en/technical-documentation/data-sheets/TMC2202_TMC2208_TMC2224_datasheet_rev1.14.pdf |
| Rev 1.13 (Digi-Key aynası, metin çıkarımı buradan) | https://media.digikey.com/pdf/Data%20Sheets/Trinamic%20Motion%20Control%20PDFs/TMC2202_TMC2208_TMC2224_Rev1.13_5-26-22.pdf |
| Watterott SilentStepStick SSS — **yön tersliği, soğutma, hot-plug** | https://learn.watterott.com/silentstepstick/faq/ |
| Watterott TMC2208 pin yapılandırması | https://learn.watterott.com/silentstepstick/pinconfig/tmc2208/ |
| BigTreeTech TMC2208 | https://global.bttwiki.com/TMC2208.html |
| FYSETC TMC2208 wiki — **v1.0 standalone / v1.2 UART farkı** | https://wiki.fysetc.com/docs/TMC2208 |

**Yerel kopya:** `datasheets/tmc2208/TMC2202-2208-2224-datasheet-rev1.13.pdf`

**Kritik bulgu:** Yaygın `I_RMS = Vref × 1.77` / `× 1.41` formülleri **0.11 Ω Rsense'li v1.2
kartlar için yanlıştır**. Doğrusu `I_RMS = 0.656 × Vref` (Watterott da bunu veriyor). Yaygın
formül ya Rsense = 0.10 Ω varsayar ya da TMC2130'un +20 mΩ terimini kullanır.

---

## NEMA 8 / SY20STH30-0604A

| Kaynak | URL |
|---|---|
| Üretici çizimi (Changzhou Songyang, Pololu barındırıyor) | https://www.pololu.com/file/0J635/SY20STH30-0604A.pdf |
| SOYO step motor aile datasheet'i | https://www.otomasyonline.com/download/Soyo/SOYO%20STEP%20MOTOR%20DATASHEET.pdf |
| Pololu ürün sayfası | https://www.pololu.com/product/1204 |
| TME | https://www.tme.com/us/en-us/details/pololu-1204/stepper-motors/pololu/sy20sth30-0604a/ |

**Yerel kopya:** `datasheets/nema8/`

> ⚠️ Tork-hız (pull-out) eğrisi datasheet'te **var** (24 VDC, sabit 0.6 A, yarım adım) ama raster
> görüntü, sayısal değerleri metin olarak çıkarılamıyor. Gözle okuyun.
> **Detent torku** tabloda alan olarak var, değeri basılmamış.

---

## Protoneer CNC Shield V3

| Kaynak | URL | Durum |
|---|---|---|
| Resmi ürün sayfası | https://web.archive.org/web/2020/https://blog.protoneer.co.nz/arduino-cnc-shield/ | ⚠️ Sadece arşiv |
| Resmi montaj kılavuzu | https://web.archive.org/web/2020/https://blog.protoneer.co.nz/arduino-cnc-shield-v3-00-assembly-guide/ | ⚠️ Sadece arşiv |
| **Resmi V3.XX şeması (JPG)** | https://web.archive.org/web/2020id_/https://blog.protoneer.co.nz/wp-content/uploads/2013/07/Arduino-CNC-Shield-Scematics-V3.XX_.jpg | ⚠️ Sadece arşiv |
| Bulkman 3D kullanım kılavuzu (topluluk) | https://bulkman3d.com/wp-content/uploads/2019/04/CNC-Shield-Guide-BM-v1.0.pdf | ✅ Canlı |
| Elecrow wiki (topluluk) | https://www.elecrow.com/wiki/arduino-cnc-shield.html | ✅ Canlı |

🔴 **`blog.protoneer.co.nz` ÖLÜ.** Kök alan alakasız bir siteye yönleniyor, derin sayfalar 404,
`wiki.` ve `forum.` alt alanları DNS'te yok. **V3.51'in şeması hiç yayınlanmamış** — üretici,
klonlar yüzünden tasarım dosyalarını yayınlamayı bıraktığını yazmış.

**Bu yüzden `datasheets/cnc-shield/` klasöründeki yerel kopyalar tek güvenilir kaynaktır.**

---

## Zonhen ZHV solenoid valf

| Kaynak | URL |
|---|---|
| ZHV-0519 datasheet | http://www.zonhen.com/pdf/ZHV-0519.pdf |
| ZHV-0520 datasheet | http://www.zonhen.com/pdf/ZHV-0520.pdf |
| ZHV-0420 datasheet | http://www.zonhen.com/pdf/ZHV-0420.pdf |
| Üretici sitesi | http://www.zonhen.com |
| Sipariş kodu örneği (`ZHV-0519L-12-A-180`) | https://www.mantech.co.za/ProductInfo.aspx?Item=15M9054 |

**Yerel kopya:** `datasheets/valf/` (8 PDF)

> ⚠️ Site yalnızca **HTTP**, HTTPS reddediliyor.
> 🔴 **"ZHV-0518" katalogda YOK.** Ya ZHV-0519'un yanlış okunması ya da OEM varyantı.

---

## Vakum ve vantuz tasarımı

| Kaynak | URL |
|---|---|
| Schmalz — vantuz tutma kuvveti hesabı | https://www.schmalz.com/en/support/know-how/vacuum-knowledge/the-vacuum-system-and-its-components/system-design-calculation-example/theoretical-holding-force-of-a-suction-cup |
| Schmalz — vantuz seçimi ve güvenlik katsayıları | https://www.schmalz.com/en/support/know-how/vacuum-knowledge/the-vacuum-system-and-its-components/vacuum-suction-cups/design-of-the-suction-cup |
| SMC ZK2 enerji tasarruflu ejektör (güç kesintisinde tutma) | https://www.smcworld.com/catalog/New-products-en/pdf/es100-102-zk2.pdf |
| SMC vakum sensörleri | https://www.smcusa.com/products/vacuum-products/vacuum-sensors~29361/ |
| NXP MPXV6115V vakum sensörü | https://www.nxp.com/docs/en/data-sheet/MPXV6115V.pdf |
| MikroElektronika Vacuum Click | https://www.mikroe.com/vacuum-click |

---

## Mean Well LCM-40DA

| Kaynak | URL |
|---|---|
| **Resmi datasheet** | https://www.meanwell.com/Upload/PDF/LCM-40DA/LCM-40DA-SPEC.PDF |
| LED ürün portalı | https://led.meanwell.com/webapp/product/search.aspx?prod=LCM-40DA |
| Kurulum kılavuzu (DIP tablosu çapraz kontrolü) | https://manuals.plus/mean-well/lcm-40da-led-driver-manual |
| Mouser | https://www.mouser.com/ProductDetail/MEAN-WELL/LCM-40DA |

**Yerel kopya:** `datasheets/led-surucu/`

---

## MCH-305A güç kaynağı

| Kaynak | URL |
|---|---|
| Üretici mağaza listesi (tek erişilebilir kaynak) | https://mch1689.en.made-in-china.com/product/hBLJZUAlbYVs/China-150W-30V-5A-Portable-DC-Power-Supply-for-Laboratory-Power-Supply-Unit-Benchtop-Supply-305A.html |
| Üretici sitesi | http://china-mch.com/ |
| 305B sabit 5 V çıkışı hakkında | https://www.wziep.com/post/193993 |

> ⚠️ **Kamuya açık PDF kılavuz yok.** Üreticinin sitesi HTTPS reddediyor. Spesifikasyonlar
> üreticinin kendi mağaza listesinden.

---

## Hareket kontrol firmware'i (olumsuz sonuç)

Hiçbirinin XMC/Infineon portu **yok** — bu, sıfırdan yazma kararının dayanağı.

| Firmware | Sürücü listesi |
|---|---|
| grblHAL | https://github.com/grblHAL/drivers · https://github.com/grblHAL/3rd_party_drivers |
| FluidNC | https://github.com/bdring/FluidNC |
| Marlin | https://marlinfw.org/docs/hardware/tmc_drivers.html |
| Klipper | `src/Kconfig` |

**Zephyr** — hem XMC4xxx SoC portu (`soc/infineon/cat3/xmc4xxx`) hem step motor alt sistemi
(`drivers/stepper/step_dir`, `adi_tmc`) var, ama `soc.yml` yalnızca xmc4500/4700/4800 tanımlıyor.
XMC4200 eklemek ileride bakılabilecek bir yol.

---

## Doğrulanmamışların listesi

| # | Konu | Nasıl kapanır |
|---|---|---|
| 1 | Elimizdeki kart varyantı (3.3 V mı 5 V mi) | M19 — TXS0108E çiplerine bak |
| 2 | ZHV-0518'in gerçek modeli ve bobin gerilimi | M15 — direnç ölç |
| 3 | Valfin normalde açık mı kapalı mı olduğu | M17 — üfleme testi |
| 4 | Beyaz LED panelinin sabit gerilim mi akım mı olduğu | M11 — etiket oku |
| 5 | NEMA 17'lerin anma akımı | M4 — etiket oku |
| 6 | Vakum pompasının tipi, gerilimi, akımı | M13 |
| 7 | XMC4200 kesme gecikmesi (çevrim) — Infineon yayınlamıyor | F2'de osiloskopla ölç |
| 8 | Gerçekçi azami adım hızı | F2/F5'te ölç |
| 9 | Z ekseninin enerjisiz düşüp düşmediği | T2 sırasında test |
| 10 | V3.51 shield'ın şeması (hiç yayınlanmamış) | Kapanamaz — V3.XX şemasıyla çalışılacak |
| 11 | NEMA 8 detent torku ve tork-hız eğrisi sayısal değerleri | PDF'i gözle oku |
| 12 | `WSPFLASH` değerini `SystemInit()`'in ne yaptığı | Register'ı hata ayıklayıcıda oku |

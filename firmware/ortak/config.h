/*
 * config.h — Petri Kabı Otomasyon Robotu
 *
 * TÜM ayarlanabilir parametreler bu dosyadadır. Koda dağılmış sihirli sayı yoktur.
 * Ölçümler tamamlandığında yalnızca bu dosya güncellenir.
 *
 * Donanım: Infineon XMC4200 Platform2Go + Protoneer CNC Shield V3 + 4x TMC2208
 * Pin haritası kaynağı: docs/03-elektrik-baglanti.md
 */

#ifndef CONFIG_H
#define CONFIG_H

#include <stdint.h>
#include <stdbool.h>
#include "xmc_gpio.h"

/* ==========================================================================
 * 1. SAAT
 * ========================================================================== */

#define F_CPU_HZ              80000000UL   /* Cortex-M4 azami hiz            */
#define F_CCU_HZ              80000000UL   /* CCU4/CCU8 zamanlayici saati    */

/* NOT: 80 MHz'de flash bekleme durumu WSPFLASH >= 2 olmalidir
 * (flash erisim suresi 20 ns, cevrim 12.5 ns). SystemInit() bunu ayarlar;
 * saat yeniden ayarlanirsa dogrulanmalidir. Bkz. docs/08-firmware-mimarisi.md */

/* ==========================================================================
 * 2. PIN HARITASI
 *
 * Arduino basligi -> XMC4200 pini -> CNC Shield islevi
 * Kaynak: XMC4200 Platform2Go kullanim kilavuzu Sekil 9 (s.15)
 *
 * !! Shield karta FIZIKSEL OLARAK TAKILMAZ (erkek-erkek baslik).
 *    Bkz. docs/03-elektrik-baglanti.md bolum 1.
 * ========================================================================== */

/* --- STEP pinleri --------------------------------------------------------- */
#define PIN_X_STEP_PORT       XMC_GPIO_PORT1     /* Arduino D2  -> P1.0      */
#define PIN_X_STEP_NUM        0u
#define PIN_Y_STEP_PORT       XMC_GPIO_PORT2     /* Arduino D3  -> P2.2      */
#define PIN_Y_STEP_NUM        2u
#define PIN_Z_STEP_PORT       XMC_GPIO_PORT2     /* Arduino D4  -> P2.9      */
#define PIN_Z_STEP_NUM        9u
#define PIN_G_STEP_PORT       XMC_GPIO_PORT0     /* Arduino D12 -> P0.0      */
#define PIN_G_STEP_NUM        0u

/* --- DIR pinleri ---------------------------------------------------------- */
#define PIN_X_DIR_PORT        XMC_GPIO_PORT2     /* Arduino D5  -> P2.3      */
#define PIN_X_DIR_NUM         3u
#define PIN_Y_DIR_PORT        XMC_GPIO_PORT2     /* Arduino D6  -> P2.4      */
#define PIN_Y_DIR_NUM         4u
#define PIN_Z_DIR_PORT        XMC_GPIO_PORT2     /* Arduino D7  -> P2.8      */
#define PIN_Z_DIR_NUM         8u
#define PIN_G_DIR_PORT        XMC_GPIO_PORT1     /* Arduino D13 -> P1.8      */
#define PIN_G_DIR_NUM         8u

/* --- Ortak sürücü etkinlestirme (AKTIF DUSUK) ----------------------------- */
#define PIN_EN_PORT           XMC_GPIO_PORT2     /* Arduino D8  -> P2.6      */
#define PIN_EN_NUM            6u

/* --- Limit anahtarlari (AKTIF DUSUK, dahili pull-up) ---------------------- */
#define PIN_X_LIMIT_PORT      XMC_GPIO_PORT0     /* Arduino D9  -> P0.11     */
#define PIN_X_LIMIT_NUM       11u
#define PIN_Y_LIMIT_PORT      XMC_GPIO_PORT1     /* Arduino D10 -> P1.7      */
#define PIN_Y_LIMIT_NUM       7u
#define PIN_Z_LIMIT_PORT      XMC_GPIO_PORT1     /* Arduino D11 -> P1.9      */
#define PIN_Z_LIMIT_NUM       9u

/* --- Sadece giris olan pinler (P14 sinifi) --------------------------------
 * !! P14 pinlerinde DAHILI PULL-UP YOKTUR. Harici 10k pull-up sart.        */
#define PIN_G_REF_PORT        XMC_GPIO_PORT14    /* Arduino A1  -> P14.6     */
#define PIN_G_REF_NUM         6u
#define PIN_ESTOP_PORT        XMC_GPIO_PORT14    /* Arduino A2  -> P14.7     */
#define PIN_ESTOP_NUM         7u
/* Vakum sensoru (analog, VADC G1CH0)              Arduino A3  -> P14.8     */
#define ADC_VACUUM_GROUP      1u
#define ADC_VACUUM_CHANNEL    0u

/* --- Cikislar ------------------------------------------------------------- */
#define PIN_VALVE_PORT        XMC_GPIO_PORT3     /* Arduino SCL -> P3.0      */
#define PIN_VALVE_NUM         0u

/* !! Arduino A0 (P14.0) KULLANILMAZ - kart uzerindeki 10k potansiyometre
 *    (silkscreen R7) hatti yukluyor. Gerekirse R7 sokulur.                  */

/* ==========================================================================
 * 3. EKSEN PARAMETRELERI
 *
 * !! DEGERLER HENUZ OLCULMEDI. formlar/olcum-formu.md doldurulunca girilecek.
 *    Asagidaki 0.0f degerleri KASITLIDIR - uydurma sayi, eksik sayidan
 *    daha tehlikelidir. Firmware bu degerlerle calismayi reddetmelidir.
 * ========================================================================== */

#define AXIS_COUNT            4
enum { AXIS_X = 0, AXIS_Y, AXIS_Z, AXIS_G };

#define MICROSTEPS            8        /* MS jumper'lari TAKILMAZ -> 1/8    */
#define MOTOR_STEPS_PER_REV   200      /* 1.8 derece                        */

/* --- adim/mm ------------------------------------------------------------
 * Vidali mil : adim/mm = (200 * MICROSTEPS) / hatve_mm
 * GT2 kayis  : adim/mm = (200 * MICROSTEPS) / (dis_sayisi * 2)
 * Bkz. docs/05-mekanik-parametreler.md                                     */
#define X_STEPS_PER_MM        0.0f     /* TODO: M1 olcumu (vidali mil)      */
#define Y_STEPS_PER_MM        0.0f     /* TODO: M1 olcumu (vidali mil)      */
#define Z_STEPS_PER_MM        0.0f     /* TODO: M1 olcumu (GT2 kayis)       */
#define G_STEPS_PER_MM        0.0f     /* TODO: M1 olcumu (tutucu)          */

/* --- Azami hiz (mm/dk) --------------------------------------------------
 * T4.3'te deneysel olarak bulunacak, sonra YARISINA ayarlanacak.           */
#define X_MAX_RATE            0.0f     /* TODO */
#define Y_MAX_RATE            0.0f     /* TODO */
#define Z_MAX_RATE            0.0f     /* TODO */
#define G_MAX_RATE            0.0f     /* TODO */

/* --- Ivme (mm/s^2) ------------------------------------------------------
 * Acik cevrim sistemde adim kacirmanin BIRINCI sebebi ivmedir.
 * 200-500 ile baslayin, kademeli artirin.                                  */
#define X_ACCEL               200.0f
#define Y_ACCEL               200.0f
#define Z_ACCEL               200.0f
#define G_ACCEL               100.0f

/* --- Azami strok (mm) - yumusak limitler icin --------------------------- */
#define X_MAX_TRAVEL          0.0f     /* TODO: M2 olcumu */
#define Y_MAX_TRAVEL          0.0f     /* TODO: M2 olcumu */
#define Z_MAX_TRAVEL          0.0f     /* TODO: M2 olcumu */
#define G_MAX_TRAVEL          0.0f     /* TODO: M2 olcumu */

/* --- Yon ters cevirme ---------------------------------------------------
 * TMC2208'in DIR polaritesi A4988'in TERSIDIR. Bu beklenen bir durumdur;
 * kablo sokulmez, burada duzeltilir. Bkz. docs/04-tmc2208-kalibrasyon.md   */
#define X_INVERT_DIR          true
#define Y_INVERT_DIR          true
#define Z_INVERT_DIR          true
#define G_INVERT_DIR          true

/* --- Homing yonu (true = negatif yone dogru ara) ------------------------ */
#define X_HOME_NEGATIVE       true
#define Y_HOME_NEGATIVE       true
#define Z_HOME_NEGATIVE       false    /* TODO: Z yukari mi asagi mi homing */
#define G_HOME_NEGATIVE       true     /* tutucu: tam acik konum referans   */

/* ==========================================================================
 * 4. ADIM URETIMI
 * ========================================================================== */

/* Ana zamanlayici tik frekansi. Bresenham bu tikte calisir.
 * Tahmini guvenli tavan ~40-80 kHz (bkz. docs/08-firmware-mimarisi.md).
 * F2'de osiloskopla OLCUN, tahmine guvenmeyin. */
#define STEPPER_TICK_HZ       30000UL

/* STEP darbesi yuksek kalma suresi (us).
 * TMC2208 en az 100 ns ister; 2 us bol paydir ve jumper kablolarda guvenli. */
#define STEP_PULSE_US         2u

/* DIR -> STEP kurulum suresi (us). TMC2208 20 ns ister. */
#define DIR_SETUP_US          1u

/* ==========================================================================
 * 5. HOMING
 * ========================================================================== */

#define HOMING_SEEK_RATE      500.0f   /* mm/dk - hizli arama               */
#define HOMING_FEED_RATE      25.0f    /* mm/dk - yavas hassas yaklasma     */
#define HOMING_PULLOFF_MM     2.0f     /* anahtardan geri cekilme           */
#define HOMING_DEBOUNCE_MS    25u

/* ==========================================================================
 * 6. VAKUM VE TUTUCU
 * ========================================================================== */

#define VACUUM_SETTLE_MS      300u     /* vakum kurulmasi icin bekleme      */
#define VACUUM_RELEASE_MS     200u     /* birakma sonrasi bekleme           */

/* Kavrama dogrulama sensoru (opsiyonel - ilk surumde kapali) */
#define VACUUM_SENSOR_ENABLED     false
#define VACUUM_GRIP_THRESHOLD_KPA (-35.0f)  /* bu degerin altina inmeli     */
#define VACUUM_LOSS_THRESHOLD_KPA (-20.0f)  /* bu degerin ustune cikarsa hata*/
#define VACUUM_GRIP_TIMEOUT_MS    500u

/* ==========================================================================
 * 7. GUVENLIK
 *
 * Bunlar oneri degil, KABUL KRITERIDIR. Bkz. docs/08-firmware-mimarisi.md
 * ========================================================================== */

#define REQUIRE_HOMING_BEFORE_MOVE   true   /* homing yapilmadan hareket yok */
#define SOFT_LIMITS_ENABLED          true   /* strok degerleri girilince     */
#define HARD_LIMITS_ENABLED          true   /* hareket sirasinda limit -> dur*/
#define WATCHDOG_ENABLED             true   /* zaman asiminda EN yuksek      */
#define WATCHDOG_TIMEOUT_MS          100u

/* Acil stop birakildiginda OTOMATIK DEVAM ETME - yeniden homing iste. */
#define ESTOP_REQUIRES_REHOME        true

/* Hata durumunda vakumu BIRAKMA - kapak tutulmaya devam etmeli.
 * Fail-safe davranis budur. Bkz. docs/06-vakum-sistemi.md */
#define HOLD_VACUUM_ON_ERROR         true

/* ==========================================================================
 * 8. DERLEME ZAMANI KONTROLLERI
 * ========================================================================== */

#if (MICROSTEPS != 2) && (MICROSTEPS != 4) && (MICROSTEPS != 8) && (MICROSTEPS != 16)
#error "TMC2208 standalone modda yalnizca 1/2, 1/4, 1/8, 1/16 destekler. \
Shield'in silkscreen tablosu A4988 icindir, bu surucude gecerli degildir."
#endif

#endif /* CONFIG_H */

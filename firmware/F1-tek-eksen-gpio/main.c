/*
 * F1 — Tek eksen, GPIO ile, sabit hiz
 *
 * Amac: Ilk hareket. Zamanlayici yok, kesme yok. Ana dongude STEP pinini
 *       elle tetikliyoruz. Hiz, gecikme dongusunun uzunluguyla belirleniyor.
 *
 * Ne ogrenilir:
 *   - STEP/DIR mantigi
 *   - Mikroadim ne demek (1600 adim = 1 tam tur, 1/8'de)
 *   - TMC2208'in yon polaritesinin A4988'in TERSI oldugu
 *   - EN pininin aktif dusuk oldugu
 *
 * Kabul kriteri (bkz. docs/10-test-plani.md, T2):
 *   - Motor duzgun donuyor, tirmalama/titreme yok
 *   - Yon degistirilebiliyor
 *   - Surucu 30 s sonra elle dokunulabilir sicaklikta
 *   - EN yuksekken mil elle serbestce donuyor
 *
 * ONCE OKUYUN: docs/07-guvenlik.md
 *   - Enerji varken motor takip cikarmayin
 *   - Vref ayari motorlar SOKULUYKEN yapilir (T1)
 *
 * Hedef: XMC4200 Platform2Go (KIT_XMC_PLT2GO_XMC4200)
 * Derleme: DAVE 4.5 "Empty Main Project" (XMCLib, APP'siz)
 */

#include <xmc_gpio.h>
#include <xmc_scu.h>
#include "../ortak/config.h"

/* --------------------------------------------------------------------------
 * Bu asamada test edilecek eksen. X ile baslayin.
 * -------------------------------------------------------------------------- */
#define TEST_STEP_PORT    PIN_X_STEP_PORT
#define TEST_STEP_NUM     PIN_X_STEP_NUM
#define TEST_DIR_PORT     PIN_X_DIR_PORT
#define TEST_DIR_NUM      PIN_X_DIR_NUM
#define TEST_INVERT_DIR   X_INVERT_DIR

/* Kac adim atilacak. 1600 = 1/8 mikroadimda tam bir motor turu.
 * T3.1 kalibrasyonunda bu sayiyi kullanacaksiniz. */
#define STEPS_PER_BURST   1600u

/* Adim araligi (us). Buyuk deger = yavas. 500 us -> 2000 adim/s.
 * Motor tirmaliyorsa BUYUTUN (yavaslatin), asla kucultmeyin. */
#define STEP_INTERVAL_US  500u

/* Yon degistirmeden once bekleme (ms) */
#define PAUSE_MS          1000u


/* --------------------------------------------------------------------------
 * Kaba gecikme. F1 kasitli olarak ilkel tutulmustur - zamanlayici F2'de gelir.
 *
 * NOT: Bu gecikme derleyici optimizasyonuna ve flash bekleme durumuna bagli
 * olarak degisir, yani gercek hiz hesaplanandan farkli olacaktir. Bu normaldir
 * ve F2'nin neden gerekli oldugunun somut gostergesidir.
 * -------------------------------------------------------------------------- */
static void delay_us(uint32_t us)
{
    /* ~80 MHz'de dongu basina kabaca 3 cevrim varsayimi */
    volatile uint32_t n = us * (F_CPU_HZ / 3000000UL);
    while (n--) {
        __asm volatile ("nop");
    }
}

static void delay_ms(uint32_t ms)
{
    while (ms--) {
        delay_us(1000u);
    }
}


/* --------------------------------------------------------------------------
 * GPIO kurulumu
 * -------------------------------------------------------------------------- */
static void gpio_init(void)
{
    /* STEP ve DIR: push-pull cikis, guclu surucu + yumusak kenar.
     * "Guclu" mod DC akim degil KENAR HIZI kazandirir (garanti DC surme
     * her iki modda da ~2 mA). Jumper kablo kullaniyorsaniz kaynak ucuna
     * 33-100 ohm seri direnc koyun. Bkz. docs/03-elektrik-baglanti.md §8 */
    const XMC_GPIO_CONFIG_t out_cfg = {
        .mode         = XMC_GPIO_MODE_OUTPUT_PUSH_PULL,
        .output_level = XMC_GPIO_OUTPUT_LEVEL_LOW,
        .output_strength = XMC_GPIO_OUTPUT_STRENGTH_STRONG_SOFT_EDGE
    };

    XMC_GPIO_Init(TEST_STEP_PORT, TEST_STEP_NUM, &out_cfg);
    XMC_GPIO_Init(TEST_DIR_PORT,  TEST_DIR_NUM,  &out_cfg);
    XMC_GPIO_Init(PIN_EN_PORT,    PIN_EN_NUM,    &out_cfg);

    /* Vana pini: acilista KAPALI olsun. Fail-safe mantigi geregi
     * enerjisiz durum = tutan durum. Bkz. docs/06-vakum-sistemi.md */
    XMC_GPIO_Init(PIN_VALVE_PORT, PIN_VALVE_NUM, &out_cfg);
    XMC_GPIO_SetOutputLow(PIN_VALVE_PORT, PIN_VALVE_NUM);
}

/* EN aktif DUSUK: dusuk = suruculer etkin, yuksek = serbest */
static void drivers_enable(bool on)
{
    if (on) {
        XMC_GPIO_SetOutputLow(PIN_EN_PORT, PIN_EN_NUM);
    } else {
        XMC_GPIO_SetOutputHigh(PIN_EN_PORT, PIN_EN_NUM);
    }
}

static void set_direction(bool forward)
{
    bool level = TEST_INVERT_DIR ? !forward : forward;

    if (level) {
        XMC_GPIO_SetOutputHigh(TEST_DIR_PORT, TEST_DIR_NUM);
    } else {
        XMC_GPIO_SetOutputLow(TEST_DIR_PORT, TEST_DIR_NUM);
    }

    /* TMC2208 DIR->STEP kurulum suresi 20 ns ister; 1 us bol paydir. */
    delay_us(DIR_SETUP_US);
}

static void step_once(void)
{
    XMC_GPIO_SetOutputHigh(TEST_STEP_PORT, TEST_STEP_NUM);
    delay_us(STEP_PULSE_US);          /* TMC2208 en az 100 ns ister */
    XMC_GPIO_SetOutputLow(TEST_STEP_PORT, TEST_STEP_NUM);
}


int main(void)
{
    gpio_init();

    /* Suruculeri etkinlestir.
     *
     * !! 5 V kart varyantindaysaniz (TXS0108E seviye cevirici takili) bu
     *    muhtemelen CALISMAYACAK: shield'in EN hattindaki 10k pull-up,
     *    TXS0108E'nin 40k tutma direnciyle bolucu olusturur ve hat ~2.64 V'ta
     *    kalir - TMC2208'in gecerli dusuk esigi 0.99 V.
     *    Belirti: motor hic donmez veya hic tutmaz.
     *    Cozum: shield'in R1 direncini sokun veya 100k yapin.
     *    Bkz. docs/03-elektrik-baglanti.md bolum 2. */
    drivers_enable(true);

    /* Suruculerin ayaga kalkmasi icin kisa bekleme */
    delay_ms(100u);

    while (1) {
        /* Ileri */
        set_direction(true);
        for (uint32_t i = 0; i < STEPS_PER_BURST; i++) {
            step_once();
            delay_us(STEP_INTERVAL_US);
        }

        delay_ms(PAUSE_MS);

        /* Geri */
        set_direction(false);
        for (uint32_t i = 0; i < STEPS_PER_BURST; i++) {
            step_once();
            delay_us(STEP_INTERVAL_US);
        }

        delay_ms(PAUSE_MS);
    }

    return 0;
}


//#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "iostream"
#include "string"
#include "sstream"

// Définitions
#define GPIO_IRQ_FALLING_EDGE 0x04

// Definitions des pins
#define Pin_En 22
#define Pin_D 20
#define Pin_CP 21
#define Pin_STR 19
#define Pin_Oe 18
#define BUTTON_MODE 17
#define BUTTON_UP 18
#define BUTTON_DOWN 16
#define Pin_MISO 4
#define Pin_MOSI 3
#define Pin_SCK 2
int64_t AlarmCallback(alarm_id_t id, void *user_data)
{
    std::cout << "AlarmCallback" << std::endl;
    gpio_set_irq_enabled(BUTTON_MODE, GPIO_IRQ_FALLING_EDGE, true);
    return 0;
}

void ButtonModeCallback(uint gpio, uint32_t events)
{
    std::cout << "ButtonModeCallback" << std::endl;
    // Anti-rebond virtuelle
    gpio_set_irq_enabled(BUTTON_MODE, GPIO_IRQ_FALLING_EDGE, false);
    add_alarm_in_ms(500, AlarmCallback, NULL, false);
    gpio_put(PICO_DEFAULT_LED_PIN, !gpio_get(PICO_DEFAULT_LED_PIN)); // Led

    // On entre dans le mode de réglage de l'heure

}


void WriteDigit(int value)
{
    int i;
    int msk = 0x0200;
    for (i = 10; i > 0; i--)
    {
        gpio_put(Pin_D, !((value & msk)>>i-1)); // Output data
        gpio_put(Pin_CP, true); // coup de clock
        sleep_ms(500);
        gpio_put(Pin_CP, false);
        sleep_ms(500);

        msk = msk >> 1;
    }
}

int main()
{
    // Initialisation des pins
    stdio_init_all();
    gpio_init(Pin_En);
    gpio_set_dir(Pin_En, GPIO_OUT);

    gpio_init(Pin_D);
    gpio_set_dir(Pin_D, GPIO_OUT);

    gpio_init(Pin_CP);
    gpio_set_dir(Pin_CP, GPIO_OUT);

    gpio_init(Pin_STR);
    gpio_set_dir(Pin_STR, GPIO_OUT);

    gpio_init(Pin_Oe);
    gpio_set_dir(Pin_Oe, GPIO_OUT);

    gpio_init(PICO_DEFAULT_LED_PIN);
    gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);

    gpio_init(BUTTON_MODE);
    gpio_set_dir(BUTTON_MODE, GPIO_IN);
    gpio_pull_up(BUTTON_MODE);
    gpio_set_irq_enabled_with_callback(BUTTON_MODE, GPIO_IRQ_FALLING_EDGE, true, ButtonModeCallback);

    // Initialize SPI pins
    gpio_set_function(Pin_SCK, GPIO_FUNC_SPI);
    gpio_set_function(Pin_MOSI, GPIO_FUNC_SPI);
    gpio_set_function(Pin_MISO, GPIO_FUNC_SPI);

    // Initialisation des états initials
    gpio_put(Pin_En, true);
    gpio_put(Pin_CP, true);
    gpio_put(Pin_Oe,true);

    // Variables locales
    unsigned int data = 0x0001;
    unsigned int msk;
    char i;
    char j;
    unsigned int tabDigit[4] = {0, 0, 0, 0};

    spi_inst_t *spi = spi0;
    // Initialize SPI port at 1 MHz
    spi_init(spi, 50000);
    // Set SPI format
    spi_set_format( spi,   // SPI instance
                    8,      // Number of bits per transfer
                    SPI_CPOL_1,      // Polarity (CPOL)
                    SPI_CPHA_1,      // Phase (CPHA)
                    SPI_MSB_FIRST);


    std::string str;
    sleep_ms(2000);
    gpio_put(Pin_STR, true); //Select this device
    // Boucle infinie
    while (true)
    {
//        // Boucle sur les chiffres
//        std::cout << "Lancement de tramme" << std::endl;
//        std::cout << "data: " << data << std::endl;
//        for (j = 0; j < 4; j++)
//        {
//            msk = 0x0800;
//            for (i = 12; i > 0; i--) {
//                gpio_put(Pin_D, !((tabDigit[j] & msk) >> i - 1)); // Output data
//                std::cout << "msk: " << msk << std::endl;
//                std::cout << "data: " << !((tabDigit[j] & msk) >> i-1) << std::endl;
//                gpio_put(Pin_CP, true); // coup de clock
//                gpio_put(PICO_DEFAULT_LED_PIN, true); // Led
//                sleep_ms(10);
//                gpio_put(Pin_CP, false);
//                gpio_put(PICO_DEFAULT_LED_PIN, false); // Led
//                sleep_ms(10);
//
//                msk = msk >> 1;
//            }
//            std::cout << "fin boucle digit" << std::endl;
//        }
//        std::cout << "Fin de tramme" << std::endl;
//        gpio_put(Pin_STR, false); //Select this device
//        sleep_ms(2000);
      }
}

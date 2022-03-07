
//#include <stdio.h>
#include "pico/stdlib.h"

// Definition des pins
#define Pin_En 22
#define Pin_D 20
#define Pin_CP 21
#define Pin_STR 19
#define Pin_Oe 18

int main()
{
    // Initialisation des pins
    stdio_init_all();
    gpio_init(Pin_En);
    gpio_init(Pin_D);
    gpio_init(Pin_CP);
    gpio_init(Pin_STR);
    gpio_init(Pin_Oe);
    gpio_init(PICO_DEFAULT_LED_PIN);


    gpio_set_dir(Pin_En, GPIO_OUT);
    gpio_set_dir(Pin_D, GPIO_OUT);
    gpio_set_dir(Pin_CP, GPIO_OUT);
    gpio_set_dir(Pin_STR, GPIO_OUT);
    gpio_set_dir(Pin_Oe, GPIO_OUT);
    gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);

    gpio_put(Pin_En, true);
    gpio_put(Pin_CP, true);
    gpio_put(Pin_Oe,1);

    // Variables locales
    unsigned int data = 0x0001;
    unsigned int msk = 0x0200;
    char i;

    // Boucle infinie
    while (true)
    {
        gpio_put(Pin_STR, true); //Select this device
        i = 0;
        for (i = 9; i >= 0; i--)
        {
            gpio_put(Pin_D, !((data & msk)>>i)); // Output data
            gpio_put(PICO_DEFAULT_LED_PIN, (data & msk)>>i ); // Led

            gpio_put(Pin_CP, true); // coup de clock
            sleep_ms(500);
            gpio_put(Pin_CP, false);
            sleep_ms(500);

            msk = msk >> 1;
        }
        msk = 0x0200;
        gpio_put(Pin_STR, false); //Select this device
        sleep_ms(100);
    }
}

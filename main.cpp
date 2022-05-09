
//#include <stdio.h>
#include "pico/stdlib.h"
#include "iostream"
#include "string"
#include "sstream"

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
    gpio_put(Pin_Oe,true);

    // Variables locales
    unsigned int data = 0x0001;
    unsigned int msk = 0x0200;
    char i;
    std::string str;

    // fonction prototype
    void WriteDigit(int value);

    sleep_ms(2000);
    gpio_put(Pin_STR, true); //Select this device
    // Boucle infinie
    while (true)
    {
        // get data from the serial port
//        std:: cout << "Entrez un nombre entre 0 et 9 : ";
//        std::getline(std::cin, str);
//        // convert data to int
//        std::stringstream ss;
//        ss << str;
//        ss >> data;
//        std::cout << "Valeur saisie : " << data << std::endl;

        gpio_put(Pin_STR, true); //Select this device
        if (data > 0x0100)
        {
            data = 0x0001;
        }
        else
        {
            data = data << 1;
        }
        // Boucle sur les chiffres
        for (i = 10; i > 0; i--)
        {
            gpio_put(Pin_D, !((data & msk)>>i-1)); // Output data

            std::cout << "msk: " << msk << std::endl;
            std::cout << "data: " << ((data & msk)>>i) << std::endl;
            gpio_put(Pin_CP, true); // coup de clock
            gpio_put(PICO_DEFAULT_LED_PIN, true ); // Led
            sleep_ms(50);
            gpio_put(Pin_CP, false);
            gpio_put(PICO_DEFAULT_LED_PIN, false ); // Led
            sleep_ms(50);

            msk = msk >> 1;
        }
        msk = 0x0200;
        gpio_put(Pin_STR, false); //Select this device
        sleep_ms(1000);
    }
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
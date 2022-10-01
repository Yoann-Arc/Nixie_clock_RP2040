
//#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "iostream"
#include "string"
#include "sstream"

// Définitions
#define GPIO_IRQ_FALLING_EDGE 0x04
#define addCTRL 0x0F
#define addSec 0x00
#define addMin 0x01
#define addHrs 0x02
#define Secondes 2
#define Minutes 1
#define Heures 0
// Definitions des pins
#define Pin_En 22
#define Pin_CP 21
#define Pin_D 20
#define Pin_STR 19

#define BUTTON_UP 18
#define BUTTON_MODE 17
#define BUTTON_DOWN 16

#define Pin_CS 29
#define Pin_1Hz 28
#define Pin_MISO 4
#define Pin_MOSI 3
#define Pin_SCK 2

// Variables globales
spi_inst_t *spi = spi0;
uint8_t tabBuf[2];
uint8_t tabHeure[3];
uint8_t etatClock = 1;


int reg_read(  spi_inst_t *spi_l,
               const uint cs,
               const uint8_t reg,
               uint8_t *buf,
               const uint8_t nbytes) {

    int num_bytes_read = 0;
    uint8_t mb = 0;

    // Determine if multiple byte (MB) bit should be set
    if (nbytes < 1) {
        return -1;
    } else if (nbytes == 1) {
        mb = 0;
    } else {
        mb = 1;
    }
    // Construct message (set ~W bit high)
    uint8_t msg = ~0x80 & (mb << 6) | reg;

    // Read from register
    gpio_put(cs, 1);
    spi_write_blocking(spi_l, &msg, 1);
    num_bytes_read = spi_read_blocking(spi_l, 0, buf, nbytes);
    gpio_put(cs, 0);

    return num_bytes_read;
}
void RegWrite(spi_inst_t *spi_l, const uint cs, const uint8_t reg, const uint8_t data)
{
    uint8_t msg[2];

    // Construct message (set ~W bit low, MB bit low)
    msg[0] = 0x80 | reg;
    msg[1] = data;

    // Write to register
    gpio_put(cs, 1);
    spi_write_blocking(spi_l, msg, 2);
    gpio_put(cs, 0);
}
void AffichageTubes(uint8_t Heure[3])
{
    uint16_t i,j,buf;
    uint16_t msk;
    uint16_t tabDigit[4] = {0, 0, 0, 0};
    uint16_t tabConvert[10]=
    {
    0x0001,//0
    0x0002,//1
    0x0004,//2
    0x0008,//3
    0x0010,//4
    0x0020,//5
    0x0040,//6
    0x0080,//7
    0x0100,//8
    0x0200//9
    };
    tabDigit[0] = tabConvert[tabHeure[Minutes] & 0x0f];
    tabDigit[1] = tabConvert[tabHeure[Minutes] >> 4];
    tabDigit[2] = tabConvert[tabHeure[Heures] & 0x0f];
    tabDigit[3] = tabConvert[tabHeure[Heures] >> 4];
    gpio_put(Pin_STR, true);
    for (j=0;j<4;j++)
    {
        msk = 0x0800;
        for (i = 12; i > 0; i--)
        {
            gpio_put(Pin_D, !((tabDigit[j] & msk) >> (i - 1))); // Output data
            gpio_put(Pin_CP, true); // coup de clock
            //sleep_ms(10);
            for(buf=0;buf<64000;buf++);
            for(buf=0;buf<64000;buf++);
            for(buf=0;buf<64000;buf++);
            gpio_put(Pin_CP, false);
            for(buf=0;buf<64000;buf++);
            for(buf=0;buf<64000;buf++);
            for(buf=0;buf<64000;buf++);

            msk = msk >> 1;
        }
    }
    gpio_put(Pin_STR, false);
}
int64_t AlarmCallback(alarm_id_t id, void *user_data)
{
    std::cout << "AlarmCallback" << std::endl;
    gpio_set_irq_enabled(BUTTON_MODE, GPIO_IRQ_FALLING_EDGE, true);
    return 0;
}
void SynchroCallback(uint gpio, uint32_t events)
{
    if (gpio == Pin_1Hz)
    {
        std::cout << "Time 1Hz interrupt" << std::endl;
        reg_read(spi, Pin_CS, addSec, tabBuf, 1);
        tabHeure[Secondes] = tabBuf[0];
        std::cout << "data: " << std::to_string(tabBuf[0]) << std::endl;
        reg_read(spi, Pin_CS, addMin, tabBuf, 1);
        tabHeure[Minutes] = tabBuf[0];
        std::cout << "data: " << std::to_string(tabBuf[0]) << std::endl;
        reg_read(spi, Pin_CS, addHrs, tabBuf, 1);
        tabHeure[Heures] = tabBuf[0];
        std::cout << "data: " << std::to_string(tabBuf[0]) << "\n" << std::endl;


        if ((tabHeure[Secondes] & 0x0F) == 0x00) {
            std::cout << "10Secondes ! " << "\n" << std::endl;
            //if((tabHeure[2]>>4)==0x03 || (tabHeure[2]>>4)==0x00)
            {
                std::cout << "Heures : " << std::to_string(tabHeure[Heures] >> 4) << std::to_string(tabHeure[Heures] & 0x0F) << ":"
                          << std::to_string(tabHeure[Minutes] >> 4) << std::to_string(tabHeure[Minutes] & 0x0F) << ":"
                          << std::to_string(tabHeure[Secondes] >> 4) << std::to_string(tabHeure[Secondes] & 0x0F) << "\n" << std::endl;
                AffichageTubes(tabHeure);
            }
        }
    }
    if(gpio==BUTTON_MODE)
    {
        std::string str;
        std::cout << "BUTTON_MODE Interrupt" << std::endl;
        // Anti-rebond virtuelle
        //gpio_set_irq_enabled(BUTTON_MODE, GPIO_IRQ_FALLING_EDGE, false);
        //add_alarm_in_ms(500, AlarmCallback, NULL, false);
        gpio_put(PICO_DEFAULT_LED_PIN, !gpio_get(PICO_DEFAULT_LED_PIN)); // Led
        if(etatClock==1)
        {
            // On entre dans le mode de réglage de l'heure
            etatClock = 0;
            std::cout << "Stop clock" << std::endl;
            RegWrite(spi, Pin_CS, addCTRL, 0x00);
        }
        else
        {
            etatClock = 1;
            std::cout << "Run clock" << std::endl;
            RegWrite(spi, Pin_CS, addCTRL, 0x04);
            AffichageTubes(tabHeure);
        }
    }
    if(gpio==BUTTON_UP)
    {
        // Prototype modification hours
//        if(etatClock==1)
//        {
//            std::cout << "Button Up" << std::endl;
//            tabHeure[Minutes]++;
//            if((tabHeure[Minutes]&0x0F)>9)
//            {
//                tabHeure[Minutes] = ((tabHeure[Minutes]>>4)+1)<<4;
//            }
//            if((tabHeure[Minutes]>>4)>5)
//            {
//                tabHeure[Minutes] = 0;
//                tabHeure[Heures]++;
//            }
//            if((tabHeure[Heures]&0x0F)>9)
//            {
//                tabHeure[Heures] = ((tabHeure[Heures]>>4)+1)<<4;
//            }
//            if((tabHeure[Heures]>>4)>2)
//            {
//                tabHeure[Heures] = 0;
//            }
//            AffichageTubes(tabDigit);
//        }
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

    gpio_init(PICO_DEFAULT_LED_PIN);
    gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);

    gpio_init(BUTTON_MODE);
    gpio_set_dir(BUTTON_MODE, GPIO_IN);
    gpio_pull_up(BUTTON_MODE);
    gpio_set_irq_enabled_with_callback(BUTTON_MODE, GPIO_IRQ_FALLING_EDGE, true, SynchroCallback);

    gpio_init(BUTTON_DOWN);
    gpio_set_dir(BUTTON_DOWN, GPIO_IN);
    gpio_pull_up(BUTTON_DOWN);
    gpio_set_irq_enabled_with_callback(BUTTON_DOWN, GPIO_IRQ_FALLING_EDGE, true, SynchroCallback);

    gpio_init(BUTTON_UP);
    gpio_set_dir(BUTTON_UP, GPIO_IN);
    gpio_pull_up(BUTTON_UP);
    gpio_set_irq_enabled_with_callback(BUTTON_UP, GPIO_IRQ_FALLING_EDGE, true, SynchroCallback);


    // Initialize SPI pins
    gpio_set_function(Pin_SCK, GPIO_FUNC_SPI);
    gpio_set_function(Pin_MOSI, GPIO_FUNC_SPI);
    gpio_set_function(Pin_MISO, GPIO_FUNC_SPI);

    gpio_init(Pin_CS);
    gpio_set_dir(Pin_CS,GPIO_OUT);

    gpio_init(Pin_1Hz);
    gpio_set_dir(Pin_1Hz,GPIO_IN);
    gpio_set_irq_enabled_with_callback(Pin_1Hz,GPIO_IRQ_EDGE_RISE, true,SynchroCallback);

    // Initialisation des états initials
    gpio_put(Pin_En, true);
    gpio_put(Pin_CP, true);

    // Variables locales
    unsigned int data = 0x0001;
    unsigned int msk;
    char i;
    char j;



    // Initialize SPI port at 1 MHz
    spi_init(spi, 50000);
    // Set SPI format
    spi_set_format( spi,   // SPI instance
                    8,      // Number of bits per transfer
                    SPI_CPOL_1,      // Polarity (CPOL)
                    SPI_CPHA_1,      // Phase (CPHA)
                    SPI_MSB_FIRST);


    std::string str;

    gpio_put(Pin_STR, true); //Select this device
    RegWrite(spi,Pin_CS,addCTRL,0x04);
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

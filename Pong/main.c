/******************************************************************************************************
Pong-spelets main.c, av Carl-Anton Grandelius. Ansvarig for korning av funktioner i skarm.c, Suhaib Abdi Muhummed.
******************************************************************************************************/

#include "main.h"
#include "centrering.h"

static uint8_t flaggatimeout;
static uint16_t antaltimeouts;

int main(void)
{
    hardvaruInitiering(); // Initiering av hårdvara samt räknare och avbrott
    initieraSkarm();      // Initiering av skarmen
    enable_interrupt();
    /*********************************************************
    Start av spel
    **********************************************************/
    uint8_t startaspel = 0;
    uint8_t knappvarde = 0;
    uint8_t tidigare_knappvarde = 0;
    uint8_t lage = 1;
    while (1)
    {
        while (flaggatimeout == false)
        {
        }

        if (startaspel == 1)
        {
            hanteraSpelSlinga(&lage);
        }
        else
        {
            spelEjStartat();
            knappvarde = (arKnappIntryck(0) | arKnappIntryck(1) | arKnappIntryck(2) | arKnappIntryck(3));
            if (tidigare_knappvarde == 0 && knappvarde != 0)
            {
                startaspel = 1;
                tidigare_knappvarde = knappvarde;
            }
        }
        flaggatimeout = 0;
    }
}
// tandLampor, av Carl-Anton Grandelius
void tandLampor(uint8_t aktiverade)
{
    PORTE = PORTE ^ (uint8_t)aktiverade; // XOR operationen kors med de ursprungliga vardet.
    return;
}

// Funktionen tar emot en unsigned 8-bit int, knapp och returnerar om den trycks in (1) eller ej (0)
uint8_t arKnappIntryck(uint8_t knapp)
{
    return (((PORTD >> 4) & 0x0E) | ((PORTF & 0x02) >> 1)) >> knapp & 1;
}

// Likadan user_Isr-funktion (user_isr) som från tidigare labbfiler, Carl-Anton Grandelius och Suhaib Abdi Muhummed.
void userIsr(void)
{
    if (IFS(0) & 0x100)
    {
        antaltimeouts = antaltimeouts + 1;
        if (antaltimeouts == 3333)
        {
            flaggatimeout = 1; // För varje flaggatimeout passerar 3333*10 mikrosekunder = 33.33 millisekunder.
            antaltimeouts = 0;
        }
        IFSCLR(0) = 0x100; // rensa avbrottsflaggan
    }
    return;
}

// hardvaruInitiering, av Carl-Anton Grandelius
void hardvaruInitiering()
{

    /*********************************************************
    Initiering av hardvara
    **********************************************************/

    OSCCON &= ~(1U << 20); // oscillator sources which determine the system clock frequency that synchronizes the operation of the different components of the system
    OSCCON |= (1U << 19);
    TRISE &= 0xFF00U; // Tri-State registers used to set the direction of ports (input or output) on the microcontroller.
    TRISD = (TRISD & 0xFF1FU) | 0x01E0U;
    TRISF |= 1U;
    SPI2BRG = 1U;  // SPI (Serial Peripheral Interface) Configuration  whiich is a common protocol for communicating with peripherals.
    SPI2CON = (1U << 15) | (1U << 6) | (1U << 5);
    SPI2STAT &= ~0x40U;

    PORTF = 0xFFFFU;
    PORTGSET = 1U << 9;
    TRISF &= ~0x70U;
    ODCF = ODCG = 0U;
    TRISG &= ~0x200U;
    PORTE = 0;

    /*********************************************************
    Initiering av raknaren. Timerkoden i denna fil styr spelets hastighet/takt.
    **********************************************************/

    T2CON = 0x0;
    T2CONSET = 1u << 6;
    TMR2 = 0x0;
    PR2 = ((80000000 / 16) * 0.00001) - 1; //10 macro seconds
    T2CONSET = 0x8000;

    // Klockfrekvensen (80 MHz delat med 16), tidsperiod (0.00001 sekunder, eller 10 mikrosekunder).
    // Alltsa, varje timerintervall är 10 mikrosekunder.

    /*********************************************************
    Initiering av Avbrott. Avbrottskoden i denna fil hanterar timeouts.
    **********************************************************/

    IPC(2) = 4; // Interrupt registers configuration to set up interrupts to handle events like button presses or timeouts.
    IECSET(0) = 0x100; //enable Interrupt Enable Control (IEC) register specific register 
    IFSCLR(0) = 0x100;

    /*
    Efter att ISR exekveras bor man rensa avbrottsflaggan i IFS-registret for att forhindra att mikrokontrollern
    exekverar ISR igen for samma avbrottshandelse. Man aktiverar avbrottet innan flaggan rensas for att vara saker pa att
    mikrokontrollern svarar pa avbrottshandelsen och att ISR endast exekveras en gang.
    */

    /*********************************************************
    Initiering av ADC-utrustningen
    **********************************************************/

    TRISE >>= 8; // De atta forsta pinnarna pa Port E stalls in som utgangar da deras deras motsvarande bitar i TRISE-registret skiftas bort.

    TRISE = (TRISE & 0xFFFFFF00); // Satt LEDs som utgangar
}

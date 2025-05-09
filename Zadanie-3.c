// PIC24FJ128GA010 Configuration Bit Settings

// 'C' source line config statements

// CONFIG2
#pragma config POSCMOD = XT             // Primary Oscillator Select (XT Oscillator mode selected)
#pragma config OSCIOFNC = ON            // Primary Oscillator Output Function (OSC2/CLKO/RC15 functions as port I/O (RC15))
#pragma config FCKSM = CSDCMD           // Clock Switching and Monitor (Clock switching and Fail-Safe Clock Monitor are disabled)
#pragma config FNOSC = PRI              // Oscillator Select (Primary Oscillator (XT, HS, EC))
#pragma config IESO = ON                // Internal External Switch Over Mode (IESO mode (Two-Speed Start-up) enabled)

// CONFIG1
#pragma config WDTPS = PS32768          // Watchdog Timer Postscaler (1:32,768)
#pragma config FWPSA = PR128            // WDT Prescaler (Prescaler ratio of 1:128)
#pragma config WINDIS = ON              // Watchdog Timer Window (Standard Watchdog Timer enabled,(Windowed-mode is disabled))
#pragma config FWDTEN = ON              // Watchdog Timer Enable (Watchdog Timer is enabled)
#pragma config ICS = PGx2               // Comm Channel Select (Emulator/debugger uses EMUC2/EMUD2)
#pragma config GWRP = OFF               // General Code Segment Write Protect (Writes to program memory are allowed)
#pragma config GCP = OFF                // General Code Segment Code Protect (Code protection is disabled)
#pragma config JTAGEN = OFF             // JTAG Port Enable (JTAG port is disabled)

#include <xc.h>
#include "libpic30.h"
#include "adc.h"

#define PRZEKROCZENIE_ADC 512
#define CZAS_MRUGANIA 10 // 10 * 500ms = 5s

void init(void) {
    ADC_SetConfiguration(ADC_CONFIGURATION_DEFAULT);
    ADC_ChannelEnable(ADC_CHANNEL_POTENTIOMETER);

    TRISA = 0x0000;        // PORTA jako wyjścia (LEDy)
    TRISB |= (1 << 3);     // RB3 jako wejście (przycisk)
}

int main(void) {
    init();

    unsigned int adc;
    char alarm = 0;
    char mruganie = 0;
    char licznik_mrugania = 0;
    char przyciskRB3_poprzedni = 1;

    while (1) {
        adc = ADC_Read10bit(ADC_CHANNEL_POTENTIOMETER);
        if (adc == 0xFFFF) continue;

        unsigned char przyciskRB3 = PORTBbits.RB3;

        // Ręczne wyłączenie alarmu
        if (alarm && przyciskRB3_poprzedni == 1 && przyciskRB3 == 0) {
            alarm = 0;
            mruganie = 0;
            licznik_mrugania = 0;
            LATA = 0x00;
        }
        przyciskRB3_poprzedni = przyciskRB3;

        if (!alarm && adc >= PRZEKROCZENIE_ADC) {
            // Start alarmu
            alarm = 1;
            mruganie = 1;
            licznik_mrugania = 0;
        }

        if (alarm) {
            if (adc < PRZEKROCZENIE_ADC) {
                // Automatyczne wyłączenie
                alarm = 0;
                mruganie = 0;
                licznik_mrugania = 0;
                LATA = 0x00;
            } else {
                if (mruganie) {
                    LATA = (licznik_mrugania % 2) ? 0x01 : 0x00; // Mrugaj pierwszą diodą
                    licznik_mrugania++;
                    if (licznik_mrugania >= CZAS_MRUGANIA) {
                        mruganie = 0;
                        LATA = 0xFF; // Wszystkie diody zapalone
                    }
                } else {
                    LATA = 0xFF;
                }
            }
        }

        __delay32(2000000); // ~500ms przy 8MHz Fcy
    }

    return 0;
}

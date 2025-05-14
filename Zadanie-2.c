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

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

#include <xc.h>
#include "libpic30.h"
#include "adc.h"
#include "buttons.h"

void init(void){
    ADC_SetConfiguration(ADC_CONFIGURATION_DEFAULT);
    ADC_ChannelEnable(ADC_CHANNEL_POTENTIOMETER);
    TRISA = 0x0000;
    TRISD |= (1 << 6);
}

int main(void) {
    init();
    
    char current6 = 0, prev6 = 0;
    char current13 = 0, prev13 = 0;

    int value = 1; // tryb dzialania
    
    unsigned char portValue = 0b00000001;
    //unsigned int bit;
    
    unsigned char bit = 1;
    unsigned char mask = 0;
    unsigned char i = 2;
    unsigned char n = 2;
    
    unsigned long adc_value;
    unsigned long opoznienie;
    
    while (1) {
        adc_value = ADC_Read10bit(ADC_CHANNEL_POTENTIOMETER);
        if (adc_value == 0xFFFF){
            continue;
        }
        opoznienie = 40000 + (adc_value * 4000);
        
        switch (value) {
            case 1: // 8 bitowy licznik binarny zliczajacy w gore (0 255)
                LATA = portValue++;
                if (portValue > 255) portValue = 0;
                break;

            case 2: // Kolejka
                LATA = mask | bit;
                if (mask == 254) { // Reset gdy wszystkie diody s? zapalone
                    bit = 1;
                    mask = 0;
                    n = 2;
                    i = 2;
                } else if (bit < mask || mask == 0) {
                    bit <<= 1;
                }

                if ((bit * i) == mask) {
                    mask += bit;
                    bit = 0;
                    i = i + (1 << (n));  // i = i + 2^n (np. 2 -> 6 -> 14 -> 30...)
                    n++;  // Zwi?kszamy n dla obliczania kolejnej pot?gi liczby 2
                } else if (bit == 0){
                    bit += 1;
                }

                if (bit == 128) {
                    mask = bit;
                    bit = 1;
                    n = 2;  // Resetowanie n do 2, aby kontynuowa? dodawanie 2^n
                }
                break;
        }

        // Przycisk RD6
        prev6 = current6;
        current6 = PORTDbits.RD6;
        if (prev6 == 1 && current6 == 0) {
            value++;
            if (value > 2) value = 1;
            portValue = 0b00000001;
        }

        // Przycisk RD13
        prev13 = current13;
        current13 = PORTDbits.RD13;
        if (prev13 == 1 && current13 == 0) {
            value--;
            if (value < 1) value = 2;
            portValue = 0b00000001;
        }

        __delay32(opoznienie);
    }

    return 0;
}

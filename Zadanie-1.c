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

int main(void) {
    TRISA = 0x0000;   // port a jako wyjscie
    TRISD = 0xFFFF;   // port d jako wejscie

    char current6 = 0, prev6 = 0;
    char current13 = 0, prev13 = 0;

    int value = 1; // tryb dzialania
    
    unsigned char portValue = 0b00000001;
    unsigned char portValueSnake = 0b00000111;
    unsigned char lfsr = 0b1110011;
    int goingRight = 1;
    //unsigned int bit;
    
    unsigned char bit = 1;
    unsigned char mask = 0;
    unsigned char i = 2;
    unsigned char n = 2;
    
    while (1) {
        switch (value) {
            case 1: // 8 bitowy licznik binarny zliczajacy w gore (0 255)
                LATA = portValue++;
                if (portValue > 255) portValue = 0;
                break;

            case 2: // 8 bitowy licznik binarny zliczajacy w dol (255 0)
                LATA = portValue--;
                if (portValue == 0xFF) portValue = 255;
                break;

            case 3: // 8 bitowy licznik w kodzie Graya zliczajacy w gore (repr. 0 255)
                LATA = portValue ^ (portValue >> 1);
                portValue++;
                if (portValue > 255) portValue = 0;
                break;

            case 4: // 8 bitowy licznik w kodzie Graya zliczajacy w dol (repr. 255 0)
                LATA = portValue ^ (portValue >> 1);
                portValue--;
                if (portValue == 255) portValue = 255;
                break;

            case 5: // 2x4 bitowy licznik w kodzie BCD zliczajacy w gore (0 99)
                LATA = ((portValue / 10) << 4) | (portValue % 10);
                portValue++;
                if (portValue > 99) portValue = 0;
                break;

            case 6: // 2x4 bitowy licznik w kodzie BCD zliczajacy w dol (99 0)
                LATA = ((portValue / 10) << 4) | (portValue % 10);
                portValue--;
                if (portValue == 255 || portValue > 99) portValue = 99;
                break;

            case 7: // 3 bitowy wezyk poruszajacy sie lewo-prawo
                LATA = portValueSnake;
                if (goingRight) {
                    if (portValueSnake == 0b11100000) goingRight = 0;
                    else portValueSnake <<= 1;
                } else {
                    if (portValueSnake == 0b00000111) goingRight = 1;
                    else portValueSnake >>= 1;
                }
                break;
                
            case 8: // Kolejka
                LATA = mask | bit;
                if (mask == 254) { // Reset gdy wszystkie diody są zapalone
                    bit = 1;
                    mask = 0;
                    n = 2;
                    i = 2;
                } else if (bit < mask || mask == 0) {
                    bit <<= 1;
                }

                if ((bit * i) == mask) {
                    mask += bit;
                    bit = 1;
                    i = i + (1 << (n));  // i = i + 2^n (np. 2 -> 6 -> 14 -> 30...)
                    n++;  // Zwiększamy n dla obliczania kolejnej potęgi liczby 2
                }

                if (bit == 128) {
                    mask = bit;
                    bit = 1;
                    n = 2;  // Resetowanie n do 2, aby kontynuować dodawanie 2^n
                }
                break;

            case 9: // 6 bitowy generator liczb pseudolosowych oparty o konfiguracje 1110011
                bit = ((lfsr >> 0) ^ (lfsr >> 1) ^ (lfsr >> 4) ^ (lfsr >> 5)) & 1;
                lfsr = (lfsr >> 1) | (bit << 5);
                LATA = lfsr & 0x3F;
                break;
        }

        // Przycisk RD6
        prev6 = current6;
        current6 = PORTDbits.RD6;
        if (prev6 == 1 && current6 == 0) {
            value++;
            if (value > 9) value = 1;
            portValue = 0b00000001;
            lfsr = 0b1110011;
        }

        // Przycisk RD13
        prev13 = current13;
        current13 = PORTDbits.RD13;
        if (prev13 == 1 && current13 == 0) {
            value--;
            if (value < 1) value = 9;
            portValue = 0b00000001;
            lfsr = 0b1110011;
        }

        __delay32(1500000);
    }

    return 0;
}

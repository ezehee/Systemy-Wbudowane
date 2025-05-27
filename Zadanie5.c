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
#include "lcd.h"
#include "buttons.h"
#include "adc.h"
#include "libpic30.h"
#include <stdio.h>

void system_init(void){
    LCD_Initialize();
    
    BUTTON_Enable(BUTTON_S3);
    BUTTON_Enable(BUTTON_S4);
    BUTTON_Enable(BUTTON_S5);
    BUTTON_Enable(BUTTON_S6);
}

void flashLEDs(){
    AD1PCFG = 0xFFFF;
    TRISA = 0x0000;
    LATA = 0xFFFF;
}

void init(void) {
    ADC_SetConfiguration(ADC_CONFIGURATION_DEFAULT);
    ADC_ChannelEnable(ADC_CHANNEL_POTENTIOMETER);

    TRISA = 0x0000;
    TRISB |= (1 << 3);
}

unsigned int getInitialTime() {
    int adc = readADC();
    if (adc < 341) return 60;     // ~1 minuta
    else if (adc < 682) return 180; // ~3 minuty
    else return 300;             // ~5 minut
}

void displayTimes(int time1, int time2) {
    char buf[17];
    LCD_ClearScreen();
    sprintf(buf, "P1: %02d:%02d", time1 / 60, time1 % 60);
    LCD_PutString(buf, strlen(buf));
    LCD_PutChar('\n');
    sprintf(buf, "P2: %02d:%02d", time2 / 60, time2 % 60);
    LCD_PutString(buf, strlen(buf));
}

void displayLoser(int player) {
    LCD_ClearScreen();
    if (player == 1) LCD_PutString("P1 przegral czas", 17);
    else LCD_PutString("P2 przegral czas", 17);
    while (1);
}

int main(void) {
    LCD_Initialize();
    BUTTON_Enable(BUTTON_S3);
    BUTTON_Enable(BUTTON_S5);
    initADC();

    int timeP1 = getInitialTime();
    int timeP2 = timeP1;
    int currentPlayer = 0; // 0 = brak, 1 = P1, 2 = P2

    displayTimes(timeP1, timeP2);

    __delay32(3000000); // 1s na zapoznanie si? z ekranem startowym

    while (1) {
        if (BUTTON_IsPressed(BUTTON_S3)) {
            if (currentPlayer != 1) {
                currentPlayer = 2;
                __delay32(300000);
                while (BUTTON_IsPressed(BUTTON_S3));
            }
        }

        if (BUTTON_IsPressed(BUTTON_S5)) {
            if (currentPlayer != 2) {
                currentPlayer = 1;
                __delay32(300000);
                while (BUTTON_IsPressed(BUTTON_S5));
            }
        }

        if (currentPlayer == 1 && timeP1 > 0) {
            __delay32(3000000); // 1s
            timeP1--;
            displayTimes(timeP1, timeP2);
            if (timeP1 == 0) displayLoser(1);
        }

        if (currentPlayer == 2 && timeP2 > 0) {
            __delay32(3000000); // 1s
            timeP2--;
            displayTimes(timeP1, timeP2);
            if (timeP2 == 0) displayLoser(2);
        }
    }

    return 0;
}

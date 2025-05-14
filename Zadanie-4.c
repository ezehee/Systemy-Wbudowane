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
#include "buttons.h"
#include "lcd.h"
#include "string.h"
#include "libpic30.h"

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

void displayText(void){
    const char* moc[] = {"800W", "600W", "350W", "200W"};
    int mocIndex = 0;
    int czas = 0;
    int ss = 0;
    
    LCD_ClearScreen();
    LCD_PutString("Moc: ", 5);
    LCD_PutChar('\n');
    LCD_PutString(moc[mocIndex],  strlen(moc[mocIndex]));
    LCD_PutChar('\n');
    LCD_PutString("Czas: 0:00 ", 10);
    
    __delay32(1000000);
    
    while(1){
        if (BUTTON_IsPressed(BUTTON_S6)){
            mocIndex = (mocIndex+1)%4;
            LCD_ClearScreen();
            LCD_PutString("Moc: ", 5);
            LCD_PutString(moc[mocIndex],  strlen(moc[mocIndex]));
            LCD_PutChar('\n');
            LCD_PutString("Czas: ", 6);
            int min = czas/60;
            int sek = czas%60;
            LCD_PutChar('0' + min);
            LCD_PutChar(':');
            LCD_PutChar('0' + sek/10);
            LCD_PutChar('0' + sek%10);
            
            __delay32(400000);
            while(BUTTON_IsPressed(BUTTON_S6));
        }
        if (BUTTON_IsPressed(BUTTON_S5)){
            czas += 60;
            LCD_ClearScreen();
            LCD_PutString("Moc: ", 5);
            LCD_PutString(moc[mocIndex],  strlen(moc[mocIndex]));
            LCD_PutChar('\n');
            LCD_PutString("Czas: ", 6);
            int min = czas/60;
            int sek = czas%60;
            LCD_PutChar('0' + min);
            LCD_PutChar(':');
            LCD_PutChar('0' + sek/10);
            LCD_PutChar('0' + sek%10);
            
            __delay32(400000);
            while(BUTTON_IsPressed(BUTTON_S5));
        }
        if (BUTTON_IsPressed(BUTTON_S4)){
            czas += 10;
            LCD_ClearScreen();
            LCD_PutString("Moc: ", 5);
            LCD_PutString(moc[mocIndex],  strlen(moc[mocIndex]));
            LCD_PutChar('\n');
            LCD_PutString("Czas: ", 6);
            int min = czas/60;
            int sek = czas%60;
            LCD_PutChar('0' + min);
            LCD_PutChar(':');
            LCD_PutChar('0' + sek/10);
            LCD_PutChar('0' + sek%10);
            
            __delay32(400000);
            while(BUTTON_IsPressed(BUTTON_S4));
        }
        if (BUTTON_IsPressed(BUTTON_S3)){
            ss = !ss;
            __delay32(1000000);
            while(BUTTON_IsPressed(BUTTON_S3));
        }
        if (ss && czas > 0){
            __delay32(3000000);
            czas--;
            LCD_ClearScreen();
            LCD_PutString("Moc: ", 5);
            LCD_PutString(moc[mocIndex],  strlen(moc[mocIndex]));
            LCD_PutChar('\n');
            LCD_PutString("Czas: ", 6);
            int min = czas/60;
            int sek = czas%60;
            LCD_PutChar('0' + min);
            LCD_PutChar(':');
            LCD_PutChar('0' + sek/10);
            LCD_PutChar('0' + sek%10);
            
            if (czas == 0){
                LCD_ClearScreen();
                LCD_PutString("Koniec!", 7);
                flashLEDs();
                __delay32(3000000);
                ss = 0;
            }
        }
    }
}


int main(void) {
    system_init();
    displayText();
    while(1){
        
    }
    return 0;
}
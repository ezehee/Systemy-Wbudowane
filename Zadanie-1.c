// Kod zapalącący lub gaszący wszystkie diody po kolei w zależności od trybu działania

#include <xc.h>
#include <libpic30.h>

int main(void) {

    unsigned portValue = 0x0001; // Zapalająca się dioda

    char  current6 = 0, prev6 = 0; // Stan aktualny i poprzedni

    TRISA = 0x0000; // Ustawienie portu A jako wyjście
    TRISD = 0xFFFF; // Ustawienie portu D jako wyjście

    int value = 1; // Tryb działania

    while(1){
        switch (value) {
        case 1:
            LATA = portValue; // Ustawienie portu A na wartość portValue
            portValue ++; // Zwiększenie wartości portValue o 1
            if(portValue == 0xFF){
                portValue = 0;
            }
            break;
        case 2:
            LATA = portValue; // Ustawienie portu A na wartość portValue
            portValue --; // Zmniejszenie wartości portValue o 1
            if(portValue == 0xFF){
                portValue = 0;
            }
            break;
        case 3:
            LATA = portValue ^ (portValue >> 1);
            portValue ++; 
            if(portValue == 0xFF){
                portValue = 0;
            }
            break;
        case 4:
            LATA = portValue ^ (portValue >> 1);
            portValue --; 
            if(portValue == 0xFF){
                portValue = 0;
            }
            break;
        case 5:
            LATA = ((portValue / 10) << 4) | (portValue % 10);
            portValue ++;
            if(portValue == 0xFF){
                portValue = 0;
            }
        case 6:
            LATA = ((portValue / 10) << 4) | (portValue % 10);
            portValue --;
            if(portValue == 0xFF){
                portValue = 0;
            }
        }
        prev6 =  PORTDbits.RD6;
        __delay32(150000); // Opóźnienie około 0.15 sekundy
        current6 = PORTDbits.RD6;
        if(prev6 == 1 && current6 == 0){
            value++;
            if(value > 2){
                value = 1;
            }
        portValue = 1;
        }
    }
    return -1;
}
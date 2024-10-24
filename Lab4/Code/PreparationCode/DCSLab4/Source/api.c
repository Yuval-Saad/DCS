#include  "../header/api.h"    		// private library - API layer
#include  "../header/halGPIO.h"     // private library - HAL layer

char c;
unsigned int x = 500;
char cleared;
char toTransmit;
char IloveMyNegev[15] = "I love my Negev";
char index;



extern void getX(void){
    int num;
    x = 0;
    do{
        enterLPM(lpm_mode);
        num = c-'0';
        if (num <= 9 && num >= 0){
            mul10(&x);
            x += num;
        }
    }while(c != '\n');
    state = state0;
}

void mul10(unsigned int* x){
    int tmp = *x << 1;
    *x = *x << 3;
    *x += tmp;
}

void buzzGen(void){
    int buzzFreq[7] = {1048 , 838 , 699 , 599 , 524 , 466 , 419};
    enableTimer1();
    unsigned int i = 0;
    while (state == state3){
        configTimer1(buzzFreq[i]);
        wait(x , state3);
        i = (i+1)%7;
    }
    disableTimer1();
}

unsigned int convert(int num){
    unsigned int tmp1 = num<<1;
    unsigned int tmp2 = num<<7;
    return tmp1 + tmp2 + num;
}

void dispalyPoten(void){
    unsigned int Nadc = 0;
    char toPrintold[6] = { 0 , 0 , 0 , 0 , 0 , 0 };
    char toPrintnew[6];
    cleared = 0;
    lcd_clear();
    while(state == state5){
        EnableADC();
        enterLPM(lpm_mode);
        Nadc = GetNadc();
        Nadc = mulBy3222(Nadc); //multiply by 0.0503 getting a number between 0 to 3300
        convetIntToStr(toPrintnew , Nadc);
        printIfneeded(toPrintnew , toPrintold);
    }
    if (cleared) lcd_clear();
}

void printIfneeded(char* toPrint , char* current){
    unsigned int i;
    lcd_goto(0);
    for (i=0; i<5; i++){
        if (toPrint[i] != current[i])
            lcd_putchar(toPrint[i]);
        else
            lcd_cursor_right();
    }
}


unsigned int mulBy3222(unsigned int num){
    int tmp1 = num>>3;
    int tmp2 = tmp1>>1;
    int tmp3 = tmp2>>1;
    int tmp4 = tmp3>>3;
    int tmp5 = tmp4>>1;
    int tmp6 = tmp5>>1;
    int tmp7 = num<<1;
    return tmp1 + tmp2 + tmp3 + tmp4 + tmp5 + tmp6 + tmp7 + num;
}

void convetIntToStr(char* str , unsigned int Nadc){
    unsigned int num = Nadc;
    str[5] = '\0';
    str[4] = (num%10) + '0';
    num = num/10;
    str[3] = (num%10) + '0';
    num = num/10;
    str[2] = (num%10) + '0';
    num = num/10;
    str[1] = '.';
    str[0] = (num%10) + '0';
}

void countUp(void){
    char toPrint[6];
    cleared = 0;
    lcd_clear();
    lcd_home();
    lcd_puts("Counter:");
    while(state == state2){
        lcd_goto(8);
        printCounter(toPrint , counter);
        lcd_puts(toPrint);
        wait(x , state2);
        counter++;
        if (counter & 0x8000)
            counter = 0;
    }
    if (cleared) lcd_clear();
}

void printCounter(char* str , int num){
    int n = num;
    str[5] = '\0';
    str[4] = (n%10) + '0';
    n = n/10;
    if (n == 0)
        str[3] = ' ';
    else{
        str[3] = (n%10) + '0';
        n = n/10;
    }
    if (n == 0)
        str[2] = ' ';
    else{
        str[2] = (n%10) + '0';
        n = n/10;
    }
    if (n == 0)
        str[1] = ' ';
    else{
        str[1] = (n%10) + '0';
        n = n/10;
    }
    if (n == 0)
        str[0] = ' ';
    else{
        str[0] = (n%10) + '0';
        n = n/10;
    }
}

void PlayRGB(void){
    char RGBval = 1;
    while(state == state1){
        updateRGB(RGBval);
        wait(x , state1);
        RGBval = (RGBval+1)%8;
    }
    updateRGB(0);
}

void wait(int milis , enum FSMstate s){
    unsigned int updatedMilis = milis;
    while (updatedMilis>500 && s == state){
        configTimer0(0xFFFF);
        startTimer0();
        enterLPM(lpm_mode);
        updatedMilis -= 500;
    }
    if (updatedMilis != 0 && s == state){
        configTimer0(convert(updatedMilis));
        startTimer0();
        enterLPM(lpm_mode);
    }
    disableTimer0();
}

void PBstr(){
    index = 0;
    enablePB1();
    enterLPM(lpm_mode);
    UARTIntVector |= UARTTransIE;
    enterLPM(lpm_mode);
    disablePB1();
    state = state0;
}


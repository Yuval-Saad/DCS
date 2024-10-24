#include  "../header/api.h"    		// private library - API layer
#include  "../header/halGPIO.h"     // private library - HAL layer

char pressed;
char timerEnded;
char resetState1;


static char data_matrix[10][40] = {
    "An apple a day keeps the doctor away ",
    "climb on the bandwagon ",
    "Dot the i's and cross the t's ",
    "He who pays the piper calls the tune ",
    "The pen is mightier than the sword ",
    "The pot calling the kettle black ",
    "shed crocodile tears ",
    "Close but no cigar ",
    "Cut from the same cloth ",
    "Strike while the iron�s hot "
};
static char data_matrix_word_sizes[10][8] = {
     {3 , 6 , 2 , 4 , 6 , 4 , 7 , 5} ,
     {6 , 3 , 4 , 10 , 0 , 0 , 0 , 0} ,
     {4 , 4 , 4 , 4 , 6 , 4 , 4 , 0} ,
     {3 , 4 , 5 , 4 , 6 , 6 , 4 , 5} ,
     {4 , 4 , 3 , 9 , 5 , 4 , 6 , 0} ,
     {4 , 4 , 8 , 4 , 7 , 6 , 0 , 0} ,
     {5 , 10 , 6, 0 , 0 , 0 , 0 , 0} ,
     {6 , 4 , 3 , 6 , 0 , 0 , 0 , 0} ,
     {4 , 5 , 4 , 5 , 6 , 0 , 0 , 0} ,
     {7 , 6 , 4 , 7 , 4 , 0 , 0 , 0}
};

void PrepareLCDState1(void){
    lcd_clear();
    lcd_home();
    lcd_puts("Insert your word");
}

void WriteWord(void){
    char howMuchCharsToKey [17] = {0,1,1,1,1,1,1,1,1,1,1,1,1,2,2,2,2};
    char KeyToChars [17][3] = {{0,0,0} , "1G" , "2H" , "3I" , "CJ" , "4K" , "5L" , "6M" , "DN" , "7O" , "8P" , "9Q" , "ER" , "AST" , "0UV" , "BWX" , "FYZ"};
    char lastChosen = 0;
    char currIndex = 0;
    char numOfletters = 0;
    char line = 0;
    timerEnded = 0;
    pressed = 0;
    resetState1 = 0;
    EnableInterrupsFromTimer();
    enableKeypad();
    enterLPM(lpm_mode);
    lcd_clear();
    lcd_home();
    while (state == state1 && numOfletters<32){
        if (resetState1){
            break;
        }
        if (numOfletters == 16 && line == 0){
            line = 1;
            lcd_home();
            lcd_new_line;
        }
        if (pressed != 0){
            if (lastChosen == 0){
                startTimerFor1sec();
                lastChosen = pressed;
                currIndex = 0;
                numOfletters++;
                lcd_putchar(KeyToChars[pressed][currIndex]);
                timerEnded = 0;
            }
            else if(lastChosen != pressed){
                rstTimerB();
                recorder_idiom[numOfletters-1] = KeyToChars[lastChosen][currIndex];
                currIndex = 0;
                numOfletters++;
                lastChosen = pressed;
                lcd_putchar(KeyToChars[pressed][currIndex]);
                timerEnded = 0;

            }
            else if (lastChosen == pressed && (howMuchCharsToKey[pressed] == currIndex + 1)){
                disableTimerB();
                timerEnded = 0;
                currIndex++;
                lastChosen = 0;
                lcd_cursor_left();
                lcd_putchar(KeyToChars[pressed][currIndex]);
                recorder_idiom[numOfletters-1] = KeyToChars[pressed][currIndex];
            }
            else{ // (lastChosen == pressed && howMuchCharsToKey[pressed] > currIndex + 1)
                rstTimerB();
                currIndex++;
                lcd_cursor_left();
                lcd_putchar(KeyToChars[pressed][currIndex]);
                timerEnded = 0;
            }
        }
        enterLPM(lpm_mode);
        if (timerEnded){
            disableTimerB();
            recorder_idiom[numOfletters-1] = KeyToChars[pressed][currIndex];
            lastChosen = 0;
            enterLPM(lpm_mode);
        }
    }
    DisableInterrupsFromTimer();
    disableKeypad();
    if (state == state1 && !resetState1){
        state = state0;
    }
}

char parse (char num){
    if (num == 14)
        return 0;
    if ((num == 1) || (num == 2) || (num == 3))
        return num;
    if ((num == 5) || (num == 6) || (num == 7))
        return num - 1 ;
    else return num-2;
}

void PromptTwoNums(char* firstNum , char* secondNum){
    lcd_home();
    lcd_new_line;
    pressed = 0;
    enableKeypad();
    do {
        enterLPM(lpm_mode);
    }
    while ((pressed>12 && pressed != 14) || (pressed%4)==0);
    *firstNum = parse(pressed);
    lcd_putchar((*firstNum) + '0');
    lcd_cursor_right();
    do {
        enterLPM(lpm_mode);
    }
    while ((pressed>12 && pressed != 14) || (pressed%4)==0);
    disableKeypad();
    *secondNum = parse(pressed);
    lcd_putchar((*secondNum) + '0');
    return;
}


void PrepareLCDState2(void){
    lcd_clear();
    lcd_home();
    lcd_puts("Insert 2 nums:");
}

char DMAMerge(char* str, char firstNum, char secondNum){
    int dstOff = 0;
    int offFirst = 0;
    int offSecond = 0;
    char i;
    for (i = 0; i<8; i++){
        if (data_matrix_word_sizes[firstNum][i]>0){
            ConfigDMA0ForStart(str + dstOff , data_matrix[firstNum] + offFirst , data_matrix_word_sizes[firstNum][i] , 0 , 0); //00for counting increment
            dstOff += data_matrix_word_sizes[firstNum][i];
            offFirst += data_matrix_word_sizes[firstNum][i];
            trigerDMA0();
        }
        if (data_matrix_word_sizes[secondNum][i]>0){
            ConfigDMA0ForStart(str + dstOff , data_matrix[secondNum] + offSecond , data_matrix_word_sizes[secondNum][i] , 0 , 0);//00for counting increment
            dstOff += data_matrix_word_sizes[secondNum][i];
            offSecond += data_matrix_word_sizes[secondNum][i];
            trigerDMA0();
        }
    }
    return dstOff;
}

void printToLDCScrolling(char* strMerge, char sumOfChars){ //toScroll press '*'
    char numToWrite = 16;
    const char* whiteSpaces = "                ";
    int i;
    pressed = 0;
    enableKeypad();
    do {
        enterLPM(lpm_mode);
    }
    while (pressed != 13);
    lcd_clear();
    lcd_home();
    lcd_writeStrLimit(strMerge , numToWrite);
    lcd_home();
    lcd_new_line;
    lcd_writeStrLimit(strMerge+16 , numToWrite);
    sumOfChars -= (numToWrite<<1);
    for (i = 16; i<64 ;i=i+16){
        do {
            enterLPM(lpm_mode);
        }
        while (pressed != 13);
        lcd_home();
        lcd_writeStrLimit(strMerge+i, numToWrite);
        lcd_home();
        lcd_new_line;
        if (sumOfChars < numToWrite){
            lcd_writeStrLimit(strMerge+i+16 , sumOfChars);
            lcd_writeStrLimit(whiteSpaces , numToWrite-sumOfChars);
            break;
        }
        else{
            lcd_writeStrLimit(strMerge+i+16 , numToWrite);
            sumOfChars -= numToWrite;
        }
    }
    do {
        enterLPM(lpm_mode);
    }
    while (pressed != 13);
    disableKeypad();
}


void Merge(void){
    char firstNum;
    char secondNum;
    char strMerge[80];
    char sumOfChars;
    disablePBsInterrupts();
    PrepareLCDState2();
    PromptTwoNums(&firstNum , &secondNum);
    sumOfChars = DMAMerge(strMerge,firstNum,secondNum);
    printToLDCScrolling(strMerge , sumOfChars);
    enablePBsInterrupts();
    state = state0;
}


void LEDsByDMA(void){
    DisableInterrupsFromTimer();
    char values [9] =  {128,64,32,16,8,4,23,13,40};
    char index = 0;
    while(state == state3){
        if (index == 9)
            index = 0;
        ConfigDMA1ForStart(values+index);
        startTimerFor05sec();
        enterLPM(lpm_mode);
        index++;
    }
    disableTimerB();
    clrLEDS();
}


char countChars (char* str){
    char i=0;
    while (str[i]!='\0'){
        i++;
    }
    return i;
}

void writeStrMirrorToLCD(char* str){ //toScroll press '*'
    lcd_clear();
    lcd_home();
    lcd_writeStrLimit(str , 16);
    lcd_home();
    lcd_new_line;
    lcd_writeStrLimit(str+16 , 16);
}

void MirrorByDMA(void){

    disablePBsInterrupts();
    char strMirror[156];
    char str[ ] = "Google Colaboratory is a free Jupyter notebook environment that runs on Google�s cloud servers, letting the user leverage backend hardware like GPUs and TPUs";
    //char numOfChars = countChars(str);
    char numOfChars = 156;
    ConfigDMA0ForStart(strMirror , str + numOfChars , numOfChars , 1 , 0); //10for decrement source increment dst
    trigerDMA0();
    writeStrMirrorToLCD(strMirror);
    enablePBsInterrupts();
    state = state0;
}

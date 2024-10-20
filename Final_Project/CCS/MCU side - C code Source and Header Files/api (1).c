#include  "../header/api.h"    		// private library - API layer
#include  "../header/halGPIO.h"     // private library - HAL layer

unsigned char recieveData;
unsigned char dataToSend[4];
unsigned char NumFile;
unsigned char PB_Pressed;


void F_Manual_Sleep(void){
    enterLPM();
}

void F_Joystick_Control(void){
    unsigned int xSampled;
    unsigned int ySampled;
    unsigned char dir = 0;
    unsigned char motorStat = 0x01;
    while(state == Joystick_Control){
        SampleJoyStickNoiseFilter(&xSampled , &ySampled , 16 , 4);
        dir = calcDirection(xSampled , ySampled , currentPos); //return 0 if not move, 1 if clockwise , 2 in counterclockwise
        if (dir != 0){
            if (dir == 1){
                StepMotorClockwise(&motorStat);
                currentPos = (currentPos + 1);
                currentPos = (currentPos == header.s_numOfStepsIn360)? 0 : currentPos;
            }
            else{ //dir == 2
                StepMotorCounterClockwise(&motorStat);
                currentPos = (currentPos>0)? currentPos-1 : header.s_numOfStepsIn360;
            }
        }
        //StartTimerA1(10); //50Hz => 20ms
    }
}

void F_Rotate(){
    unsigned char motorStat = 0x01;
    while(state == Rotate){
        StepMotorClockwise(&motorStat);
        currentPos = (currentPos + 1);
        currentPos = (currentPos == header.s_numOfStepsIn360)? 0 : currentPos;
        //StartTimerA1(10); //50Hz => 20ms
        StartTimerA1(2); //50Hz => 20ms
    }
}

void F_Painter(){
    unsigned int xSampled;
    unsigned int ySampled;
    PB_Pressed = 0;
    StartPBs();
    while(state == Painter){
        DisablePBs();
        if (PB_Pressed){
            TransmitData(1);
            PB_Pressed = 0;
        }
        SampleJoyStickNoiseFilter(&xSampled , &ySampled , 64 , 6);
        PrepareCharToSend(xSampled , ySampled);
        TransmitData(0);
        EnablePBs();
    }
    DisablePBs();
}

void F_Calibration_Sleep(){
    enterLPM();
}

void F_Calibration_before_count(){
    unsigned char motorStat = 0x01;
    SampleJoyStickNoiseFilter(&header.JoyStickMiddleX , &header.JoyStickMiddleY , 64 ,6);
    TransmitData(3);
    StartPBs();
    while (state == Calibration_before_count){
        StepMotorClockwise(&motorStat);
        DisablePBs();
        StartTimerA1(10); //25Hz => 40ms
        EnablePBs();
    }
    DisablePBs();
}

void F_Calibration_count(){
    unsigned char motorStat = 0x01;
    unsigned int count;
    count = 0;
    StartPBs();
    while (state == Calibration_count){
        StepMotorClockwise(&motorStat);
        count = count + 1;
        DisablePBs();
        StartTimerA1(10); //25Hz => 40ms
        EnablePBs();
    }
    DisablePBs();
    currentPos = 0;
    header.s_numOfStepsIn360 = count;
    WriteSegment((char*)&header , 3);
    TransmitData(2);
}

void F_Send_Script_Status(){
    SendFileStatus();
    if (state == Send_Script_Status) state = Script_Sleep;
}

void F_Script_Sleep(){
    enterLPM();
}

void F_Load(){
    char dataBuffer[64];
    unsigned char nameIndex = 0;
    unsigned char dataIndex = 0;
    unsigned char fileIndex = 1 << NumFile;
    do{
        enterLPM();
        header.filenames[NumFile][nameIndex] = recieveData;
        nameIndex += 1;
    }
    while (recieveData != 0); //Null character
    do{
        enterLPM();
        if (recieveData != 10 && recieveData != 3){ // character is not new line of EOF
            dataBuffer[dataIndex] = recieveData;
            dataIndex += 1;
        }
    }
    while (recieveData != 3); //EOF character
    header.filesState |= fileIndex;
    header.sizes[NumFile] = dataIndex;
    WriteSegment((char*)&header , 3);
    WriteSegment(dataBuffer , NumFile);
    TransmitData(5);
    if (state == Load) state = Script_Sleep;
}

void F_Execute(void){
    char dataBuffer[64];
    int filePos = 0;
    unsigned char fileIndex = 1 << NumFile;
    if ((header.filesState & fileIndex) == 0){ //file is not loaded, sending error
        TransmitData(6);
        if (state == Execute) state = Script_Sleep;
        return;
    }
    ReadSegment(dataBuffer, NumFile);
    while (state == Execute){
        filePos = ExecuteOpcode(dataBuffer , filePos);
        if (filePos == -1){ //Error happened in execution
            TransmitData(6);
            break;
        }
        if (filePos >= header.sizes[NumFile]) break;
    }
    if (state == Execute) state = Script_Sleep;
}

int ExecuteOpcode(char* file , int filePos){
    int a;
    int b;
    int myPos = filePos;
    if (file[myPos++] != '0')
        return -1;
    if (file[myPos] == '1'){
            myPos++;
            if ((a = CollectOperand(file, myPos))==-1) return -1;
            myPos = myPos + 2;
            inc_lcd((char)a);
    }
    else if (file[myPos] == '2'){
        myPos++;
        if ((a = CollectOperand(file, myPos))==-1) return -1;
        myPos = myPos + 2;
        dec_lcd((char)a);
    }
    else if (file[myPos] == '3'){
        myPos++;
        if ((a = CollectOperand(file, myPos))==-1) return -1;
        myPos = myPos + 2;
        rra_lcd((char)a);
    }
    else if (file[myPos] == '4'){
        myPos++;
        if ((a = CollectOperand(file, myPos))==-1) return -1;
        myPos = myPos + 2;
        set_delay((char)a);
    }
    else if (file[myPos] == '5'){
        myPos++;
        lcd_clear();
        lcd_home();
    }
    else if (file[myPos] == '6'){
        myPos++;
        if ((a = CollectOperand(file, myPos))==-1) return -1;
        myPos = myPos + 2;
        stepper_deg((char)a);
    }
    else if (file[myPos] == '7'){
        myPos++;
        if ((a = CollectOperand(file, myPos))==-1) return -1;
        myPos = myPos + 2;
        if ((b = CollectOperand(file, myPos))==-1) return -1;
        myPos = myPos + 2;
        stepper_scan((char)a, (char)b);
    }
    else if (file[myPos] == '8'){
            myPos++;
            enterLPM();
    }
    return myPos;
}

void inc_lcd(char x){
    char num[4] = {'0','0','0','\0'};
    unsigned int timeToWait = d * 10;
    lcd_clear();
    lcd_home();
    lcd_puts("count:");
    lcd_puts("   ");
    int i = 0;
    while(state == Execute && i<=x){
        ConvertIntToStr(num,i);
        lcd_cursor_left();
        lcd_cursor_left();
        lcd_cursor_left();
        lcd_puts(num);
        StartTimerA1(timeToWait);
        i++;
    }
}

void dec_lcd(char x){
    char num[4] = {'0','0','0','\0'};
    unsigned int timeToWait = d * 10;
    lcd_clear();
    lcd_home();
    lcd_puts("count:");
    lcd_puts("   ");
    int i = x;
    while(state == Execute && i>=0){
        ConvertIntToStr(num,i);
        lcd_cursor_left();
        lcd_cursor_left();
        lcd_cursor_left();
        lcd_puts(num);
        StartTimerA1(timeToWait);
        i--;
    }
}

void rra_lcd(char x){
    char c = x;
    unsigned int timeToWait = d * 10;
    lcd_clear();
    lcd_home();
    lcd_putchar(c);
    int i = 0;
    while(state == Execute && i<32){
        StartTimerA1(timeToWait);
        lcd_cursor_left();
        if (i == 16){
            lcd_home();
            lcd_new_line;
        }
        else{
            lcd_putchar(' ');
        }
        lcd_putchar(c);
        i++;
    }
}

void set_delay(char x){
    d = x;
}

void ConvertIntToStr(char* str, int num){
    int copyNum = num;
    int i;
    for (i=2; i>=0; i--){
        str[i] = (char)copyNum%10;
        str[i] += 48;
        copyNum = (char)copyNum/10;
    }
}

void stepper_deg(char p){
    unsigned char dir;
    unsigned char motorStat = 0x01;
    unsigned int newPos = getPosFromDeg(p);
    dir = findBestDirection(currentPos , newPos);
    while (state == Execute && currentPos != newPos){
        if (dir == 1){
            StepMotorClockwise(&motorStat);
            currentPos = (currentPos + 1);
            currentPos = (currentPos == header.s_numOfStepsIn360)? 0 : currentPos;
        }
        else if (dir == 2){
            StepMotorCounterClockwise(&motorStat);
            currentPos = (currentPos>0)? currentPos-1 : header.s_numOfStepsIn360;
        }
        TransmitData(7);
        StartTimerA1(20); //50Hz => 20ms
    }
}

void printCurrentDegree(){
    char num[4] = {'0','0','0','\0'};
    unsigned int tmp1 = currentPos << 3;
    unsigned int tmp2 = currentPos << 1;
    unsigned int tmp3 = currentPos >> 2;
    unsigned int deg = tmp1 + tmp2 + tmp3 + currentPos;
    unsigned int tmp4 = header.s_numOfStepsIn360 >> 5;
    deg = deg/tmp4;
    ConvertIntToStr(num , deg);
    lcd_clear();
    lcd_home();
    lcd_puts(num);
}

unsigned int getPosFromDeg (char deg){
    unsigned int newPos = header.s_numOfStepsIn360/360;
    unsigned int left = header.s_numOfStepsIn360%360;
    newPos *= deg;
    while (left>100){
        unsigned int smallPos = 100*deg;
        smallPos = smallPos/360;
        newPos += smallPos;
        left -= 100;
    }
    left *= deg;
    left = left / 360;
    newPos += left;
    return header.s_numOfStepsIn360 - newPos;
}

void stepper_scan(char l, char r){
    char numl[4] = {'0','0','0','\0'};
    char numr[4] = {'0','0','0','\0'};
    unsigned char motorStat = 0x01;
    unsigned int lPos = getPosFromDeg(l);
    unsigned int rPos = getPosFromDeg(r);
    unsigned char dirToFirstPos = findBestDirection(currentPos, lPos);
    unsigned char dirToSecondPos = findBestDirection(lPos, rPos);
    ConvertIntToStr(numl, l);
    ConvertIntToStr(numr, r);
    lcd_clear();
    lcd_home();
    while (state == Execute && currentPos != lPos){
        if (dirToFirstPos == 1){
            StepMotorClockwise(&motorStat);
            currentPos = (currentPos + 1);
            currentPos = (currentPos == header.s_numOfStepsIn360)? 0 : currentPos;
        }
        else if (dirToFirstPos == 2){
            StepMotorCounterClockwise(&motorStat);
            currentPos = (currentPos>0)? currentPos-1 : header.s_numOfStepsIn360;
        }
        StartTimerA1(20); //50Hz => 20ms
    }
    lcd_puts(numl);
    while (state == Execute && currentPos != rPos){
        if (dirToSecondPos == 1){
            StepMotorClockwise(&motorStat);
            currentPos = (currentPos + 1);
            currentPos = (currentPos == header.s_numOfStepsIn360)? 0 : currentPos;
        }
        else if (dirToSecondPos == 2){
            StepMotorCounterClockwise(&motorStat);
            currentPos = (currentPos>0)? currentPos-1 : header.s_numOfStepsIn360;
        }
        StartTimerA1(20); //50Hz => 20ms
    }
    lcd_clear();
    lcd_home();
    lcd_puts(numr);
    return;
}


unsigned char findBestDirection(unsigned int curr ,unsigned int newP){ //1 clockwise, 2 counter
    if (((newP > curr) && ((newP - curr) < header.s_numOfStepsIn360>>1)) || ((newP < curr) && ((curr - newP) > header.s_numOfStepsIn360>>1))) return 1;
    else return 2;
}

int CollectOperand(char* file,int filePos){
    char firstChar = file[filePos];
    char secondChar = file[filePos + 1];
    if (firstChar >= '0' && firstChar <= '9'){
        firstChar = (firstChar-48) << 4;
    }
    else if (firstChar >= 'A' && firstChar <= 'E'){
        firstChar = (firstChar-55) << 4;
    }
    else return -1;
    if (secondChar >= '0' && secondChar <= '9'){
        secondChar = (secondChar-48);
    }
    else if (secondChar >= 'A' && secondChar <= 'E'){
        secondChar = (secondChar-55);
    }
    else return -1;
    return firstChar + secondChar;
}

void SendFileStatus(){
    int i;
    ReadSegment((char*)&header, 3); //0 indicates segment A
    for (i=0; i<3 ; i++){
        NumFile = i;
        TransmitData(4);
    }
}

void SampleJoyStickNoiseFilter (unsigned int* xVal, unsigned int* yVal , unsigned char sumOfAvarage , unsigned char logSumOfAvarage){
    *xVal = 0;
    *yVal = 0;
    unsigned int singleSampleX;
    unsigned int singleSampleY;
    int i = sumOfAvarage;
    for (i = sumOfAvarage; i>0 ; i--){
        SampleJoyStick(&singleSampleX , &singleSampleY);
        *xVal += singleSampleX;
        *yVal += singleSampleY;
    }
    *xVal = *xVal >> logSumOfAvarage;
    *yVal = *yVal >> logSumOfAvarage;
}

unsigned char ShouldNotMove(unsigned int pos , unsigned int newPos){
    if (newPos >= pos){
        if ((newPos - pos) < MotorNoiseTolerance) return 1;
        if (((pos + header.s_numOfStepsIn360) - newPos) < MotorNoiseTolerance) return 1;
        return 0;
    }
    else{
        if ((pos - newPos) < MotorNoiseTolerance) return 1;
        if (((newPos + header.s_numOfStepsIn360) - pos) < MotorNoiseTolerance) return 1;
        return 0;
    }
}

unsigned char calcDirection(unsigned int xVal , unsigned int yVal , unsigned int pos){
    int centeredX = (int)(xVal - header.JoyStickMiddleX);
    int centeredY = (int)(yVal - header.JoyStickMiddleY);
    unsigned int newPos;
    if (JoyStickInMiddle(centeredX , centeredY)) return 0;
    newPos = calcNewPos(centeredX , centeredY);
    if ((ShouldNotMove(pos , newPos)) == 1) return 0;
    else return findBestDirection(pos, newPos);
}

unsigned char JoyStickInMiddle(int x , int y){
    if (( x <= JoyStickMiddleTolerance) && (x >= -JoyStickMiddleTolerance)){
        if ((y <= JoyStickMiddleTolerance) && (y >= -JoyStickMiddleTolerance)){
            return 1;
        }
    }
    return 0;
}


unsigned int hash (int quotient , int reminder , int x){
    unsigned int quants = header.s_numOfStepsIn360>>7;
    unsigned int t_16 = quants<<4;
    unsigned int t_8 = quants<<3;
    unsigned int t_4 = quants<<2;
    unsigned int t_2 = quants<<1;
    if (quotient >=20) return t_16 - quants;
    if (quotient >=10) return t_16 - t_2;
    if (quotient >=6) return t_8 + t_4 + quants;
    if (quotient >=5) return t_8 + t_4;
    if (quotient >=4) return t_8 + t_2 + quants;
    unsigned int quarter = x >> 4;
    if (quotient >=3 && reminder >= quarter) return t_8 + t_2;
    unsigned int half = x >> 2;
    if (quotient >=2 && reminder >= quarter + half) return t_8 + quants;
    if (quotient >=2 && reminder >= quarter) return t_8;
    if (quotient >=2) return t_8 - quants; // 2.113
    unsigned int eight = x >> 3;
    if (reminder >= half + quarter + eight) return t_4 + t_2; // 1.870
    if (reminder >= half + quarter) return t_4 + quants; // 1.668
    if (reminder >= half) return t_4; //1.4966
    if (reminder >= quarter) return t_2 + quants; // 1.348
    if (reminder >= eight) return t_2; // 1.218
    else return quants;
}

unsigned int eightCalcNewPos (unsigned int zeroX , unsigned int zeroY , unsigned int basePos , int y , int x , unsigned char posFIrst){
    if ((x <= JoyStickMiddleTolerance) && (x >= -JoyStickMiddleTolerance)) return zeroX;
    if ((y <= JoyStickMiddleTolerance) && (y >= -JoyStickMiddleTolerance)) return zeroY;
    int quotient = y/x;
    if (posFIrst == 1){
        if (quotient >=1) return basePos + hash(quotient , y%x , x);
        return basePos - hash(x/y , x%y ,y);
    }
    else{
        if (quotient >=1) return basePos - hash(quotient , y%x , x);
        return basePos + hash(x/y , x%y ,y);
    }
}

unsigned int calcNewPos(int x , int y){
    if (x >= 0 && y >= 0){
        unsigned int halfSteps = header.s_numOfStepsIn360>>1;
        unsigned int quarterSteps = header.s_numOfStepsIn360>>2;
        unsigned int eightsSteps = header.s_numOfStepsIn360>>3;
        unsigned int zeroY = halfSteps + quarterSteps;
        unsigned int basePos = halfSteps + eightsSteps;
        return eightCalcNewPos(halfSteps , zeroY , basePos , y , x , 0);
    }
    if (x >= 0 && y <= 0){
        int absY = 0-y;
        unsigned int halfSteps = header.s_numOfStepsIn360>>1;
        unsigned int quarterSteps = header.s_numOfStepsIn360>>2;
        unsigned int eightsSteps = header.s_numOfStepsIn360>>3;
        unsigned int zeroY = halfSteps + quarterSteps;
        unsigned int basePos = header.s_numOfStepsIn360 - eightsSteps;
        return eightCalcNewPos(0 , zeroY , basePos , absY , x , 1);
    }
    if (x <= 0 && y <= 0){
        int absY = 0-y;
        int absX = 0-x;
        unsigned int zeroY = header.s_numOfStepsIn360>>2;
        unsigned int basePos = header.s_numOfStepsIn360>>3;
        return eightCalcNewPos(0 , zeroY , basePos , absY , absX , 0);
    }
    else{
        int absX = 0-x;
        unsigned int halfSteps = header.s_numOfStepsIn360>>1;
        unsigned int eightsSteps = header.s_numOfStepsIn360>>3;
        unsigned int zeroY = header.s_numOfStepsIn360>>2;
        unsigned int basePos = halfSteps - eightsSteps;
        return eightCalcNewPos(halfSteps , zeroY ,basePos , y , absX , 1);
    }
}

void PrepareCharToSend(unsigned int x, unsigned int y){
    dataToSend[0] = ((unsigned char)((x/100) & 0xFF))+150;
    dataToSend[1] = ((unsigned char)((x%100) & 0xFF))+150;
    dataToSend[2] = ((unsigned char)((y/100) & 0xFF))+150;
    dataToSend[3] = ((unsigned char)((y%100) & 0xFF))+150;
}


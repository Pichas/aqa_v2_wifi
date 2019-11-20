#include "ledControl.h"



//private func
//effects
void ledsOff(uint32_t* delayTime); //�������� �����
void sunrise(uint32_t* delayTime); //������
void sunset(uint32_t* delayTime); //�����
void ledsOn(uint32_t* delayTime); //�������� �����
void manual(uint32_t* delayTime); //

void addEffect(pfn_effect); //�������� ������ � ������ ������� ��� ������

uint16_t uCustomNum = 0;
uint8_t uRed = 0;
uint8_t uGreen = 0;
uint8_t uBlue = 0;


//other
void setUserColor(uint16_t num, uint8_t red, uint8_t green, uint8_t blue); //RGB
ledx getLedX(uint16_t num); //�������� ��������� �� ����� �������

uint32_t colorConvToLED(uint8_t red, uint8_t green, uint8_t blue); //�������������� RGB � ������ 24 ����� ��� �����
void colorConvToRGB(ledx* led); //�� ������� � RGB




//vars
ledsArray ledCntrl; //���������� ������
effects effAr; //�������

//func
//public func
ledsArray* init(uint16_t ledCount){
  ledCntrl.ledCount = ledCount - 1; // ���������� ����������� � �����
  ledCntrl.getLedX = &getLedX; //������������� ����������
  ledCntrl.setUserColor = &setUserColor;
  
  ledCntrl.BufArray = (uint8_t*)calloc((PREAMBLESIZE + ledCount * 24), 1); //��������� ������������ ������. ���������� ����� ���������� Heap_Size ���� ��������� �����������.
  ledCntrl.BufSize = (PREAMBLESIZE + ledCount * 24); //������ �������
  
  if (!ledCntrl.BufArray) Error_Handler(); //���� ������ �� �������� ���� � �������

  memset((uint8_t*)(ledCntrl.BufArray + PREAMBLESIZE), DOWN, ledCount * 24);//led off
  
  //������������� ������� �������
  effAr.count = 0;
  
  //������ ��� ������������ ������������������ �������
  addEffect(&ledsOff);
  addEffect(&sunrise);
  addEffect(&sunset);
  addEffect(&ledsOn);
  addEffect(&manual);
    
  return &ledCntrl;
}

ledsArray* getLedControl(void){
  return &ledCntrl;
}


pfn_effect runEffect(uint8_t num){
  return effAr.arr[num % effAr.count]; //�������� ��������� �� ������� � ������� �� �������
}
uint8_t getEffectCount(void){
  return effAr.count; 
}


void setOneColor(uint8_t red, uint8_t green, uint8_t blue){
  for (int i = 0 ; i <= ledCntrl.ledCount ; i++)
    ledCntrl.setUserColor(i, red, green, blue); //
}

void setOneLedColor(uint16_t num, uint8_t red, uint8_t green, uint8_t blue){
  ledCntrl.setUserColor(num, red, green, blue); //
}

//private func
void addEffect(pfn_effect nameEff){
  effAr.count++;
  effAr.arr = (pfn_effect*)realloc(effAr.arr, effAr.count * sizeof(pfn_effect)); //�������� ������ ���������� ������ ��� ������ �������
  
  if (!effAr.arr) Error_Handler(); //���� ������ �� �������� ���� � �������

  effAr.arr[effAr.count - 1] = nameEff; //� ����� ������ ������� ������
}



uint32_t colorConvToLED(uint8_t red, uint8_t green, uint8_t blue){
  return (red<<redShift | green<<greenShift | blue<<blueShift);
}

void colorConvToRGB(ledx* led){
  uint32_t colorBit = 0;
  
  for (int i = 0 ; i < 24 ; i++)
    if(*(led->ptr + i) == UP)
      colorBit |= 1 << (23 - i);

  led->red   =   (colorBit >> redShift)     & 255;
  led->green =   (colorBit >> greenShift)   & 255;
  led->blue  =   (colorBit >> blueShift)    & 255;
}

void setUserColor( uint16_t num, uint8_t red, uint8_t green, uint8_t blue){
  ledx led = getLedX(num);
  uint32_t palette = colorConvToLED(red, green, blue); //��������������� RGB � ������ ��� ��� ������ �� �����
  uint32_t mask = 1 << 23;
  
  for (int i = 0 ; i < 24 ; i++){
    *(led.ptr + i) = mask & palette ? UP : DOWN;
    mask >>= 1;
  }
}

ledx getLedX(uint16_t num){
  ledx ledX = {ledCntrl.BufArray + PREAMBLESIZE, //������� � ������� + �������� �����. ��� ������ �� ������� ���������
               0,                   //���������� ����� 
               0,                   //red
               0,                   //green
               0};                  //blue

  if ((PREAMBLESIZE + num*24) > ledCntrl.BufSize) return ledX; //���� ���� �� ������� ������� ������ ������

  ledX.ptr = ledCntrl.BufArray + PREAMBLESIZE + num*24;
  ledX.num = num;
  colorConvToRGB(&ledX); //������ ����� �� ������� � ����������

  return ledX;
}


//effects
void ledsOff(uint32_t* delayTime){
  uint8_t red, green, blue;
  for (int i = 0 ; i <= ledCntrl.ledCount ; i++){
    red = ledCntrl.getLedX(i).red; //�������� ������� �������� ������
    green = ledCntrl.getLedX(i).green;
    blue = ledCntrl.getLedX(i).blue;
    
    //�������� �� ����������
    if(red > 0 ) red--;
    if(green > 0 ) green--;
    if(blue > 0 ) blue--;

    ledCntrl.setUserColor(i, red, green, blue); //�������� � �����
  }
  *delayTime = 1; //��������� �������� ����������
}

void ledsOn(uint32_t* delayTime){
  uint8_t red, green, blue;
  for (int i = 0 ; i <= ledCntrl.ledCount ; i++){
    red = ledCntrl.getLedX(i).red; //�������� ������� �������� ������
    green = ledCntrl.getLedX(i).green;
    blue = ledCntrl.getLedX(i).blue;
    
    //�������� �� ����������
    if(red < 255 ) red++;
    if(green < 255 ) green++;
    if(blue < 255 ) blue++;

    ledCntrl.setUserColor(i, red, green, blue); //�������� � �����
  }
  *delayTime = 1; //��������� �������� ����������
}


void sunrise(uint32_t* delayTime){
  uint8_t red, green, blue;
  for (int i = 0 ; i <= ledCntrl.ledCount ; i++){
    red = ledCntrl.getLedX(i).red;
    green = ledCntrl.getLedX(i).green;
    blue = ledCntrl.getLedX(i).blue;
    
    if(red < 255) red++;
    if(red > 255 * 0.2 && green < 255) green++;
    if(red > 255 * 0.2 && green > 255 * 0.2 && blue < 255) blue++;
    ledCntrl.setUserColor(i, red, green, blue);
  }
  *delayTime = 6000;
}

void sunset(uint32_t* delayTime){
  uint8_t red, green, blue;
  for (int i = 0 ; i <= ledCntrl.ledCount ; i++){
    red = ledCntrl.getLedX(i).red;
    green = ledCntrl.getLedX(i).green;
    blue = ledCntrl.getLedX(i).blue;
    
    if(blue > 0 ) blue--;
    if(blue < 255 * 0.8 && green > 0) green--;
    if(blue < 255 * 0.8 && green < 255 * 0.8 && red > 0) red--;
    ledCntrl.setUserColor(i, red, green, blue);
  }
  *delayTime = 6000;
}



void manual(uint32_t* delayTime){
  *delayTime = 5; 
}


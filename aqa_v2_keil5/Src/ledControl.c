#include "ledControl.h"



//private func
//effects
void ledsOff(uint32_t* delayTime); //потушить ленту
void sunrise(uint32_t* delayTime); //восход
void sunset(uint32_t* delayTime); //закат
void ledsOn(uint32_t* delayTime); //потушить ленту
void manual(uint32_t* delayTime); //

void addEffect(pfn_effect); //добавить эффект в массив эфектов для вывода

uint16_t uCustomNum = 0;
uint8_t uRed = 0;
uint8_t uGreen = 0;
uint8_t uBlue = 0;


//other
void setUserColor(uint16_t num, uint8_t red, uint8_t green, uint8_t blue); //RGB
ledx getLedX(uint16_t num); //получить светодиод со всеми данными

uint32_t colorConvToLED(uint8_t red, uint8_t green, uint8_t blue); //конвертировать RGB в массив 24 байта для ленты
void colorConvToRGB(ledx* led); //из массива в RGB




//vars
ledsArray ledCntrl; //управление лентой
effects effAr; //эффекты

//func
//public func
ledsArray* init(uint16_t ledCount){
  ledCntrl.ledCount = ledCount - 1; // количество светодиодов в ленте
  ledCntrl.getLedX = &getLedX; //инициализация указателей
  ledCntrl.setUserColor = &setUserColor;
  
  ledCntrl.BufArray = (uint8_t*)calloc((PREAMBLESIZE + ledCount * 24), 1); //выделение динамической памяти. необходимо чтобы переменная Heap_Size была правильно установлена.
  ledCntrl.BufSize = (PREAMBLESIZE + ledCount * 24); //размер массива
  
  if (!ledCntrl.BufArray) Error_Handler(); //если память не выделена уити с ошибкой

  memset((uint8_t*)(ledCntrl.BufArray + PREAMBLESIZE), DOWN, ledCount * 24);//led off
  
  //инициализация массива эфектов
  effAr.count = 0;
  
  //именно тут определяется последовательность эфектов
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
  return effAr.arr[num % effAr.count]; //получить указатель на функцию с эфектом по индексу
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
  effAr.arr = (pfn_effect*)realloc(effAr.arr, effAr.count * sizeof(pfn_effect)); //изменить размер выделенной памяти для масива эфектов
  
  if (!effAr.arr) Error_Handler(); //если память не выделена уити с ошибкой

  effAr.arr[effAr.count - 1] = nameEff; //в новую ячейку вписать эффект
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
  uint32_t palette = colorConvToLED(red, green, blue); //сконвертировать RGB в массив бит для вывода на ленту
  uint32_t mask = 1 << 23;
  
  for (int i = 0 ; i < 24 ; i++){
    *(led.ptr + i) = mask & palette ? UP : DOWN;
    mask >>= 1;
  }
}

ledx getLedX(uint16_t num){
  ledx ledX = {ledCntrl.BufArray + PREAMBLESIZE, //позиция в массиве + смещение нулей. крч укажет на нулевой светодиод
               0,                   //порядковый номер 
               0,                   //red
               0,                   //green
               0};                  //blue

  if ((PREAMBLESIZE + num*24) > ledCntrl.BufSize) return ledX; //если ушли за пределы массива вернем пустой

  ledX.ptr = ledCntrl.BufArray + PREAMBLESIZE + num*24;
  ledX.num = num;
  colorConvToRGB(&ledX); //залить цвета из массива в переменную

  return ledX;
}


//effects
void ledsOff(uint32_t* delayTime){
  uint8_t red, green, blue;
  for (int i = 0 ; i <= ledCntrl.ledCount ; i++){
    red = ledCntrl.getLedX(i).red; //получить текущие значения цветов
    green = ledCntrl.getLedX(i).green;
    blue = ledCntrl.getLedX(i).blue;
    
    //изменить по усмотрению
    if(red > 0 ) red--;
    if(green > 0 ) green--;
    if(blue > 0 ) blue--;

    ledCntrl.setUserColor(i, red, green, blue); //записать в буфер
  }
  *delayTime = 1; //выставить скорость измменения
}

void ledsOn(uint32_t* delayTime){
  uint8_t red, green, blue;
  for (int i = 0 ; i <= ledCntrl.ledCount ; i++){
    red = ledCntrl.getLedX(i).red; //получить текущие значения цветов
    green = ledCntrl.getLedX(i).green;
    blue = ledCntrl.getLedX(i).blue;
    
    //изменить по усмотрению
    if(red < 255 ) red++;
    if(green < 255 ) green++;
    if(blue < 255 ) blue++;

    ledCntrl.setUserColor(i, red, green, blue); //записать в буфер
  }
  *delayTime = 1; //выставить скорость измменения
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


#include "ledControl.h"



//private func
//effects
void ledsOff(uint32_t* delayTime); //потушить ленту
void sunrise(uint32_t* delayTime); //восход
void sunset(uint32_t* delayTime); //закат
void ledsOn(uint32_t* delayTime); //потушить ленту

void manual(uint32_t* delayTime); //
void custom(uint32_t* delayTime); //

uint16_t uCustomNum = 0;
uint8_t uRed = 0;
uint8_t uGreen = 0;
uint8_t uBlue = 0;


//other
void setPalitColor( uint16_t num, enum palit p, uint8_t bright); //установить цвет из палитры
void setUserColor( uint16_t num, uint8_t red, uint8_t green, uint8_t blue); //RGB
uint32_t colorConvToLED(uint8_t red, uint8_t green, uint8_t blue); //конвертировать RGB в массив 24 байта для ленты
void colorConvToRGB(ledx* led); //из массива в RGB

ledx getLedX(uint16_t num); //получить светодиод со всеми данными

void addEffect(pfn_effect); //добавить эффект в массив эфектов для вывода


//vars
ledsArray ledCntrl; //управление лентой
effects effAr; //эффекты

//func
//public func
ledsArray* init(uint16_t ledCount){
  ledCntrl.ledCount = ledCount - 1; // количество светодиодов в ленте
  ledCntrl.getLedX = &getLedX; //инициализация указателей
  ledCntrl.setPalitColor = &setPalitColor;
  ledCntrl.setUserColor = &setUserColor;
  
  ledCntrl.BufArray = (uint8_t*)malloc((PREAMBLESIZE+ledCount*24)*sizeof(uint8_t)); //выделение динамической памяти. необходимо чтобы переменная Heap_Size была правильно установлена.
  ledCntrl.BufSize = (PREAMBLESIZE+ledCount*24)*sizeof(uint8_t); //размер массива
  
  if (!ledCntrl.BufArray) _Error_Handler(__FILE__, __LINE__); //если память не выделена уити с ошибкой

  memset((uint8_t*)ledCntrl.BufArray, DOWN, ledCntrl.BufSize);//led off
  memset((uint8_t*)ledCntrl.BufArray, 0,    PREAMBLESIZE);//reset signal
  
  
//  for (int i = 0 ; i < ledCntrl.BufSize ; i++){ //заполнить 
//    if (i<PREAMBLESIZE) ledCntrl.BufArray[i] = 0; //первые элементы нулями для сброса ленты при выводе
//    else ledCntrl.BufArray[i] = DOWN; //это лед нули. будет черный цвет
//  }
  

  
  //инициализация массива эфектов
  effAr.count = 0;
  
  //именно тут определяется последовательность эфектов
  addEffect(&ledsOff);
  addEffect(&sunrise);
  addEffect(&sunset);
  addEffect(&ledsOn);
  addEffect(&manual);
  addEffect(&custom);
    
  return &ledCntrl;
}

ledsArray* getLedControl(void){
  return &ledCntrl;
}


void addEffect(pfn_effect nameEff){
  effAr.count++;
  effAr.arr = (pfn_effect*)realloc(effAr.arr, effAr.count * sizeof(pfn_effect)); //изменить размер выделенной памяти для масива эфектов
  
  if (!effAr.arr) _Error_Handler(__FILE__, __LINE__); //если память не выделена уити с ошибкой

  effAr.arr[effAr.count - 1] = nameEff; //в новую ячейку вписать эффект
}


pfn_effect getEffect(uint8_t num){
  return effAr.arr[num % effAr.count]; //получить указатель на функцию с эфектом по индексу
}
uint8_t getEffectCount(void){
  return effAr.count; 
}


//private func
void setPalitColor( uint16_t num, enum palit p, uint8_t bright){
  ledx led = getLedX(num); //получить светодиод из буфера
  uint32_t mask = 1 << 23;
  
  for (int i = 0 ; i < 24 ; i++){
    *(led.ptr + i) = mask & p ? UP : DOWN; //заполнение буфера данными
    mask >>= 1; //следующий бит
  }
}

void setUserColor( uint16_t num, uint8_t red, uint8_t green, uint8_t blue){
  ledx led = getLedX(num);
  uint32_t p = colorConvToLED(red, green, blue); //сконвертировать RGB в массив бит для вывода на ленту
  uint32_t mask = 1 << 23;
  
  for (int i = 0 ; i < 24 ; i++){
    *(led.ptr + i) = mask & p ? UP : DOWN;
    mask >>= 1;
  }
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
    if(red > 255 * 0.8 && green < 255) green++;
    if(red > 255 * 0.8 && green > 255 * 0.8 && blue < 255) blue++;
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
    if(blue < 255 * 0.2 && green > 0) green--;
    if(blue < 255 * 0.2 && green < 255 * 0.2 && red > 0) red--;
    ledCntrl.setUserColor(i, red, green, blue);
  }
  *delayTime = 6000;
}






void manual(uint32_t* delayTime){
  for (int i = 0 ; i <= ledCntrl.ledCount ; i++){
    ledCntrl.setUserColor(i, uRed, uGreen, uBlue); //
  }
  
  *delayTime = 5; 
}

void custom(uint32_t* delayTime){
  ledCntrl.setUserColor(uCustomNum, uRed, uGreen, uBlue); //
  *delayTime = 5; 
}


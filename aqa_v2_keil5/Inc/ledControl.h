#ifndef __LEDCONTROL_H__
#define __LEDCONTROL_H__

#ifdef __cplusplus
 extern "C" {
#endif

//inc
#include "stdlib.h"
#include <string.h>
#include "stm32f1xx_hal.h"


//#def
#define PREAMBLESIZE 300 //начальные нули для сброса ленты
#define UP 20
#define DOWN 10
   
//typedefs
   typedef void (*pfn_effect)(uint32_t*); //указатель на функцию с эфектом
   

//enum
enum color{ //сдвиги для преобразования цветов
  greenShift = 16,
  redShift = 8,
  blueShift = 0
};



enum palit{ //палитра предустановленных цветов
  //                 r         g           b
  black      =  0x00<<redShift  | 0x00<<greenShift  | 0x00<<blueShift,
  darkGray   =  0x0F<<redShift  | 0x0F<<greenShift  | 0x0F<<blueShift,
  gray       =  0x20<<redShift  | 0x20<<greenShift  | 0x20<<blueShift,
  
  red        =  0xFF<<redShift  | 0x00<<greenShift  | 0x00<<blueShift,
  green      =  0x00<<redShift  | 0xFF<<greenShift  | 0x00<<blueShift,
  blue       =  0x00<<redShift  | 0x00<<greenShift  | 0xFF<<blueShift,
  
  yellow     =  0xFF<<redShift  | 0xFF<<greenShift  |  0x00<<blueShift,
  magenta    =  0xFF<<redShift  | 0x00<<greenShift  |  0xFF<<blueShift,
  turquoise  =  0x00<<redShift  | 0xFF<<greenShift  |  0xFF<<blueShift,
  
  
  white      =  0xFF<<redShift  | 0xFF<<greenShift  |  0xFF<<blueShift
  
};



//struct
typedef struct{
  uint8_t* ptr; //указатель на адрес светодиода в выходном массиве
  uint16_t num; //порядковый номер
  uint8_t red; //значение красного цвета
  uint8_t green; //зеленого
  uint8_t blue; //синего
} ledx;

typedef struct {
  uint16_t ledCount; //количество светодиодов в ленте
  uint8_t* BufArray; //указатель на начало массива
  uint32_t BufSize; //размер буфера с цветами
  ledx (*getLedX)(uint16_t num); //указатель на функцию получения светодиода по номеру
  void (*setPalitColor)( uint16_t num, enum palit p, uint8_t bright); //указатель на функцию установления цвета светодиода по номеру из палитры
  void (*setUserColor)( uint16_t num, uint8_t red, uint8_t green, uint8_t blue); //указатель на функцию установления цвета светодиода по трем цветам в формате RGB (0x00-0xFF)
} ledsArray;


typedef struct{
  uint8_t count; //количество эффектов
  pfn_effect* arr;  //указатель на массив указателей на функции
}effects;



pfn_effect getEffect(uint8_t num); //фукнция получения нужной функции с эффектом по индексу
uint8_t getEffectCount(void);

//func
//led control
ledsArray* init(uint16_t ledCount); //функция инициализации массива и вообще всего управления лентой
ledsArray* getLedControl(void); //получить указатель на массив, если гдето потеряли и тд



#ifdef __cplusplus
}
#endif

#endif 

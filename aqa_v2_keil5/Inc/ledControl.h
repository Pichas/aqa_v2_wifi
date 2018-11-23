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
#define PREAMBLESIZE 300 //��������� ���� ��� ������ �����
#define UP 20
#define DOWN 10
   
//typedefs
   typedef void (*pfn_effect)(uint32_t*); //��������� �� ������� � �������
   

//enum
enum color{ //������ ��� �������������� ������
  greenShift = 16,
  redShift = 8,
  blueShift = 0
};



enum palit{ //������� ����������������� ������
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
  uint8_t* ptr; //��������� �� ����� ���������� � �������� �������
  uint16_t num; //���������� �����
  uint8_t red; //�������� �������� �����
  uint8_t green; //��������
  uint8_t blue; //������
} ledx;

typedef struct {
  uint16_t ledCount; //���������� ����������� � �����
  uint8_t* BufArray; //��������� �� ������ �������
  uint32_t BufSize; //������ ������ � �������
  ledx (*getLedX)(uint16_t num); //��������� �� ������� ��������� ���������� �� ������
  void (*setPalitColor)( uint16_t num, enum palit p, uint8_t bright); //��������� �� ������� ������������ ����� ���������� �� ������ �� �������
  void (*setUserColor)( uint16_t num, uint8_t red, uint8_t green, uint8_t blue); //��������� �� ������� ������������ ����� ���������� �� ���� ������ � ������� RGB (0x00-0xFF)
} ledsArray;


typedef struct{
  uint8_t count; //���������� ��������
  pfn_effect* arr;  //��������� �� ������ ���������� �� �������
}effects;



pfn_effect getEffect(uint8_t num); //������� ��������� ������ ������� � �������� �� �������
uint8_t getEffectCount(void);

//func
//led control
ledsArray* init(uint16_t ledCount); //������� ������������� ������� � ������ ����� ���������� ������
ledsArray* getLedControl(void); //�������� ��������� �� ������, ���� ����� �������� � ��



#ifdef __cplusplus
}
#endif

#endif 

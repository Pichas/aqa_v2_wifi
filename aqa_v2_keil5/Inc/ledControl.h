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
  void (*setUserColor)( uint16_t num, uint8_t red, uint8_t green, uint8_t blue); //��������� �� ������� ������������ ����� ���������� �� ���� ������ � ������� RGB (0x00-0xFF)
} ledsArray;


typedef struct{
  uint8_t count; //���������� ��������
  pfn_effect* arr;  //��������� �� ������ ���������� �� �������
}effects;



pfn_effect runEffect(uint8_t num); //������� ��������� ������ ������� � �������� �� �������
uint8_t getEffectCount(void);

//func
//led control
ledsArray* init(uint16_t ledCount); //������� ������������� ������� � ������ ����� ���������� ������
ledsArray* getLedControl(void); //�������� ��������� �� ������, ���� ����� �������� � ��

void setOneColor(uint8_t red, uint8_t green, uint8_t blue);
void setOneLedColor(uint16_t num, uint8_t red, uint8_t green, uint8_t blue);

#ifdef __cplusplus
}
#endif

#endif 

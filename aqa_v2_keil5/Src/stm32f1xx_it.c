/**
  ******************************************************************************
  * @file    stm32f1xx_it.c
  * @brief   Interrupt Service Routines.
  ******************************************************************************
  *
  * COPYRIGHT(c) 2018 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"
#include "stm32f1xx.h"
#include "stm32f1xx_it.h"
#include "rtc.h"

/* USER CODE BEGIN 0 */



//time
RTC_DateTypeDef curDate;
RTC_TimeTypeDef curTime;

//timers
timer* timerStart = 0;
uint16_t timersCount = 0;
uint8_t enTimers = 0;


//uart
uint8_t bIndex = 0;

uint8_t uartRX;
uint8_t uartRX_buf[50];
uint8_t uartTX_buf[50];
uint8_t uartPrepSend[] = "AT+CIPSEND=0,50\r\n";



//led const

extern uint16_t uCustomNum;
extern uint8_t uRed;
extern uint8_t uGreen;
extern uint8_t uBlue;


volatile extern uint8_t effectIndex;

/* USER CODE END 0 */

/* External variables --------------------------------------------------------*/
extern DMA_HandleTypeDef hdma_tim2_ch2_ch4;
extern TIM_HandleTypeDef htim4;
extern UART_HandleTypeDef huart1;

/******************************************************************************/
/*            Cortex-M3 Processor Interruption and Exception Handlers         */ 
/******************************************************************************/

/**
* @brief This function handles Non maskable interrupt.
*/
void NMI_Handler(void)
{
  /* USER CODE BEGIN NonMaskableInt_IRQn 0 */

  /* USER CODE END NonMaskableInt_IRQn 0 */
  /* USER CODE BEGIN NonMaskableInt_IRQn 1 */

  /* USER CODE END NonMaskableInt_IRQn 1 */
}

/**
* @brief This function handles Hard fault interrupt.
*/
void HardFault_Handler(void)
{
  /* USER CODE BEGIN HardFault_IRQn 0 */

  /* USER CODE END HardFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_HardFault_IRQn 0 */
    /* USER CODE END W1_HardFault_IRQn 0 */
  }
  /* USER CODE BEGIN HardFault_IRQn 1 */

  /* USER CODE END HardFault_IRQn 1 */
}

/**
* @brief This function handles Memory management fault.
*/
void MemManage_Handler(void)
{
  /* USER CODE BEGIN MemoryManagement_IRQn 0 */

  /* USER CODE END MemoryManagement_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_MemoryManagement_IRQn 0 */
    /* USER CODE END W1_MemoryManagement_IRQn 0 */
  }
  /* USER CODE BEGIN MemoryManagement_IRQn 1 */

  /* USER CODE END MemoryManagement_IRQn 1 */
}

/**
* @brief This function handles Prefetch fault, memory access fault.
*/
void BusFault_Handler(void)
{
  /* USER CODE BEGIN BusFault_IRQn 0 */

  /* USER CODE END BusFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_BusFault_IRQn 0 */
    /* USER CODE END W1_BusFault_IRQn 0 */
  }
  /* USER CODE BEGIN BusFault_IRQn 1 */

  /* USER CODE END BusFault_IRQn 1 */
}

/**
* @brief This function handles Undefined instruction or illegal state.
*/
void UsageFault_Handler(void)
{
  /* USER CODE BEGIN UsageFault_IRQn 0 */

  /* USER CODE END UsageFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_UsageFault_IRQn 0 */
    /* USER CODE END W1_UsageFault_IRQn 0 */
  }
  /* USER CODE BEGIN UsageFault_IRQn 1 */

  /* USER CODE END UsageFault_IRQn 1 */
}

/**
* @brief This function handles System service call via SWI instruction.
*/
void SVC_Handler(void)
{
  /* USER CODE BEGIN SVCall_IRQn 0 */

  /* USER CODE END SVCall_IRQn 0 */
  /* USER CODE BEGIN SVCall_IRQn 1 */

  /* USER CODE END SVCall_IRQn 1 */
}

/**
* @brief This function handles Debug monitor.
*/
void DebugMon_Handler(void)
{
  /* USER CODE BEGIN DebugMonitor_IRQn 0 */

  /* USER CODE END DebugMonitor_IRQn 0 */
  /* USER CODE BEGIN DebugMonitor_IRQn 1 */

  /* USER CODE END DebugMonitor_IRQn 1 */
}

/**
* @brief This function handles Pendable request for system service.
*/
void PendSV_Handler(void)
{
  /* USER CODE BEGIN PendSV_IRQn 0 */

  /* USER CODE END PendSV_IRQn 0 */
  /* USER CODE BEGIN PendSV_IRQn 1 */

  /* USER CODE END PendSV_IRQn 1 */
}

/**
* @brief This function handles System tick timer.
*/
void SysTick_Handler(void)
{
  /* USER CODE BEGIN SysTick_IRQn 0 */

  /* USER CODE END SysTick_IRQn 0 */
  HAL_IncTick();
  HAL_SYSTICK_IRQHandler();
  /* USER CODE BEGIN SysTick_IRQn 1 */

  /* USER CODE END SysTick_IRQn 1 */
}

/******************************************************************************/
/* STM32F1xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32f1xx.s).                    */
/******************************************************************************/

/**
* @brief This function handles DMA1 channel7 global interrupt.
*/
void DMA1_Channel7_IRQHandler(void)
{
  /* USER CODE BEGIN DMA1_Channel7_IRQn 0 */

  /* USER CODE END DMA1_Channel7_IRQn 0 */
  HAL_DMA_IRQHandler(&hdma_tim2_ch2_ch4);
  /* USER CODE BEGIN DMA1_Channel7_IRQn 1 */

  /* USER CODE END DMA1_Channel7_IRQn 1 */
}

/**
* @brief This function handles EXTI line[9:5] interrupts.
*/
void EXTI9_5_IRQHandler(void)
{ 
  /* USER CODE BEGIN EXTI9_5_IRQn 0 */
    
  /* USER CODE END EXTI9_5_IRQn 0 */
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_5);
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_6);
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_7);
  /* USER CODE BEGIN EXTI9_5_IRQn 1 */

  /* USER CODE END EXTI9_5_IRQn 1 */
}

/**
* @brief This function handles TIM4 global interrupt.
*/
void TIM4_IRQHandler(void)
{
  /* USER CODE BEGIN TIM4_IRQn 0 */
  //all timers
    /* Get the RTC current Time */
  HAL_RTC_GetTime(&hrtc, &curTime, RTC_FORMAT_BIN);
  /* Get the RTC current Date */
  HAL_RTC_GetDate(&hrtc, &curDate, RTC_FORMAT_BIN);
  
  
  timer* timerTest = timerStart;
  
  while (enTimers && timerTest){ //obrabotat vse taimeri
    if((timerTest->time.Hours == curTime.Hours) && (timerTest->time.Minutes == curTime.Minutes) && (timerTest->weekDay & (1 << (curDate.WeekDay - 1)))){ //esli nastalo vremia timera
      switch (timerTest->type){
        case OFF_0:
          HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET);
        break;
        case ON_0:
          HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_SET);
        break;
        case OFF_1:
          HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_RESET);
        break;
        case ON_1:
          HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_SET);
        break;
        case SET_EFFECT:
          effectIndex = timerTest->effNum;
        break;
        
        default:
          break;
      }
    }
    timerTest = timerTest->next; //next
  }
  
  
  /* USER CODE END TIM4_IRQn 0 */
  HAL_TIM_IRQHandler(&htim4);
  /* USER CODE BEGIN TIM4_IRQn 1 */

  /* USER CODE END TIM4_IRQn 1 */
}

/**
* @brief This function handles USART1 global interrupt.
*/
void USART1_IRQHandler(void)
{
  /* USER CODE BEGIN USART1_IRQn 0 */

  HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
  
  /* USER CODE END USART1_IRQn 0 */
  HAL_UART_IRQHandler(&huart1);
  /* USER CODE BEGIN USART1_IRQn 1 */

  /* USER CODE END USART1_IRQn 1 */
}

/* USER CODE BEGIN 1 */
void RXBUF_work(){


  
  if (uartRX_buf[0] == '>'){ //ready to send
    
    HAL_UART_Transmit_IT(&huart1, uartTX_buf, sizeof(uartTX_buf));
    
  }
  
  
  
  if (uartRX_buf[0] == '+'){ //rx data packet
    uint16_t startPos = 0;
    while(uartRX_buf[startPos++] != ':');
    
    
    switch(uartRX_buf[startPos]){
    //datetime
      case 'G':{ //get
        uartTX_buf[1] = (curTime.Hours / 10 + 0x30);
        uartTX_buf[2] = (curTime.Hours % 10 + 0x30);
        uartTX_buf[3] = (curTime.Minutes / 10 + 0x30);
        uartTX_buf[4] = (curTime.Minutes % 10 + 0x30);
        uartTX_buf[5] = (curTime.Seconds / 10 + 0x30);
        uartTX_buf[6] = (curTime.Seconds % 10 + 0x30);
        uartTX_buf[7] = (curDate.Date / 10 + 0x30);
        uartTX_buf[8] = (curDate.Date % 10 + 0x30);
        uartTX_buf[9] = (curDate.Month / 10 + 0x30);
        uartTX_buf[10] = (curDate.Month % 10 + 0x30);
        uartTX_buf[11] = (curDate.Year / 10 + 0x30);
        uartTX_buf[12] = (curDate.Year % 10 + 0x30);
        uartTX_buf[13] = (curDate.WeekDay % 10 + 0x30);
        
        uartTX_buf[0] = 'G';//com OK
        uartTX_buf[48] = 0x0D;//cr
        uartTX_buf[49] = 0x0A;//lf
        HAL_UART_Transmit_IT(&huart1, uartPrepSend, sizeof(uartPrepSend) - 1);
      break;}
      case 'S':{ //set
        curTime.Hours = (uartRX_buf[startPos + 1] - 0x30) * 10 + (uartRX_buf[startPos + 2] - 0x30); //str XY to dec XY
        curTime.Minutes = (uartRX_buf[startPos + 3] - 0x30) * 10 + (uartRX_buf[startPos + 4] - 0x30);
        curTime.Seconds = (uartRX_buf[startPos + 5] - 0x30) * 10 + (uartRX_buf[startPos + 6] - 0x30);
        curDate.Date = (uartRX_buf[startPos + 7] - 0x30) * 10 + (uartRX_buf[startPos + 8] - 0x30);
        curDate.Month = (uartRX_buf[startPos + 9] - 0x30) * 10 + (uartRX_buf[startPos + 10] - 0x30);
        curDate.Year = (uartRX_buf[startPos + 11] - 0x30) * 10 + (uartRX_buf[startPos + 12] - 0x30);
        curDate.WeekDay = uartRX_buf[startPos + 13] - 0x30;
      
        HAL_RTC_SetTime(&hrtc, &curTime, RTC_FORMAT_BIN);
        HAL_RTC_SetDate(&hrtc, &curDate, RTC_FORMAT_BIN);
      break;}
      
      //leds
      case 'E':{
        effectIndex = uartRX_buf[startPos + 1] - 0x30;
      break;}
      
      case 'M':{
        uCustomNum = (uartRX_buf[startPos + 1] - 0x30) * 100 + (uartRX_buf[startPos + 2] - 0x30) * 10 + (uartRX_buf[startPos + 3] - 0x30);
        uRed = (uartRX_buf[startPos + 4] - 0x30) * 100 + (uartRX_buf[startPos + 5] - 0x30) * 10 + (uartRX_buf[startPos + 6] - 0x30);
        uGreen = (uartRX_buf[startPos + 7] - 0x30) * 100 + (uartRX_buf[startPos + 8] - 0x30) * 10 + (uartRX_buf[startPos + 9] - 0x30);
        uBlue = (uartRX_buf[startPos + 10] - 0x30) * 100 + (uartRX_buf[startPos + 11] - 0x30) * 10 + (uartRX_buf[startPos + 12] - 0x30);
      break;}
      
      //timers
      case 'J':{
        enTimers = uartRX_buf[startPos + 1] - 0x30;
      break;}
            
      case 'A':{
        timer* timerNew = (timer*)malloc(sizeof(timer));

        if(!timerNew) break;//esli obekt ne sozdan
          
        //sbrosit ukazatel
        timerNew->next = 0;
        
        if(timerStart == 0){ //esli spisok pust
          timerStart = timerNew;
        }else{ //dobavit v konec
          timer* timerLast = timerStart; 
          while(timerLast->next)//naiti poslednii
            timerLast = timerLast->next;
          timerLast->next = timerNew;//dobavit v konec
        }
        
        timerNew->time.Hours = (uartRX_buf[startPos + 1] - 0x30) * 10 + (uartRX_buf[startPos + 2] - 0x30);
        timerNew->time.Minutes = (uartRX_buf[startPos + 3] - 0x30) * 10 + (uartRX_buf[startPos + 4] - 0x30);
        timerNew->time.Seconds = 0x00;
        timerNew->weekDay = (uint8_t)uartRX_buf[startPos + 5];
        timerNew->type = (timerType)uartRX_buf[startPos + 6];
        timerNew->effNum = (uint8_t)(uartRX_buf[startPos + 7] - 0x30);
      break;}
      
      case 'T':{
        uint8_t timerNum = (uartRX_buf[startPos + 1] - 0x30) * 10 + (uartRX_buf[startPos + 2] - 0x30); //get num timer
        timer* t = timerStart;
        
        if (!timerStart) timerNum = 0; //no timers
        
        for (int i = 1; i <= timerNum ; i++){
          if (t->next){
            t = t->next;
          }else{
            t = 0;
            break;
          }
        }
        
        if (t){
          uartTX_buf[0] = 'T'; //OK
          uartTX_buf[1] = (t->time.Hours / 10 + 0x30);
          uartTX_buf[2] = (t->time.Hours % 10 + 0x30);
          uartTX_buf[3] = (t->time.Minutes / 10 + 0x30);
          uartTX_buf[4] = (t->time.Minutes % 10 + 0x30);
          uartTX_buf[5] = (t->weekDay);
          uartTX_buf[6] = (t->type);
          uartTX_buf[7] = (t->effNum + 0x30);
        }else{
          uartTX_buf[0] = 'E'; //ERROR          
        }
      
        uartTX_buf[8] = uartRX_buf[startPos + 1];
        uartTX_buf[9] = uartRX_buf[startPos + 2];
      
        uartTX_buf[48] = 0x0D;//cr
        uartTX_buf[49] = 0x0A;//lf
        HAL_UART_Transmit_IT(&huart1, uartPrepSend, sizeof(uartPrepSend) - 1);
      break;}
      
      case 'R':{
        if (!timerStart) break; //no timers
        uint8_t timerNum = (uartRX_buf[startPos + 1] - 0x30) * 10 + (uartRX_buf[startPos + 2] - 0x30); //get num timer
        
        timer* timerDel = timerStart; //elem dlia ydalenia
        timer* timerPrev = timerStart; //pred element dlia adresacii
        
        for (uint8_t i = 0 ; i < timerNum ; i++){
          if(timerDel->next){
            timerPrev = timerDel;
            timerDel = timerDel->next; //naiti i element
          }else{
            timerDel = 0;
          }
        }
        
        if (!timerDel) break; //esli net elementa
        
        if (timerDel == timerStart){ //esli ydalit pervii
          timerStart = timerStart->next;
        }else{
          timerPrev->next = timerDel->next; //esli ne pervii
        }
        free(timerDel); //osvobodit pamiat
      break;}
      
      
      //relay
      case 'X':{
          HAL_GPIO_WritePin(GPIOB, 1 << (12 + uartRX_buf[startPos + 1] - 0x30), (GPIO_PinState)(uartRX_buf[startPos + 2] - 0x30));
      break;}
      
      //get all info
      case 'Z':{
        uartTX_buf[0] = 'Z'; //OK
        uartTX_buf[1] = enTimers + 0x30;
        uartTX_buf[2] = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_12) + 0x30;
        uartTX_buf[3] = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_13) + 0x30;
        uartTX_buf[4] = effectIndex + 0x30;
      
        uartTX_buf[48] = 0x0D;//cr
        uartTX_buf[49] = 0x0A;//lf
        HAL_UART_Transmit_IT(&huart1, uartPrepSend, sizeof(uartPrepSend) - 1);
      break;}
      
      default:
        break;
    }
  }
  
  
    //reset RX buf
    //memset((uint8_t*)uartRX_buf, 0, sizeof(uartRX_buf));
    bIndex = 0;
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
  uartRX_buf[bIndex++] = uartRX; //get byte
  
  if((uartRX_buf[bIndex - 1] == 0x0A && uartRX_buf[bIndex - 2] == 0x0D) || uartRX == '>'){
    uartRX_buf[bIndex + 1] = 0; //end transfer
    RXBUF_work();
  }
  
  if (bIndex == sizeof(uartRX_buf)) bIndex = 0; //reset if overload
 
  HAL_UART_Receive_IT(&huart1, &uartRX, sizeof(uartRX)); //new byte wt
}

/* USER CODE END 1 */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

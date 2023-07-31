#ifndef __STR_UART_H
#define __STR_UART_H

/**
 * Модуль работы асинхронного приемо-передатчика для работы MODBUS RTU
 *
 * Прием настроен - по умолчанию и UART переводится в режим передачи
 * только на момент ответа мастеру - (запрашивающему устройству)
 * В разных структурах процессоров переключение может быть как аппаратное,
 * так и программное - что необходимо учитывать
 * Программное переключение делается с пом. подачи 0 или 1 на соотв. GPIO (RST)
 *
 * Модуль UARRT работает в тандеме с модулем таймера ля реализации RTU режима
 *
  * Author: ln.starmark@gmail.com
 * Дата:  15.02.2020
 */

/* Includes ------------------------------------------------------------------*/
#include "str_helper.h"
#include "str_onoffmodule.h"
#include "str_fifo.h"
#include "str_mbs.h"

#define MAXLENBUFFERUSART2	128						// размер буферов приема и передачи

// Используются в main.c
extern UART_HandleTypeDef huart2;

extern int32_t test_cnt;							// test only
extern int32_t test14_cnt;							// test only


extern MB *pMbs;									// указатель на структуру  Modbus
extern uint8_t message[];	          				// указатель на буфер ответного сообщения
extern uint8_t errmessage[];		    			// указатель на буфер буфер сообщения об ошибке

extern uint8_t ringUSART[];

/// --- Это для работы модуля с fifo буфером --------------------------------
#ifdef FIFO_MODULE_ENABLE
extern uint8_t ringUSART2[];				// буфер fifo
extern fifo_t* pFifo;						// указатель на fifo
											// открыт в модуле str_fifo.c
											// используется в str_mbs.c и str_uart.c
#endif
//---------------------------------------------------------------------------

// Используется в stm32l4xx_it.c
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart);
void Start_Uart_Receive_IT(void);

void MBwork(void);

#ifdef STR_TESTONLY
void TestUart(bool *flag);
#endif

#endif // __STR_UART_H

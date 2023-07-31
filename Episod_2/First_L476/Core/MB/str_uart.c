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

#include "str_uart.h"
#include "str_tim.h"
#include "str_mbregisters.h"
#include "str_service.h"
#include "str_flash.h"

/// uart1 section variables (str_uart.c) --------------------------------------
uint8_t Rx_bufferUSART2[MAXLENBUFFERUSART2]; 	// буфер прихода
uint8_t bufferUSART2[MAXLENBUFFERUSART2];    	// буфер передатчика
uint8_t Rx_indxUSART2 = 0;                   	// индекс буфера прихода
uint8_t Rx_dataUSART2[2];						// данные из usart
uint8_t cnt_ind = 0;							// запоминает размер прихода в буфер

volatile int16_t cnt_message=0;					// просто счетчик для тестирования

/// local function prototipe --------------------------------------------------
void MBansver(MB* mb);
void MBerransver(MB* mb);

uint16_t sz = 0;

/// uart1 section function prototypes -----------------------------------------
#ifdef __PUTCHAR_PROTOTYPE__
#ifdef __GNUC__
  /* With GCC/RAISONANCE,
	   small printf (option LD Linker->Libraries->Small printf
     set to 'Yes') calls __io_putchar() */
  #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
  #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */

/**
  * @brief  Retargets the C library printf function to the USART.
  * @param  None
  * @retval None
  */

PUTCHAR_PROTOTYPE
{
    /* Place your implementation of fputc here */
    /* e.g. write a character to the USART */
    HAL_UART_Transmit(&huart2, (uint8_t *)&ch, 1, 100);
    return ch;
}
#endif // #ifdef __PUTCHAR_PROTOTYPE__

/// Функции обратного вызова работы прерываний по приему и отправке данных
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart->Instance == USART2)
	{
		if(HAL_UART_Receive_IT(&huart2,(uint8_t*)Rx_dataUSART2, 1)==HAL_OK)
		{
			Stop35();

#ifdef FIFO_MODULE_ENABLE
			Fifo_put(pFifo, Rx_dataUSART2[0]);
#else
			ringUSART[cnt_ind]=Rx_dataUSART2[0];	// потом убрать
			cnt_ind++;
#endif

			Start35((uint8_t)(inp.st.idbaud&0x00FF));
		}
	}
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart->Instance == USART2)
	{

	}
}

/// Старт работы модуля предполагает установку режима НА ПРИЕМ
void Start_Uart_Receive_IT(void)
{
	HAL_UART_Receive_IT(&huart2,(uint8_t*)Rx_dataUSART2, 1);
}

/// Главная функция, принимающая данные из линии Modbus
/// С совместной работой таймера, контролирующего прерывание потока или его прекращение
/// time>timeout35 	- идем парсерить, что есть в буфере
/// Stop35(); 		- это произошло в обработчике таймера
/// запрос пришел 	- парсим запрос - Отвечаем на запрос - возвращаемся в режим прослушки
void MBwork(void)
{
	if(isFlag35())
	{
		Stop35();

#ifdef FIFO_MODULE_ENABLE
		sz = Fifo_count(pFifo);
		Fifo_CopyBuffer(pFifo, ringUSART);
#else
		sz = cnt_ind;						// запомнить, солько пришло байтов в сообщении
		cnt_ind = 0;						// и сбросить счетчик uart-а
#endif

		uint16_t res = Parser(pMbs, ringUSART, sz);
		if(res==0)
		{
			MBansver(pMbs);
		}else
		if(res==2)
		{
			MBerransver(pMbs);
		}else
		if(res==0x10)
		{
#ifdef STR_FLASH
			Save_To_FLASH(inp.arr, 4);
			HAL_NVIC_SystemReset();
#endif
		}
		else
		if(res==1)
		{
			;
			// оставить без ответа
			// но можно помигать светодиодом
		}
	}
}

/// отправка ответа на правильный запрос
void MBansver(MB* mb)
{
	HAL_UART_Transmit_IT( &huart2, mb->mess, mb->len);
}

/// отправка сообщения об ошибке на не правильный запрос
void MBerransver(MB* mb)
{
	HAL_UART_Transmit_IT( &huart2, mb->errmess, sizeof(mb->errmess));
}

//---------------------------------------------------------------------------------
#ifdef STR_TESTONLY

void TestUart(bool *flag)
{
	if(*flag)
	{
		sprintf(bufferUSART2,"(%6i) test_cnt = %6i: (%s)\r\n", cnt_message++, test_cnt++, Rx_bufferUSART2);
		HAL_UART_Transmit_IT(&huart2,bufferUSART2, strlen(bufferUSART2));

		*flag = false;
	}
}
#endif

#ifndef __STR_MBS_H
#define __STR_MBS_H

/**
  ******************************************************************************
  * @file           : str_flash.h
  * @brief          : Модуль MODBUS RTU (возможно расширить до MODBUS ASCII)
  * 				  Реализованы функции работы с регистрами INPUT & HOLDING
  ******************************************************************************
  * @attention
  *
  * Алгоритм работы с регистрами INPUT & HOLDING:
  *
  *  	Получение цепочки байт данных от модуля str_UART.c посредством модуля str_FIFO.c
  * 	Разбор первичный с целью выяснения валидности пакета, соответствия ID
  * 	При этом ответа ведущему не дается.
  *
  * 	Дальнейший анализ пакета для выяснения допустимого
  * 	для данного контроллера формата данных
  * 	и ответ об ошибке формата ведущему устройству
  *
  * 	В случае правильного пакета производится формирование ответа ведущему
  * 	после выполнения соответствующей сборки данных для ответа,
  * 	либо - после размещения данных при отработке команд н а запись данных
  *
  * 	В алгоритме предусмотрены форматы запроса широковещательного в 2-х вариантах:
  * 		Вариант стандартный через BROADCAST адрес {0x00} и функцию запроса {0x41}
  * 		Вариант через запрос функцией {0x10} на адрес {0x80}
  * 	В обоих вариантах сообщение должно быть принято и обработано
  * 	всеми ведомыми устройствами на линии Modbus
  * 		В первом варианте ответ не должен формироваться и отсылаться
  * 		Во 2-м варианте - ответ стандартный на функцию {0x10}
  *
  * Author: ln.starmark@gmail.com
  * Date:  15.02.2020
 */

/*
 * Дополнить MB функцией 17 - Информация о регистрах
 * Inputs:
 * 	const char **InfoRgINP = {"idbaud:bb","timeout:uw","mode:uw","errcnt:uw","val:sw","bigcnt:ud","bigval:sd","speed:f","pi:fd"};
 * Holding:
 * 	const char **InfoRgHLD = {"idbaud:bb",..};
 *
 *
 * typedef enum {bb, uw, sw, ul, sl, f, d, cm } eTypeRg;
 *  bb	2 bytes,
 *  uw	uint16_t,
 *  sw   int16_t,
 *  ul   uint32_t,
 *  sl   int32_t,
 *  f    float,
 *  d    double,
 *  cm   command string (необходим буфер для команд - фикс. длины или для цепочки типа string)
 */

#include "str_helper.h"

typedef struct MODBUS{
  uint8_t 	id;			// ID device
  uint8_t 	fn;			// Fn = {3,4,6,16}
  uint8_t 	nbt;        // колич байт data = колич регистров * 2
  uint16_t 	addr;       // нач адрес блока регистров
  uint16_t 	quant;      // их количество
  uint8_t 	lrc;		// LRC для ASCII modbus
  uint16_t 	crc;		// CRC для RTU
  uint8_t	err;		// тип ошибки входных данных

  uint16_t  len;		// текущая длина message
  uint8_t*  mess;		// указ на message array
  uint8_t*  errmess;	// указ на error message array
}MB;

#define MBmessBUF	  	RINGLENGHT
#define BUFMASK 		(MBmessBUF-1)

//--- Работа Modbus ------------------------------------------------------
void InitMBslave(void);
bool ReInitMB(MB* mb, uint8_t newID);
uint16_t Parser(MB* mb, uint8_t* Buf, uint16_t size);


#endif // __STR_MBS_H

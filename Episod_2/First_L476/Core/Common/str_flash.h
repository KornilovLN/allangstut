#ifndef __STR_FLASH_H
#define __STR_FLASH_H

/**
  ******************************************************************************
  * @file           : str_flash.h
  * @brief          : Модуль работы с флеш-памятью для обеспечения сохранения
  * 				  параметров программы контроллера
  ******************************************************************************
  * @attention
  *
  * Запись массива Arr[size] значений uint16_t размером size ячеек
  * во флеш память по 2 значения в одно 4-х байтное слово флеш
  * Чтение сохраненных параметров из флеш для работы программы после старта
  *
  * Чтение уникального UID контроллера (функция void Flash_Info(void);),
  * который используется для удаленного назначения ID Modbus
  *
  * Также, из флеш зачитывается размер памяти флеш данного контроллера
  * с помощью void Flash_Info(void);
  *
  * Author: ln.starmark@gmail.com
  * Date:  29.02.2020
  */

/* Includes ------------------------------------------------------------------*/
#include "str_helper.h"
#include "str_onoffmodule.h"

#ifdef STR_FLASH

#define MAXSHIFTFLASH				4
//#define	FLASH_PAGE_SIZE				2048

//uint16_t BufFlash16[];

bool isFreeFlashPage(uint32_t nPage);
uint8_t FLASH_Ready(void);

void Save_To_FLASH(uint16_t *pArr, uint32_t size);
uint32_t FLASH_ReadWord(uint32_t addr);
void FLASH_Read_Array(uint32_t *pArr, uint32_t size);

#endif

void Flash_Info(void);
uint32_t GetFlashSize(void);


#endif	// __STR_FLASH_H

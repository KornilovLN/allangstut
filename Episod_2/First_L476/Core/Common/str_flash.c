/**
  ******************************************************************************
  * @file           : str_flash.c
  * @brief          : Модуль работы с флеш-памятью для обеспечения сохранения
  * 				  параметров программы контроллера
  ******************************************************************************
  * @attention
  *
  * Запись массива uint16_t Arr[size] значений  размером size ячеек
  * во флеш память по 2 значения в одно 4-х байтное слово флеш
  *
  * Чтение сохраненных параметров из флеш для работы программы после старта
  *
  * Чтение уникального UID контроллера (функция void Flash_Info(void);),
  * который используется для удаленного назначения ID Modbus
  *
  * Также, из флеш зачитывается размер памяти флеш данного контроллера
  * с помощью void Flash_Info(void);
  *
  * Author: ln.starmark@gmail.com
  * Date:   03.04.2020
  */

#include "str_flash.h"

///--- unique ID controller --------------------------
volatile uint32_t *UniqueID = (uint32_t *)0x1FFF7590;
volatile uint32_t UID[3];

///--- flash size register  --------------------------
volatile uint16_t *FlashSize = (uint16_t *)0x1FFF75E0;
volatile uint16_t FSZ[1];

#ifdef STR_FLASH
#define ADDR_FLASH_PAGE_254		((uint32_t)0x0807F000)
#define ADDR_FLASH_PAGE_255		((uint32_t)0x0807F800)

#define FLASH_USER_START_ADDR	ADDR_FLASH_PAGE_254
#define FLASH_USER_END_ADDR		(ADDR_FLASH_PAGE_255 + FLASH_PAGE_SIZE - 1)

static uint32_t GetPage(uint32_t Address);
static uint32_t GetBank(uint32_t Address);
void Err_Handler(void);

///--- Test only --------------------------------------------------
///#define DATA_64					((uint64_t)0x1A3B5C7D1E3F1828)
///#define DATA_64					((uint64_t)0xFFFFFFFFFFFFFFFF)
///----------------------------------------------------------------

__IO uint32_t data32 = 0;
__IO uint32_t MemoryProgramStatus = 0;

uint32_t	FirstPage = 0;
uint32_t	NbOfPages = 0;
uint32_t 	BankNumber= 0;
uint32_t	Address = 0;
uint32_t 	PageError = 0;

FLASH_EraseInitTypeDef EraseInitStruct;
uint32_t PAGEError;
HAL_StatusTypeDef	flash_ok = HAL_ERROR;

uint32_t BufFlash[10] = {0,0,0,0,0,0,0,0,0,0};
uint16_t BufFlash16[20] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
#endif

/// Организация чтения памяти уникального номера процессора UID и размера flash памяти
/// Чтение ранее записанного во флеш-память ID modbus
void Flash_Info(void)
{
	UID[0] = UniqueID[0];
	UID[1] = UniqueID[1];
	UID[2] = UniqueID[2];

	FSZ[0] = FlashSize[0];

#ifdef STR_FLASH
	FLASH_Read_Array(BufFlash, 10);

	for(uint16_t i = 0; i < 10; i++)
	{
		BufFlash16[i*2+1] = (uint16_t)(BufFlash[i]&0x0000FFFF);
		BufFlash16[i*2] = (uint16_t)(BufFlash[i]>>16);
	}
#endif

}

/// Функция возвращает размер флеш памяти
uint32_t GetFlashSize(void)
{
	return (uint32_t)FSZ[0];
}

#ifdef STR_FLASH
/// чтение слова по адресу
uint32_t FLASH_ReadWord(uint32_t addr)
{
	return (*(__IO uint32_t*)addr);
}

/// Проверка на чистоту страницы
bool isFreeFlashPage(uint32_t nPage)
{
	bool res = true;

	for(uint32_t i = 0; i < FLASH_PAGE_SIZE/4; i+=4)
	{
		if( FLASH_ReadWord(i + FLASH_USER_START_ADDR) != 0xFFFFFFFF)
		{
			return false;
		}
	}

	return res;
}

/// Чтение массива из флеш-памяти
void FLASH_Read_Array(uint32_t *pArr, uint32_t size)
{
	for(uint32_t i = 0; i < size; i += 1)
	{
		pArr[i] = FLASH_ReadWord(FLASH_USER_START_ADDR + i*4);
	}
}

/// Запись во флеш-память массива (Функции стирания страницы совмещена)
void Save_To_FLASH(uint16_t *pArr, uint32_t size)
{
	uint32_t ind = 0;

	HAL_FLASH_Unlock();
	
	__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_OPTVERR);

	FirstPage = GetPage(FLASH_USER_START_ADDR);
	NbOfPages = GetPage(FLASH_USER_END_ADDR) - FirstPage + 1;
    BankNumber = GetBank(FLASH_USER_START_ADDR);

	EraseInitStruct.TypeErase   = 	FLASH_TYPEERASE_PAGES;
	EraseInitStruct.Banks		=	BankNumber;
	EraseInitStruct.Page 		= 	FirstPage;
	EraseInitStruct.NbPages   = 	NbOfPages;

	if(HAL_FLASHEx_Erase(&EraseInitStruct, &PageError) != HAL_OK)
	{
				while(1)
				{
					;
				}
	}

	Address = FLASH_USER_START_ADDR;

	while((Address < FLASH_USER_END_ADDR) && (ind < size))
	{
		uint64_t dt = ((uint64_t)pArr[ind+2])<<48;
				 dt |= ((uint64_t)pArr[ind+3])<<32;
				 dt |= ((uint64_t)pArr[ind])<<16;
				 dt |= (uint64_t)pArr[ind+1];

		if(HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, Address, dt) == HAL_OK)
		{
			ind += 4;
			Address = Address + 8;
		}
		else
		{
				while(1)
				{
					;
				}
		}
	}

	HAL_FLASH_Lock();
}

/// Рабочие функции выяснения номера банка и начального адреса страницы
static uint32_t GetPage(uint32_t Addr)
{
	uint32_t page = 0;

	if(Addr < (FLASH_BASE + FLASH_BANK_SIZE))
	{
		page = (Addr - FLASH_BASE)/FLASH_PAGE_SIZE;
	}
	else
	{
		page = (Addr - (FLASH_BASE + FLASH_BANK_SIZE))/FLASH_PAGE_SIZE;
	}

	return page;
}

static uint32_t GetBank(uint32_t Addr)
{
	uint32_t bank = 0;

	if(READ_BIT(SYSCFG->MEMRMP, SYSCFG_MEMRMP_FB_MODE) == 0)
	{
		if(Addr < (FLASH_BASE + FLASH_BANK_SIZE))
		{
			bank = FLASH_BANK_1;
		}
		else
		{
			bank = FLASH_BANK_2;
		}
	}
	else
	{	// bank swap
		if(Addr < (FLASH_BASE + FLASH_BANK_SIZE))
		{
			bank = FLASH_BANK_2;
		}
		else
		{
			bank = FLASH_BANK_1;
		}
	}

	return bank;
}

#endif




/**
  ******************************************************************************
  * @file           : str_flash.h
  * @brief          : Модуль тестовых регистров для работы MODBUS RTU
  * 				  (стандартные 16 бит) INPUT & HOLDING
  ******************************************************************************
  * @attention
  *
  * Модуль тестовых регистров для работы MODBUS RTU (стандартные 16 бит)
  * INPUT & HOLDING
  * Структуры объединены в союзы с массивами 16-ти битных регистров
  * внутрипрограммное использование - по именам из структур
  * в протоколе MODBUS - по номерам в массиве
  *
  * Реализована функция отслеживания изменений главных настроечных регистров
  * ModifyMainRegisters() - можно расширять для любых других регистров
  * Редактируются все регистры holding, часть настроечных - копируются в input регистры
  * Какие регистры следует зафиксировать - указано в регистре editinfo
  * Изменения возможны только по совпадению технического пароля в регистре pwd
  * Также - запись во флеш производится по наличию условного бита 0x8000 в регистре editinfo
  *
  * Author: ln.starmark@gmail.com
  * Date:  15.02.2020
  */

#include "str_mbregisters.h"
#include "str_flash.h"

const char *InfoRgINP[] = {
		"idbaud","timeout","mode","errcnt",
		"status",
		"serial_0","serial_1","serial_2","serial_3","serial_4","serial_5",
		"flashsize",
		"devtype","devtype",
		"inp0","inp1","inp2","inp3","inp4","inp5","inp6","inp7",
		"P_86BSD","conductivity","T_fahr","T_chip","T_cels","T_86BSD","reserv1","reserv2"};

const char *InfoRgHLD[] = {
		"idbaud","timeout","mode",
		"editinfo","pwd",
		"ser_0","ser_1","ser_2","ser_3","ser_4","ser_5","newID",
		"dt0","dt1","dt2","dt3","par0","par1","par2","par3",
		"reserv0","reserv1","reserv2"};

W64_2W32_4W16_W8 uCont;

volatile uint16_t Jampers = 0;
bool flCompareUID = false;

extern uint16_t BufFlash16[];
//-----------------------------------------------------------------------------------------

void ClearUID(void);
//-----------------------------------------------------------------------------------------

/// Инициализация INPUT регистров
void InitRegsInp(void)
{
	inp.st.idbaud = hld.st.idbaud;
	inp.st.timeout = hld.st.timeout;
	inp.st.mode = hld.st.mode;
	inp.st.errcnt = 0x0000;

	inp.st.status = 0x0000;

	inp.st.serial_0 = UID[0]&0x0000FFFF;
	inp.st.serial_1 = (UID[0]>>16)&0x0000FFFF;
	inp.st.serial_2 = UID[1]&0x0000FFFF;
	inp.st.serial_3 = (UID[1]>>16)&0x0000FFFF;
	inp.st.serial_4 = UID[2]&0x0000FFFF;
	inp.st.serial_5 = (UID[2]>>16)&0x0000FFFF;

	inp.st.flashsize = FSZ[0];

	inp.st.devtype = DEVTYPE;
	inp.st.firmware = FIRMWARE;

	inp.st.inp0 = 0;
	inp.st.inp1 = 0;
	inp.st.inp2 = 0;
	inp.st.inp3 = 0;
	inp.st.inp4 = 0;
	inp.st.inp5 = 0;
	inp.st.inp6 = 0;
	inp.st.inp7 = 0;

	// давление и температура датчика 86BSD и данные других датчиков
	inp.st.P_86BSD = 0;
	inp.st.conductivity = 0;
	inp.st.T_fahr = 0;
	inp.st.T_chip = 0;
	inp.st.T_cels = 0;
	inp.st.T_86BSD = 0;
	inp.st.reserv1 = 0;
	inp.st.reserv2 = 0;
}

/// Инициализация HOLDING регистров
void InitRegsHld(void)
{
	uint16_t tmp = BufFlash16[0]>>8;
	uint16_t idbaud = 0x0000;
	if(tmp < MINID)
	{
		idbaud = ((uint16_t)MINID<<8)|(ADDRBAUD&0x00FF);
	}
	else if(tmp > MAXID)
	{
		idbaud = ((uint16_t)(MINID)<<8)|(ADDRBAUD&0x00FF);
	}else
	{
		idbaud = (BufFlash16[0]&0xFF00)|(ADDRBAUD&0x00FF);
	}

	uint16_t timeout = TIMEOUT;
	uint16_t mode = MODE;

	/************************************************/

	hld.st.idbaud = idbaud;
	hld.st.timeout = timeout;
	hld.st.mode = mode;

	hld.st.editinfo = 0x0000;
	hld.st.pwd = 0x0000;

	// Сюда заносят с верхнего уровня UID 96 бит и новый ID
	hld.st.ser0 = 0;
	hld.st.ser1 = 0;
	hld.st.ser2 = 0;
	hld.st.ser3 = 0;
	hld.st.ser4 = 0;
	hld.st.ser5 = 0;
    hld.st.newID= 0;

	hld.st.dt0 = 0;
	hld.st.dt1 = 0;
	hld.st.dt2 = 0;
	hld.st.dt3 = 0;

	hld.st.par0 = 0;
	hld.st.par1 = 0;
	hld.st.par2 = 0;
	hld.st.par3 = 0;

	hld.st.reserv0 = 0;
	hld.st.reserv1 = 0;
	hld.st.reserv2 = 0;
}

#ifdef STR_MODIFYREGISTERS

/// Функция-менеджер для отслеживания приходящих по Modbus команд контроллеру
/// Путем анализа регистров hld.st.editinfo и hld.st.pwd
/// будут выполнены соответствующие операции изменения и запоминания
/// параметров в энергонезависимой памяти FLASH
bool ModifyMainRegisters(void)
{
	bool res = false;

	if(hld.st.editinfo > 0)
	{
		if(hld.st.pwd == PASSWORD)
		{
			if((hld.st.editinfo & 0x0001) == 0x0001)
			{
				uint8_t id = (uint8_t)(hld.st.idbaud>>8);
				uint8_t bd = (uint8_t)(hld.st.idbaud&0x00FF);
				if(((id > 0) && (id < MAXID)) && ((bd > 0) && (bd < 8)))
				{
					inp.st.idbaud = hld.st.idbaud;
					res = true;
				}
			}

			if((hld.st.editinfo & 0x0002) == 0x0002)
			{
				if((hld.st.timeout >= MINTIMEOUT)  && (hld.st.timeout <= TIMEOUT))
				{
					inp.st.timeout = hld.st.timeout;
					res = true;
				}
			}

			if((hld.st.editinfo & 0x0004) == 0x0004)
			{
					inp.st.mode = hld.st.mode;
					res = true;
			}

			if((hld.st.editinfo & 0x0008) == 0x0008)
			{
				flCompareUID = CompareUID();  // Запомнить для mbs.c

				if(flCompareUID)
				{
					uint8_t id = (uint8_t)(hld.st.newID>>8);
					if((id > 0) && (id < MAXID))
					{
						hld.st.idbaud = (hld.st.idbaud&0x00FF)|(hld.st.newID&0xFF00);
						inp.st.idbaud = hld.st.idbaud;

						res = true;
					}
				}else
				{
					res = false;
				}

				ClearUID();
			}

			if(res)
			{
				// Save to flash по условию старшего бита editinfo

				if((hld.st.editinfo & 0x8000) == 0x8000)
				{
#ifdef STR_FLASH
					Save_To_FLASH(inp.arr, 4);
					HAL_NVIC_SystemReset();
#endif
				}
			}
		}

		// В любом случае - сбрасываем условия команды

		hld.st.pwd = 0x0000;
		hld.st.editinfo = 0x0000;
	}

	return res;

}

#endif

/// Функция распознавания своего UID, пришедшего в широковещательном запросе Modbus
bool CompareUID(void)
{
	return ((hld.st.ser0 == inp.st.serial_0)&&
			(hld.st.ser1 == inp.st.serial_1)&&
			(hld.st.ser2 == inp.st.serial_2)&&
			(hld.st.ser3 == inp.st.serial_3)&&
			(hld.st.ser4 == inp.st.serial_4)&&
			(hld.st.ser5 == inp.st.serial_5));
}

/// Очистка рабочих регистров после обработки широковещательного запроса
void ClearUID(void)
{
	hld.st.ser0 = 0;
	hld.st.ser1 = 0;
	hld.st.ser2 = 0;
	hld.st.ser3 = 0;
	hld.st.ser4 = 0;
	hld.st.ser5 = 0;

	hld.st.newID = 0;
}


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

#include "str_mbs.h"
#include "str_mathcommon.h"
#include "str_mbregisters.h"

/// Поддерживаемые функции Modbus данным контроллером
#define RgNameInp 40000
#define RgNameHld 30000

#define MAXFUNC		6
const uint8_t FnMB[MAXFUNC] = {3, 4, 6, 16, 17, 65};

/// Все регистры прописаны здесь {структуры регистров описаны в str_mbregisters.h
uINPUT inp;
uHOLD  hld;

extern const char *InfoRgINP[];
extern const char *InfoRgHLD[];

/// Перечисления допустимых для этого контроллера типов ошибок и номеров функций
typedef enum eErrorMB {eE0 = 0, eFn=1, eAd=2, eQt=3, eWt=4, eCrash=5} eErrMB;

/// структура и указатель на структуру Mb
MB Mbs, *pMbs;
eErrMB 	eErrMb=eE0;
uint8_t Fmb=3;

/// Буферы для работы алгоритма
uint8_t ringUSART[RINGLENGHT];	// буфер чтения входного сообщения
uint8_t message[MBmessBUF];		// буфер ответного сообщения
uint8_t errmessage[5];		    // буфер сообщения об ошибке
								// ID, Fn|0x80, ErrCode, CRCH, CRCL

/*
 * Локальные прототипы функций модуля str_mbs.c
 */
void InitMB(MB* mb, uint8_t* ms, uint8_t* erms);
void InitMBbufansver(uint16_t n);

uint16_t WorkFunc3(MB* mb, uint8_t* Buf);
uint16_t WorkFunc4(MB* mb, uint8_t* Buf);
uint16_t WorkFunc6(MB* mb, uint8_t* Buf);
uint16_t WorkFunc16(MB* mb, uint8_t* Buf);
uint16_t WorkFunc17(MB* mb, uint8_t* Buf);
uint16_t WorkFunc65(MB* mb, uint8_t* Buf);
bool isOurFunc(uint8_t fn);
//-----------------------------------------------------------------------------

/// Инициализация модуля MBslave
void InitMBslave(void)
{
	InitMBbufansver(MBmessBUF);
	pMbs = &Mbs;
	InitMB(pMbs, message, errmessage);
}

/// Инициализация структуры MB
void InitMB(MB* mb, uint8_t* ms, uint8_t* erms)
{
	mb->id = (uint8_t)(hld.st.idbaud>>8);	// default ID = 1
	mb->fn = Fmb;      						// default Fn = 3
	mb->nbt = 0;
	mb->addr = 0;
	mb->quant = 0;
	mb->lrc = 0x00;
	mb->crc = 0x0000;
	mb->err = eErrMb;						// start with err = 0

	mb->len = 0;							// рабочая переменная - счетчик
	mb->mess = ms;							// ссылка на буфер ответа
	mb->errmess = erms;						// ссылка на буфер сообщения об ошибке

}

/// Реинициализация mbs
bool ReInitMB(MB* mb, uint8_t newID)
{
	if((newID>MINID) && (newID<MAXID))
	{
		mb->id = newID;
		return true;
	}else
	{
		return false;
	}
}

/// Инициализация буфера ответа
void InitMBbufansver(uint16_t n)
{
	for(uint16_t i = 0; i < n; i++)
	{
		message[i] = 0x00;
	}
}

/**
  * --- Импл. парсер запроса Modbus RTU -----------------------------------------
  *	Сюда приходит весь пакет: ID, Fn, {nBt}, Addr, Quant, {data,..}, CRC
  * Применение ZB: Parser(pMbs, pRB);
  * Функция разбирает сообщение от буфера средствами модуля Modbus
  */

uint16_t t_crc = 0x0000;
uint16_t t_crc1 = 0x0000;

uint16_t Parser(MB* mb, uint8_t* Buf, uint16_t size)
{
	uint16_t res = 0;

	/// выяснение: пакет не наш или это не общий вызов // без ответа
	if( !((Buf[0] == mb->id) || (Buf[0] == BROADCAST) || (Buf[0] == IDCAST)) )
		return 1;


	/// Проверка валидности пришедшей цепочки байт // без ответа
	t_crc = ((uint16_t)Buf[size-2])<<8;
	t_crc |= (uint16_t)Buf[size-1];
	t_crc1 = CalcCRC(Buf, size-2);
	if(t_crc1 != t_crc)
		return 1;

	/// Допустима ли функция запроса к ведомому // Вывод ответа об ошибке
	mb->fn = Buf[1];
	if(!isOurFunc(mb->fn))
	{
		// формируем сообщение с кодом ошибки eFn=1
		mb->errmess[0] = mb->id;
		mb->errmess[1] = (mb->fn)|0x80;
		mb->errmess[2] = eFn;
		t_crc1 = CalcCRC(mb->errmess, 3);
		mb->errmess[3] = t_crc1>>8;
		mb->errmess[4] = t_crc1&0x00FF;

		return 2;

	}else
	{
		/// Все нормально - можно работать с запросом
		/// в зависимости от номера функции
        /*
		switch(mb->fn)
		{
		case 3 :if(Buf[0] == mb->id)
						{
							res = WorkFunc3(mb, Buf);
						}
			break;

		case 4 :if(Buf[0] == mb->id)
						{
							res = WorkFunc4(mb, Buf);
						}
			break;

		case 6 :if(Buf[0] == mb->id)
						{
							res = WorkFunc6(mb, Buf);
						}
			break;

		case 16:if((Buf[0] == mb->id) || (Buf[0] == IDCAST))
						{
							res = WorkFunc16(mb, Buf);
						}
			break;

		case 65:if(Buf[0] == BROADCAST)
						{
							res = WorkFunc65(mb, Buf);
						}
			break;const char *InfoRgINP[]
		}
        */

		switch(mb->fn)
		{
		case 3 :res = WorkFunc3(mb, Buf);
				break;

		case 4 :res = WorkFunc4(mb, Buf);
				break;

		case 6 :res = WorkFunc6(mb, Buf);
				break;

		case 16:if((Buf[0] == mb->id) || (Buf[0] == IDCAST))
				{
					res = WorkFunc16(mb, Buf);
				}
				break;

		case 17 ://--- чтение наименований регистров
				res = WorkFunc17(mb, Buf);
				break;

		case 65:if(Buf[0] == BROADCAST)
				{
					res = WorkFunc65(mb, Buf);
				}
				break;
		}

	}

	return res;
}

///Функция чтения HOLDING регистров
uint16_t WorkFunc3(MB* mb, uint8_t* Buf)
{
	uint16_t res = 0;								// ошибок пока не было

	/// адрес начала блока регистров
	uint16_t addr = ((uint16_t)Buf[2])<<8;
	addr |= (uint16_t)Buf[3];

	/// количество регистров
	uint16_t quant = ((uint16_t)Buf[4])<<8;
	quant |= (uint16_t)Buf[5];

	mb->nbt = (uint8_t)(quant * 2);

	if((addr > MAXMBHOLDINGREGISTERS-1) || ((addr + quant) > MAXMBHOLDINGREGISTERS))
	{
		/// формируем мессаж с кодом ошибки eAd=2
		mb->errmess[0] = mb->id;
		mb->errmess[1] = (mb->fn)|0x80;
		mb->errmess[2] = eAd;
		uint16_t tcrc = CalcCRC(mb->errmess, 3);
		mb->errmess[3] = tcrc>>8;
		mb->errmess[4] = tcrc&0x00FF;

		return 2;									// Вывод ответа об ошибке

	}else{
		/// Собираем требуемые данные и формируем ответ
		mb->mess[0] = mb->id;
		mb->mess[1] = mb->fn;
		mb->mess[2] = mb->nbt;
		for(uint16_t i=0; i < quant; i++)
		{ // holding registers
			mb->mess[3+i*2] = (uint8_t)(hld.arr[addr+i]>>8);
			mb->mess[3+i*2+1] = (uint8_t)(hld.arr[addr+i]&0x00FF);
		}
		uint16_t n = mb->nbt + 3;
		t_crc1 =CalcCRC(mb->mess, n);
		mb->mess[n]= (uint8_t)(t_crc1>>8);
		mb->mess[n+1]= (uint8_t)(t_crc1&0x00FF);

		// не забудем длину буфера посылки mb->len
		mb->len = n+2;

		res = 0;
  }

	return res;
}

///Функция чтения INPUT регистров
uint16_t WorkFunc4(MB* mb, uint8_t* Buf)
{
	uint16_t res = 0;									// ошибок пока не было

	/// адрес начала блока регистров
	uint16_t addr = ((uint16_t)Buf[2])<<8;
	addr |= (uint16_t)Buf[3];

	/// количество регистров
	uint16_t quant = ((uint16_t)Buf[4])<<8;
	quant |= (uint16_t)Buf[5];

	mb->nbt = (uint8_t)(quant * 2);

	if((addr > MAXMBINPUTREGISTERS-1) || ((addr + quant) > MAXMBINPUTREGISTERS))
	{
		/// формируем мессаж с кодом ошибки eAd=2
		mb->errmess[0] = mb->id;
		mb->errmess[1] = (mb->fn)|0x80;
		mb->errmess[2] = eAd;
		t_crc1 = CalcCRC(mb->errmess, 3);
		mb->errmess[3] = t_crc1>>8;
		mb->errmess[4] = t_crc1&0x00FF;

		return 2;										// Вывод ответа об ошибке
	}else{
		/// Собираем требуемые данные и формируем ответ
		mb->mess[0] = mb->id;
		mb->mess[1] = mb->fn;
		mb->mess[2] = mb->nbt;
		for(uint16_t i=0; i < quant; i++)
		{ // input registers
			mb->mess[3+i*2] = (uint8_t)(inp.arr[addr+i]>>8);
			mb->mess[3+i*2+1] = (uint8_t)(inp.arr[addr+i]&0x00FF);
		}
		uint16_t n = mb->nbt + 3;
		t_crc1 = CalcCRC(mb->mess, n);
		mb->mess[n]= (uint8_t)(t_crc1>>8);
		mb->mess[n+1]= (uint8_t)(t_crc1&0x00FF);

		// не забудем длину буфера посылки mb->len
		mb->len = n+2;

		res = 0;
  }

	return res;
}

/// Функция записи одного регистра в секцию HOLDING регистров
uint16_t WorkFunc6(MB* mb, uint8_t* Buf)
{
	uint16_t res = 0;									// ошибок пока не было

	/// адрес начала блока регистров
	uint16_t addr = ((uint16_t)Buf[2])<<8;
	addr |= (uint16_t)Buf[3];

	/// данные для записи по адресу addr
	uint16_t data = ((uint16_t)Buf[4])<<8;
	data |= (uint16_t)Buf[5];

	// тут не проверяется входное значение на допустимость
	// по очевидной причине - транспортный протокол
	// не может знать о допустимости передаваемых данных
	// На это выделен код ошибки = 4, но будет игнорироваться

	if((addr > MAXMBHOLDINGREGISTERS-1))
	{
		/// формируем мессаж с кодом ошибки eAd=2
		mb->errmess[0] = mb->id;
		mb->errmess[1] = (mb->fn)|0x80;
		mb->errmess[2] = eAd;
		t_crc1 = CalcCRC(mb->errmess, 3);
		mb->errmess[3] = (uint8_t)(t_crc1>>8);
		mb->errmess[4] = (uint8_t)(t_crc1&0x00FF);

		return 2;										// Вывод ответа об ошибке

	}else{
        /// Записываем данные в указанный регистр
		hld.arr[addr] = data;

		mb->mess[0] = mb->id;
		mb->mess[1] = mb->fn;
		mb->mess[2] = (uint8_t)(addr>>8);
		mb->mess[3] = (uint8_t)(addr&0x00FF);
		mb->mess[4] = (uint8_t)(data>>8);
		mb->mess[5] = (uint8_t)(data&0x00FF);
		t_crc1 = CalcCRC(mb->mess, 6);
		mb->mess[6] = (uint8_t)(t_crc1>>8);
		mb->mess[7] = (uint8_t)(t_crc1&0x00FF);

		// не забудем длину буфера посылки mb->len
		mb->len = 8;

		res = 0;
  }

	return res;
}

/// Функция множественной записи в секцию HOLDING регистров
uint16_t WorkFunc16(MB* mb, uint8_t* Buf)
{
	uint16_t res = 0;									// ошибок пока не было

	/// адрес начала блока регистров
	uint16_t addr = ((uint16_t)Buf[2])<<8;
	addr |= (uint16_t)Buf[3];

	/// количество регистров
	uint16_t quant = ((uint16_t)Buf[4])<<8;
	quant |= (uint16_t)Buf[5];

	// количество байт в цепочке регистров
	// uint8_t cntbt = Buf[6];

	// тут не проверяется входное значение на допустимость
	// по очевидной причине - транспортный протокол
	// не может знать о допустимости передаваемых данных
	// На это выделен код ошибки = 4, но будет игнорироваться

	if((addr > MAXMBHOLDINGREGISTERS-1) || ((addr + quant) > MAXMBHOLDINGREGISTERS))
	{
		/// формируем мессаж с кодом ошибки eAd=2
		mb->errmess[0] = mb->id;
		mb->errmess[1] = (mb->fn)|0x80;
		mb->errmess[2] = eAd;
		uint16_t tcrc = CalcCRC(mb->errmess, 3);
		mb->errmess[3] = tcrc>>8;
		mb->errmess[4] = tcrc&0x00FF;

		return 2;										// Вывод ответа об ошибке

	}else{

        /// данные для записи начиная с addr располагаем в соответствующих регистрах

		for(uint16_t i=0; i < quant; i++)
		{
			uint16_t dt = ((uint16_t)Buf[7 + i*2])<<8;
			dt |= (uint16_t)Buf[7 + i*2 +1];
			hld.arr[addr+i] = dt;
		}

		mb->mess[0] = mb->id;
		mb->mess[1] = mb->fn;
		mb->mess[2] = (uint8_t)(addr>>8);
		mb->mess[3] = (uint8_t)(addr&0x00FF);
		mb->mess[4] = (uint8_t)(quant>>8);
		mb->mess[5] = (uint8_t)(quant&0x00FF);
		t_crc1 = CalcCRC(mb->mess, 6);
		mb->mess[6] = (uint8_t)(t_crc1>>8);
		mb->mess[7] = (uint8_t)(t_crc1&0x00FF);

		// не забудем длину буфера посылки mb->len
		mb->len = 8;

		/// Вставка в алгоритм для обеспечения удаленной замены ID по UID контроллера
		if(!CompareUID() && (Buf[0] == IDCAST))
		{	/// Не совпадает UID при адресе запроса IDCAST = без ответа
			res = 1;
		}
		else
		{   /// Этот контроллер распознал свой UID и ответит ведущему до выполнения задания
			/// сообщением со своим старым (текущем - еще не смененным ID modbus)
			res = 0;
		}
  }

	return res;
}

/// Функция множественной записи в секцию HOLDING регистров
/*
   Запрос названий регистров inputs:
   ID	f=17	Name:HI	Name:LO		CRC16
   {Name:HI	Name:LO} = 40000	для INPUTS
   {Name:HI	Name:LO} = 30000	для HOLDING
   uint16_t RgNameInp = 40000;
   uint16_t RgNameHld = 30000;

   В ответ надо выслать: количество символов, потом строку char с разделителями ','
   ZB: массив 30-ти слов по 8 байтов займет 30*8 = 240 байт
   ID 	f=17 	Nbytes	array	CRC16
 */
uint16_t WorkFunc17(MB* mb, uint8_t* Buf)
{
	uint16_t res = 0;									// ошибок пока не было

	/// Название блока регистров: {RgNameInp, RgNameHld}
	uint16_t addr = ((uint16_t)Buf[2])<<8;
	addr |= (uint16_t)Buf[3];

	/// Проверка - есть ли такой блок регистров
	if(!((addr == RgNameInp) || (addr == RgNameHld)) )
	{
		/// формируем мессаж с кодом ошибки eAd=2
		mb->errmess[0] = mb->id;
		mb->errmess[1] = (mb->fn)|0x80;
		mb->errmess[2] = eAd;
		t_crc1 = CalcCRC(mb->errmess, 3);
		mb->errmess[3] = t_crc1>>8;
		mb->errmess[4] = t_crc1&0x00FF;
		return 2;										// Вывод ответа об ошибке

	}else{
		/// Формируем ответ
		mb->mess[0] = mb->id;
		mb->mess[1] = mb->fn;

		//--- первые 3 байта посчитали
		mb->nbt = 3;
		uint16_t len = 0;
		int sz = 0;
		switch(addr)
		{
		case RgNameInp:
			sz = 30;
			for(int j = 0; j < sz; j++)
			{
				//--- Взять след строку и выяснить длину
				char *tmp = (char *)InfoRgINP[j];
				len = sizeof(tmp);
				for(int i = 0; i < len; i++)
				{
					mb->mess[mb->nbt+i] = tmp[i];
					mb->nbt++;
				}
				//--- заменим нуль-символ на ','
				mb->mess[mb->nbt-1] = ',';
			}
			break;

		case RgNameHld:
			sz = 23;
			for(int j = 0; j < sz; j++)
			{
				//--- Взять след строку и выяснить длину
				char *tmp = (char *)InfoRgHLD[j];
				len = sizeof(tmp);
				for(int i = 0; i < len; i++)
				{
					mb->mess[mb->nbt+i] = tmp[i];
					mb->nbt++;
				}
				//--- заменим нуль-символ на ','
				mb->mess[mb->nbt-1] = ',';
			}
			break;
		}


		/// Записываем размер строки с названиями в байтах (- 3 первых)
		mb->mess[2] = mb->nbt-3;

		uint16_t n = mb->nbt;
		t_crc1 = CalcCRC(mb->mess, n);
		mb->mess[n]= (uint8_t)(t_crc1>>8);
		mb->mess[n+1]= (uint8_t)(t_crc1&0x00FF);

		// не забудем длину буфера посылки mb->len
		mb->len = n+2;

		res = 0;
  }

	return res;
}


/// Функция пользовательская - прменяется в протоколе Modbus по необходимости
/// выбран номер функции из пула номеров 0x41....
/// Ответ ведущему не посылается
uint16_t WorkFunc65(MB* mb, uint8_t* Buf)
{
	uint16_t res = 0;									// ошибок пока не было

	/// код действия для функции 65
	uint16_t workcode = ((uint16_t)Buf[2])<<8;
	workcode |= (uint16_t)Buf[3];

	/// пароль для действия
	uint16_t pwd = ((uint16_t)Buf[4])<<8;
	pwd |= (uint16_t)Buf[5];

	/// младшее 32 разрядное слово серийного номера
	/// старшее 16 разрядное слово
	uint16_t serial_hi = ((uint16_t)Buf[6])<<8;
	serial_hi |= (uint16_t)Buf[7];

    /// младшее 16 разрядное слово
	uint16_t serial_lo = ((uint16_t)Buf[8])<<8;
	serial_lo |= (uint16_t)Buf[9];

    /// новый ID устройства для работы в сети modbus
	/// подается в старшем байте слова (младший - 0x00)
	uint16_t new_ID = ((uint16_t)Buf[10])<<8;
	new_ID |= (uint16_t)Buf[11];

	if((workcode==WORKCODE65) && (pwd==PASSWORD) && (serial_hi==inp.st.serial_1) && (serial_lo==inp.st.serial_0))
	{
		uint8_t id = (uint8_t)(new_ID>>8);
		if( (id > 0) && (id < MAXID) )
		{
			hld.st.idbaud = (hld.st.idbaud&0x00FF)|(new_ID&0xFF00);
			inp.st.idbaud = hld.st.idbaud;
			ReInitMB(mb, id);

			res = 0x0010; // команда на перезапись флеш
		}

	}

	/// res = 0    - ничего не делать в основной программе
	/// res = 0x10 - перезаписать флеш

	return res;
}

/// Проверка наличия функции modbus в списке функций данного контроллера
bool isOurFunc(uint8_t fn)
{
	bool res = false;

	for(int8_t i = 0; i < MAXFUNC; i++)
	{
		if(fn == FnMB[i])
		{
			res = true;
			break;
		}
	}

	return res;
}



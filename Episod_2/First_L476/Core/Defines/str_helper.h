/*
 * str_helper.h
 *
 *  Created on: May 2, 2023
 *      Author: starmark
 */

#ifndef DEFINES_STR_HELPER_H_
#define DEFINES_STR_HELPER_H_

#include "stm32l4xx_hal.h"
#include "str_onoffmodule.h"

// -------------
#include  <math.h>
#include  <stdbool.h>
#include  <stdint.h>
#include  <stdio.h>
#include  <string.h>
#include  <stdlib.h>
#include  <time.h>
// -------------

// uint_64 - uint_32[2] - uint_16[4] - uint_8
typedef union {
	uint64_t  w64;
	uint32_t  w32[2];
	uint16_t  w16[4];
	uint8_t   w8[8];
}W64_2W32_4W16_W8;

// Long - ushort[2] - byte[4] - float
typedef union {
	uint32_t lg;
	uint16_t rg[2];
	uint8_t  bt[4];
	float   fl;
}LG_RG_BT_FL;

// ushort - byte[2]
typedef union {
	uint16_t rg;
	uint8_t  bt[2];
}RG_BT;


#define LD1_Pin GPIO_PIN_4
#define LD1_GPIO_Port GPIOA
#define LD2_Pin GPIO_PIN_5
#define LD2_GPIO_Port GPIOA


/// Заводские константы изделия - вносятся при прошивке модуля
#define BAZEID	2

#define BAUD		0x0002		/// baudrate  19200
#define ADDRBAUD	0x0202		/// Адрес на шине 2, baudrate  19200

//#define BAUD		0x0003		/// baudrate  38400
//#define ADDRBAUD	0x0103		/// Адрес на шине 1, baudrate  38400

//#define BAUD		0x0007		/// baudrate  115200
//#define ADDRBAUD	0x0107		/// Адрес на шине 1, baudrate  115200

#define TIMEOUT 	  	1000    /// таймаут данного устройства - инфо для мастера
#define MINTIMEOUT	 	10  	///

#define MODE			1  		/// Пока всего 2 режима работы контроллера

#define SHIFTFLASH		0

#define SERIAL 		0x0002    	/// в данной версии ПО не используется
#define DEVTYPE 	0x0010    	/// тип данного устройства - периферийный модуль датчиков

/// версия  и подверсия прошивки с удаленной установкой адреса
#define FIRMWARE 	0x0112

#define MINID		0x01		/// минимальный ID modbus устройства
#define MAXID		0x20		/// максимальный ID modbus устройства
#define BROADCAST	0			/// широковещательный ID modbus для всех устройств
#define IDCAST		0x80		/// условный широковещательный ID modbus для функции 0x16
#define WORKCODE65  0x1828		/// дополнительный код функции широковещательного запроса
#define PASSWORD 	0xAAAA		/// пароль разрешения модификации параметров устройства

#define VOLUME_INPUT			30			/// макс размер массива Input 16-битных слов modbus
#define VOLUME_HOLDING			23			/// Макс размер массива Holding 16-битных слов modbus
#define MAXMBINPUTREGISTERS  	VOLUME_INPUT
#define MAXMBHOLDINGREGISTERS	VOLUME_HOLDING

#define MAXLENBUFFERUSART2	    128						/// размер буферов приема и передачи
#define RINGLENGHT				MAXLENBUFFERUSART2		/// длина буфера для модуля mbs

/// частоиспользуемые макросы и перечисления
#define ARRAY_LENGHT(Value)     (sizeof(Value) / sizeof(Value[0]))

enum eTYPES {eHex = 0, eDec = 1, eFlt = 3};

#endif /* DEFINES_STR_HELPER_H_ */

#ifndef __STR_ONOFFMODULE_H
#define __STR_ONOFFMODULE_H

/**
  ******************************************************************************
  * @file           : str_onoffmodule.h
  * @brief          : Включения - отключения компиляции некоторых модулей
  ******************************************************************************
  * @attention
  *
  * Технически необходимый файл для отладки модулей поотдельности:
  *
  * Author: ln.starmark@gmail.com
  * Date:  15.02.2020
  */

#define __PUTCHAR_PROTOTYPE__
#define STR_MODIFYREGISTERS
#define FIFO_MODULE_ENABLE

/// Новая версия ПО с удаленной настройкой адреса по UID
#define STR_FLASH

/// Отладочные функции и модули
//#define STR_TESTONLY
#define STR_LEDBLINK

#endif // __STR_ONOFFMODULE_H

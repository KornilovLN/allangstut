#ifndef __STR_FIFO_H
#define __STR_FIFO_H

/**
  ******************************************************************************
  * @file           : str_fifo.h
  * @brief          : Модуль поддержки циклического буфера
  * 				  для обеспечения работы с модуля str_uart.c и str_mbs.c
  ******************************************************************************
  * @attention
  *
  * Author: ln.starmark@gmail.com
  * Date:   03.04.2020
  */

//=== Include section ==================================
#include "str_helper.h"
#include "str_onoffmodule.h"

#ifdef FIFO_MODULE_ENABLE

//=== Defines section ==================================

// Ограничения: длина массива должна быть в таком ряду:
// 2, 4, 8, 16, 32, 64, 128

#define FIFO_LENGTH MAXLENBUFFERUSART2
#define FIFO_MASK (FIFO_LENGTH-1)

typedef struct {
  uint8_t head; 	// write pointer
  uint8_t tail; 	// read pointer
  uint8_t size; 	// size buffer
  uint8_t *data;	// ptr to buffer
}fifo_t;

//=== Function section =================================
//------------------------------------------------------
// function: fifo_init - buffer FIFO initialization
// arguments:
//   fifo_t *fifo - pointer on fifo structure
//	uint8_t *data - pointer on buffer
// return: ---
//------------------------------------------------------
void Fifo_New(void);

void Fifo_init(fifo_t *fifo, uint8_t *data, uint8_t size);

//------------------------------------------------------
// function: fifo_flush - buffer FIFO re-initialization
// arguments:
//   fifo_t *fifo - pointer on fifo structure
// return: ---
//------------------------------------------------------
void Fifo_flush(fifo_t *fifo);

//------------------------------------------------------
// function: fifo_put - data to buffer FIFO put
// arguments:
//   fifo_t *fifo 	-  pointer on fifo structure
//   uint8_t dt   	-  any data
// return: 0 - Ok
//         -1 - err quantity data
//------------------------------------------------------
int8_t Fifo_put(fifo_t *fifo, uint8_t dt);

//------------------------------------------------------
// function: fifo_get - data from buffer FIFO get
// arguments:
//   fifo_t *fifo 	-  pointer on fifo structure
//   uint8_t *dt   	- any data
// return: 0 - Ok
//         -1 - err quantity data
//------------------------------------------------------
int8_t Fifo_get(fifo_t *fifo, uint8_t *dt);

//------------------------------------------------------
// function: fifo_count - get data quantity in buffer FIFO
// arguments:
//   fifo_t *fifo 	-  pointer on fifo structure
// return: int8_t   - quantity data in buffer
//------------------------------------------------------
int8_t Fifo_count(fifo_t *fifo);

//------------------------------------------------------
// function: fifo_CopyBuffer - get all data in buff exdata
// arguments:
//   fifo_t *fifo 	-  pointer on fifo structure
// uint8_t* exdata  -  array result
// return: int8_t   - quantity data in buffer
//------------------------------------------------------
void Fifo_CopyBuffer(fifo_t *fifo, uint8_t *exdata);

#endif // FIFO_MODULE_ENABLE

#endif // __STR_FIFO_H


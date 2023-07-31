/**
  ******************************************************************************
  * @file           : str_fifo.c
  * @brief          : Модуль поддержки циклического буфера
  * 				  для обеспечения работы с модуля str_uart.c и str_mbs.c
  ******************************************************************************
  * @attention
  *
  * Принцип работы циклического буфера достаточно хорошо известен
  * и в данном изложении использует структуру,
  * содержащую указатели на голову, хвост, размер буфера и на сам буфер.
  *
  * Author: ln.starmark@gmail.com
  * Date:   03.04.2020
  */

#include "str_fifo.h"

#ifdef FIFO_MODULE_ENABLE

uint8_t ringUSART2[FIFO_LENGTH];

fifo_t Fifo;
fifo_t* pFifo;

void Fifo_New(void)
{
	pFifo = &Fifo;
	Fifo_init(pFifo, ringUSART2, FIFO_LENGTH);
}

void Fifo_init(fifo_t *fifo, uint8_t *data, uint8_t size)
{
	fifo->data = data;
	fifo->size = size;
	fifo->head = fifo->tail = 0;
}

void Fifo_flush(fifo_t *fifo)
{
	fifo->head = fifo->tail = 0;
}

int8_t Fifo_put(fifo_t *fifo, uint8_t dt)
{
  if ( ((fifo->head - fifo->tail)&FIFO_MASK) >= (fifo->size))
	{
	  fifo->head = fifo->tail = 0;
    return -1;
	}

  fifo->data[fifo->head++] = dt;
  fifo->head &= FIFO_MASK;

  return 0;
}

int8_t Fifo_get(fifo_t *fifo, uint8_t *dt)
{
  if (fifo->head != fifo->tail)
	{
    *dt = fifo->data[fifo->tail++];
    fifo->tail &= FIFO_MASK;

		return 0;

  }else
	{
		return -1;
	}
}

int8_t Fifo_count(fifo_t *fifo)
{
	if(fifo->head >= fifo->tail)
		return (fifo->head - fifo->tail);
	else
		return (((fifo->size) - fifo->tail) + fifo->head);

	// или так -> //return (fifo->head - fifo->tail)&FIFO_MASK;
}

void Fifo_CopyBuffer(fifo_t *fifo, uint8_t *exdata)
{
	uint8_t i = 0;

	while(fifo->tail != fifo->head)
	{
		exdata[i] = fifo->data[fifo->tail++];
		fifo->tail &= FIFO_MASK;
		i++;
	}

	fifo->head = fifo->tail = 0; // flush
}

#endif // FIFO_MODULE_ENABLE

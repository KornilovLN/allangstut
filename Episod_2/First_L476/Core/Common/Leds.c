/*
 * Leds.c
 *
 *  Created on: May 2, 2023
 *      Author: starmark
 */

#include "Leds.h"

void LedOn(uint8_t nLed)
{
	if(nLed == 0)
		HAL_GPIO_WritePin(LD1_GPIO_Port, LD1_Pin, GPIO_PIN_SET);
	else
	if(nLed == 1)
  	    HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_SET);

}

void LedOff(uint8_t nLed)
{
	if(nLed == 0)
		HAL_GPIO_WritePin(LD1_GPIO_Port, LD1_Pin, GPIO_PIN_RESET);
	else
	if(nLed == 1)
  	    HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);
}

void LedToggle(uint8_t nLed)
{
	if(nLed == 0)
		HAL_GPIO_TogglePin(LD1_GPIO_Port, LD1_Pin);
	else
	if(nLed == 1)
  	    HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
}



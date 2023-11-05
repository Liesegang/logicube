/*
 * boards.h
 *
 *  Created on: Nov 3, 2023
 *      Author: y-yam
 */

#ifndef INC_BOARDS_H_
#define INC_BOARDS_H_

// Buttons
const GPIO_TypeDef* BUTTON_RIGHT_GPIO = GPIOB;
const uint16_t BUTTON_RIGHT_PIN = GPIO_PIN_2;

const GPIO_TypeDef* BUTTON_UP_GPIO = GPIOC;
const uint16_t BUTTON_UP_PIN = GPIO_PIN_10;

const GPIO_TypeDef* BUTTON_LEFT_GPIO = GPIOC;
const uint16_t BUTTON_LEFT_PIN = GPIO_PIN_13;

const GPIO_TypeDef* BUTTON_DOWN_GPIO = GPIOC;
const uint16_t BUTTON_DOWN_PIN = GPIO_PIN_3;

// Switch
const GPIO_TypeDef* LED_GPIO = GPIOA;
const uint16_t LED_PIN = GPIO_PIN_15;

#endif /* INC_BOARDS_H_ */

#ifndef nokia5110_LCD
#define nokia5110_LCD

#include <stdbool.h>
#include <string.h>

#include "fonts.h"
#include "stm32f0xx_hal.h"

#define LCD_COMMAND 0
#define LCD_DATA 1

#define LCD_SETYADDR 0x40
#define LCD_SETXADDR 0x80
#define LCD_DISPLAY_BLANK 0x08
#define LCD_DISPLAY_NORMAL 0x0C
#define LCD_DISPLAY_ALL_ON 0x09
#define LCD_DISPLAY_INVERTED 0x0D

#define LCD_WIDTH 84
#define LCD_HEIGHT 48
#define LCD_SIZE (LCD_WIDTH * LCD_HEIGHT / 8)

#define FONT_BIG 1
#define FONT_SMALL 0

/*
 * @brief GPIO ports used
 */
struct LCD_GPIO
{
    GPIO_TypeDef *RSTPORT;
    uint16_t RSTPIN;

    GPIO_TypeDef *DCPORT;
    uint16_t DCPIN;

    GPIO_TypeDef *CLKPORT;
    uint16_t CLKPIN;

    GPIO_TypeDef *DATAPORT;
    uint16_t DATAPIN;

    GPIO_TypeDef *CEPORT;
    uint16_t CEPIN;
};

/*----- GPIO Pins -----*/
void LCD_setRST(GPIO_TypeDef *PORT, uint16_t PIN);
void LCD_setDC(GPIO_TypeDef *PORT, uint16_t PIN);
void LCD_setCE(GPIO_TypeDef *PORT, uint16_t PIN);
void LCD_setCLK(GPIO_TypeDef *PORT, uint16_t PIN);
void LCD_setDATA(GPIO_TypeDef *PORT, uint16_t PIN);


/*----- Library Functions -----*/
void LCD_send(uint8_t *val, uint16_t len);
void LCD_write(uint8_t *data, uint8_t mode, uint16_t len);
void LCD_init();
void LCD_putChar(char c);
void LCD_print(char *str, uint8_t x, uint8_t y);
void LCD_print_large(char *str, uint8_t x, uint8_t y);
void LCD_clear();
void LCD_goXY(uint8_t x, uint8_t y);
void LCD_print_float(float number, uint8_t x, uint8_t y, uint8_t size);
void LCD_print_int(uint8_t number, uint8_t x, uint8_t y, uint8_t size);

#endif

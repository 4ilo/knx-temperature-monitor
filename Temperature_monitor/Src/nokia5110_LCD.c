#include "nokia5110_LCD.h"

struct LCD_GPIO lcd_gpio;

// Fonts
Font font_32x16 = {16, 32, d_font_32x16};
Font font_8x6 = {6, 8, d_font_8x6};

/*----- GPIO Functions -----*/
/*
 * @brief Set functions for GPIO pins used
 * @param PORT: port of the pin used
 * @param PIN: pin of the pin used
 */
void LCD_setRST(GPIO_TypeDef *PORT, uint16_t PIN)
{
    lcd_gpio.RSTPORT = PORT;
    lcd_gpio.RSTPIN = PIN;
}

void LCD_setDC(GPIO_TypeDef *PORT, uint16_t PIN)
{
    lcd_gpio.DCPORT = PORT;
    lcd_gpio.DCPIN = PIN;
}

void LCD_setDATA(GPIO_TypeDef *PORT, uint16_t PIN)
{
    lcd_gpio.DATAPORT = PORT;
    lcd_gpio.DATAPIN = PIN;
}

void LCD_setCLK(GPIO_TypeDef *PORT, uint16_t PIN)
{
    lcd_gpio.CLKPORT = PORT;
    lcd_gpio.CLKPIN = PIN;
}

void LCD_setCE(GPIO_TypeDef *PORT, uint16_t PIN)
{
    lcd_gpio.CEPORT = PORT;
    lcd_gpio.CEPIN = PIN;
}

/*----- Library Functions -----*/

/*
 * @brief Send information to the LCD using configured GPIOs
 * @param val: value to be sent
 */
void LCD_send(uint8_t* val, uint16_t len)
{
    HAL_GPIO_WritePin(lcd_gpio.CEPORT, lcd_gpio.CEPIN, 0);

    for (uint16_t x = 0; x < len; x++)
    {
        for (int8_t i = 7; i >= 0; i--)
        {
            HAL_GPIO_WritePin(lcd_gpio.DATAPORT, lcd_gpio.DATAPIN, (val[x] & (1 << i)));

            HAL_GPIO_WritePin(lcd_gpio.CLKPORT, lcd_gpio.CLKPIN, 1);
            HAL_GPIO_WritePin(lcd_gpio.CLKPORT, lcd_gpio.CLKPIN, 0);
        }
    }

    HAL_GPIO_WritePin(lcd_gpio.CEPORT, lcd_gpio.CEPIN, 1);
}

/*
 * @brief Writes some data into the LCD
 * @param data: data to be written
 * @param mode: command or data
 */
void LCD_write(uint8_t* data, uint8_t mode, uint16_t len)
{
    if (mode == LCD_COMMAND)
    {
        HAL_GPIO_WritePin(lcd_gpio.DCPORT, lcd_gpio.DCPIN, GPIO_PIN_RESET);
        LCD_send(data, len);
    }
    else
    {
        HAL_GPIO_WritePin(lcd_gpio.DCPORT, lcd_gpio.DCPIN, GPIO_PIN_SET);
        LCD_send(data, len);
    }
}

/*
 * @brief Writes some data into the LCD
 * @param data: data to be written
 * @param mode: command or data
 */
void LCD_write_byte(uint8_t data, int8_t mode)
{
    LCD_write(&data, mode, 1);
}

/*
 * @brief Initialize the LCD using predetermined values
 */
void LCD_init()
{
    HAL_GPIO_WritePin(lcd_gpio.RSTPORT, lcd_gpio.RSTPIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(lcd_gpio.RSTPORT, lcd_gpio.RSTPIN, GPIO_PIN_SET);
    HAL_GPIO_WritePin(lcd_gpio.CEPORT, lcd_gpio.CEPIN, GPIO_PIN_SET);

    LCD_write_byte(0x21, LCD_COMMAND);                   //LCD extended commands.
    LCD_write_byte(0xB8, LCD_COMMAND);                   //set LCD Vop(Contrast).
    LCD_write_byte(0x04, LCD_COMMAND);                   //set temp coefficent.
    LCD_write_byte(0x14, LCD_COMMAND);                   //LCD bias mode 1:40.
    LCD_write_byte(0x20, LCD_COMMAND);                   //LCD basic commands.
    LCD_write_byte(LCD_DISPLAY_NORMAL, LCD_COMMAND);     //LCD normal.

    LCD_clear();
}

/*
 * @brief Puts one char on the current position of LCD's cursor
 * @param c: char to be printed
 */
void LCD_putChar(char c)
{
    unsigned char * s = &font_8x6.font[(c-0x20) * font_8x6.WIDTH];
    LCD_write(s, LCD_DATA, font_8x6.WIDTH);
}

/*
 * @brief Print a large string on the LCD
 * @param x: starting point on the x-axis (column)
 * @param y: starting point on the y-axis (line)
 */
void LCD_print_large(char *str, uint8_t x, uint8_t y)
{
    int len = strlen(str);
    unsigned char *s;

    // Loop over characters
    for (int i = 0; i < len; i++)
    {
        s = &font_32x16.font[(((unsigned char) str[i]) - 32) * 64];

        for(int j = 0; j < 4; j++)
        {
            LCD_goXY(x + (i * font_32x16.WIDTH), y + j);
            LCD_write(s + font_32x16.WIDTH * j, LCD_DATA, font_32x16.WIDTH);
        }
    }
}

/*
 * @brief Print a string on the LCD
 * @param x: starting point on the x-axis (column)
 * @param y: starting point on the y-axis (line)
 */
void LCD_print(char *str, uint8_t x, uint8_t y)
{
    LCD_goXY(x, y);

    while (*str)
    {
        LCD_putChar(*str++);
    }
}

/*
 * @brief Clear the screen
 */
void LCD_clear()
{
    uint8_t clear[LCD_SIZE] = {0x00};
    LCD_write(clear, LCD_DATA, LCD_SIZE);
}

/*
 * @brief Set LCD's cursor to position X,Y
 * @param x: position on the x-axis (column)
 * @param y: position on the y-axis (line)
 */
void LCD_goXY(uint8_t x, uint8_t y)
{
    LCD_write_byte(LCD_SETXADDR | x, LCD_COMMAND); //Column.
    LCD_write_byte(LCD_SETYADDR | y, LCD_COMMAND); //Row.
}

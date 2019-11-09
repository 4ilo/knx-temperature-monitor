/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     examples
 * @{
 *
 * @file
 * @brief       Hello World application
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @author      Ludwig Knüpfer <ludwig.knuepfer@fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>
#include <xtimer.h>
#include <periph/gpio.h>

#define STDIO_UART_DEV 0

int main(void)
{
//    puts("Hello World!");

//    printf("You are running RIOT on a(n) %s board.\n", RIOT_BOARD);
//    printf("This board features a(n) %s MCU.\n", RIOT_MCU);
//        xtimer_init();

        gpio_init(GPIO_PIN(PORT_A, 4), GPIO_OUT);
        gpio_init(GPIO_PIN(PORT_A, 5), GPIO_OUT);

        gpio_set(GPIO_PIN(PORT_A, 4));
        gpio_clear(GPIO_PIN(PORT_A, 5));

        while (1) {
                gpio_toggle(GPIO_PIN(PORT_A, 4));
                gpio_toggle(GPIO_PIN(PORT_A, 5));

                xtimer_sleep(1);
        }

    return 0;
}

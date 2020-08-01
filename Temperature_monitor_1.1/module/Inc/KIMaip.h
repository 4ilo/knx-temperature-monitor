/**
 * Olivier Van den Eede (4ilo) 2019
 */

#ifndef KIMAIP_H
#define KIMAIP_H

#define KIMaip_I2C_ADDR (0x08 << 1)
#define KIM_ValueSend 0x10
#define KIM_ValueReceive 0x20

#define KIM_TYPE_BOOL 0
#define KIM_TYPE_FLOAT 1

#include <math.h>
#include "stm32f0xx_hal.h"

typedef struct {
    uint8_t type;
    uint16_t nr;

    uint8_t Bool;
    float Float;
} CommunicationObject;

typedef struct {
    I2C_HandleTypeDef* hi2c;
    uint8_t object_count;
    CommunicationObject **objects;
} KIMaip_ctx;

void KIMaip_Send_Bool(KIMaip_ctx*, uint8_t b, uint16_t objectNr);
void KIMaip_Send_Float(KIMaip_ctx*, float number, uint16_t objectNr);
void KIMaip_Handle_Interrupt(KIMaip_ctx*);

#endif //KIMAIP_H

/*
* Olivier Van den Eede (4ilo) 2019
*/

#include "KIMaip.h"

union {
    float f;
    unsigned char b[4];
} bytes;

/********** Static functions **********/
static void parse_bool(uint8_t data[], uint16_t* objectNr, uint8_t *b)
{
    *objectNr = (data[0] << 8) | data[1];
    *b = data[2];
}

static float half2float(uint16_t half)
{
    uint16_t frac = half & 0x7FF;
    uint8_t exp = (half >> 11) & 0x0F;

    float num = (0.01*frac)*pow(2, exp);

    return num;
}

static void parse_float(uint8_t data[], uint16_t* objectNr, float * number)
{
    uint16_t half;

    *objectNr = (data[0] << 8) | data[1];
    half = (data[2] << 8) | data[3];
    *number = half2float(half);
}


/********** public functions **********/
void KIMaip_Send_Int(KIMaip_ctx *ctx, uint8_t b, uint16_t objectNr)
{
    uint8_t data[5] = {0};

    data[0] = 4;                        // Length is 4
    data[1] = KIM_ValueSend;            // ValueSend command
    data[2] = (objectNr >> 8) & 0xFF;
    data[3] = objectNr & 0xFF;          // Byte 1 and 0 for objectNumber
    data[4] = b;

    HAL_I2C_Master_Transmit(ctx->hi2c, KIMaip_I2C_ADDR, data, 5, 100);
}

void KIMaip_Send_Float(KIMaip_ctx *ctx, float number, uint16_t objectNr)
{
    uint8_t data[8] = {0};

    data[0] = 7;                        // Length is 7
    data[1] = KIM_ValueSend;            // ValueSend command
    data[2] = (objectNr >> 8) & 0xFF;
    data[3] = objectNr & 0xFF;          // Byte 1 and 0 for objectNumber

    bytes.f = number;                   // Convert float to 4 bytes
    data[4] = bytes.b[3];
    data[5] = bytes.b[2];
    data[6] = bytes.b[1];
    data[7] = bytes.b[0];

    HAL_I2C_Master_Transmit(ctx->hi2c, KIMaip_I2C_ADDR, data, 8, 100);
}

void KIMaip_Handle_Interrupt(KIMaip_ctx *ctx)
{
    uint8_t buffer[8];
    HAL_I2C_Master_Receive(ctx->hi2c, KIMaip_I2C_ADDR, buffer, 8, 100);   // Read incomming data

    if (buffer[0] > 1)      // Check if we got a non data byte
    {
        if (buffer[1] == KIM_ValueReceive)
        {
            uint8_t b;
            float number;
            uint16_t objectNr;

            if (buffer[0] == 4)                 // Received bool
            {
                parse_bool(buffer+2, &objectNr, &b);
            }
            else if (buffer[0] == 5)            // Received 2 byte Float
            {
                parse_float(buffer+2, &objectNr, &number);
            }
            else
                return;

            // Store received values
            for(uint8_t i = 0; i < ctx->object_count; i++)
            {
                if(ctx->objects[i]->nr == objectNr)
                {
                    if(ctx->objects[i]->type == KIM_TYPE_BOOL)
                    {
                        ctx->objects[i]->Bool = b;
                    }
                    else if(ctx->objects[i]->type == KIM_TYPE_FLOAT)
                    {
                        ctx->objects[i]->Float = number;
                    }
                }
            }
        }
    }
}

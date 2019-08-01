#ifndef _XST_I2C_H
#define _XST_I2C_H

#include "XST_I2C.h"
#include "CMSDK_CM3.h"
#include "utility.h"

#define SDA_PIN 6
#define SCL_PIN 7

#define WRITE 1
#define READ 0

// Result of I2C procedures
typedef enum {
	I2C_ERROR   = 0,
	I2C_SUCCESS = !I2C_ERROR
} I2C_Status;

// Send or not STOP condition
typedef enum {
	I2C_STOP   = 0,
	I2C_NOSTOP = !I2C_STOP
} I2C_STOP_TypeDef;

class XST_I2C
{
    public:
        XST_I2C(I2C_TypeDef* I2C_number = I2C0);//Constructor
        I2C_Status init(uint32_t Clock, uint32_t irq_en);
        void reset(void);
        void beginTransfer(uint8_t addr, uint8_t mode);
        uint8_t check_ack(void);
        void write(uint8_t SlaveAddress,uint8_t RegisterAddress, uint8_t value);
        uint8_t read(uint8_t SlaveAddress, uint8_t RegisterAddress);
    private:
        I2C_TypeDef* I2Cx;
};
#endif


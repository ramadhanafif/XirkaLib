#include "XST_I2C.h"

/******************************
*Private Arguments and Functions
*******************************/
I2C_TypeDef* I2Cx;

/******************************
*Public Arguments and Functions
*******************************/
XST_I2C::XST_I2C(I2C_TypeDef* I2C_number){
    I2Cx = I2C_number;
};

I2C_Status XST_I2C::init(uint32_t Clock, uint32_t irq_en)
{
    //activate gpio alternative function for I2C (SDA, SCL)
    CMSDK_GPIO1->ALTFUNCSET |= (1<<SDA_PIN); 
    CMSDK_GPIO1->ALTFUNCSET |= (1<<SCL_PIN);
    
    //I2C registers reset value
    reset();
    
    //set prescaler for the I2C works at 100kHz
    switch (Clock) {
        case (50000000) : I2Cx->PRER = I2C_PRER_50MHz; break;
        case (25000000) : I2Cx->PRER = I2C_PRER_25MHz; break;
        default : return I2C_ERROR;
    }
    
    //enable/disable i2c interrupt
    if (irq_en!=0)
        I2Cx->CTR = ((I2C_CTR_EN_Msk) | (I2C_CTR_IEN_Msk));
    else
        I2Cx->CTR = I2C_CTR_EN_Msk;
  
    return I2C_SUCCESS;
};

void XST_I2C::reset(void)
{
    I2Cx->PRER  = 0xFFFF;
    I2Cx->CTR   = 0x00;
    I2Cx->TXR   = 0x00;
    I2Cx->CR    = 0x00;
};

void XST_I2C::beginTransfer(uint8_t adr, uint8_t mode)
{
    adr <<= 1;
    if (mode == 1) //write mode, write bit = 0;
        I2Cx->TXR = adr;
    else //read mode, write bit = 1
    {
        adr += 1;
        I2Cx->TXR = adr;
    }

    //set STA it and WR bit
    I2Cx->CR = (I2C_CR_STA_Msk | I2C_CR_WR_Msk);
};

uint8_t XST_I2C::check_ack()
{
    if (I2Cx->SR & I2C_SR_RxACK_Msk) return (1);
    else return (0);
};

uint8_t XST_I2C::read(uint8_t SlaveAddress, uint8_t RegisterAddress)
{
    beginTransfer(SlaveAddress, WRITE);
    while(I2Cx->SR & I2C_SR_TIP_Msk);
    if (check_ack())
    {
        I2Cx->TXR = RegisterAddress;
        I2Cx->CR = I2C_CR_WR_Msk;
        while (I2Cx->SR & I2C_SR_TIP_Msk);
        if (check_ack())
        {
            beginTransfer(SlaveAddress, READ);
            while(I2Cx->SR & I2C_SR_TIP_Msk);
            if (check_ack())
            {
                I2Cx->CR = (I2C_CR_RD_Msk | I2C_CR_STO_Msk | I2C_CR_ACK_Msk);
                while(I2Cx->SR & I2C_SR_TIP_Msk);
                return (I2Cx->RXR);
            }
        }
    }
    return I2C_ERROR;
};

void XST_I2C::write(uint8_t SlaveAddress,uint8_t RegisterAddress, uint8_t value)
{
    beginTransfer(SlaveAddress, WRITE);
    while(I2Cx->SR & I2C_SR_TIP_Msk);
    if (check_ack())
    {
        I2Cx->TXR = RegisterAddress;
        I2Cx->CR = I2C_CR_WR_Msk;
        while (I2Cx->SR & I2C_SR_TIP_Msk);
        if (check_ack())
        {
            I2Cx->TXR = value;
            I2Cx->CR = (I2C_CR_WR_Msk | I2C_CR_ACK_Msk | I2C_CR_STO_Msk);
            while(I2Cx->SR & I2C_SR_TIP_Msk);
        }
    }
}

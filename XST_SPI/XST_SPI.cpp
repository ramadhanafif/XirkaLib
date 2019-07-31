#include "XST_SPI.h"

SPI_TypeDef *SPIx;

XST_SPI::XST_SPI(SPI_TypeDef * SPI_nomer) { //constructor
  SPIx = SPI_nomer;
}

void XST_SPI::reset(void)
{
  SPIx->SPCR = 0x10;
  SPIx->SPSR = 0x05;
  SPIx->SPDR = 0x00;
  SPIx->SPER = 0x00;
}

void XST_SPI::begin(void)
{
  CMSDK_GPIO1->ALTFUNCSET = (1 << MOSI) | (1 << SCK) | (1 << MISO);
  CMSDK_GPIO0->OUTENABLESET = (1 << CS);
  CMSDK_GPIO1->OUTENABLESET |= (1 << MOSI) | (1 << SCK);
  CMSDK_GPIO1->OUTENABLESET &= ~1 << MISO;

  settings(); //set default settings

  core_enable(); //core enable
}

void XST_SPI::core_enable(bool is_enable)
{
  if (is_enable)
    SPIx->SPCR |= SPE; //core enable
  else
    SPIx->SPCR &= ~SPE; //core enable
}

//JANGAN LUPA ENABLE CORE SETELAH UBAH SETTING
bool XST_SPI::settings(bool IS_MASTER, uint8_t MODE, uint8_t CLK_DIV_MODE) {
  //SPI Default Settings: Master, CPOL CPHA Mode 3, SCK Divisor Mode 3
  reset();
  if (IS_MASTER)
    SPIx->SPCR |= MSTR;
  else
    SPIx->SPCR &= ~MSTR;

  if (MODE > 0x3)
    //Wrong CPOL CPHA mode
    return 0;
  else
    SPIx->SPCR |= MODE << 2;

  if (CLK_DIV_MODE > 0x3)
    //Wrong SCK divisor
    return 0;
  else
    SPIx->SPCR |= CLK_DIV_MODE;

  //Setting success
  return 1;
}

/* void SPI_interrupt_Enable(SPI_TypeDef *SPI, uint8_t ien_irq)
  {
    if (ien_irq)
        SPI->SPCR |= (SPI_SPCR_SPIE_Msk);
    else
        SPI->SPCR &= ~(SPI_SPCR_SPIE_Msk);
  } */

void XST_SPI::transfer(uint8_t data)
{
  CMSDK_GPIO0->DATAOUT &= ~1 << 0; //chip select enable
  SPIx->SPDR = data;  //transfer the control byte.
  delaymicros(1); //for stability
  while (!(SPIx->SPSR & WFEMPTY)); //do nothing if write fifo is full
  end_transfer();

}

void XST_SPI::end_transfer(void)
{
  __ISB();//small delay
  CMSDK_GPIO0->DATAOUT |= (1 << CS); //chip select disable
  SPIx->SPCR &= ~(1 << SPE); //disable core, reset fifo
}

/* void SPI_write_byte(SPI_TypeDef *SPIx, uint8_t RegisterAddress, uint8_t Data)
  {
    uint8_t control_byte = (RegisterAddress &= ~(1U<<7)); //set control bit (bit 7) for write ('0');
    SPI_Begin_Transfer(SPI, control_byte);
    SPIx->SPDR = Data;
    while(!(SPIx->SPSR & SPI_SPSR_WFEMPTY_Msk));
    SPI_End_Transfer(SPI);
  }

  uint8_t SPI_read_Byte(SPI_TypeDef *SPI, uint8_t RegisterAddress)
  {
    uint8_t dummy = 0x00;
    uint8_t response;
    uint8_t control_byte = (RegisterAddress |= (1UL<<7)); //set control bit (bit 7) for read ('1')
    SPI_Begin_Transfer(SPI, control_byte);
    SPIx->SPDR = dummy;
    while (!(SPIx->SPSR & SPI_SPSR_WFEMPTY_Msk));
    while (!(SPIx->SPSR & SPI_SPSR_RFEMPTY_Msk))
    {
        response = SPIx->SPDR;
    }
    SPI_End_Transfer(SPIx);
    return response;
  } */

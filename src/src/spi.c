#include <stdio.h>
#include <stdint.h>

#include "stm32f10x_spi.h"
#include "system.h"
#include "io.h"

#define SPI_PORT        SPI1

void spi_init(void)
{
    SPI_InitTypeDef spi1_init;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
    io_init(IO_PIN_SPI_SCK, IO_MODE_ALTERNATE);
    io_init(IO_PIN_SPI_MOSI, IO_MODE_ALTERNATE);
    io_init(IO_PIN_SPI_MISO, IO_MODE_INPUT);

    spi1_init.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;
    spi1_init.SPI_CPOL = SPI_CPOL_Low;
    spi1_init.SPI_CPHA = SPI_CPHA_1Edge;
    spi1_init.SPI_DataSize = SPI_DataSize_8b;
    spi1_init.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    spi1_init.SPI_FirstBit = SPI_FirstBit_MSB;
    spi1_init.SPI_Mode = SPI_Mode_Master;
    spi1_init.SPI_NSS = SPI_NSS_Soft;
    SPI_Init(SPI_PORT, &spi1_init);

    SPI_Cmd(SPI_PORT, ENABLE);
}

uint8_t spi_data(uint8_t val)
{
    while (!SPI_I2S_GetFlagStatus(SPI_PORT, SPI_I2S_FLAG_TXE));

    SPI_I2S_SendData(SPI_PORT, val);

    while (SPI_I2S_GetFlagStatus(SPI_PORT, SPI_I2S_FLAG_RXNE));

    return SPI_I2S_ReceiveData(SPI_PORT);
}
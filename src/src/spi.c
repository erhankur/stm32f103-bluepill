#include <stdio.h>
#include <stdint.h>

#include "stm32f10x_spi.h"
#include "system.h"
#include "io.h"

#define HW_SPI_PORT        SPI1

void sw_spi_delay(void)
{

}

void sw_spi_init(void)
{
    io_init(IO_PIN_SPI_SCK, IO_MODE_OUTPUT);
    io_init(IO_PIN_SPI_MOSI, IO_MODE_OUTPUT);
    io_init(IO_PIN_SPI_MISO, IO_MODE_INPUT);
}

uint8_t sw_spi_data(uint8_t val)
{
    for (int i = 0; i < 8; ++i) {
        // MSB first send recv
        io_write(IO_PIN_SPI_MOSI, val & 0x80 ? 1 : 0);
        val <<= 1; // val = val << 1;
        sw_spi_delay();       // Data setup time

        io_write(IO_PIN_SPI_SCK, 1);   // SCK idle -> active
        sw_spi_delay();       // Clock high time

        // data - sampling
        if (io_read(IO_PIN_SPI_MISO)) {
            val |= 1;
        }

        io_write(IO_PIN_SPI_SCK, 0);   // SCK active -> idle
        sw_spi_delay();
    }

    return val;
}

void hw_spi_init(void)
{
    SPI_InitTypeDef spi1_init;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
    io_init(IO_PIN_SPI_SCK, IO_MODE_ALTERNATE);
    io_init(IO_PIN_SPI_MOSI, IO_MODE_ALTERNATE);
    io_init(IO_PIN_SPI_MISO, IO_MODE_INPUT);

    spi1_init.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;  // 72/4 = 18mhz
    spi1_init.SPI_CPOL = SPI_CPOL_Low;
    spi1_init.SPI_CPHA = SPI_CPHA_1Edge;
    spi1_init.SPI_DataSize = SPI_DataSize_8b;
    spi1_init.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    spi1_init.SPI_FirstBit = SPI_FirstBit_MSB;
    spi1_init.SPI_Mode = SPI_Mode_Master;
    spi1_init.SPI_NSS = SPI_NSS_Soft;
    SPI_Init(HW_SPI_PORT, &spi1_init);

    SPI_Cmd(HW_SPI_PORT, ENABLE);
}

uint8_t hw_spi_data(uint8_t val)
{
    while (!SPI_I2S_GetFlagStatus(HW_SPI_PORT, SPI_I2S_FLAG_TXE));

    SPI_I2S_SendData(HW_SPI_PORT, val);

    while (SPI_I2S_GetFlagStatus(HW_SPI_PORT, SPI_I2S_FLAG_RXNE));

    return SPI_I2S_ReceiveData(HW_SPI_PORT);
}
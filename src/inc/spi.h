#ifndef _SPI_H_
#define _SPI_H_

#include <stdint.h>

void hw_spi_init(void);
uint8_t hw_spi_data(uint8_t val);

void sw_spi_init(void);
uint8_t sw_spi_data(uint8_t val);

#define SPI_INIT    sw_spi_init
#define SPI_DATA    sw_spi_data

#endif

#include <stdint.h>

#define PERIPH_BASE           ((uint32_t)0x40000000) /*!< Peripheral base address in the alias region */
#define APB1PERIPH_BASE       PERIPH_BASE
#define APB2PERIPH_BASE       (PERIPH_BASE + 0x10000)
#define AHBPERIPH_BASE        (PERIPH_BASE + 0x20000)

#define RCC_BASE              (AHBPERIPH_BASE + 0x1000)
#define GPIOC_BASE            (APB2PERIPH_BASE + 0x1000)

#define IOP_LED_PORT          GPIOC
#define IOP_LED_PIN           13

#define __IO    volatile       /*!< Defines 'read / write' permissions */

typedef struct {
    __IO uint32_t CR;
    __IO uint32_t CFGR;
    __IO uint32_t CIR;
    __IO uint32_t APB2RSTR;
    __IO uint32_t APB1RSTR;
    __IO uint32_t AHBENR;
    __IO uint32_t APB2ENR;
    __IO uint32_t APB1ENR;
    __IO uint32_t BDCR;
    __IO uint32_t CSR;
} RCC_TypeDef;

typedef struct {
    __IO uint32_t CRL;
    __IO uint32_t CRH;
    __IO uint32_t IDR;
    __IO uint32_t ODR;
    __IO uint32_t BSRR;
    __IO uint32_t BRR;
    __IO uint32_t LCKR;
} GPIO_TypeDef;

#define RCC_APB2ENR_IOPCEN      ((uint32_t)0x00000010)         /*!< I/O port C clock enable */
#define GPIOC                   ((GPIO_TypeDef *)GPIOC_BASE)
#define RCC                     ((RCC_TypeDef *)RCC_BASE)

void Delay(void)
{
    volatile int i = 0;
    for (i = 0; i < 1000000; i++)
        ;
}

int main()
{
    int bitOffset;

    RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;   // GPIOC clock enable

    if (IOP_LED_PIN < 8) {
        bitOffset = (IOP_LED_PIN - 8) * 4;  // GPIOC->CRH register'inda C13'e karsilik gelen bit konumu
        GPIOC->CRL &= ~(0xF << bitOffset);
        GPIOC->CRL |= (3 << bitOffset);
    } else {
        bitOffset = (IOP_LED_PIN - 8) * 4;
        GPIOC->CRH &= ~(0xF << bitOffset);   // GPIOC->CRH<23:20> &= 0000       ..000111100... 11110000111
        GPIOC->CRH |= (3 << bitOffset);      // GPIOC->CRH<23:20> |= 0011      // *(uint32_t *)0x40011004 = ...
    }

    GPIOC->ODR |= (1 << IOP_LED_PIN);
    GPIOC->ODR &= ~(1 << IOP_LED_PIN);
    GPIOC->ODR |= (1 << IOP_LED_PIN);
    GPIOC->ODR &= ~(1 << IOP_LED_PIN);

    GPIOC->BSRR = (1 << (IOP_LED_PIN + 16));    // DR<13> = 1
    GPIOC->BSRR = (1 << IOP_LED_PIN);           // DR<13> = 1
    GPIOC->BRR = (1 << IOP_LED_PIN);            // DR<13> = 1
    GPIOC->BSRR = (1 << IOP_LED_PIN);           // DR<13> = 1

    while (1) {
        GPIOC->ODR ^= (1 << IOP_LED_PIN);
        Delay();
    }
}

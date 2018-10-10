#include "stm32f4xx_ll_rcc.h"
#include "stm32f4xx_ll_system.h"
#include "stm32f4xx_ll_bus.h"
#include "stm32f4xx_ll_gpio.h"
#include "stm32f4xx_ll_usart.h"
#include <stdio.h>

#include "FreeRTOS.h"
#include "task.h"
#include "io_syscall.h"

/**
  *   System Clock Configuration
  *   The system Clock is configured as follow :
  *   System Clock source            = PLL (HSE)
  *   SYSCLK(Hz)                     = 168000000
  *   HCLK(Hz)                       = 168000000
  *   AHB Prescaler                  = 1
  *   APB1 Prescaler                 = 4
  *   APB2 Prescaler                 = 2
  *   HSE Frequency(Hz)              = 8000000
  *   PLL_M                          = 8
  *   PLL_N                          = 336
  *   PLL_P                          = 2
  *   VDD(V)                         = 3.3
  *   Main regulator output voltage  = Scale1 mode
  *   Flash Latency(WS)              = 5
  */
static void rcc_config()
{
        /* Enable HSE oscillator */
        LL_RCC_HSE_Enable();
        while(LL_RCC_HSE_IsReady() != 1);

        /* Set FLASH latency */
        LL_FLASH_SetLatency(LL_FLASH_LATENCY_5);

        /* Main PLL configuration and activation */
        LL_RCC_PLL_ConfigDomain_SYS(LL_RCC_PLLSOURCE_HSE, LL_RCC_PLLM_DIV_8,
                                    336, LL_RCC_PLLP_DIV_2);
        LL_RCC_PLL_Enable();
        while(LL_RCC_PLL_IsReady() != 1);

        /* Sysclk activation on the main PLL */
        LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
        LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL);
        while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL);

        /* Set APB1 & APB2 prescaler */
        LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_4);
        LL_RCC_SetAPB2Prescaler(LL_RCC_APB2_DIV_2);

        /* Set systick to 1ms */
        SysTick_Config(168000000/1000);

        /* Update CMSIS variable (which can be updated also
         * through SystemCoreClockUpdate function) */
        SystemCoreClock = 168000000;
}

static void uart2_config(void)
{
        /*
         * Setting UART pins
         */
        LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA);
        //USART1_TX
        LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_2, LL_GPIO_MODE_ALTERNATE);
        LL_GPIO_SetAFPin_0_7(GPIOA, LL_GPIO_PIN_2, LL_GPIO_AF_7);
        LL_GPIO_SetPinSpeed(GPIOA, LL_GPIO_PIN_2, LL_GPIO_SPEED_FREQ_HIGH);
        //USART1_RX
        LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_3, LL_GPIO_MODE_ALTERNATE);
        LL_GPIO_SetAFPin_0_7(GPIOA, LL_GPIO_PIN_3, LL_GPIO_AF_7);
        LL_GPIO_SetPinSpeed(GPIOA, LL_GPIO_PIN_3, LL_GPIO_SPEED_FREQ_HIGH);
        /*
         * USART Set clock source
         */
        LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_USART2);
        /*
         * USART Setting
         */
        LL_USART_SetTransferDirection(USART2, LL_USART_DIRECTION_TX_RX);
        LL_USART_SetParity(USART2, LL_USART_PARITY_NONE);
        LL_USART_SetDataWidth(USART2, LL_USART_DATAWIDTH_8B);
        LL_USART_SetStopBitsLength(USART2, LL_USART_STOPBITS_1);
        LL_USART_SetBaudRate(USART2, SystemCoreClock/4,
                             LL_USART_OVERSAMPLING_16, 115200);
        /*
         * USART turn on
         */
        LL_USART_Enable(USART2);
        return;
}

static void gpio_config(void)
{
        LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOD);
        LL_GPIO_SetPinMode(GPIOD, LL_GPIO_PIN_13, LL_GPIO_MODE_OUTPUT);
}

void test_FPU_test(void *p)
{
        (void) p;
        int num1 = 0;
        int num2 = 0;
        int ret = 0;

        while (1) {
                ret = scanf("%d %d", &num1, &num2);
                printf("You typed: %d %d with code %d\n", num1, num2, ret);
                LL_GPIO_TogglePin(GPIOD, LL_GPIO_PIN_13);
                vTaskDelay(1000);
        }
        return;
}

StackType_t fpuTaskStack[1024];
StaticTask_t fpuTaskBuffer;

int main(void)
{
        rcc_config();
        gpio_config();
        uart2_config();
        set_chardev(USART2);
        NVIC_SetPriorityGrouping(4U);

        xTaskCreateStatic(test_FPU_test, "FPU", 1024, NULL, 1, fpuTaskStack,
                          &fpuTaskBuffer);
        vTaskStartScheduler();

        while (1);
        return 0;
}

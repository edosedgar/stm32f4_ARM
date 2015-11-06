#include "stm32f4xx_conf.h"
#include "image.h"

#define GreenLed  GPIO_ODR_ODR_12
#define OrangeLed GPIO_ODR_ODR_13
#define RedLed    GPIO_ODR_ODR_14
#define BlueLed   GPIO_ODR_ODR_15

// Private variables
volatile uint32_t time_var1, time_var2;

// Private function prototypes
void Delay(volatile uint32_t nCount);
void Init_GPIO();
void Init_SPI();
void Init_RNG();

int main(void) {
	if (SysTick_Config(SystemCoreClock / 1000)) {
		while (1){};
	}
    Init_GPIO();
    Init_SPI();
    Init_RNG();
    Init_LCD();
    
    int counter=1;
    int counter2 =0;
    int x = 0;
    int y = 0;
    DrawFillRectangle(1,1,128,160,0xFFFFFF);
    for ( x = 160; x > 0; x-- )
        for ( y = 1; y <= 84; y++ ) 
            DrawFillRectangle(y+22, x, 1, 1, Image[(y-1)*160+161-x]-0x010101);
    while(1) {

    
        //DrawFillRectangle(RNG->DR%128,RNG->DR%160,RNG->DR%128,RNG->DR%160,RNG->DR);
        /*if ( counter2 == 60000 ) {
            DrawFillRectangle(counter-1, counter-1, 20, 20, 0x000000);
            DrawFillRectangle(counter, counter, 20, 20, RNG->DR);
            //DrawFillRectangle(1,1,128,160,0x00000);
            counter+=1;
            counter2=0;
        }
        counter2+=1;
        if ( counter == 108 ) {
            DrawFillRectangle(1,1,128,160,0x00000);
            counter = 0;
        }*/
        /*GPIOD->ODR^=GreenLed;
        Delay(1);
        GPIOD->ODR^=OrangeLed;
        Delay(1);
        GPIOD->ODR^=RedLed;
        Delay(1);
        GPIOD->ODR^=BlueLed;
        Delay(1);*/
    }
	return 0;
}

void Init_GPIO() {
    //Initialization pins with leds
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN;
    GPIOD->MODER = GPIO_MODER_MODER15_0 | GPIO_MODER_MODER14_0 | GPIO_MODER_MODER13_0 | GPIO_MODER_MODER12_0;
}
        
void Init_SPI() {   
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
    GPIOA->MODER |= GPIO_MODER_MODER5_1 | GPIO_MODER_MODER7_1;
    GPIOA->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR5 | GPIO_OSPEEDER_OSPEEDR7;
    GPIOA->AFR[0] |= (GPIO_AF_SPI1<<28)|(GPIO_AF_SPI1<<20);
                        
    RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;                    
    SPI1->CR1 = SPI_CR1_DFF | SPI_CR1_MSTR | SPI_CR1_SSI | SPI_CR1_SSM | SPI_CR1_BR_0 | SPI_CR1_CPHA | SPI_CR1_CPOL;
    SPI1->CR1 |= SPI_CR1_SPE; 
}

void Init_RNG() {
    RCC->AHB2ENR |= RCC_AHB2ENR_RNGEN;
    RNG->CR=RNG_CR_RNGEN;
}

/*
 * Called from systick handler
 */

void timing_handler() {
	if (time_var1) {
		time_var1--;
	}

	time_var2++;
}

/*
 * Delay a number of systick cycles (1ms)
 */
void Delay(volatile uint32_t nCount) {
	time_var1 = nCount;
	while(time_var1){};
}

void _init(){

}

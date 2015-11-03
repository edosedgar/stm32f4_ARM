#include "stm32f4xx.h"

#define SWRESET     0x0001 //Software Reset
#define SLPIN       0x0010 //Sleep In
#define SLPOUT      0x0011 //Sleep Out
#define PTLON       0x0012 //Partial Display Mode On
#define NORON       0x0013 //Normal Display Mode On
#define INVOFF      0x0020 //Display Inversion Off
#define INVON       0x0021 //Display Inversion On
#define DISPOFF     0x0028 //Display Off
#define DISPON      0x0029 //Display On
#define CASET       0x002A //Column Address Set
    //1 Byte = X Start (High)
    //2 Byte = X Start (Low)
    //3 Byte = X End   (High)
    //4 Byte = X End   (Low)
#define RASET       0x002B //Row Address Set
    //1 Byte = Y Start (High)
    //2 Byte = Y Start (Low)
    //3 Byte = Y End   (High)
    //4 Byte = Y End   (Low)
#define RAMWR       0x002C //Memory Write
    //1-N Byte = Data about color pixel
#define TEOFF       0x0034 //Tearing Effect Line OFF
#define TEON        0x0035 //Tearing Effect Line ON
    //1 Byte:
        //D0 = TELOM ( 0 - Only VSync; 1 - VSync and HSync )
#define MADCTR      0x0036 //Memory Data Access Control
    //1 Byte:
        //D7 (MY - Row Address Order)
        //D6 (MX - Column Address Order)
        //D5 (MV - Row/Column Exchange)
        //D4 (ML - Vertical Refresh Order)
        //    LCD vertical refresh direction control
        //    0 = LCD vertical refresh Top to Bottom
        //    1 = LCD vertical refresh Bottom to Top
        //D3 (RGB  - RGB-BGR ORDER)
        //    Color selector switch control
        //    0 =RGB color filter panel,
        //    1 =BGR color filter panel)
#define IDMOFF      0x0038 //Idle Mode Off
#define IDMON       0x0039 //Idle Mode On (8 color mode)
#define COLMOD      0x003A //Interface Pixel Format
    //1 Byte = 0x0075


void SendToLCD(uint16_t Packet)
{
    uint8_t i;
    //while (!(SPI1->SR & SPI_SR_TXE)) {};
    GPIOA->BSRRH=GPIO_BSRR_BR_3>>16; // Reset CS
    Packet=Packet<<7;
    SPI1->DR=Packet;
    for (i=9;i!=0;i--) {};
    GPIOA->BSRRL=GPIO_BSRR_BS_3; // Set CS
}

void DrawFillRectangle(uint8_t XRect,uint8_t YRect,uint8_t WidthRect,uint8_t HeightRect,uint32_t ColorRect)
{
    if ((YRect>160)|(YRect+HeightRect>170)) {return;};
    if ((XRect>128)|(XRect+WidthRect>138)) {return;};
    SendToLCD(CASET);
        SendToLCD(0x0100);
        SendToLCD(0x0100+XRect+1);
        SendToLCD(0x0100);
        SendToLCD(0x0100+XRect+WidthRect);
    SendToLCD(RASET);
        SendToLCD(0x0100);
        SendToLCD(0x0100+YRect);
        SendToLCD(0x0100);
        SendToLCD(0x0100+YRect+HeightRect-1);
    //Convert 24-bit color to 16-bit color
    uint8_t b = ColorRect;
    uint8_t g = ColorRect>>8;
    uint8_t r = ColorRect>>16;
    uint16_t Byte1 = 0x0100+((r&0xF8)|(g>>5));
    uint16_t Byte2 = 0x0100+(((g<<3)&0xE0)|(b>>3));
    uint16_t i,CountPixels;
    SendToLCD(RAMWR);
    CountPixels=(HeightRect)*(WidthRect);
    for (i=1;i<=CountPixels;i++)
    {
        SendToLCD(Byte1);
        SendToLCD(Byte2);
    }
}

void Init_LCD()
{
    uint16_t i;
    //Initialization CS pin
    GPIOA->MODER |= GPIO_MODER_MODER3_0;
    GPIOA->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR3;
    //---------------------
    for (i=10000;i!=0;i--) {};
	SendToLCD(SWRESET);
	for (i=10000;i!=0;i--) {};
	SendToLCD(SLPOUT);
	SendToLCD(NORON);
	SendToLCD(INVOFF);
	SendToLCD(COLMOD); SendToLCD(0x0105);
	SendToLCD(MADCTR); SendToLCD(0x01C0);
	SendToLCD(IDMOFF);
	DrawFillRectangle(1,1,128,160,0x000000);
	SendToLCD(DISPON);
}

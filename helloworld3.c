#include <stdio.h>
#include "platform.h"
#include "xparameters.h"
#include "xspips.h"
#include "xgpiops.h"
#include "xil_printf.h"


#define GPIO_DEVICE_ID 	XPAR_XGPIOPS_0_DEVICE_ID
#define SpiPs_RecvByte(BaseAddress) \
		(u8)XSpiPs_In32((BaseAddress) + XSPIPS_RXD_OFFSET)

XSpiPs Spi;
XGpioPs Gpio;

u32 Input_Pin, Output_Pin_1, Output_Pin_2;
u8 ReadBuffer[100];

int main()
{
    init_platform();
    u8 *BufferPtr;
    u32 StatusReg;
    XSpiPs_Config* SpiCfg;
    SpiCfg = XSpiPs_LookupConfig(XPAR_XSPIPS_0_DEVICE_ID);
    XSpiPs_CfgInitialize(&Spi ,SpiCfg, SpiCfg->BaseAddress);
    XSpiPs_SetOptions(&Spi,(XSPIPS_CR_CPHA_MASK) | \
			(XSPIPS_CR_CPOL_MASK));
    while(1)
    {
    	memset(&ReadBuffer, 0x00, sizeof(ReadBuffer));

    	XSpiPs_SetRXWatermark((&Spi),100);

    	XSpiPs_Enable((&Spi));

        StatusReg = XSpiPs_ReadReg(Spi.Config.BaseAddress,XSPIPS_SR_OFFSET);
        do{

        	StatusReg = XSpiPs_ReadReg(Spi.Config.BaseAddress,XSPIPS_SR_OFFSET);

        }while(!(StatusReg & XSPIPS_IXR_RXNEMPTY_MASK));

        for(int i = 0; i < 100; i++){

        	ReadBuffer[i] = SpiPs_RecvByte(Spi.Config.BaseAddress);

        }
        BufferPtr = ReadBuffer;
        XSpiPs_Disable((&Spi));
        xil_printf("Buffer=%d \r\n",BufferPtr);
    }
    cleanup_platform();
    return 0;
}


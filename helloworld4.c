#include "xparameters.h"
#include "xspips.h"
#include "xil_printf.h"
#include "platform.h"
#include "xgpiops.h"

static XSpiPs Spi;
static XGpioPs Gpio;
int main(void)
{
	init_platform();
	u8 BufferA = 0xAA;
	u8 BufferB = 0xBB;

	u32 Output_1, Output_2,Input_Pin;

	XGpioPs_Config *GpioCfg;
	GpioCfg = XGpioPs_LookupConfig(XPAR_XGPIOPS_0_DEVICE_ID);
	XGpioPs_CfgInitialize(&Gpio, GpioCfg,GpioCfg->BaseAddr);

	Input_Pin=0;
	Output_1=52;
	Output_2=53;
	XGpioPs_SetDirectionPin(&Gpio, Input_Pin, 0x0);
	XGpioPs_SetDirectionPin(&Gpio, Output_1, 1);
	XGpioPs_SetOutputEnablePin(&Gpio, Output_1, 1);
	XGpioPs_SetDirectionPin(&Gpio, Output_2, 1);
	XGpioPs_SetOutputEnablePin(&Gpio, Output_2, 1);

	XSpiPs_Config *SpiCfg;
	SpiCfg = XSpiPs_LookupConfig(XPAR_XSPIPS_0_DEVICE_ID);
	XSpiPs_CfgInitialize(&Spi, SpiCfg, SpiCfg->BaseAddress);
	XSpiPs_SelfTest(&Spi);
	XSpiPs_SetOptions(&Spi, XSPIPS_MASTER_OPTION |XSPIPS_FORCE_SSELECT_OPTION);
	XSpiPs_SetClkPrescaler(&Spi, XSPIPS_CLK_PRESCALE_64);

	while(1){
		XSpiPs_SetSlaveSelect(&Spi, 0x00);
		XSpiPs_PolledTransfer(&Spi, &BufferA, NULL, 1);
		xil_printf("BufferA = %d\r\n", BufferA);
		XGpioPs_WritePin(&Gpio, Output_1, 0x0);	
		XGpioPs_WritePin(&Gpio, Output_2, 0x1);

		if(XGpioPs_ReadPin(&Gpio, Input_Pin)==1)
		    	{
		           	XSpiPs_SetSlaveSelect(&Spi, 0x01);
					XSpiPs_PolledTransfer(&Spi, &BufferB, NULL, 1);
					xil_printf("BufferA = %d\r\n", BufferB);
					XGpioPs_WritePin(&Gpio, Output_1, 0x1);		
					XGpioPs_WritePin(&Gpio, Output_2, 0x0);
		    	}
		}
    cleanup_platform();
    return 0;
}
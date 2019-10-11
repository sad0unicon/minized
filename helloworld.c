/******************************************************************************
*
* Copyright (C) 2009 - 2014 Xilinx, Inc.  All rights reserved.
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* Use of the Software is limited solely to applications:
* (a) running on a Xilinx device, or
* (b) that interact with a Xilinx device through a bus or interconnect.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
* XILINX  BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
* WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF
* OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*
* Except as contained in this notice, the name of the Xilinx shall not be used
* in advertising or otherwise to promote the sale, use or other dealings in
* this Software without prior written authorization from Xilinx.
*
******************************************************************************/

/*
 * helloworld.c: simple test application
 *
 * This application configures UART 16550 to baud rate 9600.
 * PS7 UART (Zynq) is not initialized by this application, since
 * bootrom/bsp configures it to baud rate 115200
 *
 * ------------------------------------------------
 * | UART TYPE   BAUD RATE                        |
 * ------------------------------------------------
 *   uartns550   9600
 *   uartlite    Configurable only in HW design
 *   ps7_uart    115200 (configured by bootrom/bsp)
 */

#include <stdio.h>
#include <time.h>
#include "platform.h"
#include "xil_printf.h"
#include "xgpiops.h"
#include "xparameters.h"

#define GPIO_DEVICE_ID  	XPAR_XGPIOPS_0_DEVICE_ID
#define LED_DELAY		10000000
int main()
{
    init_platform();
    u32 Output_1, Output_2,Input_Pin,c;
    XGpioPs Gpio;
    XGpioPs_Config *ConfigPtr;
    ConfigPtr = XGpioPs_LookupConfig(GPIO_DEVICE_ID);
    XGpioPs_CfgInitialize(&Gpio, ConfigPtr,ConfigPtr->BaseAddr);


    Input_Pin=0;
    c=0;
    Output_1=52;
    Output_2=53;
    XGpioPs_SetDirectionPin(&Gpio, Input_Pin, 0x0);
    XGpioPs_SetDirectionPin(&Gpio, Output_1, 1);
    XGpioPs_SetOutputEnablePin(&Gpio, Output_1, 1);
    XGpioPs_SetDirectionPin(&Gpio, Output_2, 1);
    XGpioPs_SetOutputEnablePin(&Gpio, Output_2, 1);

    while(1)
    {
    	if(XGpioPs_ReadPin(&Gpio, Input_Pin)==1)
    	{
    		c=Output_1;
    		Output_1=Output_2;
    		Output_2=c;
    	}
    XGpioPs_WritePin(&Gpio, Output_1, 0x0);
    XGpioPs_WritePin(&Gpio, Output_2, 0x1);
    sleep(1);
    }
    print("Hello World\n\r");

    cleanup_platform();
    return 0;
}

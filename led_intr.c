#include "xparameters.h" 
#include "xgpiops.h" 
#include "xscugic.h" 
#include "xil_exception.h" 
#include "xplatform_info.h" 
#include <xil_printf.h> 

#define GPIO_DEVICE_ID XPAR_XGPIOPS_0_DEVICE_ID 
#define INTC_DEVICE_ID XPAR_SCUGIC_SINGLE_DEVICE_ID 
#define GPIO_INTERRUPT_ID XPAR_XGPIOPS_0_INTR 

#define GPIO_BANK XGPIOPS_BANK0 /* Bank 0 of the GPIO Device */ 

static int GpioIntrExample(XScuGic *Intc, XGpioPs *Gpio, u16 DeviceId, 
u16 GpioIntrId); 
static void IntrHandler(void *CallBackRef, u32 Bank, u32 Status); 
static int SetupInterruptSystem(XScuGic *Intc, XGpioPs *Gpio, u16 GpioIntrId); 

static XGpioPs Gpio; /* The Instance of the GPIO Driver */ 

static XScuGic Intc; /* The Instance of the Interrupt Controller Driver */ 

static u32 AllButtonsPressed; /* Intr status of the bank */ 
static u32 Input_Pin; /* Switch button */ 
static u32 Output_1; /* LED button */ 
static u32 Output_2; 

int main(void) 
{ 
int Status; 
xil_printf("GPIO Interrupt Example Test \r\n"); 

while(1){ 
Status = GpioIntrExample(&Intc, &Gpio, GPIO_DEVICE_ID, 
GPIO_INTERRUPT_ID); 
} 

if (Status != XST_SUCCESS) { 
xil_printf("GPIO Interrupt Example Test Failed\r\n"); 
return XST_FAILURE; 
} 

xil_printf("Successfully ran GPIO Interrupt Example Test\r\n"); 
return XST_SUCCESS; 
} 

int GpioIntrExample(XScuGic *Intc, XGpioPs *Gpio, u16 DeviceId, u16 GpioIntrId) 
{ 
XGpioPs_Config *ConfigPtr; 
int Status; 

ConfigPtr = XGpioPs_LookupConfig(DeviceId); 
if (ConfigPtr == NULL) { 
return XST_FAILURE; 
} 
XGpioPs_CfgInitialize(Gpio, ConfigPtr, ConfigPtr->BaseAddr); 

Status = XGpioPs_SelfTest(Gpio); 
if (Status != XST_SUCCESS) { 
return XST_FAILURE; 
} 

Input_Pin=0; 
Output_1=52; 
Output_2=53; 
XGpioPs_SetDirectionPin(Gpio, Input_Pin, 0x0); 
XGpioPs_SetDirectionPin(Gpio, Output_1, 1); 
XGpioPs_SetOutputEnablePin(Gpio, Output_1, 1); 
XGpioPs_SetDirectionPin(Gpio, Output_2, 1); 
XGpioPs_SetOutputEnablePin(Gpio, Output_2, 1); 

Status = SetupInterruptSystem(Intc, Gpio, GPIO_INTERRUPT_ID); 
if (Status != XST_SUCCESS) { 
return XST_FAILURE; 
} 

AllButtonsPressed = FALSE; 

while(AllButtonsPressed == FALSE){ 
xil_printf("LED Green\n\r"); 
XGpioPs_WritePin(Gpio, Output_1, 0x0); 
XGpioPs_WritePin(Gpio, Output_2, 0x1); 
sleep(1); 
xil_printf("LED Red\n\r"); 
XGpioPs_WritePin(Gpio, Output_1, 0x1); 
XGpioPs_WritePin(Gpio, Output_2, 0x0); 
sleep(1); 
}; 

return XST_SUCCESS; 
} 


static void IntrHandler(void *CallBackRef, u32 Bank, u32 Status) 
{ 
XGpioPs_WritePin(&Gpio, Output_1, 0x0); 
XGpioPs_WritePin(&Gpio, Output_2, 0x0); 
AllButtonsPressed = TRUE; 
} 


static int SetupInterruptSystem(XScuGic *GicInstancePtr, XGpioPs *Gpio, 
u16 GpioIntrId) 
{ 
int Status; 

XScuGic_Config *IntcConfig; /* Instance of the interrupt controller */ 

Xil_ExceptionInit(); 

/* 
* Initialize the interrupt controller driver so that it is ready to 
* use. 
*/ 
IntcConfig = XScuGic_LookupConfig(INTC_DEVICE_ID); 
if (NULL == IntcConfig) { 
return XST_FAILURE; 
} 
Status = XScuGic_CfgInitialize(GicInstancePtr, IntcConfig, 
IntcConfig->CpuBaseAddress); 
if (Status != XST_SUCCESS) { 
return XST_FAILURE; 
} 
/* 
* Connect the interrupt controller interrupt handler to the hardware 
* interrupt handling logic in the processor. 
*/ 
Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT, 
(Xil_ExceptionHandler)XScuGic_InterruptHandler, 
GicInstancePtr); 

/* 
* Connect the device driver handler that will be called when an 
* interrupt for the device occurs, the handler defined above performs 
* the specific interrupt processing for the device. 
*/ 
Status = XScuGic_Connect(GicInstancePtr, GpioIntrId, 
(Xil_ExceptionHandler)XGpioPs_IntrHandler, 
(void *)Gpio); 
if (Status != XST_SUCCESS) { 
return Status; 
} 

/* Enable falling edge interrupts for all the pins in bank 0. */ 
XGpioPs_SetIntrType(Gpio, GPIO_BANK, 0x00, 0xFFFFFFFF, 0x00); 

/* Set the handler for gpio interrupts. */ 
XGpioPs_SetCallbackHandler(Gpio, 
(void *)Gpio, IntrHandler); 

/* Enable the GPIO interrupts of Bank 0. */ 
XGpioPs_IntrEnable(Gpio, GPIO_BANK, (1 « Input_Pin)); 

/* Enable the interrupt for the GPIO device. */ 
XScuGic_Enable(GicInstancePtr, GpioIntrId); 

/* Enable interrupts in the Processor. */ 
Xil_ExceptionEnableMask(XIL_EXCEPTION_IRQ); 

return XST_SUCCESS; 
}
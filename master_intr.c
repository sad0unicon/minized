#include "xparameters.h"	/* SDK generated parameters */
#include "xplatform_info.h"
#include "xspips.h"		/* SPI device driver */
#include "xscugic.h"		/* Interrupt controller device driver */
#include "xil_exception.h"
#include "xil_printf.h"

#define SPI_DEVICE_ID		XPAR_XSPIPS_0_DEVICE_ID
#define INTC_DEVICE_ID		XPAR_SCUGIC_SINGLE_DEVICE_ID
#define SPI_INTR_ID		XPAR_XSPIPS_1_INTR

static int SpiPsSetupIntrSystem(XScuGic *IntcInstancePtr,XSpiPs *SpiInstancePtr, u16 SpiIntrId);
static void SpiPsDisableIntrSystem(XScuGic *IntcInstancePtr, u16 SpiIntrId);
void SpiPsHandler(void *CallBackRef, u32 StatusEvent, unsigned int ByteCount);
int SpiPsFlashIntrExample(XScuGic *IntcInstancePtr, XSpiPs *SpiInstancePtr,u16 SpiDeviceId, u16 SpiIntrId);

static XScuGic IntcInstance;
static XSpiPs SpiInstance;

volatile int TransferInProgress;

int Error;

u8 BufferA = 0xAA;
int main(void)
{
	int Status;
	xil_printf("SPI Interrupt Example Test \r\n");
	Status = SpiPsFlashIntrExample(&IntcInstance, &SpiInstance,
				      SPI_DEVICE_ID, SPI_INTR_ID);
	if (Status != XST_SUCCESS) {
		xil_printf("SPI FLASH Interrupt Example Test Failed\r\n");
		return XST_FAILURE;
	}
	xil_printf("Successfully ran SPI FLASH Interrupt Example Test\r\n");
	return XST_SUCCESS;
}

int SpiPsFlashIntrExample(XScuGic *IntcInstancePtr, XSpiPs *SpiInstancePtr,
			 u16 SpiDeviceId, u16 SpiIntrId)
{
	XSpiPs_Config *SpiConfig;
    int Status;

	SpiConfig = XSpiPs_LookupConfig(SpiDeviceId);

	XSpiPs_CfgInitialize(SpiInstancePtr, SpiConfig, SpiConfig->BaseAddress);

	XSpiPs_SelfTest(SpiInstancePtr);

	Status = SpiPsSetupIntrSystem(IntcInstancePtr, SpiInstancePtr,SpiIntrId);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	XSpiPs_SetStatusHandler(SpiInstancePtr, SpiInstancePtr,
				 (XSpiPs_StatusHandler) SpiPsHandler);

	XSpiPs_SetOptions(SpiInstancePtr, XSPIPS_MASTER_OPTION | \
				XSPIPS_FORCE_SSELECT_OPTION);
	XSpiPs_SetClkPrescaler(SpiInstancePtr, XSPIPS_CLK_PRESCALE_8);

	while(1){
		xil_printf("Hello world\r\n");
		}
	SpiPsDisableIntrSystem(IntcInstancePtr, SpiIntrId);
	return XST_SUCCESS;
}

void SpiPsHandler(void *CallBackRef, u32 StatusEvent, unsigned int ByteCount)
{
    XSpiPs_SetSlaveSelect(&SpiInstance, 0x00);
	XSpiPs_PolledTransfer(&SpiInstance, &BufferA, NULL, 1);
	xil_printf("BufferA = %d\r\n", BufferA);
	TransferInProgress = FALSE;

	if (StatusEvent != XST_SPI_TRANSFER_DONE) {
		Error++;
	}
}

static int SpiPsSetupIntrSystem(XScuGic *IntcInstancePtr, XSpiPs *SpiInstancePtr, u16 SpiIntrId)
{

	XScuGic_Config *IntcConfig;
	Xil_ExceptionInit();

	IntcConfig = XScuGic_LookupConfig(INTC_DEVICE_ID);
    XScuGic_CfgInitialize(IntcInstancePtr, IntcConfig,IntcConfig->CpuBaseAddress);

	Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT,
				(Xil_ExceptionHandler)XScuGic_InterruptHandler,
				IntcInstancePtr);

     XScuGic_Connect(IntcInstancePtr, SpiIntrId,
				(Xil_ExceptionHandler)XSpiPs_InterruptHandler,
				(void *)SpiInstancePtr);

	XScuGic_Enable(IntcInstancePtr, SpiIntrId);
	Xil_ExceptionEnable();

	return XST_SUCCESS;
}

static void SpiPsDisableIntrSystem(XScuGic *IntcInstancePtr, u16 SpiIntrId)
{

	XScuGic_Disable(IntcInstancePtr, SpiIntrId);
	XScuGic_Disconnect(IntcInstancePtr, SpiIntrId);
}

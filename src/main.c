

#include <p33FJ256GP506.h>
#include <dsp.h>
#include "WM8510CodecDrv.h"
#include "AT25F4096Drv.h"
#include "clipper.h"
#include "sask.h"
#include "generators.h"
#include "sibilance.h"


void SaveFilterState(void);


_FGS(GWRP_OFF & GCP_OFF);
_FOSCSEL(FNOSC_FRC);
_FOSC(FCKSM_CSECMD & OSCIOFNC_ON & POSCMD_NONE);
_FWDT(FWDTEN_OFF);



/* Allocate memory for buffers and drivers	*/

int 		codecBuffer		[WM8510DRV_DRV_BUFFER_SIZE];
char 	flashMemoryBuffer	[AT25F4096DRV_BUFFER_SIZE];

fractional inBuf[FRAME_SIZE];
fractional outBuf[FRAME_SIZE];
fractional IBuf[FRAME_SIZE];
fractional QBuf[FRAME_SIZE];
fractional IOsc[FRAME_SIZE];
fractional QOsc[FRAME_SIZE];

unsigned int SSB;
int EQ;

int flashes;
int flash_timer;

/* Instantiate the drivers 	*/
WM8510Handle codec;
AT25F4096Handle flashMemoryHandle; 


/* Create the driver handles	*/
WM8510Handle * codecHandle = &codec;
AT25F4096Handle *pFlashMemoryHandle 	= &flashMemoryHandle;


int main(void)
{
	/* Configure Oscillator to operate the device at 40MHz.
	 * Fosc= Fin*M/(N1*N2), Fcy=Fosc/2
	 * Fosc= 7.37M*40/(2*2)=80Mhz for 7.37M input clock */
 
	PLLFBD=41;				/* M=39	*/
	CLKDIVbits.PLLPOST=0;		/* N1=2	*/
	CLKDIVbits.PLLPRE=0;		/* N2=2	*/
	OSCTUN=0;			
	
	__builtin_write_OSCCONH(0x01);		/*	Initiate Clock Switch to FRC with PLL*/
	__builtin_write_OSCCONL(0x01);
	while (OSCCONbits.COSC != 0b01);	/*	Wait for Clock switch to occur	*/
	while(!OSCCONbits.LOCK);

	/* Intialize the board and the drivers	*/
	SASKInit();
	WM8510Init(codecHandle,codecBuffer);
	AT25F4096Init		(pFlashMemoryHandle,flashMemoryBuffer);	/* For the  Flash	*/

	/* Start the flash driver functions	*/
	
	AT25F4096Start	(pFlashMemoryHandle);
	
   flash_timer = 0;
	
	/* Start Audio input and output function	*/
	WM8510Start(codecHandle);
		
	/* Configure codec for 16K operation	*/
	WM8510SampleRate16KConfig(codecHandle);

   InitGen();

   // SetSineFreq(500);
   SetShiftFreq(500);
  
   InitSibil();

	/* Main processing loop. Executed for every input and 
	 * output frame	*/
	 
	while(1)
	{
			/* Obtaing the ADC samples	*/
			while(WM8510IsReadBusy(codecHandle));

			WM8510Read(codecHandle,inBuf,FRAME_SIZE);
			
         MakeSinCos(FRAME_SIZE, IOsc, QOsc, 0x7fff);
         
         ProcSibil (inBuf, IBuf, QBuf);

         VectorMultiply(FRAME_SIZE, IBuf, IBuf, IOsc);
         VectorMultiply(FRAME_SIZE, QBuf, QBuf, QOsc);

         VectorSubtract(FRAME_SIZE, outBuf, IBuf, QBuf);

		   /* Wait till the codec is available for a new  frame	*/
			while(WM8510IsWriteBusy(codecHandle));	
		
			/* Write the frame to the output	*/
			WM8510Write (codecHandle,outBuf,FRAME_SIZE);
		
         ++flash_timer;

         if (flash_timer & 0x0010)
         {
            GREEN_LED = SASK_LED_ON;
         }
         else
         {
            GREEN_LED = SASK_LED_OFF;
         }
	}
	
}// main


void SaveFilterState(void)
{
   long address = 0L;
   
   /* Erase sector. Each sector is 0xFFFF long	*/
   AT25F4096IoCtl(pFlashMemoryHandle,AT25F4096DRV_WRITE_ENABLE,0);	
   AT25F4096IoCtl(pFlashMemoryHandle,AT25F4096DRV_SECTOR_ERASE,(void *)&address);	
   while(AT25F4096IsBusy(pFlashMemoryHandle));
   
   AT25F4096IoCtl(pFlashMemoryHandle,AT25F4096DRV_WRITE_ENABLE,0);
   AT25F4096Write(pFlashMemoryHandle, address, (char *) &SSB, sizeof(int));
   while(AT25F4096IsBusy(pFlashMemoryHandle));

   address = 2L;

   AT25F4096IoCtl(pFlashMemoryHandle,AT25F4096DRV_WRITE_ENABLE,0);
   AT25F4096Write(pFlashMemoryHandle, address, (char *) &EQ, sizeof(int));
   while(AT25F4096IsBusy(pFlashMemoryHandle));
   
}// SaveFilterState


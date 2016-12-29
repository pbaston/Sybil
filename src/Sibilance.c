
/*
 **************************************************************************
 *
 * $Workfile: $
 *   $Author: $
 *  $Modtime: $
 * $Revision: $
 *
 **************************************************************************
 * Version history
 *
 * $History: $
 * 
 **************************************************************************
 */

//************************************************************************
//
// MODULES USED
//
//************************************************************************

#include <p33FJ256GP506.h>
#include <dsp.h>

#include "clipper.h"


//************************************************************************
//
// DEFINITIONS AND MACROS
//
//************************************************************************

#define SIBIL_SIZE 101


//************************************************************************
//
// LOCAL MEMORY ALLOCATIONS
//
//************************************************************************


static fractional DelayBufQ[SIBIL_SIZE] _YDATA(2);
static fractional DelayBufI[SIBIL_SIZE] _YDATA(2);

static FIRStruct FIRFilterQ;
static FIRStruct FIRFilterI;

static unsigned int page;
static fractional* offset;


const fractional SIBILI[SIBIL_SIZE] =
// I Phase Kernel Data
{0x0000, 0x0000, 0x0001, 0x0000, 0xFFFC, 0xFFFF, 0x0007, 0x0005, 0xFFF7, 0xFFF5, 
0x0007, 0x0011, 0xFFFE, 0xFFEF, 0xFFFC, 0x000A, 0x0003, 0x0002, 0x000D, 0xFFF6, 
0xFFCF, 0xFFFE, 0x0064, 0x0036, 0xFF6F, 0xFF64, 0x0099, 0x012E, 0xFFAB, 0xFE33, 
0xFFAC, 0x0242, 0x0164, 0xFDB1, 0xFD4A, 0x01BA, 0x0402, 0xFF92, 0xFB1A, 0xFE87, 
0x0505, 0x03A9, 0xFBD8, 0xFA57, 0x0251, 0x06FA, 0x003A, 0xF8C6, 0xFD04, 0x0645, 
0x0560, 0xFBBD, 0xF91A, 0x019E, 0x0740, 0x011B, 0xF991, 0xFCA7, 0x04B7, 0x04B8, 
0xFD71, 0xFAED, 0x0076, 0x0487, 0x012A, 0xFC9D, 0xFDE5, 0x0209, 0x025A, 0xFF30, 
0xFDEF, 0xFFF2, 0x017E, 0x0083, 0xFF1F, 0xFF64, 0x0063, 0x007D, 0xFFEA, 0xFFB6, 
0xFFF8, 0x001D, 0x0008, 0xFFFE, 0x0004, 0xFFFB, 0xFFF1, 0x0002, 0x0012, 0x0005, 
0xFFF2, 0xFFF7, 0x0008, 0x0008, 0xFFFD, 0xFFFB, 0x0000, 0x0002, 0x0000, 0x0000, 
0x0000, };

const fractional SIBILQ[SIBIL_SIZE] =
// Q Phase Kernel Data
{0x0000, 0x0000, 0x0000, 0x0002, 0x0000, 0xFFFB, 0xFFFD, 0x0008, 0x0008, 0xFFF7, 
0xFFF2, 0x0005, 0x0012, 0x0002, 0xFFF1, 0xFFFB, 0x0004, 0xFFFE, 0x0008, 0x001D, 
0xFFF8, 0xFFB6, 0xFFEA, 0x007D, 0x0063, 0xFF64, 0xFF1F, 0x0083, 0x017E, 0xFFF2, 
0xFDEF, 0xFF30, 0x025A, 0x0209, 0xFDE5, 0xFC9D, 0x012A, 0x0487, 0x0076, 0xFAED, 
0xFD71, 0x04B8, 0x04B7, 0xFCA7, 0xF991, 0x011B, 0x0740, 0x019E, 0xF91A, 0xFBBD, 
0x0560, 0x0645, 0xFD04, 0xF8C6, 0x003A, 0x06FA, 0x0251, 0xFA57, 0xFBD8, 0x03A9, 
0x0505, 0xFE87, 0xFB1A, 0xFF92, 0x0402, 0x01BA, 0xFD4A, 0xFDB1, 0x0164, 0x0242, 
0xFFAC, 0xFE33, 0xFFAB, 0x012E, 0x0099, 0xFF64, 0xFF6F, 0x0036, 0x0064, 0xFFFE, 
0xFFCF, 0xFFF6, 0x000D, 0x0002, 0x0003, 0x000A, 0xFFFC, 0xFFEF, 0xFFFE, 0x0011, 
0x0007, 0xFFF5, 0xFFF7, 0x0005, 0x0007, 0xFFFF, 0xFFFC, 0x0000, 0x0001, 0x0000, 
0x0000, };

//************************************************************************
//
// LOCAL FUNCTION PROTOTYPES
//
//************************************************************************


//************************************************************************
//
// EXPORTED FUNCTIONS
//
//************************************************************************



void InitSibil(void)
{
			asm("mov #psvpage(_SIBILQ),w0\nmov w0,_page");
			asm("mov #psvoffset(_SIBILQ),w0\nmov w0,_offset");
			FIRStructInit(&FIRFilterQ, SIBIL_SIZE, offset, page, DelayBufQ);
		 	FIRDelayInit(&FIRFilterQ);

			asm("mov #psvpage(_SIBILI),w0\nmov w0,_page");
			asm("mov #psvoffset(_SIBILI),w0\nmov w0,_offset");
			FIRStructInit(&FIRFilterI, SIBIL_SIZE, offset, page, DelayBufI);
		 	FIRDelayInit(&FIRFilterI);
   
}// InitSibil


void ProcSibil(fractional* pIn, fractional* IOut, fractional* QOut)
{

	FIR(FRAME_SIZE, IOut, pIn, &FIRFilterI);	//create I data
	FIR(FRAME_SIZE, QOut, pIn, &FIRFilterQ);	//create Q data
	
}// ProcSibil




/* ==========================================================================*/
/* - - - - - - - - - - -  G E N E R A T O R S , H  - - - - - - - - - - - - - */
/* ==========================================================================*/
/*	Created   06-15-2004   M. Wheatley                                       */
/*	Modified  07-06-2004   MW                                                */
/*.........................................................................  */
/* Signal Generator Module Include File                                      */
/*.........................................................................  */
#ifndef GEN_MODULE
#define GEN_MODULE
/*>>>>>>>>>>>>>>>>>>>>>> Global Defines <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/

/*>>>>>>>>>>>>>>>>>>>>>>> External memory <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/

/*>>>>>>>>>>>>>>>>>>>>>> External functions <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/
extern void InitGen( void );
extern void SetSineFreq( unsigned int freq);
extern void SetShiftFreq( int freq);
extern void MakeSine( unsigned int numsamples, int* pBuf, fractional Scale);
extern void MakeSinCos( unsigned int numsamples, int* pBufI, int* pBufQ, fractional Scale);
extern void MakeUniform( unsigned int numsamples, int* pBuf, fractional Scale);
extern void MakeGauss( unsigned int numsamples, int* pBuf );

#endif


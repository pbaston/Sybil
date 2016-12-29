; ==========================================================================
; - - - - - - - - - - - -  A s m U t i l s . S  - - - - - - - - - - - - - -
; ==========================================================================
;	Created   07-12-2004   M. Wheatley                                      
;	Modified  07-13-2004   MW                                               
;...........................................................................
; Assembly Language Utility Routines                                                     
;...........................................................................  
; 
	.text
	.global _CalcLpIirArray
	.global _CalcLogMagnitude
	.global	_CalcNCO

;+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
; C-callable routine to compute n samples of a Low Pass IIR filter
; void CalcLpIir( unsigned int Size, fractional* pIn, int* pIIROut, unsigned int LPK); 
;+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
_CalcLpIirArray:
; w0 contains array size
; w1 contains pointer to fractional input array
; w2 contains pointer to the Filter output array
; w3 contains the LP averaging constant

	push w8
	push w9

	mov  w1, w9			; transfer input buffer pointer to DSP address register
	mov  w2, w8 		; w8 = pIIROut
	mov  w3, w5			; w5 = LPK
	mov #0x8000,w6		; 
	sub w6,w5,w6		; w6 = (1.0 - LPK)
	clr A,[w9]+=2,w4	; preload DSP operand, w4, with input buffer ptr

	dec w0, w0			; reduce DO count Size by one
	do  w0, CLPImainloop	; perform DO loop 'Size' times

	mpy w4*w5, A, [w9]+=2, w4 ; ACC = LPK * pIn[i]
	mov [w8], w7		;get current IIR output value
	mac w6*w7, A		; ACC = LPK * pIn[i] + (1.0-LPK) * pIIROut
	Sac.r a,[w8++]		;pIIROut = ACC
CLPImainloop:
    clr A      

	pop w9
	pop w8
	return				; return to calling section
;..........................................................................

;+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
; C-callable routine to compute LOG2 of complex data array
; void CalcLogMagnitude(fractcomplex* pIn, int* pOut, unsigned int Size); 
;+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
_CalcLogMagnitude: 
; w0 contains pointer to complex input array
; w1 contains pointer to log2 magnitude output array
; w2 contains array size
; w6 used for prefetched data from input array
; w7 used for getting highest bit position in log2 calc
; w10 used for input array pointer
	push w10

	mov  #0x22,w4		; w4 = &ACCAL
	mov  #0x24,w5		; w5 = &ACCAH
	mov  w0, w10		; transfer input buffer pointer to DSP address register
	clr A, [w10]+=2, w6	; preload DSP operand, w6, with input buffer ptr
	dec w2, w2			; reduce DO count size by one
	do  w2, CLmainloop	; perform DO loop 'size' times

	mpy w6*w6, A, [w10]+=2, w6	; compute Re(k)^2 -> A, prefetch Im(k)^2
	mac w6*w6, A, [w10]+=2, w6	; compute Im(k)^2 + AccA -> A, prefetch Re(k)^2
						; ACCA now contains 32 bit mag square of complex sample
;	sftac a,#1			;really want log2 of sqrt so shift once before doing log
	ff1l [w5],w7		;Find first bit from left of msw
	bra  NC,CLfound2	;if C==0 then bit was found in msw and position is in w7
	ff1l [w4],w7		;Find first bit from left of lsw
	bra  NC,CLfound1 	;if C==0 then bit was found in lsw and position is in w7
	clr w7				;here if no bits found because ACCA was zero
	bra CLmainloop
CLfound1:			;found first bit in lsw so log2 = 16-w7
	subr w7,#16,w7
	bra CLmainloop
CLfound2:			;found first bit in msw so log2 = 32-w7
	subr w7,#31,w7
	inc w7,w7
CLmainloop:
;;;	mov.b  w7, [w1++]	; save LOG2( Re(k)^2 + jIm(k)^2 ) into char pOut
	mov  w7, [w1++]	; save LOG2( Re(k)^2 + jIm(k)^2 ) into int pOut
          
	pop w10
	return                    ; return to calling section
;..........................................................................

;+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
; C-callable routine to compute n samples of an Numerically Controlled Oscillator
; void CalcNCO( unsigned int Size, fractional* pOut, NCOStruct* pNCO); 
;+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
_CalcNCO:
; w0 contains array size
; w1 contains pointer to fractional output array
; w2 contains pointer to NCO variable structure
; w3 contains
	.equ oPhzAcc, #0		;16 bit phase accumulator
	.equ oPhzInc, #2		;16 bit phase increment value(frequency)
	.equ oPhzOffset, #4		;signed 16 bit phase offset(2PI==32678)
	.equ oScale, #6			;16 bit fraction amplitude scale value
	.equ oSineTable, #8		;pointer to 16 bit fractional Sine Lookup Table
;
	push w8
	push w9
	push w10
	push w11
	mov [w2+oScale],w4		; get local copies of all the structure members
	mov [w2+oPhzOffset],w8
	mov [w2+oPhzAcc],w9
	mov [w2+oPhzInc],w10
	mov [w2+oSineTable],w11
	dec w0, w0			; reduce DO count Size by one

	do  w0, CNmainloop	; perform DO loop 'Size' times

	add w9,w8,w5		; w5 = PhzAcc + PhzOffset
	lsr	w5,#7,w5		; w5 = (PhzAcc + PhzOffset)>>7
	sl	w5,w5
	add w5,w11,w5		; w5 = &SineTable[PhzAcc>>7 + PhzOffset]
	mov [w5],w5			; w5 = SineTable[PhzAcc>>7 + PhzOffset]
	mpy w4*w5,a			; ACCA = Scale * SineTable[PhzAcc>>7 + PhzOffset]
	add w9,w10,w9		; PhzAcc += PhzInc 
CNmainloop:
	Sac.r a,[w1++]
	mov w9,[w2+oPhzAcc] ; save PhzAcc 

	pop w11
	pop w10
	pop w9
	pop w8
	return				; return to calling section
;..........................................................................

	.end

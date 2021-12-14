/***********************************************************************************************************************
* File Name    : uart.c
* Project      : PES Final Project
* Version      : 2.0
* Description  : Contains all the function implementation code along with variable definitions for uart module
* Author       : Vishal Raj & referred from Dean chp 8-https://github.com/alexander-g-dean/ESF/tree/master/NXP/Code/
* 				 Chapter_8/Serial-Demo.
* Creation Date: 12.12.21
***********************************************************************************************************************/
#include "MKL25Z4.h"
#include "uart.h"
#include "huff.h"

//change uart parameters below
#define BAUD_RATE		38400
#define DATA_BITS		8
#define PARITY			0
#define STOP_BITS		2

cbfifo_t *rx_fifo = &rx;
cbfifo_t *tx_fifo = &tx;

/***********************************************************************************************
* Name			   : Init_UART0
* Description 	   : used to initialize uart module.
* Parameters 	   : baud rate of uart required
* RETURN 		   : None
***********************************************************************************************/
void Init_UART0() {
	uint16_t sbr;
	uint8_t temp;

	// Enable clock gating for UART0 and Port A
	SIM->SCGC4 |= SIM_SCGC4_UART0_MASK;
	SIM->SCGC5 |= SIM_SCGC5_PORTA_MASK;


	// Make sure transmitter and receiver are disabled before init
	UART0->C2 &= ~UART0_C2_TE_MASK & ~UART0_C2_RE_MASK;

	// Set UART clock to 48 MHz clock
	SIM->SOPT2 |= SIM_SOPT2_UART0SRC(1);//mcg or mcg/2 clock
	//SIM->SOPT2 |= SIM_SOPT2_PLLFLLSEL_MASK;//mcg/2 clock

	// Set pins to UART0 Rx and Tx
	PORTA->PCR[1] = PORT_PCR_ISF_MASK | PORT_PCR_MUX(2); // Rx
	PORTA->PCR[2] = PORT_PCR_ISF_MASK | PORT_PCR_MUX(2); // Tx

	// Set baud rate and oversampling ratio
	sbr = (uint16_t)((SYS_CLOCK)/(BAUD_RATE * UART_OVERSAMPLE_RATE));
	UART0->BDH &= ~UART0_BDH_SBR_MASK;
	UART0->BDH |= UART0_BDH_SBR(sbr>>8);
	UART0->BDL = UART0_BDL_SBR(sbr);
	UART0->C4 |= UART0_C4_OSR(UART_OVERSAMPLE_RATE-1);

	// Disable interrupts for RX active edge and LIN break detect, select two stop bit
	UART0->BDH |= UART0_BDH_RXEDGIE(0) | UART0_BDH_SBNS(1) | UART0_BDH_LBKDIE(0);

	// Don't enable loopback mode, use 8 data bit mode, don't use parity
	UART0->C1 = UART0_C1_LOOPS(0) | UART0_C1_M(0) | UART0_C1_PE(0);
	// Don't invert transmit data, don't enable interrupts for errors
	UART0->C3 = UART0_C3_TXINV(0) | UART0_C3_ORIE(0)| UART0_C3_NEIE(0)
			| UART0_C3_FEIE(0) | UART0_C3_PEIE(0);

	// Clear error flags
	UART0->S1 = UART0_S1_OR(1) | UART0_S1_NF(1) | UART0_S1_FE(1) | UART0_S1_PF(1);

	// Send LSB first, do not invert received data
	UART0->S2 = UART0_S2_MSBF(0) | UART0_S2_RXINV(0);

#if UART_INTERRUPTS

	NVIC_SetPriority(UART0_IRQn, 2); // 0, 1, 2, or 3
	NVIC_ClearPendingIRQ(UART0_IRQn);
	NVIC_EnableIRQ(UART0_IRQn);

	// Enable receive interrupts but not transmit interrupts yet
	UART0->C2 |= UART_C2_RIE(1);
	//UART0->C2 |= UART_C2_TIE(1);//enable for test-dont do this here
#endif

	// Enable UART receiver and transmitter
	UART0->C2 |= UART0_C2_RE(1) | UART0_C2_TE(1);

	// Clear the UART RDRF flag
	temp = UART0->D;
	UART0->S1 &= ~UART0_S1_RDRF_MASK;

	if(temp)
		;
}

/***********************************************************************************************
* Name			   : UART0_IRQHandler
* Description 	   : ISR for UART.
* Parameters 	   : None
* RETURN 		   : None
***********************************************************************************************/
void UART0_IRQHandler(void) {

	uint8_t ch;
	uint8_t ch_tx;

	if (UART0->S1 & (UART_S1_OR_MASK |UART_S1_NF_MASK | UART_S1_FE_MASK | UART_S1_PF_MASK)) {
			// clear the error flags
			UART0->S1 |= UART0_S1_OR_MASK | UART0_S1_NF_MASK | UART0_S1_FE_MASK | UART0_S1_PF_MASK;
			// read the data register to clear RDRF
			ch = UART0->D;
	}
	if (UART0->S1 & UART0_S1_RDRF_MASK) {//read interrupt
		// received a character
		ch = UART0->D;
		if(!(cbfifo_capacity(rx_fifo) == cbfifo_length(rx_fifo))){
			cbfifo_enqueue(rx_fifo,&ch,1);
		}else {
			// error - queue full.
			// discard character
		}

	}
	if ( (UART0->C2 & UART0_C2_TIE_MASK) && (UART0->S1 & UART0_S1_TDRE_MASK) ) { // tx buffer empty
		// can send another character
		if(cbfifo_length(tx_fifo)!=0){//not empty
			 cbfifo_dequeue(tx_fifo, &ch_tx, 1);
			 UART0->D = ch_tx;
		}else
			UART0->C2 &= ~UART0_C2_TIE_MASK;
	}
}
/***********************************************************************************************
* Name			   : __sys_write
* Description 	   : glue function for tying uart write with stdio functions in redlib. This
* 					 function has been modified for huffman encoding. All input strings are
* 					 huffman encoded. Read project document for details
* Parameters 	   : handle for stdio or stderr, buffer to be printed and upto size bytes
* RETURN 		   : 0 for success and -1 on error
***********************************************************************************************/
int __sys_write(int handle, char * buf, int size){

	uint8_t enc_buff[100];
	uint8_t *enc_buff_ptr;
	uint16_t size_idx = 0;
	enc_buff_ptr = enc_buff;

	uint16_t enc_bits = encode_string(buf, enc_buff, sizeof(enc_buff));//string encoded here

	size_idx = (enc_bits >> 3) + 1;

	/*If size of encoded buffer is less than 255 bits */
	if(enc_bits > 0xFF){
		enc_buff[size_idx] = enc_bits & 0x00FF;
		enc_buff[size_idx + 1] = (enc_bits>>8) & 0x00FF;
		enc_buff[size_idx + 2] = 0xFF;
	}else{
		enc_buff[size_idx] = enc_bits;
		enc_buff[size_idx + 1] = 0xFF;//entering special end of string token
	}

	while(cbfifo_length(tx_fifo) == cbfifo_capacity(tx_fifo))//if tx_buff full wait
		;

	while(*enc_buff_ptr != '\0'){
		if(cbfifo_enqueue(tx_fifo,enc_buff_ptr,1) == (size_t)-1)//enqueue error
			return -1;//error case
		enc_buff_ptr++;
	}

	if (!(UART0->C2 & UART0_C2_TIE_MASK)) {
		UART0->C2 |= UART0_C2_TIE(1);
	}

	return 0;//success
}
/***********************************************************************************************
* Name			   : __sys_readc
* Description 	   : glue function for tying uart read with stdio functions in redlib.
* Parameters 	   : None
* RETURN 		   : no of characters or -1 if nothing is there to be read
***********************************************************************************************/
int __sys_readc(void){

	int c;

	while(cbfifo_length(rx_fifo)==0)
			;

	if(cbfifo_dequeue(rx_fifo,&c,1))//some data in rx
		return c;
	else
		return -1;

}

/*[EOF]*/

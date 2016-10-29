/***************************************************************************

     Description: Timer and interrupt Functionality

     Project: Rat

     Reference:

     Date:	14/06/2003

****************************************************************************/
/* =========================================================================
   Compiler Controls */
#ifndef _RAT_UART_H_
#define _RAT_UART_H_

/* =========================================================================
   Function Prototypes */
void Uart_Init(void);
void Uart_Tx_Char(char c);
void Uart_Tx_String(const char *addr);
void Uart_Tx_Convert_Value(const unsigned int value);
void Uart_Handler(void);

/* =========================================================================
   Compiler Controls */
#endif /* _RAT_UART_H_ */

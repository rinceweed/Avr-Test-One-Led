/***************************************************************************

     Description: Timer and interrupt Functionality

     Project: Rat

     Reference:

     Date:  14/06/2003

****************************************************************************/
/* =========================================================================
   Additional Compiler Controls */
//#define UART_DEBUG

/* =========================================================================
   Standard Library Headers */
#include <inttypes.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <avr/signal.h>

/* =========================================================================
   External Headers */
#include "common.h"
#include "que.h"

/* =========================================================================
   Source Headers */
#include "uart.h"

/* =========================================================================
   Defines */
enum state
{
  IDLE = 1,
  BUSY,
  MAX_STATE
};

/* =========================================================================
   Private Data */
static eTrue_False_Type Tx_Busy;
static eTrue_False_Type Request_Tx;
static enum state  Uart_State;

/* =========================================================================
   Private Constants */

/* =========================================================================
   Private Function Prototypes */

/* =========================================================================
   Public Functions */
/* -------------------------------------------------------------------------
   Function    : Uart_Init
   Purpose     : Provide the initialisation For Serial Comms
   Parameter1  : void
   Return      : void
*/
/* -------------------------------------------------------------------------*/
void Uart_Init(void)
{
  /* Serial Port Init */
  UCSRA  = _BV(U2X);   /* Divisor rate halfed */
  UCSRB  = _BV(RXCIE)|_BV(TXEN)|_BV(RXEN);   /* rx/tx enable */
  UCSRC = 0 |_BV(UCSZ1) | _BV(UCSZ0);	/* ASYNCH 8-bits 1-Stop No Parity */
  UBRRH = 0;
//  UBRR = 25;  /* 9600 Baud @4MHz */
  UBRRL = 12; /* 19200 Baud @4MHz */
//  UBRRL = 1; /* 250k Baud @4MHz */
//  UBRRL = 0; /* 0.5M Baud @4MHz */
  Tx_Busy = FALSE;
  Request_Tx = FALSE;
  Uart_State = IDLE;
  return;
}/* Uart_Init */

/* -------------------------------------------------------------------------
   Function    : Uart_Handler
   Purpose     : Send a data via Serial
   Parameter1  : void
   Return      : void
*/
/* -------------------------------------------------------------------------*/
void Uart_Handler(void)
{
  Queue_Status_Type  status;

  switch (Uart_State)
  {
    case IDLE:
      status = Que_Status(SERIALTX_Q);
      if (status != Q_EMPTY)
      {
        Uart_State = BUSY;
        Tx_Busy = TRUE;
        /* Start the transmission */
        sbi(UCSRB,UDRIE);
      }
    break;
    case BUSY:
      if (Tx_Busy == FALSE)
      {
        Uart_State = IDLE;
      }
    break;
    default:
      Uart_State = IDLE;
    break;
  }/* end switch */
  return;
}/* Uart_Handler */

/* -------------------------------------------------------------------------
   Function    : Uart_Tx_Char
   Purpose     : Send a char via Serial
   Parameter1  : Char to send
   Return      : void
*/
/* -------------------------------------------------------------------------*/
void Uart_Tx_Char(char c)
{
  if (c == '\n')
    c = '\r';
  Que_Put(SERIALTX_Q, c);
}/* Uart_Tx_Char */

/* -------------------------------------------------------------------------
   Function    : Uart_Tx_String
   Purpose     : Send a "string" via Serial
   Parameter1  : "string" to send
   Return      : void
*/
/* -------------------------------------------------------------------------*/
void Uart_Tx_String(const char *addr)
{
  char c;
//  while ((c = __lpm_inline((unsigned short)addr++)))
    Uart_Tx_Char(c);
}/* Uart_Tx_String */

/* -------------------------------------------------------------------------
   Function    : Uart_Tx_Convert_Value
   Purpose     : Send a value via Serial
   Parameter1  : value to send
   Return      : void
*/
/* -------------------------------------------------------------------------*/
void Uart_Tx_Convert_Value(const unsigned int value)
{
  volatile unsigned char ene,tiene,hond,duisend;
  volatile unsigned int res,my_value;

  my_value = value;
  ene = tiene = hond = duisend = 0;
  res = 0;
  ene = my_value%10;
  if ( value >= 10 )
  {
    my_value /= 10;
    tiene = my_value%10;
  }
  if ( value >= 100 )
  {
    my_value /= 10;
    hond = my_value%10;
  }
  if ( value >= 1000 )
  {
    my_value /= 10;
    duisend = my_value%10;
  }
  duisend += 0x30;
  Uart_Tx_Char(duisend);
  hond += 0x30;
  Uart_Tx_Char(hond);
  tiene += 0x30;
  Uart_Tx_Char(tiene);
  ene += 0x30;
  Uart_Tx_Char(ene);
}/* Uart_Tx_Convert_Value */

/* =========================================================================
   Private Functions */
/* -------------------------------------------------------------------------
   Function    : SIGNAL(SIG_UART_RECV)
   Purpose     : Interrupt for serial Receive
   Parameter1  : void
   Return      : void
*/
/* -------------------------------------------------------------------------*/
SIGNAL(SIG_UART_RECV)
{
  unsigned char frame_error;
  unsigned char overrun;
  frame_error = FE;
  overrun  = DOR;
  Que_Put(SERIALRX_Q, UDR);
//  Que_Put(SERIALTX_Q, UDR);

#ifdef UART_DEBUG
  Uart_Tx_Char(UDR);
#endif
}/* SIGNAL(SIG_UART_RECV) */

/* -------------------------------------------------------------------------
   Function    : SIGNAL(SIG_UART_DATA)
   Purpose     : Interrupt for serial Data Register Empty
   Parameter1  : void
   Return      : void
*/
/* -------------------------------------------------------------------------*/
SIGNAL(SIG_UART_DATA)
{
  Queue_Status_Type  status;

  /* Check if any more to transmit, ifnot disable Int */
  status = Que_Status(SERIALTX_Q);
  if (status != Q_EMPTY)
  {
    UDR = Que_Get(SERIALTX_Q);
  }
  else
  {
    /* if no new data can be written, delete task */
    cbi(UCSRB,UDRIE);
    /* finish of TX */
    sbi(UCSRB,TXCIE);
  }

}/* SIGNAL(SIG_UART_DATA) */

/* -------------------------------------------------------------------------
   Function    : SIGNAL(SIG_UART_TRANS)
   Purpose     : Interrupt for serial Transmit complete
   Parameter1  : void
   Return      : void
*/
/* -------------------------------------------------------------------------*/
SIGNAL(SIG_UART_TRANS)
{
  Tx_Busy = FALSE;
}/* SIGNAL(SIG_UART_TRANS) */

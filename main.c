/***************************************************************************

     Description: Provide Motor Control For DC-motors via a H-Bridge

     Project: Rat

     Reference:

     Date:	01/05/2003

****************************************************************************/
/* =========================================================================
   Additional Compiler Controls */
#define _MAIN_VISIBLE

/* =========================================================================
   Standard Library Headers */
#include <inttypes.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <avr/signal.h>


/* =========================================================================
   External Headers */

/* =========================================================================
   Source Headers */
#include "common.h"

#include "timer.h"
#include "uart.h"
#include "que.h"
#include "main.h"

/* =========================================================================
   Defines */
#define HEARTBEAT_PULSE            (PORT7)

/* =========================================================================
   Private Data */
static eTrue_False_Type Pulse;

/* =========================================================================
   Private Constants */

/* =========================================================================
   Private Function Prototypes */
static void init_main(void);
static void heartbeat(void);

/* =========================================================================
   Public Functions */

/* =========================================================================
   Private Function */

/* -------------------------------------------------------------------------
   Function    : init_main
   Purpose     : provide the initialisation
   Parameter1  : void
   Return      : void
*/
/* -------------------------------------------------------------------------*/
static void init_main(void)
{
  DDRA = 0xff;
  PORTA = 0xff;
  DDRB = 0xff;
  PORTB = 0xff;
  DDRC = 0xff;
  PORTC = 0xff;
  DDRD = 0xff;
//  PORTD = 0xff;
  //DDRE = 0xff;
  //PORTE = 0xff;
  Pulse = FALSE;

  Uart_Init();
  Que_Setup();
  Timer_Init();
  
  return;
}/* end init_main */

/* -------------------------------------------------------------------------
   Function    : main
   Purpose     : main function
   Parameter1  : void
   Return      : void
*/
/* -------------------------------------------------------------------------*/
int main (void)
{
  init_main();
  /* enable interrupts */
  sei ();
  /* loop forever, the interrupts are doing the rest */
  Timer_Set_Timeout(TIMER_FLASH_LED, heartbeat, 1500);

  while(1)
  {
    Timer_Event_Manage();
	Uart_Handler();
  }/* end */

  PORTD = 0x55;

  return (0);
}/* end main */

/* -------------------------------------------------------------------------
   Function    : heartbeat
   Purpose     : Toggle LED on port C8
   Parameter1  : void
   Return      : void
*/
/* -------------------------------------------------------------------------*/
static void heartbeat(void)
{
  Timer_Set_Timeout(TIMER_FLASH_LED, heartbeat, 3000);

  if (Pulse == TRUE)
  {
    PORTA = 0x00;
    PORTB = 0x00;
    PORTC = 0x00;
//    PORTD &= 0x01;
//    PORTE = 0x00;
//	cbi(PORTD,HEARTBEAT_PULSE);
    Pulse = FALSE;
  }
  else
  {
    PORTA = 0xFF;
    PORTB = 0xFF;
    PORTC = 0xFF;
//    PORTD |= 0xFe;
//    PORTE = 0xFF;

//    sbi(PORTD,HEARTBEAT_PULSE);
    Uart_Tx_Char('?');
    Pulse = TRUE;
  }
  return;
}/* heartbeat */


/* =========================================================================
   Additional Compiler Controls */
#undef _MAIN_PUBLIC

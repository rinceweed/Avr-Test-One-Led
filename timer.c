/***************************************************************************

     Description: Timer and interrupt Functionality

     Project:

     Reference:

     Date:	14/06/2003

****************************************************************************/
/* =========================================================================
   Additional Compiler Controls */
//#define TIMER_DEBUG

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

/* =========================================================================
   Source Headers */
#include "timer.h"

/* =========================================================================
   Defines */
/* System clock 4(MHz) in Hz. */
#define SYSCLK                        (4000000UL)
#define PRE_LOAD_TIM1L                 (0xc0)
#define PRE_LOAD_TIM1H                 (0x63)

#define TICKS_PER_US                  (SYSCLK / 1000000)

#define TICKS_PER_MS                  (TICKS_PER_US * 1000)
#define TIMER0_1SEC                   (100)

/* =========================================================================
   Private Data */
static unsigned char Timer0_Count_10MS, Toggle;
static s_TIMER_Event Time_Event_List[MAX_TIMER_EVENTS];

/* =========================================================================
   Private Constants */

/* =========================================================================
   Private Function Prototypes */
static void timer01sec(void);

/* =========================================================================
   Public Functions */
/* -------------------------------------------------------------------------
   Function    : Timer_Init
   Purpose     : Provide the initialisation For Timers
   Parameter1  : void
   Return      : void
*/
/* -------------------------------------------------------------------------*/
void Timer_Init(void)
{
  uint8_t i;
  for (i = 0; i < MAX_TIMER_EVENTS; i++)
  {
    Time_Event_List[i].Timeout_Value = 0;
    Time_Event_List[i].Timeout_Counter = 0;
    Time_Event_List[i].Timeout_In_Progress = FALSE;
    Time_Event_List[i].Timer_Event_Fn_Ptr = NULL;
  }
  /* Timer 1 Init */
  TCCR1B = _BV(CS10); /* CK no Prescaler is used */
  TCNT1H = PRE_LOAD_TIM1H;
  TCNT1L = PRE_LOAD_TIM1L;
  timer_enable_int (_BV (TOIE1));
  Timer0_Count_10MS = 0;
  Toggle = 0;
  return;
}/* end Timer_Init */

/* -------------------------------------------------------------------------
   Function    : Timer_Set_Timeout
   Purpose     : Set the timer to timeout
   Parameter1  : Id of Timer to set
   Parameter2  : Timeout callback function
   Parameter3  : future timeout value in ms
   Return      : void
*/
/* -------------------------------------------------------------------------*/
void Timer_Set_Timeout(const e_EventTimerIds id,
                       const fp_Timer_Event fn_ptr,
                       const uint16_t then)
{
  if (id < MAX_TIMER_EVENTS)
  {
    Time_Event_List[id].Timeout_Value = /*Timer0_Count_10MS + */(uint16_t)(then/10);
    Time_Event_List[id].Timeout_Counter = 0;
    Time_Event_List[id].Timer_Event_Fn_Ptr = fn_ptr;
    Time_Event_List[id].Timeout_In_Progress = TRUE;
  }
  return;
}/* end Timer_Set_Timeout */

/* -------------------------------------------------------------------------
   Function    : Timer_Clear_Timeout
   Purpose     : Remove timer from timeout list
   Parameter1  : Id of Timer to remove
   Return      : void
*/
/* -------------------------------------------------------------------------*/
void Timer_Clear_Timeout(const e_EventTimerIds id)
{
  Time_Event_List[id].Timeout_In_Progress = FALSE;
  return;
}/* end Timer_Clear_Timeout */

/* -------------------------------------------------------------------------
   Function    : Timer_Event_Manage
   Purpose     : Manage the callbacks on timer event timeouts
   Parameter1  : void
   Return      : void
*/
/* -------------------------------------------------------------------------*/
void Timer_Event_Manage(void)
{
  e_EventTimerIds id;

  /* Handle all timer events */
  for (id = 0; id < MAX_TIMER_EVENTS; id++)
  {
    if ( Time_Event_List[id].Timeout_In_Progress == TRUE )
    {
      if ( Time_Event_List[id].Timeout_Counter >= Time_Event_List[id].Timeout_Value )
	  {
	    Time_Event_List[id].Timeout_In_Progress = FALSE;
        Time_Event_List[id].Timer_Event_Fn_Ptr();
	  }
    }/* endif timer set */
  }/* endfor */
  return;
}/* end Timer_Event_Manage */

/* =========================================================================
   Private Functions */
/* -------------------------------------------------------------------------
   Function    : SIGNAL(SIG_OVERFLOW0)
   Purpose     : Interrupt for 8-Bit Timer-0
   Parameter1  : void
   Return      : void
*/
/* -------------------------------------------------------------------------*/
SIGNAL(SIG_OVERFLOW1)
{
  e_EventTimerIds id;

  TCNT1H = PRE_LOAD_TIM1H;
  TCNT1L = PRE_LOAD_TIM1L;
  Timer0_Count_10MS++;

  if (Timer0_Count_10MS %TIMER0_1SEC == 0)
  {
    timer01sec();
  }

  /* Handle all timers */
  for (id = 0; id < MAX_TIMER_EVENTS; id++)
  {
    if ( Time_Event_List[id].Timeout_In_Progress == TRUE )
    {
      Time_Event_List[id].Timeout_Counter++;
    }/* endif timer set */
  }/* endfor */
 
}/* SIGNAL(SIG_OVERFLOW0) */

/* -------------------------------------------------------------------------
   Function    : timer01sec
   Purpose     : Function to call every 1 sec for timer 0
                 Put vars to increment for Application every 1sec
   Parameter1  : void
   Return      : void
*/
/* -------------------------------------------------------------------------*/
static void timer01sec(void)
{
  if (Toggle == 0)
  {
    Toggle = 1;
	cbi(PORTD,PD7);
  }
  else
  {
    Toggle = 0;
	sbi(PORTD,PD7);
  }

//  Ique_Put(SERIALTX_Q,'?');

  //Uart_Tx_Char('?');
  return;
}/*timer01sec*/

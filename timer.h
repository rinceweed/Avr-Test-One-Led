/***************************************************************************

     Description: Timer and interrupt Functionality

     Project: Rat

     Reference:

     Date:	14/06/2003

****************************************************************************/
/* =========================================================================
   Compiler Controls */
#ifndef __TIMER_H_
#define __TIMER_H_

/* =========================================================================
   Types */
typedef enum
{
  TIMER_FLASH_LED = 0,
  MAX_TIMER_EVENTS
}e_EventTimerIds;

typedef void(*fp_Timer_Event)(void);

typedef struct
{
  uint16_t               Timeout_Value;
  uint16_t               Timeout_Counter;
  eTrue_False_Type	     Timeout_In_Progress;
  fp_Timer_Event         Timer_Event_Fn_Ptr;
}s_TIMER_Event;

/* =========================================================================
   Function Prototypes */
void Timer_Init(void);
void Timer_Set_Timeout(const e_EventTimerIds id,
                       const fp_Timer_Event fn_ptr,
                       const uint16_t then);

void Timer_Clear_Timeout(const e_EventTimerIds id);
void Timer_Event_Manage(void);

/* =========================================================================
   Compiler Controls */
#endif /* __TIMER_H_ */

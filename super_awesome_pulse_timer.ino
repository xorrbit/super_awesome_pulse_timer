/* super awesome pulse timer v1
 * By Andrew Orr
 */

#ifdef __cplusplus
extern "C" {
#endif

#include <inc/hw_timer.h>
#include <inc/hw_ints.h>
#include <driverlib/timer.h>
#include <driverlib/sysctl.h>
#include <driverlib/interrupt.h>
#include <driverlib/systick.h>

volatile unsigned long g_last_measured_ticks = 0;

void timer_0_interrupt(void) {
  // timer 0 interrupt
  
  // save the timer config so we know which edge this is
  unsigned long timer_event_reg = HWREG(TIMER0_BASE + TIMER_O_CTL);

  // clear our interrupt mask  
  TimerIntClear(TIMER0_BASE, TIMER_CAPA_EVENT);
  
  if (timer_event_reg & TIMER_EVENT_NEG_EDGE) {
    // falling edge
    g_last_measured_ticks = 16777216 - SysTickValueGet();
    
    // set to positive edge
   TimerControlEvent(TIMER0_BASE, TIMER_A, TIMER_EVENT_POS_EDGE);
 } else {
    // rising edge!
    
    // reset our system tick counter to 0;
    SysTickPeriodSet(16777216);
    HWREG(NVIC_ST_CURRENT) = 0;

    // set to negative edge
    TimerControlEvent(TIMER0_BASE, TIMER_A, TIMER_EVENT_NEG_EDGE);
  }
}

void setup() { 
  //  80 mhz
  SysCtlClockSet(SYSCTL_SYSDIV_2_5 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);

  Serial.begin(9600);
  Serial.println("Starting super awesome pulse timer r3!...");
  
  // Configure PB6 as T0CCP0
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
  GPIOPinConfigure(GPIO_PB6_T0CCP0);
  GPIOPinTypeTimer(GPIO_PORTB_BASE, GPIO_PIN_6);

  // Configure timer
  
  // enable timer 0
  SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
  
  // timer a is a time capture event timer
  TimerConfigure(TIMER0_BASE, TIMER_CFG_SPLIT_PAIR | TIMER_CFG_A_CAP_TIME);
  
  // both edges
  TimerControlEvent(TIMER0_BASE, TIMER_A, TIMER_EVENT_POS_EDGE);

  // register our interrupt 
  TimerIntRegister(TIMER0_BASE, TIMER_A, timer_0_interrupt);
  
  // interrupt on timer a capture event
  TimerIntEnable(TIMER0_BASE, TIMER_CAPA_EVENT);
  
  // enable
  IntEnable(INT_TIMER0A);
  
  // enable
  IntMasterEnable();
  
  // and turn it on
  TimerEnable(TIMER0_BASE, TIMER_A);
  
}

void loop() {
  Serial.print("measured ticks : ");
  Serial.println(g_last_measured_ticks);
  Serial.print("time: ");
  Serial.println(g_last_measured_ticks * 0.0125);
  Serial.println(" ns");
  delay(50);
}


#ifdef __cplusplus
}
#endif


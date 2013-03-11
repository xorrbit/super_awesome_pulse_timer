/* super awesome pulse timer v1
 * By Andrew Orr
 */

#include <inc/hw_timer.h>
#include <inc/hw_ints.h>
#include <driverlib/timer.h>
#include <driverlib/sysctl.h>
#include <driverlib/interrupt.h>

unsigned long g_last_measured_time = 0;
 
void timer_0_interrupt(void) {
  // timer 0 interrupt
  
  // save the timer config so we know which edge this is
  unsigned long timer_event_reg = HWREG(TIMER0_BASE + TIMER_O_CTL);
  
  if ((timer_event_reg & TIMER_EVENT_POS_EDGE) == TIMER_EVENT_POS_EDGE) {
    // rising edge!
    // clear int mask
    TimerIntClear(TIMER0_BASE, TIMER_CAPA_EVENT);
    
    // reset value
    TimerLoadSet(TIMER0_BASE, TIMER_A, 0);
    
    // set to negative edge
    TimerControlEvent(TIMER0_BASE, TIMER_A, TIMER_EVENT_NEG_EDGE);
    
  } else if ((timer_event_reg & TIMER_EVENT_NEG_EDGE) == TIMER_EVENT_NEG_EDGE) {
    // falling edge!
    // clear int mask
    TimerIntClear(TIMER0_BASE, TIMER_CAPA_EVENT);
    
    // save our measure ticks
    g_last_measured_time =  TimerValueGet(TIMER0_BASE, TIMER_A);
    
    // set to positive edge
    TimerControlEvent(TIMER0_BASE, TIMER_A, TIMER_EVENT_POS_EDGE);
  }
}

void setup() {                
  Serial.begin(9600);
  Serial.println("Starting super awesome pulse timer!...");

  // Configure PB6 as T0CCP0
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
  GPIOPinConfigure(GPIO_PB6_T0CCP0);
  GPIOPinTypeTimer(GPIO_PORTB_BASE, GPIO_PIN_6);

  // Configure timer
  
  // enable timer 0
  SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
  
  // timer a is a time capture event timer
  TimerConfigure(TIMER0_BASE, TIMER_CFG_SPLIT_PAIR | TIMER_CFG_A_CAP_TIME);
  
  // positive edge for now
  TimerControlEvent(TIMER0_BASE, TIMER_A, TIMER_EVENT_POS_EDGE);
  
  // reset to 0
  TimerLoadSet(TIMER0_BASE, TIMER_A, 0);

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
  Serial.print("Ticks: ");
  Serial.println(g_last_measured_time);
  delay(10000);
}


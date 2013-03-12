/* super awesome pulse timer v3
 * By Andrew Orr
 * hook your junk up to PB6 (4 pins up from bottom right corner)
 */

#include <inc/hw_timer.h>
#include <inc/hw_ints.h>
#include <driverlib/timer.h>
#include <driverlib/sysctl.h>
#include <driverlib/interrupt.h>
#include <driverlib/systick.h>

volatile unsigned long g_last_measured_ticks = 0;

unsigned long g_rolling_average[100] = {0};
unsigned long g_current_total = 0;
unsigned int g_current_pos = 0;
unsigned long g_current_average = 0;

void timer_0_interrupt(void) {
  // timer 0 interrupt
  
  // save the timer config so we know which edge this is
  unsigned long timer_event_reg = HWREG(TIMER0_BASE + TIMER_O_CTL);

  // clear our interrupt mask  
  TimerIntClear(TIMER0_BASE, TIMER_CAPA_EVENT);
  
  if (timer_event_reg & TIMER_EVENT_NEG_EDGE) {
    // falling edge!
    
    // save our ticks
    g_last_measured_ticks = 16777216 - SysTickValueGet();
    
    // this stuff just keeps track of the last 100 readings to create an average
    g_current_total -= g_rolling_average[g_current_pos];
    g_rolling_average[g_current_pos++] = g_last_measured_ticks;
    g_current_total += g_last_measured_ticks;
    if (g_current_pos == 100)
      g_current_pos = 0;
    g_current_average = g_current_total / 100;
       
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
  Serial.println("Starting super awesome pulse timer v3!...");
  
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
  Serial.print(g_last_measured_ticks);
  Serial.print(", time: ");
  Serial.print(g_last_measured_ticks * 0.0125);
  Serial.print(" µs, current average: ");
  Serial.print(g_current_average);
  Serial.print(", current average time: ");
  Serial.print(g_current_average * 0.0125);
  Serial.println(" µs");
  delay(50);
}


/* super awesome pulse timer v1
 * By Andrew Orr
 */

#include <driverlib/timer.h>
#include <driverlib/sysctl.h>

int g_edge = TIMER_EVENT_POS_EDGE;
int g_last_measured_time = 0;
 
void timer_a_interrupt(void) {
  // timer a interrupt
  
  if (g_edge == TIMER_EVENT_POS_EDGE) {
    // rising edge!
    TimerLoadSet(TIMER0_BASE, TIMER_A, 0); // reset value
    
    // set to negative edge
    g_edge = TIMER_EVENT_POS_EDGE;
    TimerControlEvent(TIMER0_BASE, TIMER_A, g_edge);
    
  } else if (g_edge == TIMER_EVENT_NEG_EDGE) {
    // falling edge!
    g_last_measured_time =  TimerValueGet(TIMER0_BASE, TIMER_A);
    
    g_edge = TIMER_EVENT_POS_EDGE;
    TimerControlEvent(TIMER0_BASE, TIMER_A, g_edge);
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
  TimerIntRegister(TIMER0_BASE, TIMER_A, timer_a_interrupt);
  
  // interrupt on timer a capture event
  TimerIntEnable(TIMER0_BASE, TIMER_CAPA_EVENT);
  
  // and turn it on
  TimerEnable(TIMER0_BASE, TIMER_A);
  
}

void loop() {
  Serial.print("Ticks: ");
  Serial.println(g_last_measured_time);
  delay(10000);
}


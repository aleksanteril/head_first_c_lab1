#include <Arduino.h>
#define sleep() __asm__ __volatile__ ("sleep" ::: "memory")
int sleep_count = 0;

void setup_adc()
{
      //Set, ADEN bit 7 on
      ADCSRA |= (1<<ADEN);
      //Set, AD Vref to Vcc bit 6 on
      ADMUX |= (1<<REFS0);
      //ADC Prescaler set /8 = 125khz
      ADCSRA |= 0x03;
      //Start conversion, trigger enable
      ADCSRA |= (1<<ADSC) | (1<<ADATE);
}

void setup()
{    
      cli(); // Disable interrupts

      //Low power, under clock the CPU
      CLKPR = 0x80; // Enable CLKPCE, disable all other
      CLKPR = 0x04; // Set prescaler to 16 = Clock 1Mhz
      
      //Set onboard LED PIN (PB5) to Output
      DDRB |= (1<<PB5);

      //Set sleep-mode "Power-down"
      SMCR |= 0x04;

      MCUSR &= ~(1<<WDRF);
      //Watchdog Set-up to interrupt 8s
      WDTCSR = 0x18; //WDCE and WDE logic one
      WDTCSR = 0x21; // 8s time clock, interrupt
      WDTCSR |= (1<<WDIE); //Interrupt enable

      setup_adc();

      sei(); //Enable interrupts
}

int read_adc()
{
      //Read ADC high and low register for values
      return ADCL+(ADCH << 0x8);
}

void blink_led()
{
      PORTB ^= (1<<PB5); //XOR on/off led
      delay(300);
}

ISR(WDT_vect)
{     
      //Increment sleep counter when interrupt triggered
      sleep_count++;
}

void loop()
{     
      while (read_adc() < 780) blink_led();

      //Prepare for sleep
      PORTB &= ~(1<<PB5); //0 to led
      ADCSRA &= ~(1<<ADEN); //ADC Off
      SMCR |= (1<<SE); //Sleep enable on

      //Go back to sleep until time is to check moisture 10 min intervals.
      while (sleep_count*8 < 600) sleep();

      //Wake up routine, when timeout
      SMCR &= ~(1<<SE); //Sleep enable off
      sleep_count = 0;
      setup_adc(); //Set-up adc, wake up
      delay(100);
}

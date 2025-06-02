#define sleep() __asm__ __volatile__ ("sleep" ::: "memory")
#define sleep_enable() SMCR |= (1<<SE)
#define sleep_disable() SMCR &= ~(1<<SE)
#define adc_disable() ADCSRA &= ~(1<<ADEN)
#define led_toggle() PORTB ^= (1<<PB5); //XOR on/off led
#define led_off() PORTB &= ~(1<<PB5); //0 to led

#define SLEEP_TIME 600 // 10 mins

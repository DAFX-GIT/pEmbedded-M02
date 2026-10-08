#define BAUD 115200UL
#include <avr/io.h>
#include <avr/interrupt.h>

static const char msg[] = "HelloWorld\r\n";
static const char * volatile tx_ptr = 0 ;

void uart_init(void) {
    uint16_t ubrr = (F_CPU / (8UL * BAUD)) - 1;   // U2X mode
    UBRR0H = (uint8_t)(ubrr >> 8);					//ubrr > 8 so we set on two 8bit hard reg. here HIGH 
    UBRR0L = (uint8_t)ubrr;							// here LOW
    UCSR0A = (1 << U2X0);                          // also clears leftovers from a bootloader
    UCSR0B = (1 << RXEN0) | (1 << TXEN0);			//transciver and receiver on.
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);        // 8N1
}

void timer1_init() {
	TCCR1A = 0;
	TCCR1B = (1 << WGM12) |  			//CTC mode 
			(1 << CS12) | (1 << CS10); // prescaler to 1024
	OCR1A = (F_CPU / 1024 * 2) - 1;
	TIMSK1 = (1 << OCIE1A);				// trigger ISR(timer1) when reached 2sec. 
}

void uart_tx(char c) {
	while (!(UCSR0A & (1 << UDRE0))) {}	// wait until UDR0 is empty
	UDR0 = c;
}

void uart_printstr(const char* str) {
	while (*str)
		uart_tx(*str++);
}

ISR(TIMER1_COMPA_vect) {				// runs every 2s, by itself
	uart_printstr("Hello World!\r\n");
}

int main () {
	uart_init();
	timer1_init();
	sei(); 				//activate interrupts 
	while(1) {

	}
}
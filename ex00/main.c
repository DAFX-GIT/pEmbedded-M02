#define BAUD 115200UL
#include <avr/io.h>
#include <util/delay.h>

void uart_init(void) {
    uint16_t ubrr = (F_CPU / (8UL * BAUD)) - 1;   // U2X mode
    UBRR0H = (uint8_t)(ubrr >> 8);					//ubrr > 8 so we set on two 8bit hard reg. here HIGH 
    UBRR0L = (uint8_t)ubrr;							// here LOW
    UCSR0A = (1 << U2X0);                          // also clears leftovers from a bootloader
    UCSR0B = (1 << RXEN0) | (1 << TXEN0);			//transciver and receiver on.
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);        // 8N1
}

void uart_tx(char c) {
	while (!(UCSR0A & (1 << UDRE0)));
	UDR0 = 'Z';
}

int main () {
	uart_init();
	
	while(1) {
		uart_tx('Z');
		_delay_ms(1000);
	}
}
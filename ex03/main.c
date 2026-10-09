#define BAUD 115200UL
#include <avr/io.h>
//#include <avr/interrupt.h>

#define sei()  __asm__ __volatile__ ("sei" ::: "memory")
#define __INTR_ATTRS __used__, __externally_visible__
#define ISR(vector) void vector (void) __attribute__ ((__signal__,__INTR_ATTRS)) ; void vector (void)

void uart_init(void) {
    uint16_t ubrr = (F_CPU / (8UL * BAUD)) - 1;   // U2X mode
    UBRR0H = (uint8_t)(ubrr >> 8);					//ubrr > 8 so we set on two 8bit hard reg. here HIGH 
    UBRR0L = (uint8_t)ubrr;							// here LOW
    UCSR0A = (1 << U2X0);                          // also clears leftovers from a bootloader
    UCSR0B = (1 << RXEN0) | (1 << TXEN0) | (1 << RXCIE0);			//transciver and receiver on.
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);        // 8N1
}

void uart_tx(char c) {
	while (!(UCSR0A & (1 << UDRE0))) {}	// wait until UDR0 is empty
	UDR0 = c;
}

char uart_rx(void) {
	while (!(UCSR0A & (1 << RXC0))) {}   // wait until a byte is received
    return UDR0;
}

ISR(__vector_18) {                      // fires when a byte arrives. vector18 = USART_RX_vect
    char c = uart_rx();
	if (c >= 65 && c <=90) { 
		c+= 32;
	} else if (c >= 97 && c <= 122){
		c -= 32;
	}
    uart_tx(c);
}


int main () {
	uart_init();
	sei();
	while(1) {
	}
}
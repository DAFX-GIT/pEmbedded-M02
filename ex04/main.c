#define BAUD 115200UL
#define BUF_MAX 32
#include <avr/io.h>


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

void uart_printstr(const char* str) {
	while (*str)
		uart_tx(*str++);
}

uint8_t uart_readstr(char *buf, uint8_t size) {
	uint8_t i = 0;
	while (1) {
		char c = uart_rx();
		
		if(c == '\r' || c == '\n') {
			if (i == 0) continue;
			break;
		}
		if (i < size - 1) { 
			buf[i++] = c;
			uart_tx(c);
		}
	}
	buf[i] = '\0';
	return i;
}

int my_strcmp(const char *s1, const char *s2) {
	while (*s1 && *s1 == *s2) {
		s1++;
		s2++;
	}
	return (unsigned char)*s1 - (unsigned char)*s2;
}

uint8_t check_login(char login[BUF_MAX], char password[BUF_MAX]) {
	return my_strcmp(login, "oui") == 0 && my_strcmp(password, "ouioui") == 0;
}

static void set_rgb(uint8_t r, uint8_t g, uint8_t b)
{
	PORTD &= ~((1 << PD6) | (1 << PD5) | (1 << PB3));
    if (r) PORTD |= (1 << PD5);
    if (g) PORTD |= (1 << PD6);
    if (b) PORTD |= (1 << PD3);
}

int main () {
	uart_init();
	char login[BUF_MAX];
	char password[BUF_MAX];
	while(1) {
		while (1) {
			uart_printstr("Enter your login:\r\n\tusername: ");
			uart_readstr(login, BUF_MAX);
			uart_printstr("\r\n\tpassword: ");
			uart_readstr(password, BUF_MAX);
			if (check_login(login, password)) break;
			else {
				uart_printstr("\r\nBad combinaison username/password\r\n\r\n");
			}
		}
		uart_printstr("\r\nHello ");
		uart_printstr(login);
		uart_printstr("!\r\nShall we play a game?");
		break;
	}
	DDRD |= (1 << PD6) | (1 << PD5) | (1 << PB3);
	while (1) {
		set_rgb(255, 0, 0);
		_delay_ms(1000);
		set_rgb(0, 255, 0);
		_delay_ms(1000);
		set_rgb(0, 0, 255);
		_delay_ms(1000);
	}
}
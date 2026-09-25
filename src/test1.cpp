#define F_CPU 16000000UL

#include <avr/io.h>
#include <stdint.h>
DDRD &= ~(1 << PD2);
DDRB |= (1 << PB5);
DDRB |= (1 << PB3);

#define F_CPU 16000000UL

void UART_Init(void)
{
    uint16_t ubrr = 103;

    UBRR0H = (uint8_t)(ubrr >> 8);
    UBRR0L = (uint8_t)ubrr;

    UCSR0B = (1 << TXEN0);

    UCSR0C = (1 << UCSZ01) |
             (1 << UCSZ00);
}

current_state =
    (PIND & (1 << PD2)) ? 1 : 0;

    if (current_state)
{
    PORTB |= (1 << PB5);
    PORTB |= (1 << PB3);
}
else
{
    PORTB &= ~(1 << PB5);
    PORTB &= ~(1 << PB3);
}

uint8_t previous_state = 0;
uint8_t current_state;

if (current_state != previous_state)
{
    if (current_state)
    {
        UART_SendString("Motion detected!\r\n");
    }
    else
    {
        UART_SendString("No motion.\r\n");
    }

    previous_state = current_state;
}

while (1)
{
    current_state =
        (PIND & (1 << PD2)) ? 1 : 0;

    if (current_state)
    {
        PORTB |= (1 << PB5);
        PORTB |= (1 << PB3);
    }
    else
    {
        PORTB &= ~(1 << PB5);
        PORTB &= ~(1 << PB3);
    }

    if (current_state != previous_state)
    {
        if (current_state)
            UART_SendString("Motion detected!\r\n");
        else
            UART_SendString("No motion.\r\n");

        previous_state = current_state;
    }
}
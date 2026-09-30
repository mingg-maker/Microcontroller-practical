#define F_CPU 16000000UL

#include <avr/io.h>
#include <stdint.h>

#define PIR_PIN     PD2
#define LED_PIN     PB5
#define BUZZER_PIN  PB3

void UART_Init(void)
{
    uint16_t ubrr = 103;

    UBRR0H = (uint8_t)(ubrr >> 8);
    UBRR0L = (uint8_t)ubrr;

    UCSR0B = (1 << TXEN0);

    UCSR0C = (1 << UCSZ01) |
             (1 << UCSZ00);
}

void UART_SendChar(char c)
{
    while (!(UCSR0A & (1 << UDRE0)));

    UDR0 = c;
}

void UART_SendString(const char *s)
{
    while (*s)
    {
        UART_SendChar(*s++);
    }
}

void GPIO_Init(void)
{
    DDRD &= ~(1 << PIR_PIN);

    DDRB |= (1 << LED_PIN);

    DDRB |= (1 << BUZZER_PIN);

    PORTB &= ~(1 << LED_PIN);
    PORTB &= ~(1 << BUZZER_PIN);
}

int main(void)
{
    uint8_t current_state;
    uint8_t previous_state = 0;

    GPIO_Init();
    UART_Init();

    UART_SendString("PIR Motion Detection\r\n");

    while (1)
    {
        current_state =
            (PIND & (1 << PIR_PIN)) ? 1 : 0;

        if (current_state)
        {
            PORTB |= (1 << LED_PIN);

            PORTB |= (1 << BUZZER_PIN);
        }
        else
        {
            PORTB &= ~(1 << LED_PIN);

            PORTB &= ~(1 << BUZZER_PIN);
        }

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
    }
}
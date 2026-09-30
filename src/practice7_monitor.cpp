#define F_CPU 16000000UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>

volatile uint16_t previous_capture = 0;
volatile uint16_t current_capture = 0;
volatile uint16_t period_ticks = 0;
volatile uint8_t capture_done = 0;

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

void UART_SendNumber(uint32_t number)
{
    char buffer[10];
    uint8_t i = 0;

    if (number == 0)
    {
        UART_SendChar('0');
        return;
    }

    while (number > 0)
    {
        buffer[i++] = '0' + (number % 10);
        number /= 10;
    }

    while (i > 0)
    {
        UART_SendChar(buffer[--i]);
    }
}

void Timer2_Output_Init(void)
{
    DDRB |= (1 << PB3);

    TCCR2A = (1 << COM2A0) |
             (1 << WGM21);

    TCCR2B = (1 << CS22);

    OCR2A = 124;
}

void InputCapture_Init(void)
{
    DDRB &= ~(1 << PB0);

    PORTB &= ~(1 << PB0);

    TCCR1A = 0;

    TCCR1B = (1 << ICES1) |
             (1 << CS11);

    TCNT1 = 0;

    TIFR1 = (1 << ICF1);

    TIMSK1 = (1 << ICIE1);
}

ISR(TIMER1_CAPT_vect)
{
    current_capture = ICR1;

    period_ticks =
        current_capture - previous_capture;

    previous_capture = current_capture;

    capture_done = 1;
}

int main(void)
{
    uint16_t period;
    uint32_t frequency;

    UART_Init();

    Timer2_Output_Init();

    InputCapture_Init();

    sei();

    UART_SendString("\r\nINPUT CAPTURE TEST\r\n");
    UART_SendString("PB3 -> PB0\r\n");

    while (1)
    {
        if (capture_done)
        {
            cli();

            period = period_ticks;

            capture_done = 0;

            sei();

            if (period != 0)
            {
                frequency = 2000000UL / period;

                UART_SendString("Period = ");
                UART_SendNumber(period);

                UART_SendString(" ticks | Frequency = ");
                UART_SendNumber(frequency);

                UART_SendString(" Hz\r\n");
            }
        }
    }
}
#define F_CPU 16000000UL

#include <avr/io.h>

int main(void)
{
    // PB0 = LED
    // PB2 = Buzzer
    // PB3 = Relay
    DDRB |= (1 << PB0) |
            (1 << PB2) |
            (1 << PB3);

    // PD2 = Button
    DDRD &= ~(1 << PD2);

    // Enable internal pull-up
    PORTD |= (1 << PD2);

    PORTB &= ~(1 << PB0);
    PORTB &= ~(1 << PB2);
    PORTB &= ~(1 << PB3);

    while (1)
    {
        if (!(PIND & (1 << PD2)))
        {
            PORTB |= (1 << PB0);   // LED ON
            PORTB |= (1 << PB2);   // Buzzer ON
            PORTB |= (1 << PB3);   // Relay ON
        }
        else
        {
            PORTB &= ~(1 << PB0);  // LED OFF
            PORTB &= ~(1 << PB2);  // Buzzer OFF
            PORTB &= ~(1 << PB3);  // Relay OFF
        }
    }

    return 0;
}
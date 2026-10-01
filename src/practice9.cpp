#include <Arduino.h>
#include <avr/io.h>

#define SERVO_MIN_US  600
#define SERVO_MAX_US  2400

void UART_init()
{
    UBRR0H = 0;
    UBRR0L = 103;

    UCSR0B = (1 << RXEN0) | (1 << TXEN0);

    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}

void UART_sendChar(char c)
{
    while (!(UCSR0A & (1 << UDRE0)));

    UDR0 = c;
}

void UART_sendString(const char *str)
{
    while (*str)
    {
        UART_sendChar(*str++);
    }
}

void Servo_init()
{
    DDRB |= (1 << PB1);

    TCCR1A =
        (1 << COM1A1) |
        (1 << WGM11);

    TCCR1B =
        (1 << WGM13) |
        (1 << WGM12) |
        (1 << CS11);

    ICR1 = 39999;

    OCR1A = 3000;
}

void Servo_setMicroseconds(uint16_t us)
{
    OCR1A = us * 2;
}

void Servo_setAngle(int angle)
{
    if (angle < 0)
        angle = 0;

    if (angle > 180)
        angle = 180;

    uint16_t pulse;

    pulse =
        SERVO_MIN_US +
        ((uint32_t)angle *
         (SERVO_MAX_US - SERVO_MIN_US)) / 180;

    Servo_setMicroseconds(pulse);
}

void processSerial()
{
    static char buffer[10];
    static uint8_t index = 0;

    if (UCSR0A & (1 << RXC0))
    {
        char c = UDR0;

        if (c == '\r' || c == '\n')
        {
            if (index == 0)
                return;

            buffer[index] = '\0';

            int angle = atoi(buffer);

            index = 0;

            if (angle >= 0 && angle <= 180)
            {
                Servo_setAngle(angle);

                UART_sendString("OK: ");
                
                char msg[10];
                itoa(angle, msg, 10);

                UART_sendString(msg);
                UART_sendString(" degrees\r\n");
            }
            else
            {
                UART_sendString(
                    "ERROR: Angle must be 0-180\r\n"
                );
            }
        }
        else if (c >= '0' && c <= '9')
        {
            if (index < sizeof(buffer) - 1)
            {
                buffer[index++] = c;
            }
        }
        else
        {
            UART_sendString(
                "ERROR: Enter an angle from 0 to 180\r\n"
            );

            index = 0;
        }
    }
}

int main()
{
    UART_init();
    Servo_init();

    UART_sendString("\r\n");
    UART_sendString("===============================\r\n");
    UART_sendString("ATmega328P SERVO CONTROL\r\n");
    UART_sendString("===============================\r\n");
    UART_sendString("Enter angle 0-180:\r\n");

    while (1)
    {
        processSerial();
    }

    return 0;
}
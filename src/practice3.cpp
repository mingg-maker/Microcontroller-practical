#include <Arduino.h>

void servo_timer1_init()
{
    DDRB |= (1 << PB1);

    TCCR1A = (1 << COM1A1) | (1 << WGM11);

    TCCR1B = (1 << WGM13) |
             (1 << WGM12) |
             (1 << CS11);

    ICR1 = 39999;

    OCR1A = 30000;
}

void led_pwm_timer0_init()
{
    DDRD |= (1 << PD6);

    TCCR0A = (1 << COM0A1) |
             (1 << WGM01) |
             (1 << WGM00);

    TCCR0B = (1 << CS01) |
             (1 << CS00);

    OCR0A = 0;
}

void set_servo_angle(uint8_t angle)
{
    if (angle > 180)
        angle = 180;

    uint16_t pulse_ticks =
        2000 + ((uint32_t)angle * 2000) / 180;

    OCR1A = pulse_ticks;
}

void set_led_brightness(uint8_t duty)
{
    OCR0A = duty;
}

void setup()
{
    servo_timer1_init();
    led_pwm_timer0_init();
}

void loop()
{
    for (uint8_t pos = 0; pos <= 180; pos += 10)
    {
        set_servo_angle(pos);
        set_led_brightness((uint16_t)pos * 255 / 180);
        delay(100);
    }

    for (int16_t pos = 180; pos >= 0; pos -= 10)
    {
        set_servo_angle((uint8_t)pos);
        set_led_brightness((uint16_t)pos * 255 / 180);
        delay(100);
    }
}
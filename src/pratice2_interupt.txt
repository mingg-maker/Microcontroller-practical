#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdint.h>

#define LCD_RS PB0
#define LCD_EN PB1
#define LCD_D4 PB2
#define LCD_D5 PB3
#define LCD_D6 PB4
#define LCD_D7 PB5

void LCD_Enable(void)
{
    PORTB |= (1 << LCD_EN);
    _delay_us(1);

    PORTB &= ~(1 << LCD_EN);
    _delay_us(100);
}

void LCD_Send4Bit(uint8_t data)
{
    PORTB &= ~((1 << LCD_D4) |
               (1 << LCD_D5) |
               (1 << LCD_D6) |
               (1 << LCD_D7));

    if (data & 0x01)
        PORTB |= (1 << LCD_D4);

    if (data & 0x02)
        PORTB |= (1 << LCD_D5);

    if (data & 0x04)
        PORTB |= (1 << LCD_D6);

    if (data & 0x08)
        PORTB |= (1 << LCD_D7);

    LCD_Enable();
}

void LCD_Command(uint8_t command)
{
    PORTB &= ~(1 << LCD_RS);

    LCD_Send4Bit(command >> 4);
    LCD_Send4Bit(command & 0x0F);

    _delay_ms(2);
}

void LCD_Data(uint8_t data)
{
    PORTB |= (1 << LCD_RS);

    LCD_Send4Bit(data >> 4);
    LCD_Send4Bit(data & 0x0F);

    _delay_us(100);
}

void LCD_Init(void)
{
    DDRB |= (1 << LCD_RS) |
            (1 << LCD_EN) |
            (1 << LCD_D4) |
            (1 << LCD_D5) |
            (1 << LCD_D6) |
            (1 << LCD_D7);

    PORTB &= ~((1 << LCD_RS) |
               (1 << LCD_EN) |
               (1 << LCD_D4) |
               (1 << LCD_D5) |
               (1 << LCD_D6) |
               (1 << LCD_D7));

    _delay_ms(20);

    LCD_Send4Bit(0x03);
    _delay_ms(5);

    LCD_Send4Bit(0x03);
    _delay_us(150);

    LCD_Send4Bit(0x03);
    _delay_us(150);

    LCD_Send4Bit(0x02);
    _delay_us(150);

    LCD_Command(0x28);
    LCD_Command(0x0C);
    LCD_Command(0x01);
    LCD_Command(0x06);

    _delay_ms(2);
}

void LCD_Clear(void)
{
    LCD_Command(0x01);
    _delay_ms(2);
}

void LCD_SetCursor(uint8_t row, uint8_t column)
{
    uint8_t address;

    if (row == 0)
        address = 0x80 + column;
    else
        address = 0xC0 + column;

    LCD_Command(address);
}

void LCD_Print(const char *str)
{
    while (*str)
    {
        LCD_Data(*str);
        str++;
    }
}

void LCD_PrintNumber(uint16_t number)
{
    char buffer[6];
    uint8_t i = 0;

    if (number == 0)
    {
        LCD_Data('0');
        return;
    }

    while (number > 0)
    {
        buffer[i++] = (number % 10) + '0';
        number /= 10;
    }

    while (i > 0)
    {
        LCD_Data(buffer[--i]);
    }
}

volatile uint32_t adc_sum = 0;
volatile uint8_t adc_sample_count = 0;
volatile uint16_t adc_average = 0;
volatile uint8_t adc_data_ready = 0;

void ADC_Init(void)
{
    ADMUX = (1 << REFS0);

    ADCSRA = (1 << ADEN) |
             (1 << ADIE) |
             (1 << ADPS2) |
             (1 << ADPS1) |
             (1 << ADPS0);

    DDRC &= ~(1 << PC0);

    DIDR0 |= (1 << ADC0D);
}

ISR(ADC_vect)
{
    uint8_t low;
    uint8_t high;
    uint16_t adc_value;

    low = ADCL;
    high = ADCH;

    adc_value = ((uint16_t)high << 8) | low;

    adc_sum += adc_value;

    adc_sample_count++;

    if (adc_sample_count >= 16)
    {
        adc_average = (uint16_t)(adc_sum >> 4);

        adc_data_ready = 1;

        adc_sum = 0;
        adc_sample_count = 0;
    }

    ADCSRA |= (1 << ADSC);
}

uint16_t ADC_To_mV(uint16_t adc)
{
    return (uint32_t)adc * 5000UL / 1023UL;
}

int main(void)
{
    uint16_t adc_value;
    uint16_t voltage_mV;
    uint16_t voltage_integer;
    uint16_t voltage_decimal;

    LCD_Init();

    ADC_Init();

    sei();

    ADCSRA |= (1 << ADSC);

    LCD_Clear();

    while (1)
    {
        if (adc_data_ready)
        {
            cli();

            adc_value = adc_average;

            adc_data_ready = 0;

            sei();

            voltage_mV = ADC_To_mV(adc_value);

            voltage_integer = voltage_mV / 1000;
            voltage_decimal = voltage_mV % 1000;

            LCD_SetCursor(0, 0);

            LCD_Print("ADC: ");

            LCD_PrintNumber(adc_value);

            LCD_Print("     ");

            LCD_SetCursor(1, 0);

            LCD_Print("V: ");

            LCD_PrintNumber(voltage_integer);

            LCD_Data('.');

            if (voltage_decimal < 100)
            {
                LCD_Data('0');
            }

            if (voltage_decimal < 10)
            {
                LCD_Data('0');
            }

            LCD_PrintNumber(voltage_decimal);

            LCD_Print(" V");

            _delay_ms(200);
        }
    }
}
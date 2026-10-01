#define F_CPU 16000000UL

#include <Arduino.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>

#define ADC_PIN       A0
#define SENSOR_PIN    A1
#define LED_PIN       11

#define ADC_PERIOD       10
#define LED_PERIOD       20
#define SENSOR_PERIOD    100
#define UART_PERIOD      1000

volatile uint32_t systemTick = 0;

uint32_t lastADCTime = 0;
uint32_t lastLEDTime = 0;
uint32_t lastSensorTime = 0;
uint32_t lastUARTTime = 0;

uint16_t adcValue = 0;
uint16_t sensorValue = 0;

uint8_t pwmValue = 0;

void timer1_init()
{
    TCCR1A = 0;
    TCCR1B = 0;

    TCCR1B |= (1 << WGM12);

    OCR1A = 249;

    TIMSK1 |= (1 << OCIE1A);

    TCCR1B |= (1 << CS11) | (1 << CS10);
}

ISR(TIMER1_COMPA_vect)
{
    systemTick++;
}

void PWM_Init()
{
    DDRB |= (1 << PB3);

    TCCR2A = (1 << COM2A1) |
             (1 << WGM21)  |
             (1 << WGM20);

    TCCR2B = (1 << CS22);

    OCR2A = 128;
}

void PWM_SetDuty(uint8_t duty)
{
    OCR2A = duty;
}

void task_ADC()
{
    adcValue = analogRead(ADC_PIN);
}

void task_PWM()
{
    static uint8_t duty = 0;

    duty += 5;

    if (duty >= 250)
    {
        duty = 0;
    }

    PWM_SetDuty(duty);

    pwmValue = duty;
}

void task_Sensor()
{
    sensorValue = analogRead(SENSOR_PIN);
}

void task_UART()
{
    Serial.print("Time = ");
    Serial.print(systemTick);

    Serial.print(" ms");

    Serial.print(" | ADC = ");
    Serial.print(adcValue);

    Serial.print(" | PWM = ");
    Serial.print(pwmValue);

    Serial.print(" | Sensor = ");
    Serial.println(sensorValue);
}

void setup()
{
    pinMode(ADC_PIN, INPUT);

    pinMode(SENSOR_PIN, INPUT);

    pinMode(LED_PIN, OUTPUT);

    Serial.begin(9600);

    timer1_init();

    PWM_Init();

    sei();

    Serial.println();
    Serial.println("====================================");
    Serial.println(" Practice 5: Timer and Scheduler");
    Serial.println("====================================");
    Serial.println("System started...");
    Serial.println();
}

void loop()
{
    uint32_t currentTime = systemTick;

    if ((currentTime - lastADCTime) >= ADC_PERIOD)
    {
        lastADCTime = currentTime;

        task_ADC();
    }

    if ((currentTime - lastLEDTime) >= LED_PERIOD)
    {
        lastLEDTime = currentTime;

        task_PWM();
    }

    if ((currentTime - lastSensorTime) >= SENSOR_PERIOD)
    {
        lastSensorTime = currentTime;

        task_Sensor();
    }

    if ((currentTime - lastUARTTime) >= UART_PERIOD)
    {
        lastUARTTime = currentTime;

        task_UART();
    }
}
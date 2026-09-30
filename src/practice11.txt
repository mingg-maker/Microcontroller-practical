#include <Arduino.h>

const int LDR_PIN = A0;
const int LED_PWM_PIN = 10;

const bool IS_COMMON_CATHODE = false;

const int SEGMENT_PINS[7] = {2, 3, 4, 5, 6, 7, 8};

const byte DIGIT_PATTERNS[10][7] = {
    {1, 1, 1, 1, 1, 1, 0},
    {0, 1, 1, 0, 0, 0, 0},
    {1, 1, 0, 1, 1, 0, 1},
    {1, 1, 1, 1, 0, 0, 1},
    {0, 1, 1, 0, 0, 1, 1},
    {1, 0, 1, 1, 0, 1, 1},
    {1, 0, 1, 1, 1, 1, 1},
    {1, 1, 1, 0, 0, 0, 0},
    {1, 1, 1, 1, 1, 1, 1},
    {1, 1, 1, 1, 0, 1, 1}
};

unsigned long lastSerialTime = 0;

void displayDigit(int digit)
{
    if (digit < 0)
        digit = 0;

    if (digit > 9)
        digit = 9;

    for (int i = 0; i < 7; i++)
    {
        byte bitState = DIGIT_PATTERNS[digit][i];

        if (!IS_COMMON_CATHODE)
        {
            bitState = !bitState;
        }

        digitalWrite(SEGMENT_PINS[i], bitState);
    }
}

void setup()
{
    Serial.begin(9600);

    pinMode(LED_PWM_PIN, OUTPUT);

    for (int i = 0; i < 7; i++)
    {
        pinMode(SEGMENT_PINS[i], OUTPUT);
    }

    Serial.println("=== ATMEGA328 PRACTICE 11: OPTIMIZED PWM READY ===");
}

void loop()
{
    int adcValue = analogRead(LDR_PIN);

    int pwmValue = 0;

    if (adcValue >= 650)
    {
        pwmValue = 0;
    }
    else
    {
        pwmValue = map(adcValue, 650, 150, 0, 255);
        pwmValue = constrain(pwmValue, 0, 255);
    }

    analogWrite(LED_PWM_PIN, pwmValue);

    int lightLevel = map(adcValue, 150, 650, 0, 9);
    lightLevel = constrain(lightLevel, 0, 9);

    displayDigit(lightLevel);

    if (millis() - lastSerialTime >= 500)
    {
        lastSerialTime = millis();

        Serial.print("LDR ADC: ");
        Serial.print(adcValue);

        Serial.print(" | LED PWM: ");
        Serial.print(pwmValue);
        Serial.print("/255 | 7-Seg Level: ");

        Serial.println(lightLevel);
    }
}
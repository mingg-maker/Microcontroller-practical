#include <Arduino.h>

#define EN_PIN      2    
#define STEP_PIN    3    
#define DIR_PIN     4    

#define STEPS_PER_REV     200    
#define MICROSTEP_MODE    16      

#define STEP_DELAY_US     100    

void setup()
{
    pinMode(STEP_PIN, OUTPUT);
    pinMode(DIR_PIN, OUTPUT);
    pinMode(EN_PIN, OUTPUT);
    
    digitalWrite(STEP_PIN, LOW);
    digitalWrite(DIR_PIN, LOW);
    digitalWrite(EN_PIN, HIGH);  
    
    delay(500);
}

void enableMotor()
{
    digitalWrite(EN_PIN, LOW);  
}

void disableMotor()
{
    digitalWrite(EN_PIN, HIGH);  
}

void stepMotor()
{
    digitalWrite(STEP_PIN, HIGH);
    delayMicroseconds(1);
    digitalWrite(STEP_PIN, LOW);
    delayMicroseconds(STEP_DELAY_US);
}

void rotateForward(int numSteps)
{ 
    digitalWrite(DIR_PIN, HIGH);
    
    for (int i = 0; i < numSteps; i++)
    {
        stepMotor();
    }
}

void rotateBackward(int numSteps)
{
    digitalWrite(DIR_PIN, LOW);
    
    for (int i = 0; i < numSteps; i++)
    {
        stepMotor();
    }
}

void rotateDegrees(float degrees, boolean forward)
{
    int stepsNeeded = (int)((degrees / 360.0) * STEPS_PER_REV * MICROSTEP_MODE);
    
    if (forward)
    {
        rotateForward(stepsNeeded);
    }
    else
    {
        rotateBackward(stepsNeeded);
    }
}

void fullRevolution(boolean forward)
{
    int stepsPerRev = STEPS_PER_REV * MICROSTEP_MODE;
    
    if (forward)
    {
        rotateForward(stepsPerRev);
    }
    else
    {
        rotateBackward(stepsPerRev);
    }
}

void loop()
{
    enableMotor();
    fullRevolution(true);
    delay(1000);
    
    fullRevolution(false);
    delay(1000);
    
    rotateDegrees(90, true);
    delay(500);
    
    rotateDegrees(45, false);
    delay(500);
    
    disableMotor();
    delay(2000);
}
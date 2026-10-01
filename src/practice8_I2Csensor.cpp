#include <Arduino.h>
#include <avr/io.h>

#define AHT20_ADDR 0x38

void i2c_init(void)
{
    TWSR = 0x00;
    TWBR = 72;

    TWCR = (1 << TWEN);
}

void i2c_start(void)
{
    TWCR = (1 << TWINT) |
           (1 << TWSTA) |
           (1 << TWEN);

    while (!(TWCR & (1 << TWINT)));
}

void i2c_write(uint8_t data)
{
    TWDR = data;

    TWCR = (1 << TWINT) |
           (1 << TWEN);

    while (!(TWCR & (1 << TWINT)));
}

uint8_t i2c_read_ack(void)
{
    TWCR = (1 << TWINT) |
           (1 << TWEA)  |
           (1 << TWEN);

    while (!(TWCR & (1 << TWINT)));

    return TWDR;
}

uint8_t i2c_read_nack(void)
{
    TWCR = (1 << TWINT) |
           (1 << TWEN);

    while (!(TWCR & (1 << TWINT)));

    return TWDR;
}

void i2c_stop(void)
{
    TWCR = (1 << TWINT) |
           (1 << TWEN)   |
           (1 << TWSTO);

    delayMicroseconds(10);
}

void AHT20_init(void)
{
    i2c_start();

    i2c_write((AHT20_ADDR << 1) | 0);

    i2c_write(0xBE);
    i2c_write(0x08);
    i2c_write(0x00);

    i2c_stop();

    delay(10);
}

void AHT20_start_measurement(void)
{
    i2c_start();

    i2c_write((AHT20_ADDR << 1) | 0);

    i2c_write(0xAC);
    i2c_write(0x33);
    i2c_write(0x00);

    i2c_stop();
}

void AHT20_read_data(float *temperature, float *humidity)
{
    uint8_t data[7];

    delay(80);

    i2c_start();

    i2c_write((AHT20_ADDR << 1) | 1);

    data[0] = i2c_read_ack();
    data[1] = i2c_read_ack();
    data[2] = i2c_read_ack();
    data[3] = i2c_read_ack();
    data[4] = i2c_read_ack();
    data[5] = i2c_read_ack();

    data[6] = i2c_read_nack();

    i2c_stop();

    uint32_t raw_humidity;

    raw_humidity =
        ((uint32_t)data[1] << 12) |
        ((uint32_t)data[2] << 4)  |
        ((data[3] >> 4) & 0x0F);

    uint32_t raw_temperature;

    raw_temperature =
        ((uint32_t)(data[3] & 0x0F) << 16) |
        ((uint32_t)data[4] << 8) |
        data[5];

    *humidity =
        ((float)raw_humidity * 100.0) / 1048576.0;

    *temperature =
        ((float)raw_temperature * 200.0) / 1048576.0 - 50.0;
}

void setup()
{
    Serial.begin(9600);

    delay(1000);

    Serial.println();
    Serial.println("================================");
    Serial.println("Practice 8 - I2C + AHT20");
    Serial.println("ATmega328P");
    Serial.println("================================");

    i2c_init();

    Serial.println("I2C initialized");

    AHT20_init();

    Serial.println("AHT20 initialized");

    Serial.println();
}

void loop()
{
    float temperature;
    float humidity;

    AHT20_start_measurement();

    AHT20_read_data(&temperature, &humidity);

    Serial.print("Temperature: ");
    Serial.print(temperature, 2);
    Serial.println(" C");

    Serial.print("Humidity: ");
    Serial.print(humidity, 2);
    Serial.println(" %");

    Serial.println("----------------------------");

    delay(1000);
}
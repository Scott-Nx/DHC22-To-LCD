#include <Arduino.h>
#include <DHT.h>
#include <LiquidCrystal_I2C.h>

namespace Config {
    constexpr uint8_t DHT_PIN = 2;
    constexpr uint8_t DHT_TYPE = DHT22;
    constexpr unsigned long READ_INTERVAL_MS = 2000;
    constexpr unsigned long SERIAL_BAUD_RATE = 9600;
    constexpr uint8_t LCD_COLS = 16;
    constexpr uint8_t LCD_ROWS = 2;
}

DHT dht(Config::DHT_PIN, Config::DHT_TYPE);
LiquidCrystal_I2C lcd(PCF8574_ADDR_A21_A11_A01);

struct SensorReading {
    float humidity;
    float temperatureCelsius;
    float temperatureFahrenheit;

    bool isValid() const {
        return !isnan(humidity) && !isnan(temperatureCelsius) && !isnan(temperatureFahrenheit);
    }
};

SensorReading readSensor() {
    return {
        dht.readHumidity(),
        dht.readTemperature(),
        dht.readTemperature(true)
    };
}

void printReading(const SensorReading& reading) {
    Serial.print(F("Humidity: "));
    Serial.print(reading.humidity);
    Serial.print(F(" %\t"));
    Serial.print(F("Temperature: "));
    Serial.print(reading.temperatureCelsius);
    Serial.print(F(" C, "));
    Serial.print(reading.temperatureFahrenheit);
    Serial.println(F(" F"));
}

void displayOnLCD(const SensorReading& reading) {
    lcd.setCursor(0, 0);
    lcd.print("Temp: ");
    lcd.print(reading.temperatureCelsius, 1);
    lcd.print("C ");

    lcd.setCursor(0, 1);
    lcd.print("Humidity: ");
    lcd.print(reading.humidity, 1);
    lcd.print("%");
}

void setup() {
    Serial.begin(Config::SERIAL_BAUD_RATE);
    dht.begin();
    lcd.begin(Config::LCD_COLS, Config::LCD_ROWS);
    lcd.backlight();
}

void loop() {
    delay(Config::READ_INTERVAL_MS);

    SensorReading reading = readSensor();

    if (!reading.isValid()) {
        Serial.println(F("Failed to read from DHT sensor!"));
        lcd.setCursor(0, 0);
        lcd.print("Sensor Error!   ");
        lcd.setCursor(0, 1);
        lcd.print("                ");
        return;
    }

    printReading(reading);
    displayOnLCD(reading);
}

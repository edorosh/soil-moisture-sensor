#define SENSOR_PIN A0
#define BUTTON_PIN 13
#define DRY_VALUE 781
#define WATER_VALUE 384
#define DISPLAY_ADDR 0x27

#define STATE_BUFFER_SIZE 12

#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

enum HUMIDITY_STATE_ENUM {DRY, WET, VERY_WET, HUMIDITY_STATE_UNDEFINED};
typedef enum HUMIDITY_STATE_ENUM HUMIDITY_STATE;

// stateBuffer has 11 characters pluss termination char
char stateBuffer[STATE_BUFFER_SIZE];
int intervals = (DRY_VALUE - WATER_VALUE) / 3;

LiquidCrystal_I2C lcd(DISPLAY_ADDR, 16, 2);

void setup() {
  // Display
  lcd.init();
  lcd.backlight();
  lcd.clear();
}

void loop() {
  // read sensor raw value
  int val = analogRead(SENSOR_PIN);

  // calculate humidity percentage
  int humidityPercent = map(val, DRY_VALUE, WATER_VALUE, 0, 100);

  // get humidity state
  getHumidityString(
      getHumidityStateEnum(val),
      stateBuffer
    );

  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Soil ");
  lcd.print(stateBuffer);

  lcd.setCursor(0,1);
  lcd.print("Hum ");
  lcd.print(humidityPercent);
  lcd.print("% ");

  lcd.print("RAW ");
  lcd.print(val);
  
  delay(1500);
}

// getHumidityStateEnum returns state enum depending on RAW value coming back from a Soil Moisture Sensor
const HUMIDITY_STATE getHumidityStateEnum(int soilMoistureValue) {
   HUMIDITY_STATE state = HUMIDITY_STATE_UNDEFINED;
   
  if (soilMoistureValue > WATER_VALUE && soilMoistureValue < (WATER_VALUE + intervals)) {
    state = VERY_WET;
  }
  else if (soilMoistureValue > (WATER_VALUE + intervals) && soilMoistureValue < (DRY_VALUE - intervals)) {
    state = WET;
  }
  else if (soilMoistureValue < DRY_VALUE && soilMoistureValue > (DRY_VALUE - intervals)) {
    state = DRY;
  }

  return state;
}

// getHumidityString fills in a character buffer with a state
const void getHumidityString(HUMIDITY_STATE state, char* buffer) {
    size_t bufferSize = sizeof(stateBuffer);
    
    // clear buffer
    memset(buffer, 0, bufferSize);
    
    switch(state) {
      case WET:
        strncpy(buffer, "WET", bufferSize);
        break;
        
      case VERY_WET:
        strncpy(buffer, "VERY WET", bufferSize);
        break;

      case DRY:
        strncpy(buffer, "DRY", bufferSize);
        break;

      default:
        strncpy(buffer, "UNDEFINED", bufferSize);
    }
}

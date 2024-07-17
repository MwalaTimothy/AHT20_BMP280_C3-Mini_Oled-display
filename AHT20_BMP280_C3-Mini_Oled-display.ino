#include <BMP280_DEV.h>                           // Include the BMP280_DEV.h library
#include <Wire.h>
#include <AHT20.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Fonts/FreeSans9pt7b.h >

AHT20 aht20;
BMP280_DEV bmp280(8, 10);                         // Instantiate BMP280 object

float temperature, pressure, altitude;            // Create the temperature, pressure, and altitude variables
float ahtTemperature, humidity;                   // Create AHT20 temperature and humidity variables

#define SCREEN_WIDTH 128                          // OLED display width, in pixels
#define SCREEN_HEIGHT 64                          // OLED display height, in pixels
#define OLED_RESET -1                             // Reset pin (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

unsigned long previousMillis = 0;
const long interval = 5000;                       // Interval for updating the display
int page = 0;                                     // Current page index

void setup() 
{
  Serial.begin(115200);                           // Initialise the serial port
  Wire.begin();                                   // Join I2C bus

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {  // Initialise the OLED display with I2C address 0x3C
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }
  display.display();
  delay(2000);                                    // Pause for 2 seconds

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  bmp280.begin();                                 // Default initialisation, place the BMP280 into SLEEP_MODE 
  bmp280.setTimeStandby(TIME_STANDBY_2000MS);     // Set the standby time to 2 seconds
  bmp280.startNormalConversion();                 // Start BMP280 continuous conversion in NORMAL_MODE

  // Check if the AHT20 will acknowledge
  if (aht20.begin() == false) {
    Serial.println("AHT20 not detected. Please check wiring. Freezing.");
    while (1);
  }
  Serial.println("AHT20 acknowledged.");
}

void loop() 
{
  unsigned long currentMillis = millis();

  // Update sensor readings
  if (bmp280.getMeasurements(temperature, pressure, altitude)) {  // Check if the measurement is complete
    Serial.print(temperature);                    // Display the results    
    Serial.print(F("*C   "));
    Serial.print(pressure);    
    Serial.print(F("hPa   "));
    Serial.print(altitude);
    Serial.println(F("m")); 
  }

  if (aht20.available() == true) {
    ahtTemperature = aht20.getTemperature();
    humidity = aht20.getHumidity();
    Serial.print("Temperature: ");
    Serial.print(ahtTemperature, 2);
    Serial.print(" C\t");
    Serial.print("Humidity: ");
    Serial.print(humidity, 2);
    Serial.print("% RH");
    Serial.println();
  }

  // Update display every 'interval' milliseconds
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    display.clearDisplay();
    if (page == 0) {
      display.setCursor(0, 15);
      display.setFont(&FreeSans9pt7b);
      display.setTextColor(WHITE); 
      display.print("AHT20 Sensor:");
      display.setCursor(0, 35);
      display.print("Temp: ");
      display.print(ahtTemperature, 2);
      display.print(" C");
      display.setCursor(0, 60);
      display.print("Hum: ");
      display.print(humidity, 2);
      display.print(" %");
    } else {
      display.setCursor(0, 15);
      display.setFont(&FreeSans9pt7b);
      display.setTextColor(WHITE); 
      display.print("BMP280 :");
      display.setCursor(0, 35);
      display.print("Temp:");
      display.print(temperature, 2);
      display.print(" C");
      display.setCursor(0, 60);
      display.print("P:");
      display.print(pressure, 2);
      display.print("hPa");

    }
    display.display();

    // Switch pages
    page = (page + 1) % 2;
  }
}

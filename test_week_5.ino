#include <Wire.h>
#include <math.h> 
#include <Adafruit_BMP280.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define BLYNK_PRINT Serial

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

Adafruit_BMP280 bmp; // I2C Interface

int BH1750address = 0x23; //i2c address
 
byte buff[2];

// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
char auth[] = "Ar1PjwKigsksRGmKdsGZS049LMMtn22l";

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "stefwifi";
char pass[] = "stef1234";
void setup()
{
    delay(10);
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, pass);
  int wifi_ctr = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("WiFi connected");  

  Blynk.begin(auth, ssid, pass);
 

  Wire.begin();
  Serial.begin(57600);
   Serial.println(F("BMP280 test"));

  if (!bmp.begin()) {
    Serial.println(F("Could not find a valid BMP280 sensor, check wiring!"));
    while (1);
  }

  /* Default settings from datasheet. */
  bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,     /* Operating Mode. */
                  Adafruit_BMP280::SAMPLING_X2,     /* Temp. oversampling */
                  Adafruit_BMP280::SAMPLING_X16,    /* Pressure oversampling */
                  Adafruit_BMP280::FILTER_X16,      /* Filtering. */
                  Adafruit_BMP280::STANDBY_MS_500); /* Standby time. */
}
 
void loop()
{
  Blynk.run();
  
  int i;
  uint16_t val=0;
  BH1750_Init(BH1750address);
  delay(200);
 
  if(2==BH1750_Read(BH1750address))
  {
    val=((buff[0]<<8)|buff[1])/1.2;
    Serial.print(val,DEC);
    Serial.println(" lux");
  }
  delay(150);
    Serial.print(F("Temperature = "));
    Serial.print(bmp.readTemperature());
    Serial.println(" *C");
    Serial.println();
    if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
    delay(2000);
    
    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(WHITE);
    display.setCursor(0, 10);
    display.print(val,DEC);
    display.println(" lux");
    display.print(bmp.readTemperature());
    display.println(" *C");
    display.display(); 
    Blynk.virtualWrite(V1, bmp.readTemperature()); //send temperature value to virtual pin 1; add gauge to app & assign to virtual pin 1
    Blynk.virtualWrite(V2, val,DEC); //send lux value to virtual pin 2; add gauge to app & assign to virtual pin 2
}
 
int BH1750_Read(int address) //
{
  int i=0;
  Wire.beginTransmission(address);
  Wire.requestFrom(address, 2);
  while(Wire.available()) //
  {
    buff[i] = Wire.read();  // receive one byte
    i++;
  }
  Wire.endTransmission();
  return i;
}
 
void BH1750_Init(int address)
{
  Wire.beginTransmission(address);
  Wire.write(0x10);//1lx reolution 120ms
  Wire.endTransmission();
}

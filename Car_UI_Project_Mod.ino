/***************************************************
  This is a library for the Adafruit 1.8" SPI display.

This library works with the Adafruit 1.8" TFT Breakout w/SD card
  ----> http://www.adafruit.com/products/358
The 1.8" TFT shield
  ----> https://www.adafruit.com/product/802
The 1.44" TFT breakout
  ----> https://www.adafruit.com/product/2088
as well as Adafruit raw 1.8" TFT display
  ----> http://www.adafruit.com/products/618

  Check out the links above for our tutorials and wiring diagrams
  These displays use SPI to communicate, 4 or 5 pins are required to
  interface (RST is optional)
  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.
  MIT license, all text above must be included in any redistribution
 ****************************************************/

#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library
#include <SPI.h>
//#include <SD.h>

// For the breakout, you can use any 2 or 3 pins
// These pins will also work for the 1.8" TFT shield
#define TFT_CS     9
#define TFT_RST    7  // you can also connect this to the Arduino reset
                      // in which case, set this #define pin to 0!
#define TFT_DC     8

//#define SD_CS 5

// Option 1 (recommended): must use the hardware SPI pins
// (for UNO thats sclk = 13 and sid = 11) and pin 10 must be
// an output. This is much faster - also required if you want
// to use the microSD card (see the image drawing example)
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS,  TFT_DC, TFT_RST);

// Option 2: use any pins but a little slower!
#define TFT_SCLK 13   // set these to be whatever pins you like!
#define TFT_MOSI 11   // set these to be whatever pins you like!

float p = 3.1415926;

int rpm = 1;
int rpmCycle = 0;
int direct = 1;

inline void Adafruit_ST7735::spiwrite(uint8_t c) {
  SPDR = c;
  while(!(SPSR & _BV(SPIF)));
}

void setup(void) {
  Serial.begin(9600);
  tft.initR(INITR_BLACKTAB);  
  tft.fillScreen(ST7735_BLACK);
  dravUI();
}

void loop() {
  tftPrintRPM();
  tftPrintInTemp();
  tftPrintOutTemp();
  tftPrintEngineTemp();
  tftPrintVoltage();
  delay(100);
}

/* drav ui functions */

void dravUI(){
  tft.setTextWrap(false);
  tft.setTextSize(1);
  tft.setTextColor(ST7735_WHITE);
  dravBox(0, 5, 128, 35, ST7735_BLUE);
  dravLabel(2, 0, 1, "RPM", ST7735_RED, ST7735_WHITE);
  dravBox(0, 50, 68, 25, ST7735_BLUE);
  dravLabel(2, 42, 1, "Temp In", ST7735_RED, ST7735_WHITE);  
  dravThermometer(2, 55, ST7735_RED);
  dravBox(70, 50, 58, 25, ST7735_BLUE);
  dravLabel(72, 42, 1, "Temp Out", ST7735_RED, ST7735_WHITE);  
  dravThermometer(72, 55, ST7735_RED);
  dravBox(0, 84, 68, 35, ST7735_BLUE);
  dravLabel(2, 77, 1, "Engine", ST7735_RED, ST7735_WHITE);  
  dravBox(70, 84, 58, 35, ST7735_BLUE);
  dravLabel(72, 77, 1, "Voltage", ST7735_RED, ST7735_WHITE);    
  dravBox(0, 142, 128, 17, ST7735_BLUE);
}

void dravBox(int _left, int _top, int _width, int _height, uint16_t _BG){
  tft.fillRoundRect(_left, _top, _width, _height, 3, _BG);
  tft.drawRoundRect(_left, _top, _width, _height, 3, ST7735_WHITE);
}

void dravLabel(int _left, int _top, int _fsize, char *_text, uint16_t _BG, uint16_t _COLOR){
  tft.setTextWrap(false);
  tft.setTextSize(_fsize);
  tft.setTextColor(_COLOR);
  int _width = ((5*_fsize)*(strlen(_text)+1))+strlen(_text);
  int _height = 7*_fsize;
  tft.fillRoundRect(_left, _top, _width, _height+6, 3, _BG);
  tft.drawRoundRect(_left, _top, _width, _height+6, 3, ST7735_WHITE);
  tft.setCursor(_left+3, _top+3);
  tft.println(_text);
}

void dravValue(int _left, int _top, int _fsize, int _value, uint16_t _BG, uint16_t _COLOR){
  tft.setTextWrap(false);
  tft.setTextSize(_fsize);
  tft.setTextColor(_COLOR, _BG);
  tft.setCursor(_left, _top);
  tft.println(_value);
}

void dravThermometer(int _left, int _top, uint16_t _BG){
  tft.fillCircle(_left+5, _top, 2, _BG);
  tft.drawCircle(_left+5, _top, 2, ST7735_WHITE);
  tft.fillCircle(_left+5, _top+12, 5, _BG);
  tft.drawCircle(_left+5, _top+12, 5, ST7735_WHITE);
  tft.drawRect(_left+3, _top+1, 5, 8, ST7735_WHITE);
  tft.fillRect(_left+4, _top+1, 3, 8, _BG);
  tft.drawPixel(_left+6, _top+1, ST7735_WHITE);
  tft.drawPixel(_left+6, _top+3, ST7735_WHITE);
  tft.drawPixel(_left+6, _top+5, ST7735_WHITE);
  tft.drawPixel(_left+8, _top+13, ST7735_WHITE);
  tft.drawPixel(_left+7, _top+14, ST7735_WHITE);
  tft.drawPixel(_left+6, _top+15, ST7735_WHITE);
  tft.drawPixel(_left+5, _top+15, ST7735_WHITE);

}

void dravRpmGraph(int _value, int _bar, uint16_t _COLOR){
  int top = 120+20;
  int persent = round((_value*100)/6000);
  int heignt = round((20*persent)/100);
  if(_bar == 1){
    tft.fillRect(0, top-20, 128, 21, ST7735_BLACK);
    tft.fillRect(0, top, 128, 1, ST7735_BLUE);
    tft.fillRect(0, top-5, 128, 1, ST7735_BLUE);
    tft.fillRect(0, top-10, 128, 1, ST7735_BLUE);
    tft.fillRect(0, top-15, 128, 1, ST7735_BLUE);
  }
  tft.drawLine(_bar, top, _bar, top-heignt, _COLOR);
}


/* drav dynamics functions */

void tftPrintRPM() {
  if(rpmCycle <= 128){
    rpmCycle++;
  }
  else{
    rpmCycle = 1;
  }
  //rpm = random(800, 6000);
  uint16_t _color = ST7735_WHITE;
  if(rpm < 600)
    {_color = ST7735_WHITE;}
  else if(rpm >= 600 && rpm <= 2900)
    {_color = ST7735_GREEN;}
  else if(rpm >= 2901 && rpm <= 5200)
    {_color = ST7735_YELLOW;}    
  else
    {_color = ST7735_RED;}
  //tftPrintRPMBar(frame);
  dravValue(35, 12, 3, rpm, ST7735_BLUE, _color);
  dravRpmGraph(rpm, rpmCycle, _color);
  if(direct == 1){
    rpm = rpm + random(50, 200);
  }
  else{
    rpm = rpm - random(100, 500);
  }
  if(rpm >= 6000){
    rpm = 6000;
    direct = 0;
  }
  if(rpm <= 800){
    rpm = 801;
    direct = 1;
  }
}

void tftPrintInTemp(){
  dravValue(20, 57, 2, 27, ST7735_BLUE, ST7735_WHITE);
}

void tftPrintOutTemp(){
  dravValue(88, 57, 2, 18, ST7735_BLUE, ST7735_WHITE);
}

void tftPrintEngineTemp(){
  dravValue(8, 93, 3, 92, ST7735_BLUE, ST7735_WHITE);
}

void tftPrintVoltage(){
  dravValue(80, 93, 3, 14, ST7735_BLUE, ST7735_WHITE);
}

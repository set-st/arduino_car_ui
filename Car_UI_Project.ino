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
//Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST);


float p = 3.1415926;

int rpm = 1;
int shift_up = 0;

inline void Adafruit_ST7735::spiwrite(uint8_t c) {
  SPDR = c;
  while(!(SPSR & _BV(SPIF)));
}

void setup(void) {
  Serial.begin(9600);
  //Init the SD Card
  /*
  Serial.print("Initializing SD card...");
  if (!SD.begin(SD_CS)) {
    Serial.println("failed!");
    return;
  }
  Serial.println("OK!");
  */
  //Draw TRD Logo before turing on backlight
  //bmpDraw("trdlogo.bmp", 40, 60);
  // Use this initializer if you're using a 1.8" TFT
  tft.initR(INITR_BLACKTAB);   // initialize a ST7735S chip, black tab
  tft.fillScreen(ST7735_BLACK);
  initText();
  delay(1000);
  tft.fillScreen(ST7735_BLACK);
  dravUI();
  tftPrintStates();
}

void loop() {
  //tft.invertDisplay(true);
  //delay(500);
  //tft.invertDisplay(false);
  //delay(500);
  tftPrintRPM();
  tftPrintInnerTemp();
  tftPrintOuterTemp();
  tftPrintEngineTemp();
  tftPrintVoltage();
  if(shift_up == 1){
    //tftPrintShiftUP(0);
  }
  delay(10);
}

/*
void testlines(uint16_t color) {
  tft.fillScreen(ST7735_BLACK);
  for (int16_t x=0; x < tft.width(); x+=6) {
    tft.drawLine(0, 0, x, tft.height()-1, color);
  }
  for (int16_t y=0; y < tft.height(); y+=6) {
    tft.drawLine(0, 0, tft.width()-1, y, color);
  }

  tft.fillScreen(ST7735_BLACK);
  for (int16_t x=0; x < tft.width(); x+=6) {
    tft.drawLine(tft.width()-1, 0, x, tft.height()-1, color);
  }
  for (int16_t y=0; y < tft.height(); y+=6) {
    tft.drawLine(tft.width()-1, 0, 0, y, color);
  }

  tft.fillScreen(ST7735_BLACK);
  for (int16_t x=0; x < tft.width(); x+=6) {
    tft.drawLine(0, tft.height()-1, x, 0, color);
  }
  for (int16_t y=0; y < tft.height(); y+=6) {
    tft.drawLine(0, tft.height()-1, tft.width()-1, y, color);
  }

  tft.fillScreen(ST7735_BLACK);
  for (int16_t x=0; x < tft.width(); x+=6) {
    tft.drawLine(tft.width()-1, tft.height()-1, x, 0, color);
  }
  for (int16_t y=0; y < tft.height(); y+=6) {
    tft.drawLine(tft.width()-1, tft.height()-1, 0, y, color);
  }
}
*/


void testdrawtext(char *text, uint16_t color) {
  tft.setCursor(0, 0);
  tft.setTextColor(color);
  tft.setTextWrap(true);
  tft.print(text);
}

void initText(){
  tft.setTextColor(ST7735_WHITE);
  tft.setTextWrap(false);
  tft.setTextSize(1);
  tft.setCursor(0, 0);
  tft.println("Power ON..");
  delay(400);
  tft.setCursor(0, 10);
  tft.println("Checking sensors..");
  delay(900);
  tft.setCursor(10, 20);
  tft.println("find temp 01");
  delay(400);  
  tft.setCursor(10, 30);
  tft.println("find temp 02");
  delay(300);    
  tft.setCursor(10, 40);
  tft.println("find temp 03");
  delay(450);
  tft.setCursor(10, 50);
  tft.println("find voltage 01");
  delay(520);    
  tft.setCursor(10, 60);
  tft.println("find tachometer 01");
  delay(900);    
  tft.setCursor(0, 70);
  tft.println("Finishing boot..");
  delay(1200);    
}

void dravUI(){
  tft.setCursor(0, 0);    
  tft.setTextSize(1);
  tft.println("RPM");
  //left, top, width, height
  tft.fillRect(0, 10, 128, 25, ST7735_BLUE);
  tft.setCursor(0, 40);  
  tft.println("TEMP IN");
  //left, top, width, height
  tft.fillRect(0, 50, 63, 25, ST7735_BLUE);
  tft.setCursor(64, 40);  
  tft.println("TEMP OUT");
  //left, top, width, height
  tft.fillRect(64, 50, 128, 25, ST7735_BLUE);  
  tft.setCursor(0, 80);  
  tft.println("TEMP ENGINE");
  //left, top, width, height
  tft.fillRect(0, 90, 79, 25, ST7735_BLUE);  
  tft.setCursor(80, 80);  
  tft.println("VOLTAGE");
  //left, top, width, height
  tft.fillRect(80, 90, 48, 25, ST7735_BLUE);
}

void tftPrintRPM() {
  //
  //char *string="";
  int frame = random(800, 6000);
  //int frame = 1;
  //sprintf(string, "R%d", frame);
  //
  tft.setTextWrap(false);
  tft.setCursor(10, 11);
  if(frame < 600)
    {tft.setTextColor(ST7735_WHITE,ST7735_BLUE);}
  else if(frame >= 600 && frame <= 2900)
    {tft.setTextColor(ST7735_GREEN,ST7735_BLUE);}
  else if(frame >= 2901 && frame <= 5200)
    {tft.setTextColor(ST7735_YELLOW,ST7735_BLUE);}    
  else
    {tft.setTextColor(ST7735_RED,ST7735_BLUE);}
  tft.setTextSize(3);
  tftPrintRPMBar(frame);
  if(frame >= 2500){
    shift_up = 1;
  }
  else{
    shift_up = 0;
  }
  tft.println(frame);
}

void tftPrintShiftUP(int _clear){
  if(_clear == 0){
    tft.fillRect(103, 10, 25, 25, ST7735_RED);
    //tft.drawTriangle
    tft.fillTriangle(
        115, 14, // peak
        106, 30, // bottom left
        125, 30, // bottom right
        ST7735_WHITE);
  }
  else{
    tft.fillRect(103, 10, 25, 25, ST7735_BLUE);
  }
}

void tftPrintRPMBar(int value){
  int persent = round((value*100)/6000);
  int width = round((128*persent)/100);
  //
  tft.fillRect(0, 158, 128, 2, ST7735_BLUE);
  //
  if(value < 600)
    {  tft.fillRect(0, 158, width, 2, ST7735_WHITE);}
  else if(value >= 600 && value <= 2900)
    {  tft.fillRect(0, 158, width, 2, ST7735_GREEN);}
  else if(value >= 2901 && value <= 5200)
    {  tft.fillRect(0, 158, width, 2, ST7735_YELLOW);}    
  else
    {  tft.fillRect(0, 158, width, 2, ST7735_RED);}
}

void tftPrintInnerTemp() {
//  tft.drawLine(62, 50, 62, 75, ST7735_BLACK);
  int frame = random(0, 40);
  tft.setTextWrap(false);
  tft.setCursor(10, 56);
  if(frame < 32)
    {tft.setTextColor(ST7735_WHITE, ST7735_BLUE);}
  else
    {tft.setTextColor(ST7735_RED, ST7735_BLUE);}
  tft.setTextSize(2);
  tft.println(frame);
}

void tftPrintOuterTemp() {  
  int frame = random(0, 40);
  //int frame = analogRead(5);
  
  tft.setTextWrap(false);
  tft.setCursor(70, 56);
  if(frame < 32)
    {tft.setTextColor(ST7735_WHITE, ST7735_BLUE);}
  else
    {tft.setTextColor(ST7735_RED, ST7735_BLUE);}
  tft.setTextSize(2);
  tft.println(frame);
}

void tftPrintEngineTemp() {
  //
  int frame = random(0, 120);
  tft.setTextWrap(false);
  tft.setCursor(10, 96);
  if(frame < 65)
    {tft.setTextColor(ST7735_WHITE, ST7735_BLUE);}
  else if(frame >= 65 && frame <= 95)
    {tft.setTextColor(ST7735_GREEN, ST7735_BLUE);}
  else if(frame >= 96 && frame <= 110)
    {tft.setTextColor(ST7735_YELLOW, ST7735_BLUE);}    
  else
    {tft.setTextColor(ST7735_RED, ST7735_BLUE);}
  tft.setTextSize(2);
  tft.println(frame);
}

void tftPrintVoltage() {
  //
  int frame = random(10, 15);
  tft.setTextWrap(false);
  tft.setCursor(84, 96);
  tft.setTextColor(ST7735_WHITE, ST7735_BLUE);
  tft.setTextSize(2);
  tft.println(frame);
}

void tftPrintStates() {
  //left, top, width, height
  tft.fillRect(0, 116, 128, 44, ST7735_BLACK);
  //
  int frame = random(10, 15);
  tft.setTextWrap(false);
  tft.setTextSize(1);
  tft.setTextColor(ST7735_WHITE);
  // POWER
  tft.setCursor(4, 123);
  tft.setTextColor(ST7735_BLACK);
  tft.fillRect(0, 116, 64, 20, ST7735_YELLOW);
  //tft.fillRect(0, 133, 64, 3, ST7735_GREEN);
  tft.println("POWER");
  // FRONT FAN
  tft.setTextColor(ST7735_WHITE);
  tft.setCursor(69, 123);
  tft.fillRect(65, 116, 63, 20, ST7735_BLUE);  
  tft.println("FRONT FAN");
  // LIGHTS
  tft.fillRect(0, 137, 64, 20, ST7735_BLUE);
  tft.setCursor(5, 144);
  tft.println("LIGHTS");
  // HELLO
  tft.setCursor(69, 144);
  tft.fillRect(65, 137, 63, 20, ST7735_BLUE);  
  tft.println("HELLO ;)");
  
}


/*
void testfastlines(uint16_t color1, uint16_t color2) {
  tft.fillScreen(ST7735_BLACK);
  for (int16_t y=0; y < tft.height(); y+=5) {
    tft.drawFastHLine(0, y, tft.width(), color1);
  }
  for (int16_t x=0; x < tft.width(); x+=5) {
    tft.drawFastVLine(x, 0, tft.height(), color2);
  }
}
*/


/*
void testdrawrects(uint16_t color) {
  tft.fillScreen(ST7735_BLACK);
  for (int16_t x=0; x < tft.width(); x+=6) {
    tft.drawRect(tft.width()/2 -x/2, tft.height()/2 -x/2 , x, x, color);
  }
}
*/

/*
void testfillrects(uint16_t color1, uint16_t color2) {
  tft.fillScreen(ST7735_BLACK);
  for (int16_t x=tft.width()-1; x > 6; x-=6) {
    tft.fillRect(tft.width()/2 -x/2, tft.height()/2 -x/2 , x, x, color1);
    tft.drawRect(tft.width()/2 -x/2, tft.height()/2 -x/2 , x, x, color2);
  }
}
*/

/*
void testfillcircles(uint8_t radius, uint16_t color) {
  for (int16_t x=radius; x < tft.width(); x+=radius*2) {
    for (int16_t y=radius; y < tft.height(); y+=radius*2) {
      tft.fillCircle(x, y, radius, color);
    }
  }
}
*/

/*
void testdrawcircles(uint8_t radius, uint16_t color) {
  for (int16_t x=0; x < tft.width()+radius; x+=radius*2) {
    for (int16_t y=0; y < tft.height()+radius; y+=radius*2) {
      tft.drawCircle(x, y, radius, color);
    }
  }
}
*/

/*
void testtriangles() {
  tft.fillScreen(ST7735_BLACK);
  int color = 0xF800;
  int t;
  int w = tft.width()/2;
  int x = tft.height()-1;
  int y = 0;
  int z = tft.width();
  for(t = 0 ; t <= 15; t+=1) {
    tft.drawTriangle(w, y, y, x, z, x, color);
    x-=4;
    y+=4;
    z-=4;
    color+=100;
  }
}
*/

/*
void testroundrects() {
  tft.fillScreen(ST7735_BLACK);
  int color = 100;
  int i;
  int t;
  for(t = 0 ; t <= 4; t+=1) {
    int x = 0;
    int y = 0;
    int w = tft.width()-2;
    int h = tft.height()-2;
    for(i = 0 ; i <= 16; i+=1) {
      tft.drawRoundRect(x, y, w, h, 5, color);
      x+=2;
      y+=3;
      w-=4;
      h-=6;
      color+=1100;
    }
    color+=100;
  }
}
*/


/*
void tftPrintTest() {
  tft.setTextWrap(false);
  tft.fillScreen(ST7735_BLACK);
  tft.setCursor(0, 30);
  tft.setTextColor(ST7735_RED);
  tft.setTextSize(1);
  tft.println("Hello World!");
  tft.setTextColor(ST7735_YELLOW);
  tft.setTextSize(2);
  tft.println("Hello World!");
  tft.setTextColor(ST7735_GREEN);
  tft.setTextSize(3);
  tft.println("Hello World!");
  tft.setTextColor(ST7735_BLUE);
  tft.setTextSize(4);
  tft.print(1234.567);
  delay(1500);
  tft.setCursor(0, 0);
  tft.fillScreen(ST7735_BLACK);
  tft.setTextColor(ST7735_WHITE);
  tft.setTextSize(0);
  tft.println("Hello World!");
  tft.setTextSize(1);
  tft.setTextColor(ST7735_GREEN);
  tft.print(p, 6);
  tft.println(" Want pi?");
  tft.println(" ");
  tft.print(8675309, HEX); // print 8,675,309 out in HEX!
  tft.println(" Print HEX!");
  tft.println(" ");
  tft.setTextColor(ST7735_WHITE);
  tft.println("Sketch has been");
  tft.println("running for: ");
  tft.setTextColor(ST7735_MAGENTA);
  tft.print(millis() / 1000);
  tft.setTextColor(ST7735_WHITE);
  tft.print(" seconds.");
}
*/


/*
void mediabuttons() {
  // play
  tft.fillScreen(ST7735_BLACK);
  tft.fillRoundRect(25, 10, 78, 60, 8, ST7735_WHITE);
  tft.fillTriangle(42, 20, 42, 60, 90, 40, ST7735_RED);
  delay(500);
  // pause
  tft.fillRoundRect(25, 90, 78, 60, 8, ST7735_WHITE);
  tft.fillRoundRect(39, 98, 20, 45, 5, ST7735_GREEN);
  tft.fillRoundRect(69, 98, 20, 45, 5, ST7735_GREEN);
  delay(500);
  // play color
  tft.fillTriangle(42, 20, 42, 60, 90, 40, ST7735_BLUE);
  delay(50);
  // pause color
  tft.fillRoundRect(39, 98, 20, 45, 5, ST7735_RED);
  tft.fillRoundRect(69, 98, 20, 45, 5, ST7735_RED);
  // play color
  tft.fillTriangle(42, 20, 42, 60, 90, 40, ST7735_GREEN);
}
*/

//--------------------------------------------------------------------------------
//I didn't write anything below here. The code below is from the
//Adafruit library and it is used to load images from the SD card.
//--------------------------------------------------------------------------------
 
 
// This function opens a Windows Bitmap (BMP) file and
// displays it at the given coordinates.  It's sped up
// by reading many pixels worth of data at a time
// (rather than pixel by pixel).  Increasing the buffer
// size takes more of the Arduino's precious RAM but
// makes loading a little faster.  20 pixels seems a
// good balance.
/* 
#define BUFFPIXEL 50
 
void bmpDraw(char *filename, uint8_t x, uint8_t y) {
 
  File     bmpFile;
  int      bmpWidth, bmpHeight;   // W+H in pixels
  uint8_t  bmpDepth;              // Bit depth (currently must be 24)
  uint32_t bmpImageoffset;        // Start of image data in file
  uint32_t rowSize;               // Not always = bmpWidth; may have padding
  uint8_t  sdbuffer[3*BUFFPIXEL]; // pixel buffer (R+G+B per pixel)
  uint8_t  buffidx = sizeof(sdbuffer); // Current position in sdbuffer
  boolean  goodBmp = false;       // Set to true on valid header parse
  boolean  flip    = true;        // BMP is stored bottom-to-top
  int      w, h, row, col;
  uint8_t  r, g, b;
  uint32_t pos = 0, startTime = millis();
 
  if((x >= tft.width()) || (y >= tft.height())) return;
 
  Serial.println();
  Serial.print("Loading image '");
  Serial.print(filename);
  Serial.println('\'');
 
  // Open requested file on SD card
  if ((bmpFile = SD.open(filename)) == NULL) {
    Serial.print("File not found");
    return;
  }
 
  // Parse BMP header
  if(read16(bmpFile) == 0x4D42) { // BMP signature
    Serial.print("File size: "); Serial.println(read32(bmpFile));
    (void)read32(bmpFile); // Read & ignore creator bytes
    bmpImageoffset = read32(bmpFile); // Start of image data
    Serial.print("Image Offset: "); Serial.println(bmpImageoffset, DEC);
    // Read DIB header
    Serial.print("Header size: "); Serial.println(read32(bmpFile));
    bmpWidth  = read32(bmpFile);
    bmpHeight = read32(bmpFile);
    if(read16(bmpFile) == 1) { // # planes -- must be '1'
      bmpDepth = read16(bmpFile); // bits per pixel
      Serial.print("Bit Depth: "); Serial.println(bmpDepth);
      if((bmpDepth == 24) && (read32(bmpFile) == 0)) { // 0 = uncompressed
 
        goodBmp = true; // Supported BMP format -- proceed!
        Serial.print("Image size: ");
        Serial.print(bmpWidth);
        Serial.print('x');
        Serial.println(bmpHeight);
 
        // BMP rows are padded (if needed) to 4-byte boundary
        rowSize = (bmpWidth * 3 + 3) & ~3;
 
        // If bmpHeight is negative, image is in top-down order.
        // This is not canon but has been observed in the wild.
        if(bmpHeight < 0) {
          bmpHeight = -bmpHeight;
          flip      = false;
        }
 
        // Crop area to be loaded
        w = bmpWidth;
        h = bmpHeight;
        if((x+w-1) >= tft.width())  w = tft.width()  - x;
        if((y+h-1) >= tft.height()) h = tft.height() - y;
 
        // Set TFT address window to clipped image bounds
        tft.setAddrWindow(x, y, x+w-1, y+h-1);
 
        for (row=0; row<h; row++) { // For each scanline...
 
          // Seek to start of scan line.  It might seem labor-
          // intensive to be doing this on every line, but this
          // method covers a lot of gritty details like cropping
          // and scanline padding.  Also, the seek only takes
          // place if the file position actually needs to change
          // (avoids a lot of cluster math in SD library).
          if(flip) // Bitmap is stored bottom-to-top order (normal BMP)
            pos = bmpImageoffset + (bmpHeight - 1 - row) * rowSize;
          else     // Bitmap is stored top-to-bottom
            pos = bmpImageoffset + row * rowSize;
          if(bmpFile.position() != pos) { // Need seek?
            bmpFile.seek(pos);
            buffidx = sizeof(sdbuffer); // Force buffer reload
          }
 
          for (col=0; col<w; col++) { // For each pixel...
            // Time to read more pixel data?
            if (buffidx >= sizeof(sdbuffer)) { // Indeed
              bmpFile.read(sdbuffer, sizeof(sdbuffer));
              buffidx = 0; // Set index to beginning
            }
 
            // Convert pixel from BMP to TFT format, push to display
            b = sdbuffer[buffidx++];
            g = sdbuffer[buffidx++];
            r = sdbuffer[buffidx++];
            tft.pushColor(tft.Color565(r,g,b));
          } // end pixel
        } // end scanline
        Serial.print("Loaded in ");
        Serial.print(millis() - startTime);
        Serial.println(" ms");
      } // end goodBmp
    }
  }
 
  bmpFile.close();
  if(!goodBmp) Serial.println("BMP format not recognized.");
}
 
// These read 16- and 32-bit types from the SD card file.
// BMP data is stored little-endian, Arduino is little-endian too.
// May need to reverse subscript order if porting elsewhere.
 
uint16_t read16(File f) {
  uint16_t result;
  ((uint8_t *)&result)[0] = f.read(); // LSB
  ((uint8_t *)&result)[1] = f.read(); // MSB
  return result;
}
 
uint32_t read32(File f) {
  uint32_t result;
  ((uint8_t *)&result)[0] = f.read(); // LSB
  ((uint8_t *)&result)[1] = f.read();
  ((uint8_t *)&result)[2] = f.read();
  ((uint8_t *)&result)[3] = f.read(); // MSB
  return result;
}
*/

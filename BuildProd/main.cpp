#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include "StepEngine.h"
#include "Button.h"

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

#define OLED_RESET     -1
#define SCREEN_ADDRESS 0x3C 

#define MICROSTEP 1600

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
StepEngine eng1(5, 2);
Button btn_start(10);

void setup()
{
    Serial.begin(9600);
    Serial.setTimeout(10);

    // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
    /*if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
      Serial.println(F("SSD1306 allocation failed"));
      for(;;); // Don't proceed, loop forever
    }*/

    // Show initial display buffer contents on the screen --
    // the library initializes this with an Adafruit splash screen.
    //display.display();
    //delay(2000); // Pause for 2 seconds

    // Clear the buffer
    //display.clearDisplay();
}

void loop()
{   
    //display.clearDisplay();

    //display.setTextSize(2);             
    //display.setTextColor(SSD1306_WHITE);       
    //display.setCursor(0,0);            
    //display.println("Rounds " + String(b));

    //display.display();

    if (!btn_start.getSignal())
    {
        for (int i = 0; i < MICROSTEP; i++)
        {
            if (btn_start.getSignal())
            {
                break;
            }

            eng1.spin(150);
        }
    }
}
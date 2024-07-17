#include <SPI.h>
#include <Wire.h>
#include <GyverTimers.h>
#include <GyverOLED.h>

#include "StepEngine.h"
#include "Button.h"

#define MICROSTEP 1600

GyverOLED<SSD1306_128x32, OLED_BUFFER> oled;
StepEngine eng1(5, 2);
Button btn_start(3);

void setup()
{
    pinMode(11, OUTPUT); // GND для OLED
    digitalWrite(11, LOW);

    Serial.begin(9600);
    Serial.setTimeout(10);

    oled.init();
    Wire.setClock(800000L);
    oled.setScale(2);
    oled.clear();
    oled.update();

    Timer5.setFrequency(1);
    Timer5.enableISR(CHANNEL_A);
}

volatile bool show = 0;
uint counter = 0;
uint rounds = 0;


ISR(TIMER5_A)
{
    show = 1;
}

void loop()
{
    if (show)
    {
        oled.home();
        oled.print("R " + String(rounds));
        oled.update();
        oled.clear();
        show = 0;
    }

    if (!btn_start.getSignal())
    {
        for (int i = 0; i < MICROSTEP; i++)
        {
            if (btn_start.getSignal())
            {
                break;
            }

            eng1.spin(200);
            counter++;
        }

        rounds = counter / MICROSTEP;
    }
    else
    {
        rounds = 0;
        counter = 0;
    }
}
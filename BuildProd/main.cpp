#include <SPI.h>
#include <Wire.h>
#include <GyverTimers.h>
#include <GyverOLED.h>

#include "StepEngine.h"
#include "Button.h"

#define MICROSTEP 1600

GyverOLED<SSD1306_128x32, OLED_BUFFER> oled(0x3C);
StepEngine eng1(5, 2, S_3, 0.49f);
Button btn_start(3);

volatile bool show    = 0; // Флаг отрисовки данных на OLED
unsigned long counter = 0; // Счётчик оборотов
uint          rounds  = 0; // Кол-во оборотов сначала старта
uint          param = 0;

String menu[] = 
{ 
    "Menu 1", 
    "Menu 2", 
    "Menu 3" 
};

void print_log()
{
    Serial.println(eng1.get_log());
}

// Функция инициализации
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

#ifdef DBG
    print_log();
#endif

    while (true)
    {
        btn_start.tick();

        if (btn_start.isClick())
        {
            if (param + 1 < 3)
            {
                param++;
            }
            else
            {
                param = 0;
            }
        }

        oled.home();
        oled.println(menu[param]);
        oled.update();
        oled.clear();
    }
}

ISR(TIMER5_A)
{
    show = 1;
}

void loop()
{
    if (show)
    {
        oled.home();
        oled.print(String(rounds * eng1.get_k()) + " ml");
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

            eng1.spin();
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
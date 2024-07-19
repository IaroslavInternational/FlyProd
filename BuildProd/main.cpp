#include <SPI.h>
#include <Wire.h>
#include <GyverTimers.h>
#include <GyverOLED.h>

#include "StepEngine.h"
#include "Button.h"

#define MICROSTEP 1600

enum mode
{
    Single_Engine = 1,
    Double_Engine,
    Tripple_Engine,
    Four_Engine,
    Five_Engine,
    Six_Engine,
    Count
};


mode work_mode = Single_Engine; // Режим работы

GyverOLED<SSD1306_128x32, OLED_BUFFER> oled(0x3C);
StepEngine eng1(5, 2, S_3, 0.66f);
Button btn_start(3);

volatile bool show    = 0; // Флаг отрисовки данных на OLED
unsigned long counter = 0; // Счётчик оборотов
uint          rounds  = 0; // Кол-во оборотов сначала старта
uint          param = 0;

String menu[] = 
{ 
    "Заливка", 
    "Настройки" 
};

// Вывод лога
void print_log()
{
    Serial.println(eng1.get_log());
}

// Выбор двигателя
void chose_engine(mode m)
{

}

// Функция калибровки двигателя
void settings(StepEngine* engine)
{
    bool isFilled     = 0;
    bool isCalibrated = 0;
    uint clicks       = 0;

    oled.autoPrintln(true);

    for (int i = 0; i < 5; i++)
    {
        engine->spin();
    }

    while (true)
    {
        btn_start.tick();

        if (!isFilled)
        {
            oled.home();
            oled.println("Наполните трубку");
            oled.update();
            oled.clear();

            if (btn_start.isClick())
            {
                engine->set_speed(S_2);

                for (int i = 0; i < MICROSTEP * 5; i++)
                {
                    engine->spin();
                }

                engine->set_speed(S_3);

                isFilled = 1;
            }
        }
        else
        {
            oled.home();
            oled.println("Наберите");
            oled.println("50ml");
            oled.update();
            oled.clear();

            if (btn_start.isHolded())
            {
                clicks++;
            }

            if (clicks != 2 && clicks > 0)
            {
                for (int i = 0; i < MICROSTEP; i++)
                {
                    engine->spin();
                    counter++;
                }
            }

            if (clicks == 2)
            {
                rounds = counter / MICROSTEP;

                oled.home();
                oled.println("50ml - " + String(rounds) + "r");
                oled.update();
                oled.clear();

                delay(1500);

                isCalibrated = 1;
            }
        }

        if (isFilled && isCalibrated)
        {
            oled.home();
            oled.println("Настройка завершена");
            oled.update();
            oled.clear();

            delay(1500);

            oled.home();
            oled.println("k = " + String(50.0f/rounds));
            oled.update();
            oled.clear();

            delay(5000);

            oled.autoPrintln(false);

            return;
        }

    }
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
            if (param + 1 < (sizeof(menu) / sizeof(*menu)))
            {
                param++;
            }
            else
            {
                param = 0;
            }
        }
        else if (btn_start.isHolded())
        {
            switch (param)
            {
            case 0:
                for (;;)
                {
                    run();
                }
                break;
            case 1:
                chose_engine(work_mode);
                settings(&eng1);
                break;
            default:
                break;
            }

            param = 0;
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

void run()
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

void loop()
{

}
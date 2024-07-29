/*
* Исходный файл для калибровки насосов
* Выбор количества двигателей: ENGINES (либо 2, либо 4)
*/


#include <SPI.h>
#include <Wire.h>
#include <GyverTimers.h>
#include <GyverOLED.h>

#include "StepEngine.h"
#include "Button.h"

#define MICROSTEP 1600  // Микрошаг
#define ENGINES   2     // Кол-во двигателей
#define BTN_1_PIN 22    // Пин для кнопки 1

#if ENGINES == 2
    #define TWO_ENGINE 1
#elif ENGINES == 4
    #define FOUR_ENGINE 1
#endif

#ifdef TWO_ENGINE
StepEngine eng1(2, 3, S_3);  // PUL+, DIR+, Скорость
StepEngine eng2(4, 5, S_3);  // PUL+, DIR+, Скорость
#endif // TWO_ENGINE

#ifdef FOUR_ENGINE
StepEngine eng1(2, 3, S_3);  // PUL+, DIR+, Скорость
StepEngine eng2(4, 5, S_3);  // PUL+, DIR+, Скорость
StepEngine eng3(6, 7, S_3);  // PUL+, DIR+, Скорость
StepEngine eng4(8, 9, S_3);  // PUL+, DIR+, Скорость
#endif // FOUR_ENGINE


GyverOLED<SSD1306_128x32, OLED_BUFFER> oled(0x3C);
Button btn_start(BTN_1_PIN);

volatile bool show = 0;    // Флаг отрисовки данных на OLED
unsigned long counter = 0; // Счётчик оборотов
uint          rounds = 0;  // Кол-во оборотов сначала старта
uint          param = 0;

String menu[] =
{
    "Инфо",
    "Настройка"
};

// Вывод лога
void print_log()
{
    Serial.println(eng1.get_log());
}


// Вывод информации о пинах двигателй
void ShowInfo()
{
    oled.setScale(1);
    oled.home();
#ifdef TWO_ENGINE
    oled.println("1) pins(2, 3)");
    oled.println("2) pins(4, 5)");
#endif // TWO_ENGINE
#ifdef FOUR_ENGINE
    oled.println("1) pins(2, 3)");
    oled.println("2) pins(4, 5)");
    oled.println("3) pins(6, 7)");
    oled.println("4) pins(8, 9)");
#endif // FOUR_ENGINE
    oled.update();
    oled.clear();

    delay(5000);
    oled.setScale(2);
}

StepEngine* EngineChoose()
{
#ifdef TWO_ENGINE
#endif // TWO_ENGINE
}

// Функция калибровки двигателя
void settings(StepEngine* engine)
{
    bool isFilled = 0;
    bool isCalibrated = 0;
    uint clicks = 0;

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
            oled.println("k = " + String(50.0f / rounds));
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
    pinMode(11, OUTPUT);   // GND для OLED
    digitalWrite(11, LOW); // GND для OLED

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

        if (btn_start.isClick())  // Переключение меню
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
        else if (btn_start.isHolded())  // Выбор меню
        {
            switch (param)
            {
            case 0: // Info
                ShowInfo();
                break;
            case 1:             
                settings(EngineChoose());
                break;
            default:
                break;
            }

            param = 0;
        }

        oled.home();
        for (uint i = 0; i < (sizeof(menu) / sizeof(*menu)); i++)
        {
            if (i == param)
            {
                oled.print("- ");
            }

            oled.println(menu[i]);
        }
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
/*
* Исходный файл для калибровки насосов
* Выбор количества насосов: ENGINES (либо 2, либо 4)
* Выбор микрошага насоса: MICROSTEP (в зависимости от настроек драйвера)
* Выбор пина сигнала для кнопки 1: BTN_1_PIN (цифровой вход, не ШИМ)
* Выбор перекачиваемого объёма для калибровки: CONFIG_VOLUME (в мл)
*/


#include <SPI.h>
#include <Wire.h>
#include <GyverTimers.h>
#include <GyverOLED.h>

#include "StepEngine.h"
#include "Button.h"

/********************* КОНФИГУРАЦИЯ *********************/

#define MICROSTEP     1600  // Микрошаг
#define ENGINES       2     // Кол-во насосов
#define BTN_1_PIN     22    // Пин для кнопки 1

#define ENGINE_1 100.0f   // Коэ-нт объёма насоса 1
#define ENGINE_2 80.0f    // Коэ-нт объёма насоса 2 относительно насоса 1
#define ENGINE_3 0.0f     // Коэ-нт объёма насоса 3 относительно насоса 1
#define ENGINE_4 0.0f     // Коэ-нт объёма насоса 4 относительно насоса 1

/********************************************************/

#if ENGINES == 2
#define TWO_ENGINE 1
#elif ENGINES == 4
#define THREE_ENGINE 1
#endif

#ifdef TWO_ENGINE
StepEngine eng1(2, 3, S_3, 0.6f);  // PUL+, DIR+, Скорость, k
StepEngine eng2(4, 5, S_3, 0.6f);  // PUL+, DIR+, Скорость, k
#endif // TWO_ENGINE

#ifdef THREE_ENGINE
StepEngine eng1(2, 3, S_3, 0.6f);  // PUL+, DIR+, Скорость, k
StepEngine eng2(4, 5, S_3, 0.6f);  // PUL+, DIR+, Скорость, k
StepEngine eng3(6, 7, S_3, 0.6f);  // PUL+, DIR+, Скорость, k
StepEngine eng4(8, 9, S_3, 0.6f);  // PUL+, DIR+, Скорость, k
#endif // THREE_ENGINE


GyverOLED<SSD1306_128x32, OLED_BUFFER> oled(0x3C);
Button btn_start(BTN_1_PIN);

volatile bool show    = 0; // Флаг отрисовки данных на OLED
volatile bool spin    = 0; // Флаг
unsigned long counter = 0; // Счётчик оборотов
uint          rounds  = 0; // Кол-во оборотов сначала старта
uint          param   = 0; // Выбранный (индекс) пункт меню

String menu[] =
{
    "Заливка",
    "Информация"
};

// Вывод лога
void print_log()
{
    Serial.println("Settings:");
    Serial.println("Micro-step:" + String(MICROSTEP));
    Serial.println("Engines:" + String(ENGINES));
    Serial.println("Pin BTN 1:" + String(BTN_1_PIN) + "\n");

#ifdef TWO_ENGINE
    Serial.println(eng1.get_log());
    Serial.println(eng2.get_log());
#endif // TWO_ENGINE

#ifdef THREE_ENGINE
    Serial.println(eng1.get_log());
    Serial.println(eng2.get_log());
    Serial.println(eng3.get_log());
    Serial.println(eng4.get_log());
#endif // THREE_ENGINE
}

// Вывод информации о пинах двигателй
void ShowInfo()
{
    oled.home();
#ifdef TWO_ENGINE
    oled.println("1) k=" + String(eng1.get_k()) + "; s=" + String(ENGINE_1) + "%");
    oled.println("2) k=" + String(eng2.get_k()) + "; s=" + String(ENGINE_2) + "%");
#endif // TWO_ENGINE

#ifdef THREE_ENGINE
    oled.println("1) k=" + String(eng1.get_k()) + "; s=" + String(ENGINE_1) + "%");
    oled.println("2) k=" + String(eng2.get_k()) + "; s=" + String(ENGINE_2) + "%");
    oled.println("3) k=" + String(eng3.get_k()) + "; s=" + String(ENGINE_3) + "%");
    oled.println("4) k=" + String(eng4.get_k()) + "; s=" + String(ENGINE_4) + "%");
#endif // THREE_ENGINE
    oled.update();
    oled.clear();

    delay(5000);
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
    oled.setScale(1);
    oled.clear();
    oled.update();

    Timer5.setFrequency(1);
    Timer5.enableISR(CHANNEL_A);

    Timer1.setFrequency(1000);
    Timer1.enableISR(CHANNEL_C);

    eng1.speed_reconfig(ENGINE_1);
    eng2.speed_reconfig(ENGINE_2);

#ifdef DBG
    print_log();
#endif

    bool away = false;

    while (!away)
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
                away = true;
                break;
            case 1:
                ShowInfo();
                break;
            default:
                break;
            }

            param = 0;
        }

        oled.home();
        oled.println("Главное меню");
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

    for (;;) { run(); } // После меню
}

ISR(TIMER5_A)
{
    show = 1;

    /*if (spin)
    {
        for (int i = 0; i < MICROSTEP; i++)
        {
            eng1.spin();
        }
    }*/
}

ISR(TIMER1_C)
{
    if (spin)
    {
        for (int i = 0; i < MICROSTEP; i++)
        {
            if (btn_start.getSignal())
            {
                spin = 0;
                break;
            }

            eng1.spin();
        }
    }
}

short step = 0;
void run()
{
    btn_start.tick();

    if (show)
    {
        oled.home();
        oled.print("Компонента 1:");
        oled.println(String(rounds * eng1.get_k()) + " ml");
        oled.print("Компонента 2:");
        oled.println(String(rounds * eng2.get_k()) + " ml");
        oled.update();
        oled.clear();
        show = 0;
    }

    if (!btn_start.getSignal())
    {
        spin = 1;

        for (step = 0; step < MICROSTEP; step++)
        {
            if (btn_start.getSignal())
            {
                spin = 0;
                break;
            }

            //eng1.spin();
            eng2.spin();
            counter++;
        }

        rounds = counter / MICROSTEP;
    }
    else
    {
        //rounds = 0;
        //counter = 0;
    }
}

void loop(){}
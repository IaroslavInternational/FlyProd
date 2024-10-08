/*
* �������� ���� ��� ���������� �������
* ����� ���������� �������: ENGINES (���� 2, ���� 4)
* ����� ��������� ������: MICROSTEP (� ����������� �� �������� ��������)
* ����� ���� ������� ��� ������ 1: BTN_1_PIN (�������� ����, �� ���)
* ����� ��������������� ������ ��� ����������: CONFIG_VOLUME (� ��)
*/


#include <SPI.h>
#include <Wire.h>
#include <GyverTimers.h>
#include <GyverOLED.h>

#include "StepEngine.h"
#include "Button.h"

/********************* ������������ *********************/

#define MICROSTEP     1600  // ��������
#define ENGINES       2     // ���-�� �������
#define BTN_1_PIN     22    // ��� ��� ������ 1

#define ENGINE_1 100.0f   // ���-�� ������ ������ 1
#define ENGINE_2 80.0f    // ���-�� ������ ������ 2 ������������ ������ 1
#define ENGINE_3 0.0f     // ���-�� ������ ������ 3 ������������ ������ 1
#define ENGINE_4 0.0f     // ���-�� ������ ������ 4 ������������ ������ 1

/********************************************************/

#if ENGINES == 2
#define TWO_ENGINE 1
#elif ENGINES == 4
#define THREE_ENGINE 1
#endif

#ifdef TWO_ENGINE
StepEngine eng1(2, 3, S_3, 0.6f);  // PUL+, DIR+, ��������, k
StepEngine eng2(4, 5, S_3, 0.6f);  // PUL+, DIR+, ��������, k
#endif // TWO_ENGINE

#ifdef THREE_ENGINE
StepEngine eng1(2, 3, S_3, 0.6f);  // PUL+, DIR+, ��������, k
StepEngine eng2(4, 5, S_3, 0.6f);  // PUL+, DIR+, ��������, k
StepEngine eng3(6, 7, S_3, 0.6f);  // PUL+, DIR+, ��������, k
StepEngine eng4(8, 9, S_3, 0.6f);  // PUL+, DIR+, ��������, k
#endif // THREE_ENGINE


GyverOLED<SSD1306_128x32, OLED_BUFFER> oled(0x3C);
Button btn_start(BTN_1_PIN);

volatile bool show    = 0; // ���� ��������� ������ �� OLED
volatile bool spin    = 0; // ����
unsigned long counter = 0; // ������� ��������
uint          rounds  = 0; // ���-�� �������� ������� ������
uint          param   = 0; // ��������� (������) ����� ����

String menu[] =
{
    "�������",
    "����������"
};

// ����� ����
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

// ����� ���������� � ����� ���������
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

// ������� �������������
void setup()
{
    pinMode(11, OUTPUT);   // GND ��� OLED
    digitalWrite(11, LOW); // GND ��� OLED

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
        oled.println("������� ����");
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

    for (;;) { run(); } // ����� ����
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
        oled.print("���������� 1:");
        oled.println(String(rounds * eng1.get_k()) + " ml");
        oled.print("���������� 2:");
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
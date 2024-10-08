/*
* �������� ���� ��� ���������� �������
* ����� ���������� �������: ENGINES (���� 2, ���� 3)
* ����� ��������� ������: MICROSTEP (� ����������� �� �������� ��������)
* ����� ���� ������� ��� ������ 1: BTN_1_PIN (�������� ����, �� ���)
* ����� ��������������� ������ ��� ����������: CONFIG_VOLUME (� ��)
* ����� �������� �������: SPEED (��� ��� � ���)
* �������� ���� 8-11, ��� GND: ADD_GND (1 - �������� ����, ��� GND; 0 - �� ��������)
*/


#include <SPI.h>
#include <Wire.h>
#include <GyverTimers.h>
#include <GyverOLED.h>

#include "StepEngine.h"
#include "Button.h"


/********************* ������������ *********************/

#define MICROSTEP     1600  // ��������
#define ENGINES       3     // ���-�� �������
#define BTN_1_PIN     22    // ��� ��� ������ 1

#define SPEED    50 / (ENGINES-1)
#define ADD_GND  1

#define K1 0.6f
#define K2 0.6f
#define K3 0.6f

/********************************************************/

#if ENGINES == 2
#define TWO_ENGINE 1
#elif ENGINES == 3
#define THREE_ENGINE 1
#endif

#ifdef TWO_ENGINE
StepEngine eng1(2, 3, SPEED, K1);  // PUL+, DIR+, ��������, k
StepEngine eng2(4, 5, SPEED, K2);  // PUL+, DIR+, ��������, k
#endif // TWO_ENGINE

#ifdef THREE_ENGINE
StepEngine eng1(2, 3, SPEED, K1);  // PUL+, DIR+, ��������, k
StepEngine eng2(4, 5, SPEED, K2);  // PUL+, DIR+, ��������, k
StepEngine eng3(6, 7, 50, K3);  // PUL+, DIR+, ��������, k
#endif // THREE_ENGINE


GyverOLED<SSD1306_128x32, OLED_BUFFER> oled(0x3C);
Button btn_start(BTN_1_PIN);

volatile bool show    = 0; // ���� ��������� ������ �� OLED
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
    oled.println("1) k=" + String(eng1.get_k()) + ";");
    oled.println("2) k=" + String(eng2.get_k()) + ";");
    oled.println("3) k=" + String(eng3.get_k()) + ";");
#endif // THREE_ENGINE
    oled.update();
    oled.clear();

    delay(5000);
}

// ������� �������������
void setup()
{
    Timer5.setFrequency(1);
    Timer5.enableISR(CHANNEL_A);

    #if ADD_GND == 1
    // ���� �� ������� ����� ��� �����
    {
        int gnd_pins[] = { 8, 9, 10, 11 }; // ���� ��� GND

        for (int i = 0; i < (sizeof(gnd_pins) / sizeof(*gnd_pins)); i++)
        {
            pinMode(gnd_pins[i], OUTPUT);
            digitalWrite(gnd_pins[i], LOW);
        }
    }
    #endif

    Serial.begin(9600);
    Serial.setTimeout(10);

    oled.init();
    Wire.setClock(800000L);
    oled.setScale(1);
    oled.clear();
    oled.update();

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
}

ISR(TIMER5_A)
{
    show = 1;
}

short r = 0;
void loop()
{
    if(show)
    {
        oled.home();
        oled.print("���������� A:");
        oled.println(String(rounds * eng1.get_k()) + " ml");
        oled.print("���������� B:");
        oled.println(String(rounds * eng2.get_k()) + " ml");
        oled.update();
        oled.clear();
        show = 0;
    }

    btn_start.tick();

    if (!btn_start.getSignal())
    {
        for (r = 0; r < MICROSTEP; r++)
        {
            eng1.spin();
            eng2.spin();

            counter++;
        }

        rounds = counter / MICROSTEP;
    }
}
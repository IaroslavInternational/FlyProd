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
#define CONFIG_VOLUME 50.0f // ����� � �� ��� ���������
#define SPEED         Speed::MAX
#define ADD_GND       1

/********************************************************/

#if ENGINES == 2
    #define TWO_ENGINE 1
#elif ENGINES == 3
    #define THREE_ENGINE 1
#endif

#ifdef TWO_ENGINE
StepEngine eng1(2, 3, SPEED);  // PUL+, DIR+, ��������
StepEngine eng2(4, 5, SPEED);  // PUL+, DIR+, ��������

bool configurated_eng[] = { 0, 0 };
#endif // TWO_ENGINE

#ifdef THREE_ENGINE
StepEngine eng1(2, 3, SPEED);  // PUL+, DIR+, ��������
StepEngine eng2(4, 5, SPEED);  // PUL+, DIR+, ��������
StepEngine eng3(6, 7, SPEED);  // PUL+, DIR+, ��������

bool configurated_eng[] = { 0, 0, 0 };
#endif // THREE_ENGINE


GyverOLED<SSD1306_128x32, OLED_BUFFER> oled(0x3C);
Button btn_start(BTN_1_PIN);

unsigned long counter = 0;  // ������� ��������
uint          rounds  = 0;  // ���-�� �������� ������� ������
uint          param   = 0;  // ��������� (������) ����� ����

String menu[] =
{
    "����������",
    "���������"
};

String eng_list[] =
{
    #ifdef TWO_ENGINE
    "����� 1",
    "����� 2",
    #endif // TWO_ENGINE

    #ifdef THREE_ENGINE
    "����� 1",
    "����� 2",
    "����� 3"
    #endif // TWO_ENGINE
};

// ����� ����
void print_log()
{
    Serial.println("Settings:");
    Serial.println("Micro-step:" + String(MICROSTEP));
    Serial.println("Engines:" + String(ENGINES));
    Serial.println("Pin BTN 1:" + String(BTN_1_PIN));
    Serial.println("Volume setup:" + String(CONFIG_VOLUME) + "\n");

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
    oled.println("1) pins(2, 3), k=" + String(eng1.get_k()));
    oled.println("2) pins(4, 5), k=" + String(eng2.get_k()));
#endif // TWO_ENGINE

#ifdef THREE_ENGINE
    oled.println("1) pins(2, 3), k=" + String(eng1.get_k()));
    oled.println("2) pins(4, 5), k=" + String(eng2.get_k()));
    oled.println("3) pins(6, 7), k=" + String(eng3.get_k()));
#endif // THREE_ENGINE
    oled.update();
    oled.clear();

    delay(5000);
}

// ���� ������ ��������� ��� ���������
StepEngine* EngineChoose()
{
    oled.home();
    param = 0;

    while (true)
    {
        btn_start.tick();

        if (btn_start.isClick())  // ������������ ����
        {
            if (param + 1 < (sizeof(eng_list) / sizeof(*eng_list)))
            {
                param++;
            }
            else
            {
                param = 0;
            }
        }
        else if (btn_start.isHolded())  // ����� ����
        {
            switch (param)
            {
            case 0: 
                configurated_eng[0] = 1;
                return &eng1;
                break;
            case 1:
                configurated_eng[1] = 1;
                return &eng2;
                break;
            #ifdef THREE_ENGINE
            case 2:
                configurated_eng[2] = 1;
                return &eng3;
                break;
            #endif // THREE_ENGINE
            default:
                continue;
                break;
            }

            param = 0;
        }

        oled.home();
        for (uint i = 0; i < (sizeof(eng_list) / sizeof(*eng_list)); i++)
        {
            if (i == param)
            {
                oled.print("- ");
            }
            
            if (configurated_eng[i])
            {
                oled.println(eng_list[i] + " +");
            }
            else
            {
                oled.println(eng_list[i]);
            }
        }
        oled.update();
        oled.clear();
    }
}

// ������� ���������� ���������
void EngineSetup(StepEngine* engine)
{
    bool isFilled = 0;
    bool isCalibrated = 0;
    uint clicks = 0;
    counter = 0;

    oled.autoPrintln(true);

    for (short i = 0; i < short(MICROSTEP*0.1); i++)
    {
        engine->spin();
    }

    while (true)
    {
        btn_start.tick();

        if (!isFilled)
        {
            oled.home();
            oled.println("���������� �.1:");
            oled.println("��������� ������:");
            oled.println("-> ������� ������");
            oled.update();
            oled.clear();

            if (btn_start.isHolded())
            {
                engine->set_speed(S_2);

                for (int i = 0; i < MICROSTEP * 5; i++)
                {
                    engine->spin();
                }

                engine->set_speed(SPEED);

                isFilled = 1;
            }
        }
        else
        {
            oled.home();
            oled.println("���������� �.2:");
            oled.println("�������� " + String(CONFIG_VOLUME) + "ml");
            oled.println("-> ������� ������");
            oled.update();
            oled.clear();

            if (btn_start.isHolded())
            {
                clicks++;
            }

            if (clicks != 2 && clicks > 0)
            {
                for (short i = 0; i < MICROSTEP; i++)
                {
                    engine->spin();
                    counter++;
                }
            }

            if (clicks == 2)
            {
                rounds = counter / MICROSTEP;
                isCalibrated = 1;
            }
        }

        if (isFilled && isCalibrated)
        {
            float k = CONFIG_VOLUME / float(rounds);

            oled.home();
            oled.println("���������� ���������:");
            oled.println(String(CONFIG_VOLUME) + "ml = " + String(rounds) + " ��������");
            oled.println("k = " + String(k));
            oled.update();
            oled.clear();

            engine->set_k(k);

            delay(6000);

            oled.autoPrintln(false);

            return;
        }
    }
}

// ������� �������������
void setup()
{
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

    while (true)
    {
        btn_start.tick();

        if (btn_start.isClick())  // ������������ ����
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
        else if (btn_start.isHolded())  // ����� ����
        {
            switch (param)
            {
            case 0: // Info
                ShowInfo();
                break;
            case 1:             
                EngineSetup(EngineChoose());
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

void loop(){}
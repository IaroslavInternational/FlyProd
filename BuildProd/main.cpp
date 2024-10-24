/*
* ========== �������� ���� ��������� ������ ==========
* ����� ���������� �������: ENGINES (���� 2, ���� 3)
* ����� ��������� ������: MICROSTEP (� ����������� �� �������� ��������)
* 
* ����� ���� ������� ��� ������ 1 (������): BTN_1_PIN (�������� ����, �� ���)
* ����� ���� ������� ��� ������ 2 (�������): BTN_2_PIN (�������� ����, �� ���)
* ����� ���� ������� ��� ������ 3 (�������): BTN_3_PIN (�������� ����, �� ���)
* 
* ����� �������� �������: SPEED (��� ��� � ���)
* 
* �������� ���� [34, 36, 38, 40, 42], ��� GND: ADD_GND (1 - �������� ����, ��� GND; 0 - �� ��������)
* �������� ���� [26], ��� +5V: ADD_5V (1 - �������� ����, ��� GND; 0 - �� ��������)
* 
* ����-�� K1 � K2 ��� �������
* ����-�� K_REV: ����� ��������� �������������� ������ ������ 2 �� �� ������ 1 (�������� [0; 1])
* ====================================================
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
#define BTN_2_PIN     24    // ��� ��� ������ 2
#define BTN_3_PIN     28    // ��� ��� ������ 3
#define ENG_MIX_PIN   8     // ��� ��� ��������� (�������)
#define ENG_MIX_SPEED 1     // �������� ��������� (�������)

#define SPEED    50 / (ENGINES-1)  // �������� �������
#define ADD_GND  1                 // �������� GND ����
#define ADD_5V   1                 // �������� +5V ����

#define K1 0.5f     // ����. ��� ������ 1
#define K2 0.5f     // ����. ��� ������ 2
#define K_REV 0.8f  // ����. ��������� ������ 2 � ������ 1
#define K_ENG2 K_REV * MICROSTEP

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
StepEngine eng3(6, 7, 50, K1);     // PUL+, DIR+, ��������, k
#endif // THREE_ENGINE


GyverOLED<SSD1306_128x32, OLED_BUFFER> oled(0x3C);
Button btn_start(BTN_1_PIN);
Button btn_clean(BTN_2_PIN);
Button btn_mix(BTN_3_PIN);

volatile bool show    = 0;    // ���� ��������� ������ �� OLED
uint          counter = 0;    // ������� ��������
float         roundsA = 0.0f; // ���-�� �������� ������� ������
float         roundsB = 0.0f; // ���-�� �������� ������� ������
uint          param   = 0;    // ��������� (������) ����� ����

String menu[] =
{
    "�������",
    "����������"
};

void mix(uint val)
{
    val = map(val, 0, 100, 544, 2400);

    digitalWrite(ENG_MIX_PIN, HIGH);
    delayMicroseconds(val);
    digitalWrite(ENG_MIX_PIN, LOW);
    delayMicroseconds(val);
}

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

// ����� ���������� � ����� ����������
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
    pinMode(ENG_MIX_PIN, OUTPUT);
    mix(0);

    Timer5.setFrequency(1);
    Timer5.enableISR(CHANNEL_A);

    #if ADD_GND == 1
    // ���� �� ������� ����� ��� �����
    {
        int gnd_pins[] = { 34, 36, 38, 40, 42 }; // ���� ��� GND

        for (int i = 0; i < (sizeof(gnd_pins) / sizeof(*gnd_pins)); i++)
        {
            pinMode(gnd_pins[i], OUTPUT);
            digitalWrite(gnd_pins[i], LOW);
        }
    }
    #endif

    #if ADD_5V == 1
    // ���� �� ������� ����� ��� �������
    {
        int v_pins[] = { 26 }; // ���� ��� 5V

        for (int i = 0; i < (sizeof(v_pins) / sizeof(*v_pins)); i++)
        {
            pinMode(v_pins[i], OUTPUT);
            digitalWrite(v_pins[i], HIGH);
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
bool  is_clean = 0;

void loop()
{
    if(show)
    {
        roundsA = (eng1.get_counter() / MICROSTEP) * eng1.get_k();
        roundsB = (eng2.get_counter() / MICROSTEP) * eng2.get_k();
        
        oled.home();
        oled.print("����. A: ");
        oled.println(String(roundsA) + " ml");
        oled.print("����. B: ");
        oled.println(String(roundsB) + " ml");
        if (is_clean)
        {
            oled.println("�������...");
            is_clean = 0;
            eng1.reset_counter();
            eng2.reset_counter();
        }
        else
        {
            oled.print("�����: ");
            oled.println(String(roundsA + roundsB) + " ml");
        }
        oled.update();
        oled.clear();
        show = 0;
    }

    btn_start.tick();
    btn_clean.tick();
    btn_mix.tick();

    if (!btn_start.getSignal())
    {
        for (r = 0; r < MICROSTEP; r++)
        {
            eng1.spin();
            if (r <= K_ENG2)
            {
                eng2.spin();
            }
        }
    }
    else if (!btn_clean.getSignal())
    {
        for (r = 0; r < MICROSTEP; r++)
        {
            eng3.spin();
        }

        is_clean = 1;
    }
    else if (!btn_mix.getSignal())
    {
        mix(ENG_MIX_SPEED);
    }
}
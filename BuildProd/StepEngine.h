#pragma once

#include <Arduino.h>

#include "Base.h"

enum Speed 
{
	MIN, // 500 ���
	S_1, // 250 ���
	S_2, // 150 ���
	S_3, // 80  ���
	MAX, // 50  ���
} ;

class StepEngine
{
public:
	StepEngine(uint stepPin, uint dirPin, Speed speed = S_2, float k = 0.0f);
public:
	void spin() const;
	void switch_dir();
	void set_speed(Speed speed);
	void set_k(float k);
	float get_k() const;
private:
	bool  direction;
	uint  stepPin;
	uint  dirPin;
	uint  speed;
	float k;
#ifdef DBG				
public:
	String get_log() const { return log_s; };
private:
	String log_s;
#endif			
};


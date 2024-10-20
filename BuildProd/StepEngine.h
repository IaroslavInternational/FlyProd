#pragma once

#include <Arduino.h>

#include "Base.h"

enum Speed 
{
	MIN, // 500 мкс
	S_1, // 250 мкс
	S_2, // 150 мкс
	S_3, // 80  мкс
	MAX  // 50  мкс
};

class StepEngine
{
public:
	StepEngine(uint stepPin, uint dirPin, Speed speed = S_2, float k = 0.0f);
	StepEngine(uint stepPin, uint dirPin, uint speed = 50, float k = 0.0f);
public:
	void  spin();
	void  start_spin() const;
	void  end_spin() const;
	void  switch_dir();
	void  set_speed(Speed speed);
	void  set_k(float k);
	float get_k() const;
	unsigned long long  get_counter() const;
	void  reset_counter();
private:
	bool  direction;
	uint  stepPin;
	uint  dirPin;
	uint  speed;
	unsigned long long counter = 0;
	float k;
#ifdef DBG				
public:
	String get_log() const { return log_s; };
private:
	String log_s;
#endif			
};


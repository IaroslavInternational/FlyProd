#pragma once

#include <Arduino.h>

#include "Base.h"

class StepEngine
{
public:
	StepEngine(uint stepPin, uint dirPin);
public:
	void spin(uint delay);
	void switch_dir();
private:
	bool direction;
	uint stepPin;
	uint dirPin;
};


#include <Arduino.h>

#include "Base.h"

class Button
{
public:
	Button(uint pin);
public:
	int getSignal();
private:
	uint pin;
};
#include "Button.h"

Button::Button(uint pin) 
{
	this->pin = pin;

	pinMode(pin, INPUT);
}

int Button::getSignal()
{
	return digitalRead(pin);
}
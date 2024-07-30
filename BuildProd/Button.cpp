#include "Button.h"

Button::Button(uint pin) 
	:
	btn(pin)
{
	this->pin = pin;

	pinMode(pin, INPUT);
}

void Button::tick()
{
	btn.tick();
}

int Button::getSignal()
{
	return digitalRead(pin);
}

bool Button::isClick()
{
	return btn.isClick();
}

bool Button::isHolded()
{
	return btn.isHolded();
}

bool Button::isDoubleClick()
{
	return btn.isDouble();
}

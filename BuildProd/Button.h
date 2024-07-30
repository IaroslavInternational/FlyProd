#include <Arduino.h>
#include <GyverButton.h>

#include "Base.h"

class Button
{
public:
	Button(uint pin);
public:
	int getSignal();
	void tick();
	bool isClick();
	bool isHolded();
	bool isDoubleClick();
private:
	GButton btn;
	uint   pin;			
};
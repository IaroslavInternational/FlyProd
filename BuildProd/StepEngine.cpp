#include "StepEngine.h"

StepEngine::StepEngine(uint stepPin, uint dirPin)
{	
	this->stepPin = stepPin;
	this->dirPin = dirPin;

	direction = HIGH;

	pinMode(stepPin, OUTPUT);
	pinMode(dirPin,  OUTPUT);

	digitalWrite(dirPin, HIGH);
}

void StepEngine::spin(uint delay)
{
	digitalWrite(stepPin, HIGH);
	delayMicroseconds(delay);
	digitalWrite(stepPin, LOW);
	delayMicroseconds(delay);
}

void StepEngine::switch_dir()
{
	direction == HIGH ? LOW : HIGH;
}

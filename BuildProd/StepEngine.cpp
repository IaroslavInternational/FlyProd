#include "StepEngine.h"

StepEngine::StepEngine(uint stepPin, uint dirPin, Speed speed, float k)
	:
	stepPin(stepPin),
	dirPin(dirPin),
	direction(HIGH),
	k(k)
{	
	set_speed(speed);

	pinMode(stepPin, OUTPUT);
	pinMode(dirPin,  OUTPUT);

	digitalWrite(dirPin, HIGH);

	LOG("Engine init");
	LOG("Pins:");
	LOG("PUL+ (" + String(stepPin) + "), " + "DIR+ (" + String(dirPin) + ")");
	LOG("Velocity: " + String(speed));
	LOG("K: " + String(k));
}

void StepEngine::spin() const
{
	digitalWrite(stepPin, HIGH);
	delayMicroseconds(speed);
	digitalWrite(stepPin, LOW);
	delayMicroseconds(speed);
}

void StepEngine::switch_dir()
{
	direction == HIGH ? LOW : HIGH;
}

void StepEngine::set_speed(Speed speed)
{
	switch (speed)
	{
	case MIN:
		this->speed = 500;
		break;
	case S_1:
		this->speed = 250;
		break;
	case S_2:
		this->speed = 150;
		break;
	case S_3:
		this->speed = 80;
		break;
	case MAX:
		this->speed = 50;
		break;
	default:
		break;
	}
}

void StepEngine::set_k(float k)
{
	this->k = k;
}

float StepEngine::get_k() const
{
	return k;
}

String StepEngine::get_log() const
{
	return log_s;
}

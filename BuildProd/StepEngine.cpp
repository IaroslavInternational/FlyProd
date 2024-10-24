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

StepEngine::StepEngine(uint stepPin, uint dirPin, uint speed, float k)
	:
	stepPin(stepPin),
	dirPin(dirPin),
	direction(HIGH),
	speed(speed),
	k(k)
{
	pinMode(stepPin, OUTPUT);
	pinMode(dirPin, OUTPUT);

	digitalWrite(dirPin, HIGH);

	LOG("Engine init");
	LOG("Pins:");
	LOG("PUL+ (" + String(stepPin) + "), " + "DIR+ (" + String(dirPin) + ")");
	LOG("Velocity: " + String(speed));
	LOG("K: " + String(k));

	log_s = "";
}

void StepEngine::spin()
{
	digitalWrite(stepPin, HIGH);
	delayMicroseconds(speed);
	digitalWrite(stepPin, LOW);
	delayMicroseconds(speed);
	counter++;
}

void StepEngine::start_spin() const
{
	digitalWrite(stepPin, HIGH);
}

void StepEngine::end_spin() const
{
	digitalWrite(stepPin, LOW);
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

unsigned long long StepEngine::get_counter() const
{
	return counter;
}

void StepEngine::reset_counter()
{
	counter = 0;
}
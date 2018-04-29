#include "Arduino.h"
#include "ControlRGB.h"

ControlRGB::ControlRGB() {
	_rPin = 3;
	_gPin = 5;
	_bPin = 6;
	pinMode(_rPin, OUTPUT);
	pinMode(_gPin, OUTPUT);
	pinMode(_bPin, OUTPUT);
}

ControlRGB::ControlRGB(int rPin, int gPin, int bPin) {
	_rPin = bPin;
	_gPin = gPin;
	_bPin = rPin;
	pinMode(_rPin, OUTPUT);
	pinMode(_gPin, OUTPUT);
	pinMode(_bPin, OUTPUT);
}

void ControlRGB::turnOn(int rBrightness, int gBrightness, int bBrightness, int delayTime) {
	int rCounter = 0;
	int gCounter = 0;
	int bCounter = 0;
	int stepsMax = 500;
	for (int steps = 0; steps < stepsMax; steps++) {
		analogWrite(_rPin, rCounter);
		analogWrite(_gPin, gCounter);
		analogWrite(_bPin, bCounter);
		rCounter += rBrightness / stepsMax;
		gCounter += gBrightness / stepsMax;
		bCounter += bBrightness / stepsMax;
		delay(delayTime / stepsMax);
	}
}

void ControlRGB::turnOff() {
	analogWrite(_rPin, 0);
	analogWrite(_gPin, 0);
	analogWrite(_bPin, 0);
}
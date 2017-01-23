/*
 * DigitalIoPin.h
 *
 *  Created on: 3.11.2016
 *      Author: Tony
 */

#ifndef DIGITALIOPIN_H_
#define DIGITALIOPIN_H_

class DigitalIoPin {
public:
	DigitalIoPin(int pin, int port, bool input = true, bool pullup = true, bool invert = false);
	virtual ~DigitalIoPin();
	bool read();
	void write(bool value);
private:
	int iopin, ioport;
	// add these as needed
};

#endif /* DIGITALIOPIN_H_ */

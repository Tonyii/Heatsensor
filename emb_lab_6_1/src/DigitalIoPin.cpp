#include "chip.h"
#include "DigitalIoPin.h"


DigitalIoPin::DigitalIoPin(int pin, int port, bool input, bool pullup, bool invert) {
	// Set Port.Pin as a digital input with pull up
	if (input){
	Chip_IOCON_PinMuxSet(LPC_IOCON, port, pin, (IOCON_MODE_PULLUP | IOCON_DIGMODE_EN | IOCON_INV_EN));
	Chip_GPIO_SetPinDIRInput(LPC_GPIO, port, pin);
	}
	else{
	Chip_IOCON_PinMuxSet(LPC_IOCON, port, pin, (IOCON_MODE_PULLUP | IOCON_DIGMODE_EN | IOCON_INV_EN));
	Chip_GPIO_SetPinDIROutput(LPC_GPIO, port, pin);
	}
	ioport = port;
	iopin = pin;
}

bool DigitalIoPin::read(){
	if (Chip_GPIO_GetPinState(LPC_GPIO, ioport, iopin)){
		return true;
	}
	return false;
}

void DigitalIoPin::write(bool value){
	Chip_GPIO_SetPinState(LPC_GPIO, ioport, iopin, value);
}

DigitalIoPin::~DigitalIoPin() {
	// TODO Auto-generated destructor stub
}


/*
===============================================================================
 Name        : main.c
 Author      : $(author)
 Version     :
 Copyright   : $(copyright)
 Description : main definition
===============================================================================
*/

#if defined (__USE_LPCOPEN)
#if defined(NO_BOARD_LIB)
#include "chip.h"
#else
#include "board.h"
#endif
#endif
#include "ITM_write.h"
#include "BarGraph.h"
#include "LiquidCrystal.h"
#include "DigitalIoPin.h"
#include "CoordinatedBarGraph.h"

#include <cr_section_macros.h>

// TODO: insert other include files here

// TODO: insert other definitions and declarations here
#define TICKRATE_HZ (1000)	/* 100 ticks per second */
static volatile uint32_t ticks;
static volatile uint32_t counter;
static volatile int erotus = 0;
static volatile int rate = 0;
static volatile uint8_t ledNow = 0;
static volatile int samples0 = 0;
static volatile int samples10 = 0;
static volatile uint8_t samplecount = 0;

static volatile uint32_t a0;
static volatile uint32_t d0;
static volatile uint32_t a10;
static volatile uint32_t d10;


extern "C" {

void SysTick_Handler(void)
{
	static uint32_t count;
	static uint32_t count2;
	/* Every 1/2 second */
	count++;
	count2++;
	if (erotus > 20){
		rate = erotus;
		ledNow = 0;
		Board_LED_Set(1, false);
		Board_LED_Set(2, false);
	}
	else if (erotus < -20){
		rate = 0 - erotus;
		ledNow = 2;
		Board_LED_Set(0, false);
		Board_LED_Set(1, false);
	}
	else {
		rate = 5;
		ledNow = 1;
		Board_LED_Set(0, false);
		Board_LED_Set(2, false);
	}
	if (count >= rate) {
		count = 0;
		Board_LED_Toggle(ledNow);

	}

	ticks++;
	if(count2 >= 5){
		 Chip_ADC_StartSequencer(LPC_ADC0, ADC_SEQA_IDX);
		 count2 = 0;
	}
	if(counter > 0) counter--;
}

/*This is for handling sample taking*/
void ADC0A_IRQHandler(void)
{
uint32_t pending;
/* Get pending interrupts */
pending = Chip_ADC_GetFlags(LPC_ADC0);
/* Sequence A completion interrupt */

if (pending & ADC_FLAGS_SEQA_INT_MASK) {
/* Read the ADC values here */

	// samplecount, samples
	a0 = Chip_ADC_GetDataReg(LPC_ADC0, 8); // raw value
	samples0 += ADC_DR_RESULT(a0); // ADC result with status bits masked to zero and shifted to start from zero
	a10 = Chip_ADC_GetDataReg(LPC_ADC0, 10);
	samples10 += ADC_DR_RESULT(a10); //add samples

	samplecount++;
	if (samplecount >= 9){  	//take the average of the samples

		d0 = samples0 / samplecount;
		d10 = samples10 / samplecount;

		samplecount = 0;	//initialize
		samples0 = 0;
		samples10= 0;

		erotus = (d0 - d10);   // new value for erotus is set

	}
}
/* Clear any pending interrupts */
Chip_ADC_ClearFlags(LPC_ADC0, pending);
}

} // extern "C"


// returns the interrupt enable state before entering critical section
bool enter_critical(void)
{
	uint32_t pm = __get_PRIMASK();
	__disable_irq();
	return (pm & 1) == 0;
}

// restore interrupt enable state
void leave_critical(bool enable)
{
	if(enable) __enable_irq();
}

void Sleep(uint32_t time)	//for pauses
{
	counter = time;
	while(counter > 0) {
		__WFI();
	}
}

int main(void) {

#if defined (__USE_LPCOPEN)
    // Read clock settings and update SystemCoreClock variable
    SystemCoreClockUpdate();
#if !defined(NO_BOARD_LIB)
    // Set up and initialize all required blocks and
    // functions related to the board hardware
    Board_Init();
    Chip_RIT_Init(LPC_RITIMER);
    // Set the LED to the state of "On"
    Board_LED_Set(0, true);
#endif
#endif
    ITM_init();


    /*THIS IS PREWRITTEN HERE*/


    /* Setup ADC for 12-bit mode and normal power */
	Chip_ADC_Init(LPC_ADC0, 0);
	/* Setup for maximum ADC clock rate */
	Chip_ADC_SetClockRate(LPC_ADC0, ADC_MAX_SAMPLE_RATE);
	/* For ADC0, sequencer A will be used without threshold events.
	   It will be triggered manually, convert CH8 and CH10 in the sequence  */
	Chip_ADC_SetupSequencer(LPC_ADC0, ADC_SEQA_IDX, (ADC_SEQ_CTRL_CHANSEL(8) | ADC_SEQ_CTRL_CHANSEL(10) | ADC_SEQ_CTRL_MODE_EOS));
	/* For ADC0, select analog input pin for channel 0 on ADC0 */
	Chip_ADC_SetADC0Input(LPC_ADC0, 0);
	/* Use higher voltage trim for both ADC */
	Chip_ADC_SetTrim(LPC_ADC0, ADC_TRIM_VRANGE_HIGHV);
	/* Assign ADC0_8 to PIO1_0 via SWM (fixed pin) and ADC0_10 to PIO0_0 */
	Chip_SWM_EnableFixedPin(SWM_FIXED_ADC0_8);
	Chip_SWM_EnableFixedPin(SWM_FIXED_ADC0_10);
	/* Need to do a calibration after initialization and trim */
	Chip_ADC_StartCalibration(LPC_ADC0);
	while (!(Chip_ADC_IsCalibrationDone(LPC_ADC0)));
	/* Clear all pending interrupts and status flags */
	Chip_ADC_ClearFlags(LPC_ADC0, Chip_ADC_GetFlags(LPC_ADC0));
	/* Enable sequence A completion interrupts for ADC0 */
	Chip_ADC_EnableInt(LPC_ADC0, ADC_INTEN_SEQA_ENABLE);
	/* Enable related ADC NVIC interrupts */
	NVIC_EnableIRQ(ADC0_SEQA_IRQn);
	/* We don't enable the corresponding interrupt in NVIC so the flag is set but no interrupt is triggered */
	/* Enable sequencer */
	Chip_ADC_EnableSequencer(LPC_ADC0, ADC_SEQA_IDX);
	/* Configure systick timer */
	SysTick_Config(Chip_Clock_GetSysTickClockRate() / TICKRATE_HZ);

	/*PREWRITTEN UNTIL HERE*/

	char str[80];
	char trimmer[5];
	char temp[5];
	DigitalIoPin a0(8, 0, false);
	DigitalIoPin a1(6, 1, false);
	DigitalIoPin a2(8, 1, false);
	DigitalIoPin a3(5, 0, false);
	DigitalIoPin a4(6, 0, false);
	DigitalIoPin a5(7, 0, false);
	LiquidCrystal lcd(&a0, &a1, &a2, &a3, &a4, &a5);
	CoordinatedBarGraph trimmerBar(lcd, 50,false, 4, 0);
	CoordinatedBarGraph temperatureBar(lcd, 50,false, 4, 1);
	lcd.begin(16, 2);	//set lcd display
	int tempr = 0;

	while(1) {

		/*for testing some info*/
		sprintf(str, "a0 = %08lX, d0 = %lu a10 = %08lX, d10 = %lu\n", a0, d0, a10, d10);
		sprintf(trimmer, "%04lu", d0); //set trimmer buffer
		sprintf(temp, "%04lu", d10);	//set temperature buffer
		ITM_write(str);
		lcd.setCursor(0,0);
		lcd.print(trimmer);
		trimmerBar.draw(d0 / 82);	//scales the trimmers barGraph for the size of lcd

		/*setting thresholds */
		if (d10 <= 2400) tempr = 0;
		else if (d10 >= 2500) tempr = 100;
		else tempr = d10-2400;	//this is the middle point of temperature
		tempr = (tempr / 2);

		/*prints*/
		lcd.setCursor(0,1);
		lcd.print(temp);
		temperatureBar.draw(tempr);

		Sleep(100);

	}

    return 0 ;
}

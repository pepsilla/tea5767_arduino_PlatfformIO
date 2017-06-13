/*
 * radioFM.h
 *
 *  Created on: 12 ago. 2016
 *      Author: JoseMaria
 */

#ifndef LIBRARIES_radioFM_H_
#define LIBRARIES_radioFM_H_
#include "Arduino.h"

#define radioFM_ADDRESS 		0x60 // I2C Chip address
#define radioFM_INIT_FQR 		96.6 // Initial frecuency setup

// Table 11 Search stop level setting
#define radioFM_senseLow		1		 // low; level ADC output = 5
#define radioFM_senseMiddle	2		 // mid; level ADC output = 7
#define radioFM_senseHigh		3		 // high; level ADC output = 10

class radioFM
{
	public:
		radioFM();
		uint8_t init();

		// Settings
		uint8_t setFrecuency(double frecuency);
		uint8_t setLowSideInjection();
		uint8_t setHigSideInjection();
		uint8_t setSense(uint8_t sense);

		// Gettings
		double getFrecuency();
		uint8_t getIfCounter();
		uint8_t getSignal();

		//Commands and infos
		uint8_t scanUp();
		uint8_t scanDown();
		uint8_t muteScanDown();
		uint8_t muteScanUp();
		uint8_t goUp();
		uint8_t goDowm();
		uint8_t mute();
		uint8_t unMute();
		uint8_t forceMono();
		uint8_t stereo();
		bool loInjection();
		bool highInjection();
		bool isMute();
		bool isStereo();
		bool isRf();
		bool isBandLimit();
		uint8_t readStatus();
	private:
		int _address;
		double _frequency;
		uint8_t _readBuffer[5];
		uint8_t _writeBuffer[5];
};



#endif /* LIBRARIES_radioFM_H_ */

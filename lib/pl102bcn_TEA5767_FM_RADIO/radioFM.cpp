/*
 * radioFM.cpp
 *
 *  Created on: 12 ago. 2016
 *      Author: JoseMaria
 */
#include "Wire.h"
#include <radioFM.h>

radioFM::radioFM(){
	_address = radioFM_ADDRESS;
	_frequency = radioFM_INIT_FQR;
	uint8_t counter;
	for(counter=0;counter<5;counter++){
		_readBuffer[counter] = 0;
		_writeBuffer[counter] = 0;
	}

}

uint8_t radioFM::radioFM::init(){

	//calculating PLL word

	this->setHigSideInjection();
	this->setFrequency(_frequency);
	this->stereo();
	this->resetScanMode();
	this->unMute();
	this->setSense(radioFM_senseMiddle);
	_writeBuffer[4] = 0;
	_writeBuffer[3] |= B00010000;
	this->updateStatus();
	delay(50);
	this->readStatus();
	return 0;
}

uint8_t radioFM::setFrequency(double frequency){

	uint16_t fB;

	if(this->isHighSideInjection()){
		fB = round(4*(frequency+225000)/32768); //calculating PLL word
	}
	else{
		fB = round(4*(frequency-225000)/32768); //calculating PLL word
	}
	uint8_t fqH = fB >> 8;
  uint8_t fqL = fB & 0xFF;
	_writeBuffer[0] = fqH;
	_writeBuffer[1] = fqL;
	return 0;
}

uint8_t radioFM::setLowSideInjection(){
	_writeBuffer[2] &= B11101111;
	return 0;
}

uint8_t radioFM::setHigSideInjection(){
	_writeBuffer[2] |= B00010000;
	return 0;
}

uint8_t radioFM::setSense(uint8_t sense){
	if(sense == 0 || sense == 1){
		_writeBuffer[2] &= B10011111;
		_writeBuffer[2] |= B00100000;
		return 0;
	}
	if(sense == 2){
		_writeBuffer[2] &= B10011111;
		_writeBuffer[2] |= B01000000;
		return 0;
	}
	if(sense == 3){
		_writeBuffer[2] &= B10011111;
		_writeBuffer[2] |= B01100000;
		return 0;
	}
	return sense;
}

void radioFM::setScanMode(){
	_writeBuffer[0] |= B01000000;
}

void radioFM::resetScanMode(){
	_writeBuffer[0] &= B10111111;
}

double radioFM::getFrecuency(){
	double frequency;
	if(this->isHighSideInjection()){
			frequency=round((((_readBuffer[0]&0x3F)<<8)+_readBuffer[1])*32768/4-225000);
	}
	else{
		frequency=round((((_readBuffer[0]&0x3F)<<8)+_readBuffer[1])*32768/4+225000);
	}
	return frequency;
}

uint8_t radioFM::getIfCounter(){
	return(_readBuffer[2] & B01111111);
}

uint8_t radioFM::getSignal(){
	return (_readBuffer[3]>>4);
}

uint8_t radioFM::scanUp(){
	_writeBuffer[2] |= B10000000;
	return 0;
}

uint8_t radioFM::scanDown(){
	_writeBuffer[2] &= B01111111;
	return 0;
}

uint8_t radioFM::mute(){
	_writeBuffer[0] |= B10000000;
	return 0;
}

uint8_t radioFM::unMute(){
	_writeBuffer[0] &= B01111111;
	return 0;
}

void radioFM::forceMono(){
	_writeBuffer[2] |= B00001000;
}

void radioFM::stereo(){
	_writeBuffer[2] &= B11110111;
}

bool radioFM::isHighSideInjection(){
	if(_writeBuffer[2] & B00010000) return true;
	return false;
}

bool radioFM::isLowSideInjection(){
	if(_writeBuffer[2] & B00010000) return false;
	return true;
}

bool radioFM::isMute(){
	if(_readBuffer[0] & B10000000)return true;
	return false;
}

bool radioFM::isStereo(){
	if(_readBuffer[2] & B10000000)return true;
	return false;
}

bool radioFM::isRf(){
	if(_readBuffer[0] & B10000000) return true;
	return false;
}

bool radioFM::isBandLimit(){
	if(_readBuffer[0] & B01000000) return true;
	return false;
}

uint8_t radioFM::readStatus(){
	Wire.requestFrom(_address,5); //reading TEA5767
	uint8_t counter = 0;
	while (Wire.available()){
		if(counter<5) _readBuffer[counter]= Wire.read();
		counter++;
	}
	return counter;
}

uint8_t radioFM::updateStatus(){
	Wire.beginTransmission(_address);   //writing TEA5767
	uint8_t counter = 0;
	for (counter = 0; counter < 5; counter++){
		Wire.write(_writeBuffer[counter]);
	}
	Wire.endTransmission();
	return counter;
}

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
	uint16_t fReg=(4*((_frequency*1000000)+225000))/32768;
  uint8_t fHigh=fReg >> 8;
  uint8_t fLow=fReg & 0XFF;

	Wire.beginTransmission(_address);
  Wire.write(fHigh);
  Wire.write(fLow);
  Wire.write(0xB0);
  Wire.write(0x10);
  Wire.write(0x00);
  Wire.endTransmission();
	delay(50);
	this->readStatus();

	return 0;
}

uint8_t radioFM::setFrecuency(double frecuency){
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

double radioFM::getFrecuency(){
	double frequency;
	frequency=(((_readBuffer[0]&0x3F)<<8)+_readBuffer[1])*32768/4-225000;
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

uint8_t radioFM::muteScanDown(){
	return 0;
}

uint8_t radioFM::muteScanUp(){
	return 0;
}

uint8_t radioFM::goUp(){
	return 0;
}

uint8_t radioFM::goDowm(){
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

uint8_t radioFM::forceMono(){
	return 0;
}

uint8_t radioFM::stereo(){
	return 0;
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
	Wire.requestFrom(0x60,5); //reading TEA5767
	uint8_t counter = 0;
	while (Wire.available()){
		if(counter<5) _readBuffer[counter]= Wire.read();
		counter++;
	}
	return counter;
}

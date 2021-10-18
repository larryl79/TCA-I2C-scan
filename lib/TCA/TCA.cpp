/*
*   TCA9546A (4 channel) TCA9548A (8 channel) I2C MUX
*
*
*/

//
//    FILE: TCA9548.cpp
// AUTHOR larryl79 URL: https://github.com/larryl79/TCA
// FORKED FROM: Rob Tillaart URL: https://github.com/RobTillaart/TCA9548
// FORKED VERSION: 0.1.0
// FORKED DATE: 2021-03-16
// PURPOSE: Library for TCA I2C multiplexer
//
//  HISTORY:
//  0.0.2   2021-10-18  initial version


#include "TCA.h"

TCA::TCA(const uint8_t deviceAddr, TwoWire *wire)
{
  _address  = deviceAddr;
  _wire     = wire;
  _mask     = 0x00;
  _resetPin = -1;
  _forced   = false;
}

#if defined (ESP8266) || defined(ESP32)
bool TCA::begin(uint8_t dataPin, uint8_t clockPin, uint8_t mask)
{
  if ((dataPin < 255) && (clockPin < 255))
  {
    _wire->begin(dataPin, clockPin);
  } else {
    _wire->begin();
  }
  if (! isConnected()) return false;
  setChMask(mask);
  return true;
}
#endif

bool TCA::begin(uint8_t mask)
{
  _wire->begin();
  if (! isConnected()) return false;
  setChMask(mask);
  return true;
}

bool TCA::isConnected()
{
  _wire->beginTransmission(_address);
  return ( _wire->endTransmission() == 0);
}

bool TCA::isConnected(uint8_t addr)
{
  _wire->beginTransmission(addr);
  return ( _wire->endTransmission() == 0);
}

void TCA::enableCh(uint8_t ch)
{
  if (isEnabled(ch)) return;
  setChMask(_mask | (0x01 << ch));
}

void TCA::disableCh(uint8_t ch)
{
  if (!isEnabled(ch)) return;
  setChMask(_mask & ~(0x01 << ch));
}

void TCA::selectCh(uint8_t ch)
{
  setChMask(0x01 << ch);
}

bool TCA::isEnabled(uint8_t ch)
{
  if (ch > 7) return false;
  return (_mask & (0x01 << ch));
}

void TCA::setChMask(uint8_t mask)
{
  if ((_mask == mask) && (! _forced)) return;
  _mask = mask;
  _wire->beginTransmission(_address);
  _wire->write(_mask);
  _error = _wire->endTransmission();
}

uint8_t TCA::getChMask()
{
  return _mask;
}

void TCA::setResetPin(uint8_t resetPin)
{
  _resetPin = resetPin;
  pinMode(_resetPin, OUTPUT);
  digitalWrite(_resetPin, LOW);  // CHECK
}

void TCA::reset()
{
  digitalWrite(_resetPin, HIGH);  // CHECK
  delay(1);
  digitalWrite(_resetPin, LOW);   //  TCA reset
  delay(1);
  digitalWrite(_resetPin, HIGH);  // TCA release reset
}

uint8_t TCA::getCh() {
  switch (_mask) {
  case 0x01: _currentCh = 0; break;  
  case 0x02: _currentCh = 1; break;  
  case 0x04: _currentCh = 2; break;  
  case 0x08: _currentCh = 3; break;  
  case 0x10: _currentCh = 4; break;  
  case 0x20: _currentCh = 5; break;  
  case 0x40: _currentCh = 6; break;  
  case 0x80: _currentCh = 7; break;  
  default:
    _currentCh = -1;
    break;
  }
  return _currentCh;
}

uint8_t TCA::setNextCh() {
  if ( getCh() < 7 ) 
    { _changeCh = getCh() + 1; }
   else
    { _changeCh = getCh(); }
  return _changeCh;
}

uint8_t TCA::setPrevCh() {
  if ( getCh() > 2 ) 
    { _changeCh = getCh() - 1; }
   else
    { _changeCh = getCh(); }
  return _changeCh;
}

uint8_t TCA::getMaxCh() {
  if ( isConnected() )
    {
    for ( int i=7; i > 1; i-- )
      {
      selectCh(i);
      if ( isEnabled(i) )
        {
        _maxCh = i;
        return _maxCh; 
        }
      }
    }
    else
    { return _maxCh; }
}

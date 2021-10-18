/*
*   TCA9546A (4 channel) TCA9548A (8 channel) I2C MUX
*
*
*/

#pragma once
//
//    FILE: TCA.h
// AUTHOR larryl79 URL: https://github.com/larryl79/TCA
// FORKED FROM: Rob Tillaart URL: https://github.com/RobTillaart/TCA9548
// FORKED VERSION: 0.1.0
// FORKED DATE: 2021-03-16
// PURPOSE: Library for TCA I2C multiplexer
//
//     
//

#include "Arduino.h"
#include "Wire.h"

#define TCA_LIB_VERSION    (F("0.0.2"))


class TCA
{
public:
  // address = 0x70 .. 0x77
  TCA(const uint8_t deviceAddress, TwoWire *wire = &Wire);

#if defined (ESP8266) || defined(ESP32)
  bool    begin(uint8_t sda, uint8_t scl, uint8_t mask = 0x00);  // default no channels enabled
#endif
  bool    begin(uint8_t mask = 0x00);         // default no channels enabled
  bool    isConnected();                      // find multiplexer on I2C bus
  bool    isConnected(uint8_t address);       // find any addr on I2C bus

  // channel = 0.. 7
  void    enableCh(uint8_t ch);
  void    disableCh(uint8_t ch);
  void    selectCh(uint8_t ch);   // enable only this channel
  bool    isEnabled(uint8_t ch);
  uint8_t getCh();      // get active channel
  uint8_t setNextCh();
  uint8_t setPrevCh();
  uint8_t getMaxCh();

  // mask = 0x00 .. 0xFF - every bit is a channel.
  void    setChMask(uint8_t mask);
  uint8_t getChMask();

  void    setResetPin(uint8_t resetPin);
  void    reset();     // trigger reset pin

  // set forced write
  void    setForced(bool forced) { _forced = forced; };
  bool    getForced()            { return _forced; };
  
  

private:
  uint8_t   _mask      = 0x00;             // caching mask
  uint8_t   _currentCh = -1;               // channel number
  int       _changeCh  = -1;
  int       _maxCh     = -1;
  uint8_t   _resetPin  = -1;
  int       _error     = 0;
  uint8_t   _address;
  TwoWire*  _wire;
  bool      _forced;
};

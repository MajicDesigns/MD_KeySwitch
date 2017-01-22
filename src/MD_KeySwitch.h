/*
  MD_KeySwitch.h - Library for user keyswitch on digital input.
  
  Small library to allow the use of momentary push switches as user
  input devices.
  - Detects the transition from ON state to OFF state (either low/high or high/low)
  - Software debounce
  - Press, double press and long press detection
  - Software auto repeat
  
  Update Jan 2017 v1.4.1
  - Added enableRepeatResult
  
  Update November 2016 v1.4
  - Early long press returned if auto repeat is not enabled

  Update April 2016 v1.3
  - Added facility to enable/disable presses

  Updated Feb 2016 v1.2
  - Added double press detection

  Updated June 2015 v1.1
  - Added long press and changed return code from boolean to enumerated type

  v1.0 March 2014
  - New library

  Copyright Marco Colli 2014-2016.
  
  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.
 */

#pragma once

#include <Arduino.h>

// Default values for timed events.
// Note these are all off the same base (ie when the switch is first detected)
#define	KEY_DEBOUNCE_TIME	  50	// in milliseconds
#define KEY_DPRESS_TIME     250 // in milliseconds
#define KEY_LONGPRESS_TIME  450 // in milliseconds
#define	KEY_REPEAT_TIME		  650	// in milliseconds
#define KEY_ON_STATE		    LOW	// default active low - transition high to low

// Bit enable/disable 
#define REPEAT_RESULT_ENABLE  3   // return KS_REPEAT instead of KS_PRESS for repeat keys
#define DPRESS_ENABLE         2   // enable double press
#define LONGPRESS_ENABLE      1   // enable long press
#define REPEAT_ENABLE         0   // enable repeat key

class MD_KeySwitch
{
public:
	enum keyResult_t
	{
    KS_NULL,    // no key press
  	KS_PRESS,   // simple press, or a repeated press sequence if not REPEAT_RESULT_ENABLE (default)
    KS_DPRESS,  // double press
    KS_LONGPRESS,  // long press
    KS_RPTPRESS    // repeated key press (if REPEAT_RESULT_ENABLE)
	};
  
  MD_KeySwitch(uint8_t pin, uint8_t onState = KEY_ON_STATE);
	
	~MD_KeySwitch() { };
		
	inline void setDebounceTime(uint16_t t)     { _timeDebounce = t; }
  inline void setDoublePressTime(uint16_t t)  { _timeDoublePress = t; enableDoublePress(true); }
  inline void setLongPressTime(uint16_t t)    { _timeLongPress = t; enableLongPress(true); }
  inline void setRepeatTime(uint16_t t)	      { _timeRepeat = t; enableRepeat(true); }
  inline void enableDoublePress(boolean f)    { if (f) bitSet(_enableFlags, DPRESS_ENABLE); else bitClear(_enableFlags, DPRESS_ENABLE); };
  inline void enableLongPress(boolean f)      { if (f) bitSet(_enableFlags, LONGPRESS_ENABLE); else bitClear(_enableFlags, LONGPRESS_ENABLE); };
  inline void enableRepeat(boolean f)         { if (f) bitSet(_enableFlags, REPEAT_ENABLE); else bitClear(_enableFlags, REPEAT_ENABLE); };
  inline void enableRepeatResult(boolean f)   { if (f) bitSet(_enableFlags, REPEAT_RESULT_ENABLE); else bitClear(_enableFlags, REPEAT_RESULT_ENABLE); };

	void begin(void);
	keyResult_t read(void);
	
protected:
  // FSM state values
  enum state_t { S_IDLE, S_DEBOUNCE1, S_DEBOUNCE2, S_PRESS, S_DPRESS, S_LPRESS, S_REPEAT, S_WAIT };

	uint8_t		_pin;		    // the pin number
	uint8_t		_onState;		// the digital state for ON
	state_t		_state;			// the current state (FSM)
	uint32_t	_timeActive;	// the millis() time it was last activated
  uint8_t   _enableFlags; // functions enabled/disabled
  
  // Note that Debounce time < Long Press Time < Repeat time. No checking is done in the 
  // library to enforce this relationship.
	uint16_t	_timeDebounce;  // debounce time in milliseconds
  uint16_t  _timeDoublePress; // double press detection time in milliseconds
  uint16_t  _timeLongPress; // long press time in milliseconds
	uint16_t	_timeRepeat;	  // repeat time delay in milliseconds
};

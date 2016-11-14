/*
  MD_KeySwitch.cpp - Library for user keyswitch on digital input.

  See main header file for information.
  
  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.
 */

#include "MD_KeySwitch.h"

#define DEBUG 0

#if DEBUG
#define PRINTS(s)   { Serial.print(F(s)); }
#define PRINT(s, v) { Serial.print(F(s)); Serial.print(v); }
#else
#define PRINTS(s)
#define PRINT(s, v)
#endif // DEBUG

MD_KeySwitch::MD_KeySwitch(uint8_t pin, uint8_t onState) :
_pin(pin), _onState(onState), _state(S_IDLE)
{
	setDebounceTime(KEY_DEBOUNCE_TIME);
  setDoublePressTime(KEY_DPRESS_TIME);
  setLongPressTime(KEY_LONGPRESS_TIME);
	setRepeatTime(KEY_REPEAT_TIME);
}

void MD_KeySwitch::begin(void)
{
	if (_onState == LOW)
		pinMode(_pin, INPUT_PULLUP);
	else
		pinMode(_pin, INPUT);	
}

MD_KeySwitch::keyResult_t MD_KeySwitch::read(void)
// return one of the keypress types depending on what has been detected
{
	bool b = (digitalRead(_pin) == _onState);
  keyResult_t k = KS_NULL;

	switch (_state)
	{
		case S_IDLE:		// waiting for first transition
      //PRINTS("\nS_IDLE");
			if (b)
			{
				_state = S_DEBOUNCE1;		// edge detected, initiate debounce
				_timeActive = millis();
			}
			break;
		
		case S_DEBOUNCE1:		
      PRINTS("\nS_DEBOUNCE1");
      // Wait for debounce time to run out and ignore any key switching
      // that might be going on as this may be bounce
			if ((millis() - _timeActive) < _timeDebounce)
				break;
		
      // after debounce - possible long or double press so move to next state
      _state = S_PRESS;
			// fall through
		
    case S_PRESS:   // press?
      PRINTS("\nS_PRESS");
      // Key off before a long press registered, so it is either double press or a press.
			if (!b)
      {
        if (bitRead(_enableFlags, DPRESS_ENABLE))  // DPRESS allowed
          _state = S_DPRESS;
        else      // this is just a press
        {
          k = KS_PRESS;
          _state = S_IDLE;
        }
			}
      else  // if (b)
      {
        // if the switch is still on and we have not run out of longpress time, then
        // just wait for the timer to expire
        if (millis() - _timeActive < _timeLongPress)
          break;

        // time has run out, we either have a long press or are heading 
        // towards repeats if they are enabled   
        if (bitRead(_enableFlags, LONGPRESS_ENABLE) || bitRead(_enableFlags, REPEAT_ENABLE))
          _state = S_LPRESS;
        else    // no other option as we have detected something!
        {
          k = KS_PRESS;
          _state = S_WAIT;
        }
      }
      break;

		case S_LPRESS:		// long press or auto repeat?
      PRINTS("\nS_LPRESS");
      // It is a long press if
      // - Key off before a repeat press is registered, or
      // - Auto repeat is disabled
      // Set the return code and go back to waiting
      if (!b || !bitRead(_enableFlags, REPEAT_ENABLE))
			{
        k = bitRead(_enableFlags, LONGPRESS_ENABLE) ? KS_LONGPRESS : KS_PRESS;
				_state = S_WAIT;
        break;
			}
      // fall through but remain in this state

    case S_REPEAT: // repeat?      
      PRINTS("\nS_REPEAT");
      // Key off before another repeat press is registered, so we have finished.
      // Go back to waiting
      if (!b)
      {
        _state = S_IDLE;
        break;
      }
      
      // if the switch is still on and we have not run out of repeat time, then
      // just wait for the timer to expire
      if ((millis() - _timeActive) < _timeRepeat)
        break;

      // we are now sure we have a repeat, set the return code and remain in this 
      // state checking for further repeats if enabled
      k = KS_PRESS;
      _state = bitRead(_enableFlags, REPEAT_ENABLE) ? S_REPEAT : S_WAIT;
      _timeActive = millis();	// next key repeat time starts now
			break;

    case S_DPRESS:
      PRINTS("\nS_DPRESS");
      if (!b)
      {
        // we didn't get a second press within time then this was just a press
        if (millis() - _timeActive >= _timeDoublePress)
        {
          k = KS_PRESS;
          _state = S_IDLE;
        }
      }
      else  // if (b)
      {
        _state = S_DEBOUNCE2;		// edge detected, initiate debounce
        _timeActive = millis();
      }
      break;
      
    case S_DEBOUNCE2:
      PRINTS("\nS_DEBOUNCE2");
      // Wait for debounce time to run out and ignore any key switching
      // that might be going on as this may be bounce
      if ((millis() - _timeActive) < _timeDebounce)
        break;

      k = b ? KS_DPRESS : KS_PRESS;
      _timeActive = millis();
      _state = S_WAIT;
      break;

    case S_WAIT:
    default:
      // After completing while still key active, t allow the user to release the switch
      // to meet starting conditions for S_IDLE
      PRINTS("\nS_WAITING");
      if (!b)  _state = S_IDLE;
      break;
	}

	return(k);
}

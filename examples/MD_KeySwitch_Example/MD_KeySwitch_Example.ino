// Example showing use of the MD_KeySwitch library
//
#include <MD_KeySwitch.h>

#define  SWITCH_PIN  2

MD_KeySwitch  S(SWITCH_PIN);

void setup() 
{
  Serial.begin(57600);
  Serial.print("\n[MD_KeySwitch example]");
  
  S.begin();
  //S.enableDoublePress(false);
  //S.enableLongPress(false);
  //S.enableRepeat(false);
  S.enableRepeatResult(true);
}

void loop() 
{
  switch(S.read())
  {
    case MD_KeySwitch::KS_NULL:       /* Serial.println("NULL"); */   break;
    case MD_KeySwitch::KS_PRESS:      Serial.print("\nSINGLE PRESS"); break;
    case MD_KeySwitch::KS_DPRESS:     Serial.print("\nDOUBLE PRESS"); break;
    case MD_KeySwitch::KS_LONGPRESS:  Serial.print("\nLONG PRESS");   break;
    case MD_KeySwitch::KS_RPTPRESS:   Serial.print("\nREPEAT PRESS"); break;
    default:                          Serial.print("\nUNKNOWN");      break;
  }    
}

#include "myServo.h"


int MyServo::write(int angle, int servoMin, int servoMax){
  angle = constrain (angle,0,180);
  int pulseWidth = map(angle,0,180,servoMin,servoMax);
  return pulseWidth;


}
#ifndef MYSERVO_H
#define MYSERVO_H

#include <Arduino.h>
#include <stdint.h>

class MyServo {
public:  
  int write(int angle, int servoMin, int servoMax);
 
};

#endif
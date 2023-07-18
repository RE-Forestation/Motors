#include <Wire.h>
#include <MPU9250_asukiaaa.h>

#define pi 3.1415926
#define two_pi 3.1415926*2

MPU9250_asukiaaa mySensor;
float gX, gY, gZ; //gyro
float aX, aY, aZ; //accel 
float mX, mY, mZ; //magnetometer

float roll, pitch, yaw; 

int num_calibration = 500; 

//Gyroscope sensor offset 
float gyroXoffset = 0;
float gyroYoffset = 0;
float gyroZoffset = 0;
int gyroCalibrated = 0;

//Gyroscope sensor scales 
float gXscale = 1.15;
float gYscale = 1.15;
float gZscale = 1.15;

//Accelerometer sensor offeset 
float accelXoffset = 0.00;
float accelYoffset = 0.00;
float accelZoffset = 0.00;
int accelCalibrated = 0; 

//Magnetometer sensor offset
float magXoffset = 0.00;
float magYoffset = 0.00;
float magZoffset = 0.00;

uint8_t sensorId = 0;
int result = 0;
double theta_gyro = 0.0;


void setupSensors() {
  Wire.begin();
  Wire.setClock(400000);
  mySensor.setWire(&Wire); //Please understand why // it's just to make all the myWire == Wire in the code. 
  Serial.println("Initializing GY-91. This can take up to 10s. Please wait.");
  //Serial.println(mySensor.readId(&sensorId) !=0);// To read the device's register and store it inside snesorId. 
  if (mySensor.readId(&sensorId) != 0){
    Serial.println(F("GY-91 not found - check your wiring. Freezing"));
    while(1){
      if(mySensor.readId(&sensorId) ==0){
        break;
      }
    }
      
  }
  Serial.println(F("GY-91 found!"));
  mySensor.beginAccel();
  mySensor.beginGyro();
  mySensor.beginMag();
  
  Serial.println("Calibrating sensors.");
  calibrateGyro();
  calibrateAccel();
  //calibrateMag();
  Serial.println(F("Sensor calibrated!"));
}

void calibrateGyro() {
  //gyroReset
  gX = 0;
  gY = 0;
  gZ = 0;
  for (int count = 0; count < num_calibration; count++){
      //Poll for GY-91 for new data 
      result = mySensor.gyroUpdate(); //Ask the gyro to update the values and send it back/ 
      if (result ==0){
        gX = mySensor.gyroX();
        gY = mySensor.gyroY();
        gZ = mySensor.gyroZ();
        //Serial.println(" gX:" + String(gX) + " gY:" + String(gY) + " gZ:" + String(gZ)); 
      } else {
        Serial.println("Cannot read gyro values" + String(result));
        return;
      }

      gyroXoffset +=gX;
      gyroYoffset +=gY;
      gyroZoffset +=gZ;
    }

    gyroXoffset /= num_calibration;
    gyroYoffset /= num_calibration;
    gyroZoffset /= num_calibration; 
    gyroCalibrated = 1; 
    Serial.println(" gyroXoffset: " + String(gyroXoffset) + " gyroYoffset: " + String(gyroYoffset) + " gyroZoffset: " + String(gyroZoffset));
}

void calibrateAccel(){
  aX = 0;
  aY = 0;
  aZ = 0;
  for ( int count = 0; count < num_calibration; count++){
    result = mySensor.accelUpdate();
    if (result == 0) {
      aX = mySensor.accelX();
      aY = mySensor.accelY();
      aZ = mySensor.accelZ();
      //Serial.println( " aX:" + String(aX) + " aY:" + String(aY)+ " aZ:" + String(aZ));
    }else {
      Serial.println("Cannot get accel values" + String(result));
      return; 
    }
    accelXoffset += aX;
    accelYoffset += aY;
    accelZoffset += aZ;
  }
  accelXoffset /= num_calibration;
  accelYoffset /= num_calibration;
  accelZoffset /= num_calibration;
  accelCalibrated = 1; 
  Serial.println("accelXoffset: " + String(accelXoffset) + " accelYoffset: " + String(accelYoffset) + " accelZoffset: " + String(accelZoffset));
  
}

/*void calibrateMag() { 
  mX = 0;
  mY = 0;
  mZ = 0;
  for (int count = 0; count < num_calibration; count++){
    result = mySensor.magUpdate();
    if (result ==0){
      mX = mySensor.magX();
      mY = mySensor.magY();
      mZ = mySensor.magZ();
    } else{
      Serial.print("Magnetometer value unavailable" + String(result));
      return;
    } 
     magXoffset += mX;
     magYoffset += mY;
     magZoffset += mZ;
    }
  magXoffset /=num_calibration;
  magYoffset /=num_calibration;
  magZoffset /=num_calibration;
  
  Serial.println("magXoffset" + String(magXoffset) + "magYoffset" + String(magYoffset) + "magZoffset" + String(magZoffset));
}
*/

void getGyroReadings() { 
  gX = 0;
  gY = 0;
  gZ = 0;

  //Polling 
  result = mySensor.gyroUpdate();
  if (result ==0){
    gX = mySensor.gyroX() - gyroXoffset; 
    gY = mySensor.gyroY() - gyroYoffset; 
    gZ = mySensor.gyroZ() - gyroZoffset; 
  
    gX *= -pi/180;
    gY *= -pi/180;
    gZ *= -pi/180; 

    //Multiply by Gyro scale 
    gX *= gXscale; 
    gY *= gYscale; 
    gZ *= gZscale; 
}else { 
  Serial.println("Can't get value for gyroReadings" + String(result));
  return;
  }

  
}
void getAccelReadings() { 
  aX = 0;
  aY = 0;
  aZ = 0;

  result = mySensor.accelUpdate();
  if (result == 0){
    aX = mySensor.accelX(); - accelXoffset; 
    aY = mySensor.accelY(); - accelYoffset; 
    aZ = mySensor.accelZ(); - accelZoffset; 
  } else {
    Serial.println("Can't get value for accelReadings" + String(result));
    return;
  }
}
  
void getMagReadings() {
  mX = 0;
  mY = 0;
  mZ = 0;

  result = mySensor.magUpdate();
  if (result ==0) {
    mX = mySensor.magX() - magXoffset;
    mY = mySensor.magY() - magYoffset;
    mZ = mySensor.magZ() - magZoffset;
  } else {
    Serial.println("Can't get value for magReadings" + String(result));
    return;
  }
  
}


void setup(){
  Serial.begin(115200);
  setupSensors();
 
}

void loop() {
  getGyroReadings();
  getAccelReadings();
  Serial.print("gX: " + String(gX) + "gY: " + String(gY) + "gZ: " + String(gZ));
  Serial.print(" | ");
  Serial.print("aX: " + String(aX) + "aY: " + String(aY) + "aZ: " + String(aZ));
  Serial.print(" | ");
  

  //Integrating gyro around z-axis:
  double dt = 0.1;
  double dth_gyro = dt * gZ; 

  theta_gyro += dth_gyro;
  Serial.println("theta_gyro: " + String(theta_gyro));


  delay(100);
  
  
  
}

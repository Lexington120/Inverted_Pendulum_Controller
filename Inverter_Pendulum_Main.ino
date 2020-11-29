#include <MagAlpha.h>

//Check https://www.arduino.cc/en/reference/SPI for SPI signals connections

#define UART_BAUDRATE       115200        //UART data rate in bits per second (baud)
#define SPI_SCLK_FREQUENCY  10000000      //SPI SCLK Clock frequency in Hz
#define SPI_CS_PIN_Pend     10            //SPI CS pin for pendulum arm
#define SPI_CS_PIN_Motor    3             //SPI CS pin for motor

double pAngle;
double pError;
double pOffset;

double mAngle;
double mDisplacement = 0;
double mOffset;
double lastAngle;
boolean lastDirection;

int count = 0;

MagAlpha pendulumSensor;
MagAlpha motorSensor;

void setup() {
  //Set the SPI SCLK frequency, SPI Mode and CS pin
  pendulumSensor.begin(SPI_SCLK_FREQUENCY, MA_SPI_MODE_3, SPI_CS_PIN_Pend);
  motorSensor.begin(SPI_SCLK_FREQUENCY, MA_SPI_MODE_3, SPI_CS_PIN_Motor);

  //Set the Serial Communication used to report the angle
  Serial.begin(UART_BAUDRATE);

  delay(1000);

  //Set pendulum and motor offsets
  pOffset = pendulumSensor.readAngle();
  mOffset = motorSensor.readAngle();
}

void loop() {
  //Read pendulum angle
  pAngle = pendulumSensor.readAngle() - pOffset;
  //Update pendulum error
  if (pAngle > 180.0) {
    pError = pAngle - 360.0;
  } else {
    pError = pAngle;
  }

  //Read motor angle
  mAngle = motorSensor.readAngle();
  if (count == 0) {
    updateDisplacement(mAngle, mOffset);
  } else {
    updateDisplacement(mAngle, lastAngle);
  }

  if ((count % 1000) == 0) {
    Serial.print("The pendulum error is ");
    Serial.println(pError, 3);
    Serial.println();

    Serial.print("The motor angle is ");
    Serial.println(mAngle, 3);
    Serial.print("The motor displacement is ");
    Serial.println(mDisplacement, 3);
    Serial.println();
    if (lastDirection) {
      Serial.println("->->->->->");
    } else {
      Serial.println("<-<-<-<-<-");
    }

    Serial.print("Count: ");
    Serial.println(count);
    Serial.println("--------------------------------------------------------");
  }

  lastAngle = mAngle;
  count++;
}

void updateDisplacement(double current, double last) {
  double change;
  if (current < 5.0 && last > 355.0) {
    change = current + 360 - last;
    Serial.println("++++++++");
  } else if (current > 355.0 && last < 5.0) {
    change = current - 360 - last;
    Serial.println("--------");
  } else {
    change = current - last;
  }
  if (change > 0.0) {
    lastDirection = true;
  } else {
    lastDirection = false;
  }
//  Serial.println(change);
  mDisplacement = mDisplacement + change;
}

void updatePositionArray(double

#include <Wire.h>
#include <MPU6050.h>
#include <KalmanFilter.h>
#include "GyverMotor.h"
#include "GyverPID.h"

#define PARSE_AMOUNT 3

float K[PARSE_AMOUNT];
boolean recievedFlag;
boolean getStarted;
byte index;
String string_convert = "";
float filtred_U = 0;
float accPitch = 0;
float accRoll = 0;
float kalPitch = 0;
float kalRoll = 0;
float val = 0;

MPU6050 mpu;
KalmanFilter kalmanX(0.001, 0.003, 0.03);
KalmanFilter kalmanY(0.001, 0.003, 0.03);
GMotor motor_1(DRIVER2WIRE_NO_INVERT, 4, 5, HIGH);
GMotor motor_2(DRIVER2WIRE_NO_INVERT, 7, 6, HIGH);

GyverPID regulator(20, 400, 0.8, 10); // Коэффиценты ПИД регулятора
const float Ko = 1;                   // Коэффицент фильтра на выход моторов
int SHIFT = 0;                        // Смещение ШИМ моторов

void setup()
{
  Serial.begin(115200);
  while (!mpu.begin(MPU6050_SCALE_2000DPS, MPU6050_RANGE_2G))
  {
    delay(500);
  }
  mpu.calibrateGyro();
  motor_1.setMode(FORWARD);
  motor_2.setMode(FORWARD);
  motor_1.setMinDuty(0);
  motor_2.setMinDuty(3);
  regulator.setDirection(REVERSE);                // Направление регулирования
  regulator.setLimits(-255 + SHIFT, 255 - SHIFT); // пределы ШИМ
  regulator.setpoint = 1;                         // Точка к которой стремится регулятор
  Serial.println("filtred_roll, U, filtred_U");
}

void loop()
{
  parsing(); // Функция парсинга, отправлять строку вида $5.1 1.2 2.3; (Kp Ki Kd)
  if (recievedFlag)
  {
    recievedFlag = false;
    regulator.Kp = K[0];
    regulator.Ki = K[1];
    regulator.Kd = K[2];
  }

  float filtred_roll = get_filtred_roll();
  regulator.input = filtred_roll;
  float U = regulator.getResultTimer();
  if (U > 0)
  {
    U = U + SHIFT;
  }
  else
  {
    U = U - SHIFT;
  }

  filtred_U = filtred_U * (1 - Ko) + U * Ko;

  motor_1.setSpeed(filtred_U);
  motor_2.setSpeed(filtred_U);
  //  Serial.print(filtred_roll);
  //  Serial.print(" ");
  //  Serial.print(U);
  //  Serial.print(" ");
  //  Serial.print(filtred_U);
  //  Serial.println(" ");
  //  Serial.print(regulator.Kp);
  //  Serial.print(" ");
  //  Serial.print(regulator.Ki);
  //  Serial.print(" ");
  //  Serial.println(regulator.Kd);
  delay(10);
}

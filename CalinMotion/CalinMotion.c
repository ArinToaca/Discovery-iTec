#include <Wire.h>

#include "CalinMotion.h"

typedef union accel_t_gyro_union
{
  struct
  {
    uint8_t x_accel_h;
    uint8_t x_accel_l;
    uint8_t y_accel_h;
    uint8_t y_accel_l;
    uint8_t z_accel_h;
    uint8_t z_accel_l;
    uint8_t t_h;
    uint8_t t_l;
    uint8_t x_gyro_h;
    uint8_t x_gyro_l;
    uint8_t y_gyro_h;
    uint8_t y_gyro_l;
    uint8_t z_gyro_h;
    uint8_t z_gyro_l;
  } reg;
  struct 
  {
    int16_t x_accel;
    int16_t y_accel;
    int16_t z_accel;
    int16_t temperature;
    int16_t x_gyro;
    int16_t y_gyro;
    int16_t z_gyro;
  } value;
};

int prev_x;
int prev_y;
int delay_rot;
int delay_rot2;

CalinMotion :: CalinMotion()
{
	int error;
	uint8_t c;


	// Initialize the 'Wire' class for the I2C-bus.
	Wire.begin();


	// default at power-up:
	//    Gyro at 250 degrees second
	//    Acceleration at 2g
	//    Clock source at internal 8MHz
	//    The device is in sleep mode.
	//

	error = MPU6050_read (MPU6050_WHO_AM_I, &c, 1);
	error = MPU6050_read (MPU6050_PWR_MGMT_1, &c, 1);



	// Clear the 'sleep' bit to start the sensor.
	MPU6050_write_reg (MPU6050_PWR_MGMT_1, 0);
}

int CalinMotion :: DoStuff()
{
	int error;
	double dT;
	accel_t_gyro_union accel_t_gyro;

	int stuff=0;
	
	//Serial.println(F(""));

	// Read the raw values.
	// Read 14 bytes at once, 
	// containing acceleration, temperature and gyro.
	// With the default settings of the MPU-6050,
	// there is no filter enabled, and the values
	// are not very stable.
	error = MPU6050_read (MPU6050_ACCEL_XOUT_H, (uint8_t *) &accel_t_gyro, sizeof(accel_t_gyro));


	// Swap all high and low bytes.
	// After this, the registers values are swapped, 
	// so the structure name like x_accel_l does no 
	// longer contain the lower byte.
	uint8_t swap;
	#define SWAP(x,y) swap = x; x = y; y = swap

	SWAP (accel_t_gyro.reg.x_accel_h, accel_t_gyro.reg.x_accel_l);
	SWAP (accel_t_gyro.reg.y_accel_h, accel_t_gyro.reg.y_accel_l);
	SWAP (accel_t_gyro.reg.z_accel_h, accel_t_gyro.reg.z_accel_l);
	SWAP (accel_t_gyro.reg.t_h, accel_t_gyro.reg.t_l);
	SWAP (accel_t_gyro.reg.x_gyro_h, accel_t_gyro.reg.x_gyro_l);
	SWAP (accel_t_gyro.reg.y_gyro_h, accel_t_gyro.reg.y_gyro_l);
	SWAP (accel_t_gyro.reg.z_gyro_h, accel_t_gyro.reg.z_gyro_l);

	if(accel_t_gyro.value.x_accel > 9000){
		stuff = 12;
		Serial.println("Dreapta");
	}
	if(accel_t_gyro.value.x_accel < -9000){
		stuff = 11;
		Serial.println("Stanga");
	}

	if(delay_rot != 0){
		delay_rot--;  
	}
	else
	{
		if(prev_x - accel_t_gyro.value.x_accel > 12000){
			stuff = 1;
			Serial.println("Rotire Stanga");
			delay_rot = ROT_VAL;
		}
		if(prev_x - accel_t_gyro.value.x_accel < -12000){
			stuff = 2;
			Serial.println("Rotire Dreapta");
			delay_rot = ROT_VAL;
		}
	}
	prev_x = accel_t_gyro.value.x_accel;


	if(delay_rot2 != 0){
		delay_rot2--;  
	}
	else{
		if(prev_y - accel_t_gyro.value.y_accel > 12000){
			stuff = 3;
			Serial.println("Rotire Sus");
			delay_rot2 = ROT_VAL;
		}
		if(prev_y - accel_t_gyro.value.y_accel < -12000){
			stuff = 4;
			Serial.println("Rotire Jos");
			delay_rot2 = ROT_VAL;
		}
	}
	//delay(500);
	prev_x = accel_t_gyro.value.x_accel;
	prev_y = accel_t_gyro.value.y_accel;
	
	return stuff;
}

int CalinMotion :: MPU6050_read(int start, uint8_t *buffer, int size)
{
  int i, n, error;

  Wire.beginTransmission(MPU6050_I2C_ADDRESS);
  n = Wire.write(start);
  if (n != 1)
    return (-10);

  n = Wire.endTransmission(false);    // hold the I2C-bus
  if (n != 0)
    return (n);

  // Third parameter is true: relase I2C-bus after data is read.
  Wire.requestFrom(MPU6050_I2C_ADDRESS, size, true);
  i = 0;
  while(Wire.available() && i<size)
  {
    buffer[i++]=Wire.read();
  }
  if ( i != size)
    return (-11);

  return (0);  // return : no error
}

int CalinMotion :: MPU6050_write(int start, const uint8_t *pData, int size)
{
  int n, error;

  Wire.beginTransmission(MPU6050_I2C_ADDRESS);
  n = Wire.write(start);        // write the start address
  if (n != 1)
    return (-20);

  n = Wire.write(pData, size);  // write data bytes
  if (n != size)
    return (-21);

  error = Wire.endTransmission(true); // release the I2C-bus
  if (error != 0)
    return (error);

  return (0);         // return : no error
}

int CalinMotion :: MPU6050_write_reg(int reg, uint8_t data)
{
  int error;

  error = MPU6050_write(reg, &data, 1);

  return (error);
}

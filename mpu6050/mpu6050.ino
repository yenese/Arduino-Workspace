#include <stdint.h>
#include "Wire.h"

//MPU6050 SLEEP MODE DEĞİŞTİRME VE X,Y,Z EKSENLERİNDEKİ İVME VERİLERİNİ OKUMA

#define MPU6050_ADDRESS 0x68
#define MPU6050_REG_WHOAMI 0x75
#define MPU6050_PWR_MGMT 0x6B
#define MPU_6050_ACCEL_XOUT_HIGH 0x3B
#define MPU_6050_ACCEL_XOUT_LOW 0x3C
#define MPU_6050_ACCEL_YOUT_HIGH 0x3D
#define MPU_6050_ACCEL_YOUT_LOW 0x3E
#define MPU_6050_ACCEL_ZOUT_HIGH 0x3F
#define MPU_6050_ACCEL_ZOUT_LOW 0x40


void read_sleep_mode();
void set_sleepmode_0();
void read_accelerometer();

uint8_t sleep = 0;
uint8_t accel_data[6];
uint16_t accel_x, accel_y, accel_z;
char set_slpmode;
bool f_situation = true;

void setup() {

  Wire.begin();
  set_sleepmode_0();
  
  Serial.begin(9600);
  Serial.println("Press f to change the sleep mode : ");

  read_accelerometer();
  
}

void loop() {
  
  // Kullanıcıdan Sleep Mode'u değiştirmek için input aldık.
  if(Serial.available()){
    set_slpmode = Serial.read();
    Serial.println((char)set_slpmode); 
  }

  // Her 'f' tuşuna basıldığında sleep mode değişiyor.
  if(set_slpmode == 'f'){
    if(f_situation == true){
      Wire.beginTransmission(MPU6050_ADDRESS);
      Wire.write(MPU6050_PWR_MGMT);
      Wire.write(0x40);  // sleep moduna alıyoruz. (01000000)
      Wire.endTransmission();

      Serial.println("Sleep mode on");
      f_situation = false;
    }
    else{
      Wire.beginTransmission(MPU6050_ADDRESS);
      Wire.write(MPU6050_PWR_MGMT);
      Wire.write(0x00);  // Sleep moddan çıkarıyoruz. (00000000)
      Wire.endTransmission();

      Serial.println("Sleep mode off");
      f_situation = true;
    }
  }

  if(f_situation == true){
  read_accelerometer();  // İvmeölçer verilerini oku
  read_sleep_mode();
  delay(2000);  // 2 saniye bekle
  }
  else{
    Serial.print("Sleep mode on sensor is not working ");
    Serial.print("");
    read_sleep_mode();
    delay(2000);
  }


}

// Fonksiyonlar


// Sleep mode'u off yapar.
void set_sleepmode_0(){
  Wire.beginTransmission(MPU6050_ADDRESS);
  Wire.write(MPU6050_PWR_MGMT);
  Wire.write(0x00);  // Sleep moddan çıkardık. (00000000)
  Wire.endTransmission();
}

// Sleep mode değerini okuyan fonksiyon
void read_sleep_mode() {
  Wire.beginTransmission(MPU6050_ADDRESS);
  Wire.write(MPU6050_PWR_MGMT); 
  Wire.endTransmission();
  Wire.requestFrom(MPU6050_ADDRESS, 1); 

  if (Wire.available()) { 
    sleep = Wire.read();
    Serial.println(sleep, BIN);
  } else {
    Serial.println("Error no data");
  }
}

// İvmeölçer verilerini okur.
void read_accelerometer(){
  Wire.beginTransmission(MPU6050_ADDRESS);
  Wire.write(MPU_6050_ACCEL_XOUT_HIGH);  // X ekseni yüksek byte
  Wire.endTransmission();

  Wire.requestFrom(MPU6050_ADDRESS, 6);  // X, Y, Z eksenleri için toplamda 6 byte veri talep ediyoruz
  accel_data[0] = Wire.read();  // X yüksek byte
  accel_data[1] = Wire.read();  // X düşük byte

  accel_data[2] = Wire.read();  // Y yüksek byte
  accel_data[3] = Wire.read();  // Y düşük byte

  accel_data[4] = Wire.read();  // Z yüksek byte
  accel_data[5] = Wire.read();  // Z düşük byte

  // Veriyi 16 bit formatında birleştiriyoruz.
  accel_x = (accel_data[0] << 8) | accel_data[1];
  accel_y = (accel_data[2] << 8) | accel_data[3];
  accel_z = (accel_data[4] << 8) | accel_data[5];

  float accel_x_g = accel_x / 16384.0;
  float accel_y_g = accel_y / 16384.0;
  float accel_z_g = accel_z / 16384.0;

  // İvme verilerini yazdırıyoruz
  Serial.print("X: ");
  Serial.print(accel_x_g);
  Serial.print(" ");
  Serial.print("Y: ");
  Serial.print(accel_y_g);
  Serial.print(" ");
  Serial.print("Z: ");
  Serial.print(accel_z_g);
  Serial.println(" ");
}

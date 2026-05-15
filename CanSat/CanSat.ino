#include <SPI.h>
#include <LoRa.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include "MPU9250.h"

// ---------- LoRa pins ----------
#define LORA_SCK    18
#define LORA_MISO   19
#define LORA_MOSI   23
#define LORA_NSS    5
#define LORA_RST    14
#define LORA_DIO0   2

// ---------- I2C pins ----------
#define I2C_SDA     21   // GPIO21
#define I2C_SCL     22   // GPIO22

#define LORA_ID  "My group"
#define SEALEVELPRESSURE_HPA (1013.25)

Adafruit_BME280 bme;
MPU9250 mpu;
int counter = 0;

void setup() {
  Serial.begin(115200);
  delay(2000);
  Serial.println("LoRa Transmitter - My group");

  // ---------- I2C sensors ----------
  Wire.begin(I2C_SDA, I2C_SCL);           // ✅ ระบุ SDA=21, SCL=22 อย่างชัดเจน

  // BME280 (address 0x76)
  if (!bme.begin(0x76, &Wire)) {
    Serial.println("BME280 ไม่เจอ! ตรวจสาย/address");
    while (1);
  }
  Serial.println("BME280 OK");

  // MPU9250 โหมด 6 แกน (address 0x69)
  MPU9250Setting setting;
  mpu.setup(0x69, setting);
  Serial.println("MPU9250 calibrating... วางให้นิ่ง");
  delay(1000);
  mpu.calibrateAccelGyro();
  Serial.println("MPU9250 OK");

  // ---------- LoRa ----------
  SPI.begin(LORA_SCK, LORA_MISO, LORA_MOSI, LORA_NSS);
  LoRa.setPins(LORA_NSS, LORA_RST, LORA_DIO0);
  if (!LoRa.begin(433E6)) {
    Serial.println("LoRa init FAILED!");
    while (1);
  }
  LoRa.setTxPower(20);
  LoRa.setSpreadingFactor(9);
  LoRa.setSignalBandwidth(125E3);
  LoRa.setCodingRate4(8);
  LoRa.setSyncWord(0xF3);
  Serial.println("LoRa init OK - เริ่มส่งข้อมูล");
}

void loop() {
  // ---------- อ่าน MPU9250 ----------
  mpu.update_accel_gyro();
  float ax = mpu.getAccX();
  float ay = mpu.getAccY();
  float az = mpu.getAccZ();
  float gx = mpu.getGyroX();
  float gy = mpu.getGyroY();
  float gz = mpu.getGyroZ();

  // ---------- อ่าน BME280 ----------
  float temp     = bme.readTemperature();
  float pressure = bme.readPressure() / 100.0F;
  float humidity = bme.readHumidity();
  float altitude = bme.readAltitude(SEALEVELPRESSURE_HPA);

  // ---------- สร้าง packet CSV ----------
  String packet = String(LORA_ID) + "," +
                  String(ax, 2) + "," + String(ay, 2) + "," + String(az, 2) + "," +
                  String(gx, 2) + "," + String(gy, 2) + "," + String(gz, 2) + "," +
                  String(temp, 2) + "," + String(pressure, 2) + "," +
                  String(humidity, 2) + "," + String(altitude, 2) + "," +
                  String(counter);

  // ---------- ส่งออก LoRa ----------
  LoRa.beginPacket();
  LoRa.print(packet);
  LoRa.endPacket();

  Serial.println("TX: " + packet);
  counter++;
  delay(500);
}
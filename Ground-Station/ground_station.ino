#include <SPI.h>
#include <LoRa.h>

// ---------- LoRa pins ----------
#define LORA_SCK    5
#define LORA_MISO   17
#define LORA_MOSI   27
#define LORA_NSS    18
#define LORA_RST    14
#define LORA_DIO0   26

void setup() {
  Serial.begin(115200);
  delay(2000);
  Serial.println("LoRa Receiver - Ground Station");

  SPI.begin(LORA_SCK, LORA_MISO, LORA_MOSI, LORA_NSS);
  LoRa.setPins(LORA_NSS, LORA_RST, LORA_DIO0);
  if (!LoRa.begin(433E6)) {
    Serial.println("LoRa init FAILED!");
    while (1);
  }
  // ต้องตรงกับ Transmitter เป๊ะ
  LoRa.setSpreadingFactor(7);
  LoRa.setSignalBandwidth(125E3);
  LoRa.setCodingRate4(5);
  LoRa.setSyncWord(0xF3);
  Serial.println("LoRa init OK - รอรับข้อมูล");
}

void loop() {
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    String data = "";
    while (LoRa.available()) {
      data += (char)LoRa.read();
    }
    int rssi = LoRa.packetRssi();
    float snr = LoRa.packetSnr();

    // ส่งออก Serial บรรทัดเดียว ต่อท้ายด้วย rssi,snr
    // รูปแบบสุดท้าย: ID,ax,ay,az,gx,gy,gz,temp,pressure,humidity,altitude,counter,rssi,snr
    Serial.print(data);
    Serial.print(",");
    Serial.print(rssi);
    Serial.print(",");
    Serial.println(snr);
  }
}
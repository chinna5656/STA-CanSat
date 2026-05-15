void loop() {
    mpu.update_accel_gyro(); // อัปเดตค่า IMU ตลอดเวลาเพื่อให้ได้ข้อมูลที่สดใหม่

    static uint32_t last_send = 0;
    uint32_t interval = 2000; // ตั้งเวลาส่งข้อมูลทุก 2 วินาที (ปรับได้)

    if (millis() - last_send > interval) {
        last_send = millis();

        // --- ส่วนการแสดงผล ---
        Serial.print("[IMU] AccZ: "); Serial.print(mpu.getAccZ(), 2);
        Serial.print(" | GyrZ: "); Serial.print(mpu.getGyroZ(), 1);

        if (bme_found) {
            float t = bme.readTemperature();
            float h = bme.readHumidity();
            float p = bme.readPressure() / 100.0F;

            Serial.print(" || [ENV] Temp: "); Serial.print(t, 1);
            Serial.print("C | Hum: "); Serial.print(h, 0);
            Serial.print("% | Pres: "); Serial.print(p, 1);
            Serial.println(" hPa");
        } else {
            Serial.println(" || [ENV] BME280 Sensor Missing!");
        }
    }
}
#include <DHT.h>
#include <Wire.h>
#include <BH1750.h>
#include <SPI.h>
#include <SD.h>
#include "BluetoothSerial.h"

BluetoothSerial SerialBT;

// === DHT11 ===
#define DHTPIN 33
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

// === BH1750 ===
BH1750 lightMeter;

// === Soil Moisture (Analog) ===
const int soilMoisturePin = 36; // GPIO36 (A0)

// === SD Card ===
#define SD_CS 5
File dataFile;

void setup() {
  Serial.begin(115200);
  Serial.println("=== ESP32 Bangun dari Deep Sleep ===");

  // Bluetooth
  if (!SerialBT.begin("ESP32_Sensor_Test")) {
    Serial.println("❌ Bluetooth gagal diinisialisasi");
  } else {
    Serial.println("✅ Bluetooth aktif");
  }

  // DHT11
  dht.begin();
  delay(2000); // beri waktu sensor stabil

  // BH1750
  Wire.begin(21, 22); // SDA=21, SCL=22
  if (!lightMeter.begin()) {
    Serial.println("❌ BH1750 tidak ditemukan!");
  } else {
    Serial.println("✅ BH1750 siap");
  }

  // SD Card
  if (!SD.begin(SD_CS)) {
    Serial.println("❌ Gagal inisialisasi SD card");
  } else {
    Serial.println("✅ SD card terdeteksi");
    if (!SD.exists("/logdata.csv")) {
      dataFile = SD.open("/logdata.csv", FILE_WRITE);
      if (dataFile) {
        dataFile.println("SoilMoisture,Temperature,Humidity,Lux");
        dataFile.close();
        Serial.println("✅ File logdata.csv dibuat");
      }
    }
  }

  // ==== Baca Sensor ====
  float soilRaw = analogRead(soilMoisturePin);
  float soilMoisture = (1.0 - (soilRaw / 4095.0)) * 100.0;
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();
  float lux = lightMeter.readLightLevel();

  // Validasi DHT
  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("❌ Gagal membaca DHT11, cek koneksi!");
    temperature = -999; // nilai dummy
    humidity = -999;
  }

  // ==== Kirim ke Serial & Bluetooth ====
  SerialBT.print("Soil: "); SerialBT.print(soilMoisture); SerialBT.print("% | ");
  SerialBT.print("Temp: "); SerialBT.print(temperature); SerialBT.print("°C | ");
  SerialBT.print("Humidity: "); SerialBT.print(humidity); SerialBT.print("% | ");
  SerialBT.print("Light: "); SerialBT.print(lux); SerialBT.println(" lx");

  Serial.print("Soil: "); Serial.print(soilMoisture); Serial.print("% | ");
  Serial.print("Temp: "); Serial.print(temperature); Serial.print("°C | ");
  Serial.print("Humidity: "); Serial.print(humidity); Serial.print("% | ");
  Serial.print("Light: "); Serial.print(lux); Serial.println(" lx");

  // ==== Simpan ke SD Card ====
  dataFile = SD.open("/logdata.csv", FILE_APPEND);
  if (dataFile) {
    dataFile.print(soilMoisture); dataFile.print(",");
    dataFile.print(temperature); dataFile.print(",");
    dataFile.print(humidity); dataFile.print(",");
    dataFile.println(lux);
    dataFile.close();
    Serial.println("✅ Data tersimpan ke SD card");
  } else {
    Serial.println("❌ Gagal membuka file logdata.csv untuk menulis");
  }

  // ==== Masuk Deep Sleep ====
  Serial.println("Masuk deep sleep selama 10 menit...");
  esp_sleep_enable_timer_wakeup(600e6); // 600 detik = 10 menit
  esp_deep_sleep_start();
}

void loop() {
  // Tidak dipakai, ESP32 akan restart ke setup() setelah deep sleep
}

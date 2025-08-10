#include <DHT.h>
#include <Wire.h>
#include <BH1750.h>
#include <SPI.h>
#include <SD.h>
#include "BluetoothSerial.h"

BluetoothSerial SerialBT;

#define DHTPIN 33
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

BH1750 lightMeter;

const int soilMoisturePin = 36; // A0

#define SD_CS 5  // CS pin for SD card

File dataFile;

void setup() {
  Serial.begin(115200);
  delay(2000);

  Serial.println("Inisialisasi Bluetooth...");


  if (!SerialBT.begin("ESP32_Sensor_Test")) { 
    Serial.println("Bluetooth gagal diinisialisasi!");
    while (true){
      delay(1000);
    };
  }

  Serial.println("Bluetooth aktif: ESP32_Sensor_Test");

  // Inisialisasi DHT
  dht.begin();

  // Inisialisasi BH1750
  Wire.begin(21, 22); // SDA, SCL
  if (!lightMeter.begin()) {
    SerialBT.println("BH1750 not found");
    while (1);
  }

  // Inisialisasi SD card
  if (!SD.begin(SD_CS)) {
    SerialBT.println("Gagal memulai SD card!");
    return;
  }
  SerialBT.println("SD card berhasil diinisialisasi.");

  // Buat file jika belum ada
  if (!SD.exists("/logdata.txt")) {
    dataFile = SD.open("/logdata.txt", FILE_WRITE);
    if (dataFile) {
      dataFile.println("SoilMoisture,Temperature,Humidity,Lux");
      dataFile.close();
    }
  }
}

void loop() {
  float soilValue = analogRead(soilMoisturePin); 
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();
  float lux = lightMeter.readLightLevel();

  //Fungsi map
  float soilMoistureVal = (1.0 - (soilValue/4095.0)) * 100.0;
  
  // Debug ke Serial Monitor
  SerialBT.print("Soil: "); SerialBT.print(soilMoistureVal);
  SerialBT.print(" | Temp: "); SerialBT.print(temperature);
  SerialBT.print("°C | Humidity: "); SerialBT.print(humidity);
  SerialBT.print("% | Light: "); SerialBT.print(lux); SerialBT.println(" lx");

  // Simpan ke SD card
  dataFile = SD.open("/logdata.txt", FILE_APPEND);
  if (dataFile) {
    dataFile.print(soilMoistureVal); dataFile.print(",");
    dataFile.print(temperature); dataFile.print(",");
    dataFile.print(humidity); dataFile.print(",");
    dataFile.println(lux);
    dataFile.close();
  } else {
    SerialBT.println("Gagal membuka file log untuk menulis.");
  }

  delay(5000); // Delay 5 detik
}

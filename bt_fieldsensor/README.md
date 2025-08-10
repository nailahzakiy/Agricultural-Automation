## 🌱 Field Sensor Integration with Bluetooth

The integrated module measures light intensity, soil moisture, air humidity, and air temperature. Serial readings are performed via Bluetooth serial on a device connected to the microcontroller. Data is logged to an SD card.

# 🎒 Module used
- BH1750 (for light intensity)
- Soil Moisture Sensor Module
- DHT 11 (for air humidity and temperature)
- SD Card Module

Board = ESP32 Wemos (but you can also reuse this code with another board)

# 🎁 Library Requirement
- DHT
- Wire
- BH1750
- SPI
- SD
- BluetoothSerial

# BLE_Mesh
ESP32 Bluetooth Low Energy Mesh

# Create a BLUES mesh network!

Clone the __BLUES__ Repository first:

__In Linux Terminal__
```
    git clone https://github.com/sinku1196/BLE_Mesh.git
    cd BLUES-esp32/
```
Compile the project:

```
    idf.py build
```
Once the compilation process ends, you can upload the compiled code inside the ESP32 board.

```
    idf.py -p /dev/ttyUSBx flash
```
Replace _ttyUSBx_ with the device on which the ESP32 is connected.
Repeat the same process on all the required devices, then open the serial monitor of the connected ESP32 to display its routing table

```
    idf.py -p /dev/ttyUSBx monitor
```
Replace _ttyUSBx_ with the device on which the ESP32 is connected.

## Pre-requisites

- Two or more ESP32 boards
- Download the ESP-IDF Official Development framework [here](https://docs.espressif.com/projects/esp-idf/en/stable/get-started/index.html#step-2-get-esp-idf)

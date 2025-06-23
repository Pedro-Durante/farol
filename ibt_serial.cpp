#include <Arduino.h>
#include "ibt_serial.h"

BluetoothSerial SerialBT;
InterfaceBTSerial::InterfaceBTSerial()
{}

char InterfaceBTSerial::obterTeclas(){
    char read;
  if (SerialBT.available()>0){
    return (char)SerialBT.read();
  }
  return -1;
}
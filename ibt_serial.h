#ifndef IHM_SERIAL_H_INCLUDED
#define IHM_SERIAL_H_INCLUDED
#include "ibt.h"
#include "BluetoothSerial.h"

extern BluetoothSerial SerialBT;

class InterfaceBTSerial: public InterfaceBT{
    public:
    InterfaceBTSerial();
    char obterTeclas();
};
#endif
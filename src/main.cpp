#include <Arduino.h>
#include <midea_ir.h>

#include "BluetoothSerial.h"
const uint8_t MIDEA_RMT_CHANNEL = 1;
const uint8_t MIDEA_TX_PIN = 4;

MideaIR ir;

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

BluetoothSerial SerialBT;

char incomingChar;
String message = "";
void setup()
{
  Serial.begin(9600);
  // init library
  midea_ir_init(&ir, MIDEA_RMT_CHANNEL, MIDEA_TX_PIN);

  SerialBT.begin("May Tho");
}

void loop()
{
  if (SerialBT.available())
  {
 
    char incomingChar = SerialBT.read();
    if (incomingChar != '\n')
    {
      message += String(incomingChar);
    }
    else
    {
      message = "";
    }
    Serial.write(incomingChar);
  }
  if (message == "on")
  {
    Serial.println("Sending IR signal to turn on the A/C");
    // set mode, temperature and fan level and internal state to enabled
    ir.enabled = true;
    ir.mode = MODE_COOL;
    ir.fan_level = 3;
    ir.temperature = 23;
    midea_ir_send(&ir);
  }
  else if (message == "off")
  {
    Serial.println("Sending IR signal to turn off the A/C");
    // set state to disabled
    ir.enabled = false;

    // send the IR signal which will turn the A/C off
    midea_ir_send(&ir);
  }
}
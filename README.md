# Home Air Conditional controller via esp32
- Opps I lost my ac remote :D it's ok let's create one with esp32
# How it works
- A lot of household things (TV, AC, Speaker) is controlled by IR signal -> we can use esp32 with an IR transmitter module to control them
# I use
- esp32 dev kit v1 (30 pins)
- KY-005 IR transmitter module which can emit ir light in range of 10m
  ![image](https://github.com/user-attachments/assets/2b4367b7-2f20-4291-9b62-0508f350ff23)

# Code
- thanks to this repo: https://github.com/morcibacsi/esp32_rmt_midea_ir_tx things getting easier
- Setting ir transmitter and esp32_rmt_midea_ir_tx lib
```cpp
#include <midea_ir.h>

const uint8_t MIDEA_RMT_CHANNEL = 1;
const uint8_t MIDEA_TX_PIN = 4;

void setup()
{
  Serial.begin(9600);
  // init library
  midea_ir_init(&ir, MIDEA_RMT_CHANNEL, MIDEA_TX_PIN);
}
```
- for example to turn on AC with 23 degree celcilius with mode cool and fan level is 3
```cpp
    ir.enabled = true;
    ir.mode = MODE_COOL;
    ir.fan_level = 3;
    ir.temperature = 23;
    midea_ir_send(&ir);
```
- easy right ?
# toodo
- Complete logic code by implement functions to control the AC
- Remote interfaces: bluetooth, web app, telegram bot
- Some extra feature like: auto turn off when outside temperature is cool (so my roomate dont abuse my ac which increase my electricity's budget)
- Auto balance the ac temperature and outside temperature via and DHT22 sensor
- More base on my imaginations....

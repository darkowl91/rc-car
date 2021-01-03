// Board: ESP32 wroom 32
// https://dl.espressif.com/dl/package_esp32_index.json
// Upload speed 115200


#include <PS4Controller.h>

// PIN
const int pin_fwd = 32;
const int pin_bwd = 33;
const int pin_speed = 34;
const int pin_servo = 35;


void setup()
{
    Serial.begin(115200);
    // Connect to dualshock 4, using predifined mac with SixaxisPairTool
    PS4.begin("03:03:03:03:03:03");
}

void controller() {
    // move forward - R2
    if (PS4.data.button.r2)
    {
        Serial.print("r2 button at ");
        Serial.println(PS4.data.analog.button.r2, DEC);
    }

    // move backward -  L2
    if (PS4.data.button.l2) 
    {
        Serial.print("l2 button at ");
		Serial.println(PS4.data.analog.button.l2, DEC);
	}

    // controll direction - L3
    if (PS4.event.analog_move.stick.lx)
    {
        Serial.print("Left Stick x at ");
        Serial.println(PS4.data.analog.stick.lx, DEC);
    }

    if (PS4.event.analog_move.stick.ly)
    {
        Serial.print("Left Stick y at ");
        Serial.println(PS4.data.analog.stick.ly, DEC);
    }
}

void loop()
{
    if(!PS4.isConnected()) {
        Serial.println("Could not connect to Dualshock4");
        return;
    }
 
    // Print controller baterry status
    Serial.print("Battey Percent : ");
    Serial.println(PS4.data.status.battery, DEC);

    // start listen controller events
    PS4.attach(controller);
}
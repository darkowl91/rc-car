// Board: ESP32 wroom 32
// https://dl.espressif.com/dl/package_esp32_index.json
// Upload speed 115200

#include <PS4Controller.h>
#include <ESP32Servo.h>

// PIN
const int pin_fwd = 27;
const int pin_bwd = 26;
const int pin_speed = 14;
const int pin_servo = 33;

// l289n
const int freq = 30000;
const int pwmChannel = 0;
const int resolution = 8;

const int dutyCycleMin=140;
const int dutyCycleMax=250;


void setup()
{
    Serial.begin(115200);

    // connect to dualshock 4, using predifined mac with SixaxisPairTool
    if(!PS4.begin("03:03:03:03:03:03")){
        Serial.println("Could not begin PS4");
    }
    // attach controller callback
    PS4.attach(handleControllerEvent);

    // setup pins
    pinMode(pin_fwd, OUTPUT);
    digitalWrite(pin_fwd, LOW);
    
    pinMode(pin_bwd, OUTPUT);
    digitalWrite(pin_bwd, LOW);

    pinMode(pin_speed, OUTPUT);
    digitalWrite(pin_speed, LOW);

    ledcSetup(pwmChannel, freq, resolution);
    ledcAttachPin(pin_speed, pwmChannel);
}

void handleControllerEvent() {
    // move forward - R2
    if (PS4.data.button.r2)
    {
        uint8_t r2 = PS4.data.analog.button.r2;
        Serial.println(r2);

        int dutyCycle = map(r2, 15, 255, dutyCycleMin, dutyCycleMax);

        digitalWrite(pin_fwd, HIGH);
        digitalWrite(pin_bwd, LOW);

        ledcWrite(pwmChannel, dutyCycle);
    }

     // move backward -  L2
    if (PS4.data.button.l2)
    {
        uint8_t l2 = PS4.data.analog.button.l2;
        Serial.println(l2);

        int dutyCycle = map(l2, 15, 255, dutyCycleMin, dutyCycleMax);

        digitalWrite(pin_bwd, HIGH);
        digitalWrite(pin_fwd, LOW);

        ledcWrite(pwmChannel, dutyCycle);
    }

    // direction - L3
    // if (PS4.event.analog_move.stick.lx)
    // {
    //     Serial.print("Left Stick x at ");
    //     int8_t x = PS4.data.analog.stick.lx;
    //     servo.write(map(PS4.data.analog.stick.lx, 0, 255, 0, 180));
    // }
}




void loop()
{
    // test motor speed control
    // digitalWrite(pin_fwd, HIGH);
    // for(int dutyCycle = 0; dutyCycle <= 1000; dutyCycle++) {
    //     ledcWrite(pwmChannel, dutyCycle);
    //     delay(500);
    //     Serial.println(dutyCycle);
    // }
}
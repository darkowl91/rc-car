// Board: ESP32 wroom 32
// https://dl.espressif.com/dl/package_esp32_index.json
// Upload speed 115200

#include <PS4Controller.h>
#include <ESP32Servo.h>

// PIN
const int pin_fwd = 27;
const int pin_bwd = 26;
const int pin_speed = 14;
const int pin_servo = 23;

// l289n
const int freq = 30000;
const int pwmChannel = 0;
const int resolution = 8;

const int dutyCycleMin = 130;
const int dutyCycleMax = 255;

// servo
Servo servo;
int prev_angle = 0;
const int servoMin = 0;
const int servoMax = 180;

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

    servo.attach(pin_servo);
}

void handleControllerEvent() {
    // move forward - R2
    if (PS4.data.button.r2)
    {
        uint8_t r2 = normolizeAnalogData(PS4.data.analog.button.r2, 35);
        Serial.print("R2: ");
        Serial.println(r2);
        moveFroward(map(r2, 0, 255, dutyCycleMin, dutyCycleMax));
    }

     // move backward -  L2
    if (PS4.data.button.l2)
    {
        uint8_t l2 = normolizeAnalogData(PS4.data.analog.button.l2, 40);
        Serial.print("L2: ");
        Serial.println(l2);
        moveBackward(map(l2, 0, 255, dutyCycleMin, dutyCycleMax));
    }

    // stop - circle
    if (PS4.data.button.circle)
    {
        Serial.println("O");
        stop();
    }

    // direction - L3
    if (PS4.event.analog_move.stick.lx)
    {
        int8_t x = normolizeAnalogData(PS4.data.analog.stick.lx, 4);
        Serial.print("L3: ");
        Serial.println(x);
        rotate(map(PS4.data.analog.stick.lx, -128, 127, servoMin, servoMax));
    }
}

void moveFroward(int dutyCycle)
{
    digitalWrite(pin_bwd, LOW);
    digitalWrite(pin_fwd, HIGH);
    ledcWrite(pwmChannel, dutyCycle);
}

void moveBackward(int dutyCycle)
{
    digitalWrite(pin_fwd, LOW);
    digitalWrite(pin_bwd, HIGH);
    ledcWrite(pwmChannel, dutyCycle);
}

void stop()
{
    digitalWrite(pin_bwd, LOW);
    digitalWrite(pin_fwd, LOW);
    ledcWrite(pwmChannel, 0);
}

void rotate(int angle) 
{
    if (prev_angle != angle)
    {
        prev_angle = angle;
        servo.write(angle);
    }
}

int normolizeAnalogData(uint8_t data, uint8_t deadzone) {
    if (abs(data) < deadzone)
    {
        return 0;
    }
    return data;
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
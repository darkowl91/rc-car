// Board: ESP32 wroom 32
// https://dl.espressif.com/dl/package_esp32_index.json
// Upload speed 115200

#include <PS4Controller.h>

// PIN
const int pin_fwd = 27;
const int pin_bwd = 26;
const int pin_speed = 14;
const int pin_servo = 23;

// l289n
const int motor_pwmChannel = 1;
const int motor_res = 8;
const int motor_freq = 30000;
const int motor_dutyCycleMin = 15;
const int motor_dutyCycleMax = 255;

// servo
int prev_angle = 0;
const int servo_pwmChannel = 2;
const int servo_res = 16;
const int servo_freq = 50;
const int servo_dutyCycleMin = 3000;
const int servo_dutyCycleMax = 5500;


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

    ledcSetup(motor_pwmChannel, motor_freq, motor_res);
    ledcAttachPin(pin_speed, motor_pwmChannel);

    ledcSetup(servo_pwmChannel, servo_freq, servo_res);
    ledcAttachPin(pin_servo, servo_pwmChannel);
}

void handleControllerEvent() {
    // move forward - R2
    if (PS4.data.button.r2)
    {
        uint8_t r2 = normolizeAnalogData(PS4.data.analog.button.r2, 35);
       // Serial.print("R2: ");
        //Serial.println(r2);
        moveFroward(map(r2, 0, 255, motor_dutyCycleMin, motor_dutyCycleMax));
    }

     // move backward -  L2
    if (PS4.data.button.l2)
    {
        uint8_t l2 = normolizeAnalogData(PS4.data.analog.button.l2, 40);
       // Serial.print("L2: ");
       // Serial.println(l2);
        moveBackward(map(l2, 0, 255, motor_dutyCycleMin, motor_dutyCycleMax));
    }

    // stop - circle
    if (PS4.data.button.circle)
    {
       // Serial.println("O");
        stop();
    }

    // direction - L3
    if (PS4.event.analog_move.stick.lx && abs(PS4.data.analog.stick.lx) > 10)
    {
        // int x = normolizeAnalogData(PS4.data.analog.stick.lx, 10);
        // Serial.print("L3: ");
        // Serial.println(x);
        int d = map(PS4.data.analog.stick.lx, -128, 127, servo_dutyCycleMin, servo_dutyCycleMax);
        Serial.println(d);
        rotate(d);
    }
}

void moveFroward(int dutyCycle)
{
    digitalWrite(pin_bwd, LOW);
    digitalWrite(pin_fwd, HIGH);
    ledcWrite(motor_pwmChannel, dutyCycle);
}

void moveBackward(int dutyCycle)
{
    digitalWrite(pin_fwd, LOW);
    digitalWrite(pin_bwd, HIGH);
    ledcWrite(motor_pwmChannel, dutyCycle);
}

void stop()
{
    digitalWrite(pin_bwd, LOW);
    digitalWrite(pin_fwd, LOW);
    ledcWrite(motor_pwmChannel, 0);
    ledcWrite(servo_pwmChannel, 0);
}

void rotate(int dutyCycle) 
{
    if (prev_angle != dutyCycle)
    {
        prev_angle = dutyCycle;
        ledcWrite(servo_pwmChannel, dutyCycle);
        delay(2);
    }
}

int normolizeAnalogData(int data, int deadzone)
{
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

    // test servo angle control
    // for(int dutyCycle = servo_dutyCycleMin; dutyCycle <= servo_dutyCycleMax; dutyCycle=dutyCycle+10) {
    //     rotate(dutyCycle);
    //     delay(10);
    //     Serial.println(dutyCycle);
    // }

}
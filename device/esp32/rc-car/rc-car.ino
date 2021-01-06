// Board: ESP32 wroom 32
// https://dl.espressif.com/dl/package_esp32_index.json
// Upload speed 115200

#include <PS4Controller.h>

// controller mac could be configured with SixaxisPairTool
#define PS4_CONTROLLER_MAC "03:03:03:03:03:03"

// PIN
const int PIN_FWD   = 27;
const int PIN_BWD   = 26;
const int PIN_EN    = 14;
const int PIN_SERVO = 23;
const int PIN_LED   = 5;

// l289n
const int MOTOR_PWM_CH  = 1;
const int MOTOR_RES     = 8;
const int MOTOR_FREQ    = 40000;
const int MOTOR_MIN     = 100;
const int MOTOR_MAX     = 255;

// servo
const int SERVO_PWM_CH  = 2;
const int SERVO_RES     = 16;
const int SERVO_FREQ    = 50;
const int SERVO_MIN     = 3000;
const int SERVO_MAX     = 5500;

typedef enum
{
    FORWARD = PIN_FWD,
    BACKWARD = PIN_BWD
} Direction;

int prevRotateValue = 0;

void setup()
{
    Serial.begin(115200);

    // connect to dualshock 4
    if (PS4.begin(PS4_CONTROLLER_MAC))
    {
        // attach ps cpntroller callbacks
        PS4.attachOnConnect(handleConnect);
        PS4.attachOnDisconnect(handleDisconnect);
        PS4.attach(handleControllerEvents);
    }
    else
    {
        Serial.println("Could not begin PS4!");
    }

    // setup pins
    pinMode(PIN_FWD, OUTPUT);
    pinMode(PIN_BWD, OUTPUT);
    pinMode(PIN_SERVO, OUTPUT);
    pinMode(PIN_EN, OUTPUT);
    pinMode(PIN_LED, OUTPUT);
    digitalWrite(PIN_LED, !digitalRead(PIN_LED));

    // setup PWM using default ledc
    ledcSetup(MOTOR_PWM_CH, MOTOR_FREQ, MOTOR_RES);
    ledcAttachPin(PIN_EN, MOTOR_PWM_CH);
    ledcSetup(SERVO_PWM_CH, SERVO_FREQ, SERVO_RES);
    ledcAttachPin(PIN_SERVO, SERVO_PWM_CH);
}

void handleConnect()
{
    // Serial.println("Controller connected");
    // reset outputs
    digitalWrite(PIN_FWD, LOW);
    digitalWrite(PIN_BWD, LOW);
    digitalWrite(PIN_SERVO, LOW);
    digitalWrite(PIN_EN, LOW);
    ledcWrite(SERVO_PWM_CH, 0);
    ledcWrite(MOTOR_PWM_CH, 0);
    // notify about sucsess connection with builti in led
    digitalWrite(PIN_LED, !digitalRead(PIN_LED));
    // set ps4 controller led to blue
    PS4.setLed(0, 0, 255);
    PS4.sendToController();
}

void handleDisconnect()
{
    // Serial.println("Controller disconnected");
    // reset outputs
    digitalWrite(PIN_FWD, LOW);
    digitalWrite(PIN_BWD, LOW);
    digitalWrite(PIN_SERVO, LOW);
    digitalWrite(PIN_EN, LOW);
    ledcWrite(SERVO_PWM_CH, 0);
    ledcWrite(MOTOR_PWM_CH, 0);

    // notify about disconnect with built in led
    digitalWrite(PIN_LED, !digitalRead(PIN_LED));
}

const uint8_t CONTROLLER_OFFSET_R2  = 30;
const uint8_t CONTROLLER_OFFSET_L2  = 40;
const int8_t CONTROLLER_OFFSET_L3   = 15;

void handleControllerEvents()
{
    // move forward
    if (PS4.data.button.r2 && (PS4.data.analog.button.r2 > CONTROLLER_OFFSET_R2))
    {
        move(PS4.data.analog.button.r2, CONTROLLER_OFFSET_R2, 255, Direction::FORWARD);
    }

    // move backward
    if (PS4.data.button.l2 && (PS4.data.analog.button.l2 > CONTROLLER_OFFSET_L2))
    {
        move(PS4.data.analog.button.l2, CONTROLLER_OFFSET_L2, 255, Direction::BACKWARD);
    }

    // steering
    if (PS4.event.analog_move.stick.lx && abs(PS4.data.analog.stick.lx) > CONTROLLER_OFFSET_L3)
    {
        rotate(PS4.data.analog.stick.lx, -127, 128);
    }

    // stop, reset all pins to LOW
    if (PS4.data.button.circle && PS4.event.button_down.circle)
    {
        stop();
    }
}

void move(int value, int min, int max, Direction direction)
{
    if (value == min)
    {
        digitalWrite(direction, LOW);
        ledcWrite(MOTOR_PWM_CH, 0);
        return;
    }

    int duty = map(value, min, max, MOTOR_MIN, MOTOR_MAX);
    // Serial.print(direction == Direction::FORWARD ? "foward: " : "backward: ");
    // Serial.println(duty);

    digitalWrite((direction == Direction::FORWARD ? Direction::BACKWARD : Direction::FORWARD), LOW);
    digitalWrite(direction, HIGH);
    ledcWrite(MOTOR_PWM_CH, duty);
}

void rotate(int value, int min, int max)
{
    if (prevRotateValue != value)
    {
        int duty = map(value, min, max, SERVO_MAX, SERVO_MIN);
        // Serial.print("rotate: ");
        // Serial.println(duty);
        ledcWrite(SERVO_PWM_CH, duty);
        delay(2);
    }
}

void stop() {
    // Serial.println("Stop.");

    ledcWrite(MOTOR_PWM_CH, 0);
    ledcWrite(SERVO_PWM_CH, 0);
    digitalWrite(PIN_FWD, LOW);
    digitalWrite(PIN_BWD, LOW);
    digitalWrite(PIN_EN, LOW);
    digitalWrite(PIN_SERVO, LOW);
}

void loop()
{
    // empty, cancel RTOS Task
    vTaskDelete(NULL);
}
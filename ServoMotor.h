#pragma once
#include <ESP32Servo.h>
class ServoMotor{
    private:
    Servo servo;
    byte servoPin;
    public:
    ServoMotor(byte pin) : servoPin(pin) {
        servo.attach(servoPin);
    }
    void setAngle(int angle) {
        if (angle >= 0 && angle <= 180)
            servo.write(angle);
        else
            Serial.println("Angle out of range");
    }
    void open() {
        setAngle(0); 
    }
    void close() {
        setAngle(180); 
    }
};

#ifndef HBRIDGE_H_
#define HBRIDGE_H_

#include "Modulos/02-GPIO/GPIO.h"
#include "Modulos/PWM/pwm.h"

class HBridge
{
public:
    enum state_t { COAST, FORWARD, REVERSE, BRAKE };

    // El PWM y los Gpio se crean afuera y se pasan por referencia
    HBridge(Gpio& in1, Gpio& in2, SctPwm& pwm);

    void init();                        // arranca en COAST
    void forward(float speedPercent);   // 0..100
    void reverse(float speedPercent);   // 0..100
    void setSpeed(float percent);       // -100..100 (el signo da el sentido)
    void brake();                       // IN1 = IN2 = 1
    void coast();                       // IN1 = IN2 = 0, motor libre

    state_t getState() const { return state_; }
    float   getSpeed() const { return speed_; }

private:
    Gpio&   in1_;
    Gpio&   in2_;
    SctPwm& pwm_;
    state_t state_ = COAST;
    float   speed_ = 0.0f;

    void apply(state_t st, float speedPercent);
};

#endif

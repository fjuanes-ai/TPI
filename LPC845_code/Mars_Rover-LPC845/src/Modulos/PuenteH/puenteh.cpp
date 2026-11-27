#include "Modulos/PuenteH/puenteh.h"

HBridge::HBridge(Gpio& in1, Gpio& in2, SctPwm& pwm)
    : in1_(in1), in2_(in2), pwm_(pwm)
{
}

void HBridge::init()
{
    in1_.SetDirOutputs();
    in2_.SetDirOutputs();
    coast();
}

void HBridge::apply(state_t st, float speed)
{
    // Los extremos del PWM no son confiables (0 % y 100 % hacen coincidir
    // SET y CLR), así que se limita el rango útil.
    if (speed < 1.0f)  speed = 1.0f;
    if (speed > 99.0f) speed = 99.0f;

    // Si cambia el sentido, primero se apagan los dos pines para
    // no cruzar los transistores del puente
    if (st != state_)
    {
        in1_.ClrPin();
        in2_.ClrPin();
    }

    switch (st)
    {
        case FORWARD:
            in2_.ClrPin();
            in1_.SetPin();
            pwm_.setDutyCycle(speed);
            break;

        case REVERSE:
            in1_.ClrPin();
            in2_.SetPin();
            pwm_.setDutyCycle(speed);
            break;

        case BRAKE:
            in1_.SetPin();
            in2_.SetPin();
            pwm_.setDutyCycle(99.0f);   // EN alto para que frene
            break;

        case COAST:
        default:
            in1_.ClrPin();
            in2_.ClrPin();
            break;
    }

    state_ = st;
    speed_ = (st == FORWARD || st == REVERSE) ? speed : 0.0f;
}

void HBridge::forward(float s) { apply(FORWARD, s); }
void HBridge::reverse(float s) { apply(REVERSE, s); }
void HBridge::brake()          { apply(BRAKE, 99.0f); }
void HBridge::coast()          { apply(COAST, 0.0f); }

void HBridge::setSpeed(float percent)
{
    if (percent > 0.0f)       forward(percent);
    else if (percent < 0.0f)  reverse(-percent);
    else                      coast();
}

#include "pwm.h"

// En un proyecto real de LPC845, se incluye la cabecera CMSIS oficial
// (ej. LPC845.h o fsl_sctimer.h)

SctPwm::SctPwm(uint32_t systemClockHz,uint8_t pin_movible , uint8_t port , uint8_t pin): systemClock_(systemClockHz),frequency_(1000U),dutyPercent_(50.0f),pinMovible_(pin_movible),
port_(port),pin_(pin)
{

 instancia_ = this;
}

void SctPwm::init()
{
    // Habilitar el reloj del SCTimer
    SYSCON->SYSAHBCLKCTRL0 |= (1U << 8);

    // Quitar el reset del SCTimer
    SYSCON->PRESETCTRL0 |= (1U << 8);

    // Detener los contadores durante la configuración inicial
    LPC_SCT->CTRL |= (1U << 2) | (1U << 18);

    // Configurar el SCTimer en modo dividido: L y H
    LPC_SCT->CONFIG &= ~(1U << 0);

    NVIC->ISER[0] |=  (0x01 << 9);
}

void SctPwm::Config(
    SctCounter counter,
    uint8_t pinMovible,
    uint8_t port,
    uint8_t pin,
	uint32_t frequencyHz)
{

    // Guardar la configuración
    counter_ = counter;
    pinMovible_ = pinMovible;
    port_ = port;
    pin_ = pin;


    frequency_ = frequencyHz;
    dutyPercent_ = 50.0f;

    PINASSIGN_Config(pinMovible_, port_, pin_);


    if (counter_ == SctCounter::L)
    {
        LPC_SCT->CONFIG |= (1U << 17);  // AUTOLIMIT_L
    }
    else
    {
        LPC_SCT->CONFIG |= (1U << 18);  // AUTOLIMIT_H
    }

    // Detener el contador SCTimer (bits HALT_L y STOP_L en CTRL)
    if (counter_ == SctCounter::L)
       {
           LPC_SCT->CTRL |= (1U << 2);  // HALT_L
           // Contador ascendente (BIDIR_L = 0)
           LPC_SCT->CTRL &= ~(1U << 4);
       }
       else
       {
           LPC_SCT->CTRL |= (1U << 18); // HALT_H
           // Contador ascendente (BIDIR_L = 0)
           LPC_SCT->CTRL &= ~(1U << 20);
       }

    // Modo Match para todos los registros (REGMODE = 0)
    LPC_SCT->REGMODE = 0U;

    // Asignar valores de período y ciclo de trabajo
    updatePeriodAndDuty();

    // Configurar Evento 0 (EV0):
    // Coincidencia con MATCH0 en Estado 0

    uint32_t hEvent = (counter_ == SctCounter::H) ? (1U << 4) : 0U;

    LPC_SCT->EV[0].STATE = (1U << 0);
    LPC_SCT->EV[0].CTRL = (0U << 0)
                     | (1U << 12)
                     | hEvent;

    // 7. Configurar Evento 1 (EV1):
    // Coincidencia con MATCH1 en Estado 0
    LPC_SCT->EV[1].STATE = (1U << 0);
    LPC_SCT->EV[1].CTRL = (1U << 0)
                        | (1U << 12)
                        | hEvent;

    // 8. Evento 0 activa la salida (SET)
    // y Evento 1 la desactiva (CLR)
    LPC_SCT->OUT[1].SET = (1U << 0);
    LPC_SCT->OUT[1].CLR = (1U << 1);
}

void SctPwm::setFrequency(uint32_t frequencyHz)
{
    if (frequencyHz == 0U)
        return;

    frequency_ = frequencyHz;
    updatePeriodAndDuty();
}

void SctPwm::setDutyCycle(float dutyPercent)
{
    if (dutyPercent < 0.0f)
        dutyPercent = 0.0f;

    if (dutyPercent > 100.0f)
        dutyPercent = 100.0f;
    dutyPercent_ = dutyPercent;
    updatePeriodAndDuty();
}

void SctPwm::updatePeriodAndDuty()
{

	 if (frequency_ == 0U)
	    return;

	 uint32_t periodTicks = systemClock_ / frequency_;
    // El período debe caber en un contador de 16 bits
    if (periodTicks == 0U || periodTicks > 65536U)
        return;

    uint32_t match0 = periodTicks - 1U;

    uint32_t match1 = static_cast<uint32_t>(
        (static_cast<float>(periodTicks) * dutyPercent_) / 100.0f
    );

    // Evitar que MATCH1 supere el período
    if (match1 > match0)
        match1 = match0;


    for (uint32_t i = 0U; i < 2U; i++)
       {
           uint32_t oldValue = LPC_SCT->MATCHREL[i];
           uint32_t newValue;

           if (counter_ == SctCounter::L)
           {
               newValue = (oldValue & 0xFFFF0000U)
                        | ((i == 0U ? match0 : match1) & 0xFFFFU);
           }
           else
           {
               newValue = (oldValue & 0x0000FFFFU)
                        | (((i == 0U ? match0 : match1) & 0xFFFFU) << 16);
           }

           LPC_SCT->MATCHREL[i] = newValue;
       }
}

void SctPwm::start()
{

    if (counter_ == SctCounter::L)
       {
    	LPC_SCT->CTRL |= (1U << 3); // Limpiar contador L

    	 // Desactivar HALT_L y STOP_L
    	    LPC_SCT->CTRL &= ~((1U << 2) | (1U << 1));
       }
       else
       {
    	   // Limpiar contador H
    	   LPC_SCT->CTRL |= (1U << 19);
    	   // Desactivar HALT_H y STOP_H
    	   LPC_SCT->CTRL &= ~((1U << 17) | (1U << 18));
       }
}
void SctPwm::stop()
{

    if (counter_ == SctCounter::L)
       {
    		LPC_SCT->CTRL |= (1U << 1); // Activar STOP_L
       }
       else
       {
    	   LPC_SCT->CTRL |= (1U << 17); // Activar STOP_H
       }

}

void SctPwm::setCallback(Callback callback)
{
    callback_ = callback;

    handler();
}

void SctPwm::handler()
{
    if (callback_ != nullptr)
    {
        callback_();
    }
}

void SctPwm::callbackPWM()
{
    ciclosPWM_++;
}

uint32_t SctPwm::getCiclosPWM() const
{
    return ciclosPWM_;
}

SctPwm* SctPwm::instancia_ = nullptr;


void SctPwm::SCT_IRQHandler()
{
    if (instancia_ != nullptr)
    {
        instancia_->callbackPWM();
    }
}



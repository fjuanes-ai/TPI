#include "pwm.h"

// En un proyecto real de LPC845, se incluye la cabecera CMSIS oficial
// (ej. LPC845.h o fsl_sctimer.h)

SctPwm::SctPwm(uint32_t systemClockHz, uint8_t pin_movible, uint8_t port, uint8_t pin)
    : systemClock_(systemClockHz), frequency_(1000U), dutyPercent_(50.0f),
      pinMovible_(pin_movible), port_(port), pin_(pin)
{
    counter_   = SctCounter::L;
    tickClock_ = systemClockHz;   // sin prescaler
    periodUs_  = 1000U;           // 1 ms por defecto
    instancia_ = this;
}

void SctPwm::init()
{
    static bool iniciado = false;
    if (iniciado) return;
    iniciado = true;

    SYSCON->SYSAHBCLKCTRL0 |= (1U << 8);   // reloj del SCT
    SYSCON->PRESETCTRL0    |= (1U << 8);   // quitar reset

    LPC_SCT->CTRL   |= (1U << 2) | (1U << 18);   // HALT_L y HALT_H
    LPC_SCT->CONFIG &= ~(1U << 0);               // modo dividido (UNIFY = 0)

    NVIC->ISER[0] |= (0x01 << 9);
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
        LPC_SCT->CTRL |= (1U << 2);  // HALT_L
        // Contador ascendente (BIDIR_L = 0)
        LPC_SCT->CTRL &= ~(1U << 4);
    }
    else
    {
        LPC_SCT->CONFIG |= (1U << 18);  // AUTOLIMIT_H
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

    const uint8_t evSet = (counter_ == SctCounter::L) ? 0U : 2U;  // L: EV0/EV1, H: EV2/EV3
    const uint8_t evClr = evSet + 1U;
    const uint8_t out   = (counter_ == SctCounter::L) ? 1U : 2U;  // L: CTOUT_1, H: CTOUT_2

    LPC_SCT->EV[evSet].STATE = (1U << 0);
    LPC_SCT->EV[evSet].CTRL  = (0U << 0) | (1U << 12) | hEvent;   // MATCH0

    LPC_SCT->EV[evClr].STATE = (1U << 0);
    LPC_SCT->EV[evClr].CTRL  = (1U << 0) | (1U << 12) | hEvent;   // MATCH1

    LPC_SCT->OUT[out].SET = (1U << evSet);
    LPC_SCT->OUT[out].CLR = (1U << evClr);
}

void SctPwm::setFrequency(uint32_t frequencyHz)
{
    if (frequencyHz == 0U)
        return;

    setPeriodUs(1000000U / frequencyHz);
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

/*
void SctPwm::writeMatch(uint8_t idx, uint32_t value)
{
    uint32_t mask  = (counter_ == SctCounter::L) ? 0x0000FFFFU : 0xFFFF0000U;
    uint32_t shift = (counter_ == SctCounter::L) ? 0U : 16U;
    uint32_t v     = (value & 0xFFFFU) << shift;

    LPC_SCT->MATCHREL[idx] = (LPC_SCT->MATCHREL[idx] & ~mask) | v;

    // Si el contador está detenido, cargar también MATCH para que valga ya
    bool halted = (counter_ == SctCounter::L) ? (LPC_SCT->CTRL & (1U << 2))
                                              : (LPC_SCT->CTRL & (1U << 18));
    if (halted)
        LPC_SCT->MATCH[idx] = (LPC_SCT->MATCH[idx] & ~mask) | v;
}*/

void SctPwm::writeMatch(uint8_t idx, uint32_t value)
{
    const uint8_t half = (counter_ == SctCounter::L) ? 0U : 1U;

    volatile uint16_t* rel = reinterpret_cast<volatile uint16_t*>(&LPC_SCT->MATCHREL[idx]);
    rel[half] = static_cast<uint16_t>(value);

    bool halted = (counter_ == SctCounter::L) ? (LPC_SCT->CTRL & (1U << 2))
                                              : (LPC_SCT->CTRL & (1U << 18));
    if (halted)
    {
        volatile uint16_t* mat = reinterpret_cast<volatile uint16_t*>(&LPC_SCT->MATCH[idx]);
        mat[half] = static_cast<uint16_t>(value);
    }
}

void SctPwm::updatePeriodAndDuty()
{
    uint32_t periodTicks = static_cast<uint32_t>(
        (static_cast<uint64_t>(tickClock_) * periodUs_) / 1000000ULL);

    if (periodTicks < 2U || periodTicks > 65536U)
        return;

    uint32_t match0 = periodTicks - 1U;
    uint32_t match1 = static_cast<uint32_t>(
        (static_cast<float>(periodTicks) * dutyPercent_) / 100.0f);

    if (match1 > match0)
        match1 = match0;

    writeMatch(0, match0);
    writeMatch(1, match1);
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


void SctPwm::writePrescaler(uint16_t div)
{
    uint32_t pre = div - 1U;                 // PRE = divisor - 1

    if (counter_ == SctCounter::L)
    {
        LPC_SCT->CTRL = (LPC_SCT->CTRL & ~(0xFFU << 5)) | (pre << 5);   // PRE_L: bits 12:5
    }
    else
    {
        LPC_SCT->CTRL = (LPC_SCT->CTRL & ~(0xFFU << 21)) | (pre << 21); // PRE_H: bits 28:21
    }
}


bool SctPwm::setPeriodUs(uint32_t us)
{
    if (us == 0U)
        return false;

    // Ticks totales sin prescaler (64 bits para no desbordar)
    uint32_t totalTicks = (uint64_t)systemClock_ * us / 1000000ULL;

    // Prescaler mínimo para que entre en 16 bits (redondeo hacia arriba)
    uint32_t div = (totalTicks + 65535ULL) / 65536ULL;
    if (div == 0U) div = 1U;
    if (div > 256U)
        return false;                        // no entra ni con el prescaler máximo


    prescaler_ = static_cast<uint16_t>(div);
    tickClock_ = systemClock_ / prescaler_;
    periodUs_  = us;

    // Detener mientras se cambia el prescaler
    bool wasRunning = (counter_ == SctCounter::L) ? !(LPC_SCT->CTRL & (1U << 2))
                                                  : !(LPC_SCT->CTRL & (1U << 18));
    if (counter_ == SctCounter::L) LPC_SCT->CTRL |= (1U << 2);
    else                           LPC_SCT->CTRL |= (1U << 18);

    writePrescaler(prescaler_);
    updatePeriodAndDuty();                   // escribe MATCH y MATCHREL (está en halt)

    if (wasRunning)
        start();                             // limpia el contador y lo vuelve a correr

    return true;
}

bool SctPwm::setPeriodMs(uint32_t ms)
{
    return setPeriodUs(ms * 1000U);
}

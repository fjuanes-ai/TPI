#include "Drivers/LPC845.h"
#include "Modulos/swm/swm.h"
#ifndef SCT_PWM_H
#define SCT_PWM_H


enum class SctCounter : uint8_t {L,H};
// clase

class SctPwm {
public:
    /**
     * @brief Constructor de la clase PWM.
     * @param systemClockHz Frecuencia del reloj del sistema asignado al SCTimer (por defecto 12 MHz).
     */
    explicit SctPwm(uint32_t systemClockHz = 12000000U,uint8_t pin_movible = 0, uint8_t port = 0 , uint8_t pin = 0);

    /**
     * @brief Inicializa los registros del SCTimer para generar la señal PWM.
     * @param frequencyHz Frecuencia deseada en Hz.
     * @param initialDutyPercent Ciclo de trabajo inicial en porcentaje (0.0% a 100.0%).
     */
    void Config(SctCounter counter,uint8_t pinMovible,uint8_t port,uint8_t pin,uint32_t frequencyHz);


    void init();
    /**
     * @brief Cambia la frecuencia del PWM en tiempo de ejecución.
     * @param frequencyHz Nueva frecuencia en Hz.
     */
    void setFrequency(uint32_t frequencyHz);

    /**
     * @brief Modifica el ciclo de trabajo de la señal.
     * @param dutyPercent Ciclo de trabajo en porcentaje (0.0% a 100.0%).
     */
    void setDutyCycle(float dutyPercent);

    /**
     * @brief Inicia el conteo y la generación de la señal PWM.
     */
    void start();

    using Callback = void (*)();

    void setCallback(Callback callback);

    void callbackPWM();

    void handler();

    uint32_t getCiclosPWM() const;

    /**
     * @brief Detiene la generación del PWM.
     */
    void stop();

    // Getters
    uint32_t getFrequency() const {
        return frequency_;
    }

    float getDutyCycle() const {
        return dutyPercent_;
    }

private:
    uint32_t systemClock_;   ///< Frecuencia de reloj del sistema
    uint32_t frequency_;     ///< Frecuencia de salida del PWM
    float dutyPercent_;      ///< Porcentaje de ciclo de trabajo
    SctCounter counter_;
    uint8_t pinMovible_;
    uint8_t port_;
    uint8_t pin_;
    volatile uint32_t ciclosPWM_ = 0;
    static SctPwm* instancia_;
    static void SCT_IRQHandler();
    Callback callback_ = nullptr;

    /**
     * @brief Recalcula y actualiza los registros MATCH y MATCHREL del SCTimer.
     */
    void updatePeriodAndDuty();
    void configurarPinPWM();
};

#endif // SCT_PWM_H

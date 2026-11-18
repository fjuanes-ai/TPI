#ifndef MPU6000_H
#define MPU6000_H

/*
 * Mpu6000 - Acelerometro MPU-6000 / MPU-6050 sobre I2cMaster (solo acelerometro).
 *
 * Ejemplo:
 *   I2cMaster i2c(LPC_I2C0, 12000000);
 *   i2c.begin(400000);
 *
 *   Mpu6000 imu(i2c, 0x68, delayMs);        // delayMs: tu funcion de espera en ms
 *   imu.begin(Mpu6000::RANGE_4G, 3, 100);   // +-4g, filtro 44 Hz, 100 Hz de muestreo
 *   imu.calibrate();                        // sensor QUIETO en su posicion de trabajo
 *
 *   // cada 10 ms (timer):
 *   imu.update(0.010f);
 *   Mpu6000::Vec3 a = imu.acceleration();   // m/s2 (sin gravedad)
 *   Mpu6000::Vec3 v = imu.velocity();       // m/s
 *   float s = imu.speed();                  // m/s (modulo)
 */

#include "Modulos/I2C/I2C.h"

class Mpu6000 {
public:
    enum Range : uint8_t { RANGE_2G = 0, RANGE_4G = 1, RANGE_8G = 2, RANGE_16G = 3 };

    // Ademas de estos, los metodos pueden devolver los codigos de I2C::Result.
    enum Result : int { OK = 0, ERR_ID = -10 };

    struct Vec3 { float x, y, z; };

    typedef void (*DelayFn)(uint32_t ms);

    // addr7: 0x68 (AD0 a GND) o 0x69 (AD0 a VCC).
    // delayMs: funcion de espera en milisegundos. Si es nullptr se usa un
    // bucle de espera aproximado (calibrado para ~12 MHz).
    Mpu6000(I2C &bus, uint8_t addr7 = 0x68, DelayFn delayMs = nullptr);

    // dlpf: 1..6 -> ancho de banda del acelerometro 184, 94, 44, 21, 10, 5 Hz.
    // sampleRateHz: 4..1000. checkId=false si tu modulo es un clon con otro WHO_AM_I.
    int begin(Range range = RANGE_2G, uint8_t dlpf = 3,
              uint16_t sampleRateHz = 100, bool checkId = true);

    int whoAmI(uint8_t &id);

    // ---- Lectura directa ----
    int readRaw(int16_t raw[3]);          // cuentas del ADC
    int readAccelG(Vec3 &a);              // en g, incluye gravedad
    int readAccel(Vec3 &a);               // en m/s2, incluye gravedad

    // ---- Aceleracion lineal y velocidad ----
    // Con el sensor quieto, promedia 'samples' lecturas y las guarda como
    // referencia (gravedad + offset). Pone la velocidad en cero.
    int calibrate(uint16_t samples = 200);

    // Lee el sensor, resta la referencia e integra (trapecio). dt en segundos.
    int update(float dt);
    int update();                         // usa dt = 1 / frecuencia de muestreo

    Vec3  acceleration() const { return acc_; }   // m/s2, ultima lectura sin gravedad
    Vec3  velocity()     const { return vel_; }   // m/s
    float speed() const;                          // m/s, modulo de la velocidad
    void  resetVelocity();

    // Zona muerta: aceleraciones menores (m/s2) se toman como ruido (default 0.08).
    void setDeadband(float ms2) { deadband_ = ms2; }

    // Velocidad cero automatica: si |a| < thr (m/s2) durante 'seconds' segundos
    // se asume que esta quieto y se pone v = 0. Desactivado por defecto.
    // OJO: un movimiento a velocidad constante tambien tiene a ~ 0.
    void enableZeroVelocity(bool enable, float thr = 0.15f, float seconds = 0.3f);

private:
    I2C &bus_;
    uint8_t    addr_;
    DelayFn    delay_;

    float      scale_;        // m/s2 por cuenta
    uint16_t   rate_;         // Hz reales de muestreo

    Vec3       ref_, acc_, vel_, prev_;
    bool       first_;
    float      deadband_;

    bool       zuptEn_;
    float      zuptThr_, zuptSec_;
    uint32_t   stillCnt_;

    int  wr(uint8_t reg, uint8_t val);
    void delayMs(uint32_t ms);
};

#endif // MPU6000_H


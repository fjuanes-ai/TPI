#include "Modulos/MPU-6000/Mpu6000.h"
#include <math.h>
namespace {

// Registros
constexpr uint8_t REG_SMPLRT_DIV   = 0x19;
constexpr uint8_t REG_CONFIG       = 0x1A;
constexpr uint8_t REG_ACCEL_CONFIG = 0x1C;
constexpr uint8_t REG_ACCEL_XOUT_H = 0x3B;
constexpr uint8_t REG_PWR_MGMT_1   = 0x6B;
constexpr uint8_t REG_WHO_AM_I     = 0x75;

constexpr uint8_t WHO_AM_I_VALUE   = 0x68;
constexpr float   GRAVITY          = 9.80665f;   // m/s2 por g

inline float applyDeadband(float v, float db)
{
    return (v > -db && v < db) ? 0.0f : v;
}

} // namespace

// ---------------------------------------------------------------------------

Mpu6000::Mpu6000(I2C &bus, uint8_t addr7, DelayFn delayMs)
    : bus_(bus), addr_(addr7), delay_(delayMs),
      scale_(GRAVITY / 16384.0f), rate_(100),
      ref_{0, 0, 0}, acc_{0, 0, 0}, vel_{0, 0, 0}, prev_{0, 0, 0},
      first_(true), deadband_(0.08f),
      zuptEn_(false), zuptThr_(0.15f), zuptSec_(0.3f), stillCnt_(0)
{
}

int Mpu6000::begin(Range range, uint8_t dlpf, uint16_t sampleRateHz, bool checkId)
{
    uint8_t id = 0;
    int rc = whoAmI(id);
    if (rc != OK) return rc;
    if (checkId && id != WHO_AM_I_VALUE) return ERR_ID;

    // Reset y despertar con PLL del giroscopio X como fuente de clock
    rc = wr(REG_PWR_MGMT_1, 0x80);
    if (rc != OK) return rc;
    delayMs(100);
    rc = wr(REG_PWR_MGMT_1, 0x01);
    if (rc != OK) return rc;
    delayMs(10);

    // Frecuencia de muestreo = 1 kHz / (1 + SMPLRT_DIV) (con DLPF activo)
    if (dlpf < 1) dlpf = 1;
    if (dlpf > 6) dlpf = 6;
    if (sampleRateHz < 4)    sampleRateHz = 4;
    if (sampleRateHz > 1000) sampleRateHz = 1000;
    uint8_t div = (uint8_t)(1000u / sampleRateHz - 1u);
    rate_ = (uint16_t)(1000u / (div + 1u));

    rc = wr(REG_SMPLRT_DIV, div);
    if (rc != OK) return rc;
    rc = wr(REG_CONFIG, dlpf);
    if (rc != OK) return rc;
    rc = wr(REG_ACCEL_CONFIG, (uint8_t)(range << 3));
    if (rc != OK) return rc;

    // Sensibilidad: 16384, 8192, 4096, 2048 LSB/g
    scale_ = GRAVITY / (float)(16384u >> range);

    resetVelocity();
    return OK;
}

int Mpu6000::whoAmI(uint8_t &id)
{
    return bus_.readReg(addr_, REG_WHO_AM_I, &id, 1);
}

// ---------------------------------------------------------------------------
// Lectura directa
// ---------------------------------------------------------------------------

int Mpu6000::readRaw(int16_t raw[3])
{
    uint8_t b[6];
    int rc = bus_.readReg(addr_, REG_ACCEL_XOUT_H, b, 6);   // burst: X, Y, Z
    if (rc != OK) return rc;

    raw[0] = (int16_t)(uint16_t)((b[0] << 8) | b[1]);
    raw[1] = (int16_t)(uint16_t)((b[2] << 8) | b[3]);
    raw[2] = (int16_t)(uint16_t)((b[4] << 8) | b[5]);
    return OK;
}

int Mpu6000::readAccel(Vec3 &a)
{
    int16_t r[3];
    int rc = readRaw(r);
    if (rc != OK) return rc;

    a.x = r[0] * scale_;
    a.y = r[1] * scale_;
    a.z = r[2] * scale_;
    return OK;
}

int Mpu6000::readAccelG(Vec3 &a)
{
    int rc = readAccel(a);
    if (rc != OK) return rc;

    a.x /= GRAVITY;
    a.y /= GRAVITY;
    a.z /= GRAVITY;
    return OK;
}

// ---------------------------------------------------------------------------
// Aceleracion lineal y velocidad
// ---------------------------------------------------------------------------

int Mpu6000::calibrate(uint16_t samples)
{
    if (samples == 0) samples = 1;

    uint32_t period = 1000u / rate_;
    if (period == 0) period = 1;

    float sx = 0, sy = 0, sz = 0;
    for (uint16_t i = 0; i < samples; ++i) {
        Vec3 a;
        int rc = readAccel(a);
        if (rc != OK) return rc;
        sx += a.x; sy += a.y; sz += a.z;
        delayMs(period);                    // esperar una muestra nueva
    }

    ref_.x = sx / samples;
    ref_.y = sy / samples;
    ref_.z = sz / samples;

    resetVelocity();
    return OK;
}

int Mpu6000::update()
{
    return update(1.0f / (float)rate_);
}

int Mpu6000::update(float dt)
{
    Vec3 a;
    int rc = readAccel(a);
    if (rc != OK) return rc;

    // Aceleracion lineal = lectura - (gravedad + offset de referencia)
    a.x -= ref_.x;
    a.y -= ref_.y;
    a.z -= ref_.z;

    float mag = sqrtf(a.x * a.x + a.y * a.y + a.z * a.z);

    a.x = applyDeadband(a.x, deadband_);
    a.y = applyDeadband(a.y, deadband_);
    a.z = applyDeadband(a.z, deadband_);

    if (first_) { prev_ = a; first_ = false; }

    // Integracion trapezoidal: v += (a_prev + a) / 2 * dt
    vel_.x += 0.5f * (prev_.x + a.x) * dt;
    vel_.y += 0.5f * (prev_.y + a.y) * dt;
    vel_.z += 0.5f * (prev_.z + a.z) * dt;

    prev_ = a;
    acc_  = a;

    // Velocidad cero automatica (opcional)
    if (zuptEn_) {
        if (mag < zuptThr_) {
            if (++stillCnt_ >= (uint32_t)(zuptSec_ * rate_)) {
                vel_.x = vel_.y = vel_.z = 0.0f;
            }
        } else {
            stillCnt_ = 0;
        }
    }
    return OK;
}

float Mpu6000::speed() const
{
    return sqrtf(vel_.x * vel_.x + vel_.y * vel_.y + vel_.z * vel_.z);
}

void Mpu6000::resetVelocity()
{
    vel_ = Vec3{0, 0, 0};
    acc_ = Vec3{0, 0, 0};
    prev_ = Vec3{0, 0, 0};
    first_ = true;
    stillCnt_ = 0;
}

void Mpu6000::enableZeroVelocity(bool enable, float thr, float seconds)
{
    zuptEn_   = enable;
    zuptThr_  = thr;
    zuptSec_  = seconds;
    stillCnt_ = 0;
}

// ---------------------------------------------------------------------------
// Privado
// ---------------------------------------------------------------------------

int Mpu6000::wr(uint8_t reg, uint8_t val)
{
    return bus_.writeReg(addr_, reg, &val, 1);
}

void Mpu6000::delayMs(uint32_t ms)
{
    if (delay_) {
        delay_(ms);
    } else {
        // Espera aproximada (~12 MHz). Pasale tu propia funcion al constructor.
        for (volatile uint32_t i = 0; i < ms * 1500u; ++i) { }
    }
}

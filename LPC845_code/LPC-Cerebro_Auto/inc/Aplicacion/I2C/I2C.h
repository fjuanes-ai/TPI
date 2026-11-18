
#ifndef I2C_
#define I2C_

#include "Drivers/LPC845.h"

class I2C {
public:
    enum Result : int {
        OK             =  0,
        ERR_NACK_ADDR  = -1,   // el esclavo no respondio a la direccion
        ERR_NACK_DATA  = -2,   // el esclavo NACKeo un dato
        ERR_ARBLOSS    = -3,   // perdida de arbitraje
        ERR_BUS        = -4,   // START/STOP en posicion invalida
        ERR_TIMEOUT    = -5
    };

    I2C(I2C_Type *inst, uint32_t srcClockHz);

    // Inicializa el periferico. Para I2C0 configura tambien los pines fijos
    void begin(uint32_t baud = 100000);

    void setTimeout(uint32_t loops);

    // Escribe len bytes. stop=false deja el bus retenido (repeated start luego).
    int write(uint8_t addr7, const uint8_t *data, uint32_t len, bool stop = true);

    // Lee len bytes (NACK + STOP automaticos tras el ultimo byte).
    int read(uint8_t addr7, uint8_t *buf, uint32_t len, bool stop = true);

    // Escribe registros
    int writeReg(uint8_t addr7, uint8_t reg, const uint8_t *data, uint32_t len);

    // Lee registros con repeated start
    int readReg(uint8_t addr7, uint8_t reg, uint8_t *buf, uint32_t len);

    // Devuelve true si algun esclavo responde en esa direccion.
    bool probe(uint8_t addr7);

    // Escanea el bus; guarda direcciones encontradas en 'found'. Devuelve cantidad.
    uint32_t scan(uint8_t *found, uint32_t maxFound);

private:
    I2C_Type *i2c_;
    uint32_t  srcHz_;
    uint32_t  timeout_;

    uint32_t state() const;
    bool     waitPending();
    bool     waitIdle();
    void     doStop();
    int      start(uint8_t addr7, bool rd);
    int      checkState(uint32_t expected, int nackErr);
    void     finish(int rc, bool stop);
    void     enableClock();
    static void enableFixedPinsI2C0();
};

#endif // I2C

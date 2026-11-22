#include "Modulos/I2C/I2C.h"

namespace {

// --- Bits de registros (UM11029) ---
constexpr uint32_t CFG_MSTEN        = 1u << 0;
constexpr uint32_t STAT_MSTPENDING  = 1u << 0;
constexpr uint32_t STAT_MSTSTATE_SH = 1;
constexpr uint32_t STAT_MSTSTATE_MK = 7u << 1;
constexpr uint32_t STAT_MSTARBLOSS  = 1u << 4;
constexpr uint32_t STAT_MSTSTSTPERR = 1u << 6;
constexpr uint32_t MSTCTL_CONTINUE  = 1u << 0;
constexpr uint32_t MSTCTL_START     = 1u << 1;
constexpr uint32_t MSTCTL_STOP      = 1u << 2;

enum : uint32_t {
    ST_IDLE = 0, ST_RX_READY = 1, ST_TX_READY = 2,
    ST_NACK_ADDR = 3, ST_NACK_DATA = 4
};

} // namespace

// ---------------------------------------------------------------------------
// Publico
// ---------------------------------------------------------------------------

I2C::I2C (I2C_Type *inst, uint32_t srcClockHz)
    : i2c_(inst), srcHz_(srcClockHz), timeout_(200000)
{
}

void I2C::begin(uint32_t baud)
{
    enableClock();
    if (i2c_ == LPC_I2C0) enableFixedPinsI2C0();

    i2c_->CFG = 0;

    // Bit rate = srcHz / (CLKDIV+1) / (SCLHIGH + SCLLOW)
    // Con MSTTIME = 0 -> SCLLOW = SCLHIGH = 2 clocks -> 4 clocks por bit.
    uint32_t div = srcHz_ / (baud * 4u);
    if (div == 0) div = 1;
    i2c_->CLKDIV  = div - 1;
    i2c_->MSTTIME = 0;
    i2c_->CFG     = CFG_MSTEN;
}

void I2C::setTimeout(uint32_t loops)
{
    timeout_ = loops;
}

int I2C::write(uint8_t addr7, const uint8_t *data, uint32_t len, bool stop)
{
    int rc = start(addr7, false);
    for (uint32_t i = 0; rc == OK && i < len; ++i) {
        i2c_->MSTDAT = data[i];
        i2c_->MSTCTL = MSTCTL_CONTINUE;
        if (!waitPending()) { rc = ERR_TIMEOUT; break; }
        rc = checkState(ST_TX_READY, ERR_NACK_DATA);
    }
    finish(rc, stop);
    return rc;
}

int I2C::read(uint8_t addr7, uint8_t *buf, uint32_t len, bool stop)
{
    if (len == 0) return OK;

    int rc = start(addr7, true);
    for (uint32_t i = 0; rc == OK && i < len; ++i) {
        buf[i] = (uint8_t)i2c_->MSTDAT;
        if (i == len - 1) {
            if (stop) { i2c_->MSTCTL = MSTCTL_STOP; waitIdle(); }
            // si !stop se deja el bus retenido para un repeated start
        } else {
            i2c_->MSTCTL = MSTCTL_CONTINUE;      // ACK + siguiente byte
            if (!waitPending()) { rc = ERR_TIMEOUT; break; }
            rc = checkState(ST_RX_READY, ERR_BUS);
        }
    }
    if (rc != OK && rc != ERR_ARBLOSS && rc != ERR_TIMEOUT) doStop();
    return rc;
}

int I2C::writeReg(uint8_t addr7, uint8_t reg, const uint8_t *data, uint32_t len)
{
    int rc = start(addr7, false);
    if (rc == OK) {
        i2c_->MSTDAT = reg;
        i2c_->MSTCTL = MSTCTL_CONTINUE;
        if (!waitPending()) rc = ERR_TIMEOUT;
        else rc = checkState(ST_TX_READY, ERR_NACK_DATA);
    }
    for (uint32_t i = 0; rc == OK && i < len; ++i) {
        i2c_->MSTDAT = data[i];
        i2c_->MSTCTL = MSTCTL_CONTINUE;
        if (!waitPending()) { rc = ERR_TIMEOUT; break; }
        rc = checkState(ST_TX_READY, ERR_NACK_DATA);
    }
    finish(rc, true);
    return rc;
}

int I2C::readReg(uint8_t addr7, uint8_t reg, uint8_t *buf, uint32_t len)
{
    int rc = write(addr7, &reg, 1, false);
    if (rc != OK) return rc;
    return read(addr7, buf, len, true);
}

bool I2C::probe(uint8_t addr7)
{
    return write(addr7, nullptr, 0, true) == OK;
}

uint32_t I2C::scan(uint8_t *found, uint32_t maxFound)
{
    uint32_t n = 0;
    for (uint8_t a = 0x08; a < 0x78 && n < maxFound; ++a)
        if (probe(a)) found[n++] = a;
    return n;
}

// ---------------------------------------------------------------------------
// Privado
// ---------------------------------------------------------------------------

uint32_t I2C::state() const
{
    return (i2c_->STAT & STAT_MSTSTATE_MK) >> STAT_MSTSTATE_SH;
}

bool I2C::waitPending()
{
    uint32_t t = timeout_;
    while (!(i2c_->STAT & STAT_MSTPENDING))
        if (--t == 0) return false;
    return true;
}

bool I2C::waitIdle()
{
    uint32_t t = timeout_;
    while (!(i2c_->STAT & STAT_MSTPENDING) || state() != ST_IDLE)
        if (--t == 0) return false;
    return true;
}

void I2C::doStop()
{
    i2c_->MSTCTL = MSTCTL_STOP;
    waitIdle();
}

int I2C::start(uint8_t addr7, bool rd)
{
    if (!waitPending()) return ERR_TIMEOUT;
    i2c_->MSTDAT = (uint32_t)((addr7 << 1) | (rd ? 1 : 0));
    i2c_->MSTCTL = MSTCTL_START;            // START o repeated START
    if (!waitPending()) return ERR_TIMEOUT;
    return checkState(rd ? ST_RX_READY : ST_TX_READY, ERR_NACK_ADDR);
}

int I2C::checkState(uint32_t expected, int nackErr)
{
    uint32_t stat = i2c_->STAT;
    if (stat & STAT_MSTARBLOSS)  { i2c_->STAT = STAT_MSTARBLOSS;  return ERR_ARBLOSS; }
    if (stat & STAT_MSTSTSTPERR) { i2c_->STAT = STAT_MSTSTSTPERR; return ERR_BUS; }

    uint32_t st = (stat & STAT_MSTSTATE_MK) >> STAT_MSTSTATE_SH;
    if (st == expected)     return OK;
    if (st == ST_NACK_ADDR) return ERR_NACK_ADDR;
    if (st == ST_NACK_DATA) return ERR_NACK_DATA;
    return nackErr;
}

void I2C::finish(int rc, bool stop)
{
    if (rc == ERR_ARBLOSS || rc == ERR_TIMEOUT) return;
    if (stop || rc != OK) doStop();
}

void I2C::enableClock()
{
    uint32_t bit;
    if      (i2c_ == LPC_I2C0) { bit = 1u << 5;  SYSCON->FCLKSEL[6] = 1; }  // 1 = main clock
    else if (i2c_ == LPC_I2C1) { bit = 1u << 21; SYSCON->FCLKSEL[7] = 1; }
    else if (i2c_ == LPC_I2C2) { bit = 1u << 22; SYSCON->FCLKSEL[8] = 1; }
    else                   { bit = 1u << 23; SYSCON->FCLKSEL[9] = 1; }

    SYSCON->SYSAHBCLKCTRL0 |= bit;
    SYSCON->PRESETCTRL0    &= ~bit;         // reset del periferico
    SYSCON->PRESETCTRL0    |=  bit;
}

void I2C::enableFixedPinsI2C0()
{
    SYSCON->SYSAHBCLKCTRL0 |= (1u << 7);                 // clock de la SWM
    SWM0->PINENABLE0 &= ~((1u << 11) | (1u << 12));      // I2C0_SDA, I2C0_SCL
}

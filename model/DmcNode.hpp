#ifndef PAQ8PX_DMCNODE_HPP
#define PAQ8PX_DMCNODE_HPP

#include <cstdint>

/**
 * c0,c1: adaptive counts of zeroes and ones;
 * fixed point numbers with 6 integer and 10 fractional bits, i.e. scaling factor = 1024;
 * thus the values 0 .. 65535 represent real counts of 0.0 .. 63.999
 * nx0, nx1: indexes of next DMC nodes in the state graph
 * state: bit history state - as in a contextmap
 */
struct DMCNode { // 12 bytes
private:
    uint32_t _nx0, _nx1; // packed: their higher 28 bits are nx0, nx1; the lower 4+4 bits give the bit history state byte

public:
    uint16_t c0, c1;
    [[nodiscard]] uint8_t getState() const;
    void setState(uint8_t state);
    [[nodiscard]] uint32_t getNx0() const;
    void setNx0(uint32_t nx0);
    [[nodiscard]] uint32_t getNx1() const;
    void setNx1(uint32_t nx1);
};

#endif //PAQ8PX_DMCNODE_HPP

#include <filterUtils.hpp>
#include <bitset>
#define MEM_UNIT 8

using namespace std;

int asm_rank(uint64_t val, int pos)
{
    // Keep only bits up to pos pos non-including.
    val = val & (((2ULL) << pos) - 1);

    asm("popcnt %[val], %[val]"
        : [val] "+r" (val)
        :
        : "cc");

    return (val);
}

int asm_select(uint64_t val, int n)
{          
    uint64_t pos = 1ULL << n;

    asm("pdep %[val], %[mask], %[val]"
		: [val] "+r" (val)
		: [mask] "r" (pos));
	
    asm("tzcnt %[bit], %[index]"
        : [index] "=r" (pos)
        : [bit] "g" (val)
		: "cc");

    return (pos);
}
using namespace std;

void set8(uint8_t * pos, uint8_t value, uint8_t mask)
{
    *pos = (*pos & ~mask);
    *pos |= (value & mask);
}

inline uint8_t getNthBitFrom(uint64_t vec, int n) {
    return (vec >> n) & 0b1;
}

inline void setNthBitFrom(uint64_t &vec, int n) {
    vec |= 1ULL << n;
}

inline void setNthBitToX(uint64_t &vec, int n, int x) {
    vec ^= (-x ^ vec) & (1ULL << n);
}

inline void clearNthBitFrom(uint64_t &vec, int n) {
    vec &= ~(1ULL << n);
}

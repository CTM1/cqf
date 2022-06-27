#include <filterUtils.hpp>
#define MEM_UNIT 8

int asmRank(uint64_t val, int pos)
{
    // Keep only bits up to pos pos non-including.
    val = val & (((2ULL) << pos) - 1);

    asm("popcnt %[val], %[val]"
        : [val] "+r" (val)
        :
        : "cc");

    return (val);
}

int asmSelect(uint64_t val, int n)
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
    cout << (uint64_t)mask << endl;
    cout << (uint64_t)value << " -> ";
    *pos = (*pos & ~mask);
    *pos += (value & mask);
    cout << (uint64_t)*pos << endl;
}

void printbits(uint8_t * mem, size_t offset, size_t num_bits) {
    uint8_t current_byte = (*mem) >> offset;
    size_t remaining_bits = MEM_UNIT - offset;

    uint i;
    for (i=0 ; i<num_bits ; i++) {
        cout << ((current_byte & 0b1) ? '1' : '0');
        current_byte >>= 1;
        remaining_bits -= 1;

        if (remaining_bits == 0) {
            mem += 1;
            current_byte = *mem;
            remaining_bits = MEM_UNIT;
        }
    }
    cout << endl;
}

// void printbits(unsigned long number, unsigned int num_bits_to_print)
// {
//      if (num_bits_to_print > 0) {
//          printbits(number >> 1, num_bits_to_print - 1);
//          printf("%d", (int) number & 1);
//      }
//      if (num_bits_to_print == 0)
//          printf("\n");
// }

// void printbits64(unsigned long number) {
//     printbits(number, 64);
// }

// void printbits8(unsigned long number) {
//     printbits(number, 8);
// }

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

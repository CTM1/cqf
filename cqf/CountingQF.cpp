#include <CountingQF.hpp>
#include <filterUtils.hpp>
#include <bitset>

#define MEM_UNIT 8
#define MAX_UINT 64
using namespace std;

CountingQF::CountingQF(uint32_t powerOfTwo)
{
    /*
    * Need total remainders (h1) memory to be (x * (VEC_LEN - log2(x)))
    * aka. so len(h0) needs to be of len log2(x) bits to have enough
    * values for the possible slots.
    */

    uint64_t numberOfSlots = 1ULL << powerOfTwo;
    uint64_t totalOccLen = numberOfSlots;
    uint64_t totalRunLen = numberOfSlots;

    uint64_t quotientLen = powerOfTwo;
    uint64_t remLen = MAX_UINT - quotientLen;

    uint64_t totalRemsLen = numberOfSlots * remLen;

    /* Blocks are made of an offset (8b) + occ (64b) 
    + run (64b) + remainders (64b * remLen) */

    uint64_t numberOfBlocks = numberOfSlots / MAX_UINT;
    if (numberOfBlocks == 0)
        numberOfBlocks++;

    uint64_t filterSize = (MEM_UNIT * numberOfBlocks) + totalOccLen + totalRunLen + totalRemsLen;
    
    uint64_t blockByteSize = filterSize / numberOfBlocks / MEM_UNIT;

    this -> qf = new uint8_t[(blockByteSize * numberOfBlocks)];

    memset(qf, 0, blockByteSize * numberOfBlocks);

    this -> filterSize = filterSize;
    this -> numberOfSlots = numberOfSlots;
    this -> numberOfBlocks = numberOfBlocks;
    this -> blockByteSize = blockByteSize;
    this -> quotientLen = quotientLen;
    this -> remainderLen = remLen;
}

bool CountingQF::query(uint64_t val)
{
    return false;
}

void CountingQF::insertValue(uint64_t val)
{
}

uint64_t CountingQF::getRem(uint8_t * blockAddr, uint32_t slot)
{
    uint32_t slotOffset = (slot * remainderLen) / MEM_UNIT;
    uint32_t bitOffset = (slot * remainderLen) % MEM_UNIT;
    uint64_t res = 0;

    uint8_t * slotAddr = blockAddr + 17 + slotOffset;
    uint8_t firstBitMask = ((1ULL << (MEM_UNIT - bitOffset)) - 1);

    cout << (*slotAddr & firstBitMask) << endl;
    res += (*slotAddr & firstBitMask);
    res <<= MEM_UNIT;

    uint32_t iterations = ((remainderLen + (MEM_UNIT - 1)) / MEM_UNIT) - 1;
    
    for (uint8_t i = 1; i < iterations; i++)
    {
        slotAddr += 1;
        res <<= MEM_UNIT;
        res += *slotAddr;
    }

    res <<= MEM_UNIT;
    uint8_t remainingBits = remainderLen - (MEM_UNIT * (iterations));
    cout << "remainingbits: " << (uint32_t)remainingBits << endl;

    uint8_t lastBitMask = (((1ULL << remainingBits)) - 1);

    cout << bitset<8>(lastBitMask) << endl;
    cout << bitset<8>(*slotAddr & lastBitMask) << endl;
    res += (*slotAddr & lastBitMask);
    return (res);
}

// FRANCESCO DONT USE THIS ONE, IT PLACES THE BYTES IN INVERTED ORDER!
void CountingQF::setRem(uint8_t * blockAddr, uint32_t slot, uint64_t rem)
{
    uint32_t slotOffset = (slot * remainderLen) / MEM_UNIT;
    uint32_t bitOffset = (slot * remainderLen) % MEM_UNIT;
    cout << "remainder: " << bitset<64>(rem) << endl;

    // Mask to keep only leftmost bitOffset bits
    uint8_t firstBitMask =  ((1ULL << (MEM_UNIT - bitOffset)) - 1);
    cout << "first mask: " << bitset<8>(firstBitMask) << endl;
    cout << "shfited by how much:" << (remainderLen - MEM_UNIT) << endl;
    cout << "remainder: " << bitset<64>((rem >> (MAX_UINT - MEM_UNIT))) << endl;
    
    // BlockAddr + Offset + Occupieds and Runends length + slotOffset
    uint8_t * slotAddr = blockAddr + 1 + ((MAX_UINT / MEM_UNIT) * 2) 
        + slotOffset;

    cout << "first part of remainder" << bitset<64>((rem >> (MAX_UINT - MEM_UNIT)) & firstBitMask) << endl;
    // Set the first part of the remainder
    set8(slotAddr, (rem >> (MAX_UINT - MEM_UNIT)), firstBitMask);

    // How many parts are left in memory units
    uint32_t iterations = ((remainderLen + (MEM_UNIT - 1)) / MEM_UNIT) - 1;

    for (uint8_t i = 1; i < iterations; i++)
    {
        cout << "shfited by how much:" << (MAX_UINT - (MEM_UNIT) * (i + 1)) << endl;
        cout << "mid byte: " << bitset<8>((rem >> (MAX_UINT - (MEM_UNIT * (i + 1))))) << endl;
        slotAddr += 1;
                    // Get bits corresponding to given part of the remainder 
        set8(slotAddr,(rem >> (MAX_UINT - (MEM_UNIT * (i + 1)))) & 0xff,0xff);
        
    }

    slotAddr += 1;

    uint8_t remainingBits = remainderLen - (MEM_UNIT * (iterations));

    cout << "bitoffset: " << (uint32_t)bitOffset << endl;
    cout << "remainingbits: " << (uint32_t)remainingBits << endl;

    // Mask to keep only rightmost bits of last memory unit.
    uint8_t lastBitMask = ((1ULL << (MEM_UNIT - remainingBits)) - 1);
    cout << "last mask: " << bitset<8>(lastBitMask) << endl;
    cout << "remainder (not shifted, supposedly last byte): " << bitset<64>(rem) << endl;
    cout << "remainder masked: " << bitset<64>(rem) << endl;
    cout << "remainder converted: " << bitset<32>(((uint8_t) rem)) << endl;
    
    // Set the last part of the remainder   
    set8(slotAddr, (uint8_t) rem, lastBitMask);
}

// FRANCESCO USE THIS ONE, IT SETS THE BYTE IN THE CORRECT ORDER!
void CountingQF::setRemRev(uint8_t * blockAddr, uint32_t slot, uint64_t rem)
{
    uint32_t slotOffset = (slot * remainderLen) / MEM_UNIT;
    uint32_t bitOffset = (slot * remainderLen) % MEM_UNIT;

    // Mask to keep only leftmost bitOffset bits
    uint8_t firstBitMask = ~((1ULL << bitOffset) - 1);
    
    /*
    cout << "first bit mask: "<< bitset<8>(firstBitMask) << endl;
    cout << "remainder: " << bitset<64>(rem) << endl;
    cout << "masked remainder:" << bitset<64>(rem & firstBitMask) << endl;
    */
    // BlockAddr + Offset + Occupieds and Runends length + slotOffset
    uint8_t * slotAddr = blockAddr + 1 + ((MAX_UINT / MEM_UNIT) * 2) 
        + slotOffset;

    // Set the first part of the remainder
    set8(slotAddr, rem, firstBitMask);

    // How many parts are left in memory units
    uint32_t iterations = ((remainderLen + (MEM_UNIT - 1)) / MEM_UNIT) - 1;

    for (uint8_t i = 1; i < iterations; i++)
    {
        slotAddr += 1;
                    // Get bits corresponding to given part of the remainder 
        // cout << "mid remainder: " << bitset<64>(rem >> (MEM_UNIT * i) & 0xff) << endl;
    
        set8(slotAddr,(rem >> (MEM_UNIT * i)) & 0xff,0xff);
        
    }

    slotAddr += 1;

    uint8_t remainingBits = remainderLen - (MEM_UNIT * (iterations));

    // Mask to keep only rightmost bits of last memory unit.
    uint8_t lastBitMask = ((1ULL << (MEM_UNIT - remainingBits)) - 1);
    
    // Set the last part of the remainder   
    set8(slotAddr, (rem >> (MAX_UINT - MEM_UNIT)) & 0xff, lastBitMask);
}
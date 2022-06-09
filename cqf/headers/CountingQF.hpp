#ifndef COUNTINGQF_HPP
#define COUNTINGQF_HPP

#include <iostream>
#include <cstdint>
#include <vector>
#include <cmath>

/**
 * @brief Counting Quotient Filter class
 * 
 */
class CountingQF 
{   
    public:
        uint64_t occupieds;
        uint64_t runends;
        uint64_t remainders[];

        /**
         * @brief Instantiate an empty CQF.
         * 
         */
        explicit CountingQF();
        
        /**
         * @brief Query CQF for presence.
         * 
         * @param val Check if given value is present in the CQF, 
         *            CQFs may announce false-positives.
         * @return true
         * @return false
         */
        bool query(uint64_t val);

        /**
         * @brief Returns index of the first unused slot in the filter 
         *        using Rank and Select operations after the slot fromPos.
         * 
         * @param fromPos Position to start looking for free slots from. 
         * @return int Index of free slot.
         */
        int findFirstUnusedSlot(int fromPos);
        
        /**
         * @brief Insert an already hashed value into the CQF.
         * 
         * @param val Value to insert.
         */
        void insertValue(uint64_t val);

        /**
         * @brief Returns number of set bits in val up to index i inclusive.
         * 
         * @param val Value to count set bits from.
         * @param i Inclusive upper bound for counting. 
         * @return int Number of set bits in val.
         */
        int asmSelect(uint64_t val, int i);

        /**
         * @brief Returns index of the n-th set bit in val, couting from 0.
         * 
         * @param val Value to search set bit in.
         * @param n The rank of a given set bit to look for.
         * @return int Returns index of the n-th set bit if found,
         * returns 64 if there were not enough set bits to go up to rank n.
         */
        int asmRank(uint64_t val, int n);
        
        /**
         * @brief Get the value of the n-th bit from a given uint.
         * 
         * @param vec Value to search n-th bit in.
         * @param n Position of desired bit.
         * @return uint8_t 1 or 0.
         */
        inline uint8_t getNthBitFrom(uint64_t vec, int n);
        
        /**
         * @brief Set the n-th bit of a given uint by passing it
         * by reference.
         * 
         * @param vec Passed-by-reference value to modify.
         * @param n Position of bit you wish to modify.
         */
        inline void setNthBitFrom(uint64_t &vec, int n);


        //TODO: Set x to a boolean?
        /**
         * @brief Set the n-th bit of a given uint to x by passing it
         * by reference.
         * 
         * @param vec Passed-by-reference value to modify.
         * @param n Position of bit you wish to modify.
         * @param x Value you wish the n-th bit to take, 1 or 0 conventionally.
         */
        inline void setNthBitToX(uint64_t &vec, int n, int x);

        /**
         * @brief Unset the n-th bit of a given uint by passing it
         * by reference
         * 
         * @param vec Passed-by-reference value to modify.
         * @param n Position of bit you wish to modify.
         */
        inline void clearNthBitFrom(uint64_t &vec, int n);

};

#endif
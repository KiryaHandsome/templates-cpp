#pragma once


#include <bit>
#include <iostream>


template <size_t bits>
class Bitset
{
private:
    enum { BITS_IN_BYTE = 8 };
    using type = unsigned long long;
    static const size_t bits_per_word = BITS_IN_BYTE * sizeof(type);
    static const size_t words = bits == 0 ? 0 : (bits - 1) / bits_per_word + 1;
    type _array[words];
public:
    // proxy for an element
    class reference {
        friend Bitset<bits>;

    public:
        ~reference()  {} 

        reference& operator=(bool val)  {
            _bitset->set_unchecked(position, val);
            return *this;
        }

        reference& operator=(const reference& bit_ref)  {
            _bitset->set_unchecked(position, static_cast<bool>(bit_ref));
            return *this;
        }

        reference& flip() {
            _bitset->flip(position);
            return *this;
        }

        bool operator~() const  {
            return !_bitset->subscript(position);
        }

        operator bool() const noexcept {
            return _bitset->subscript(position);
        }

    private:
        reference() noexcept : _bitset(nullptr), position(0) {}

        reference(Bitset<bits>& _Bitset, size_t pos) : _bitset(&_Bitset), position(pos) {}

        Bitset<bits>* _bitset;
        size_t position; // position of element in bitset
    };

    Bitset() :_array{} { }

    Bitset(unsigned long long value) :_array{ value } {}

    Bitset(const std::string& value) :_array{} {
        size_t count = value.size();
        if (count > bits) {
            count = bits;
        }
        size_t pos = 0;
        for (int i = count - 1; i >= 0; --i) {
            set_unchecked(pos, value[i] - '0');
            pos++;
        }
    }

    reference operator[](size_t pos) {
        if (pos < bits) {
            return reference(*this, pos);
        }
        throw std::out_of_range("bitset index outside range");
    }

    bool operator[](size_t pos) const {
        if (pos < bits) {
            return subscript(pos);
        }
        throw std::out_of_range("bitset index outside range");
    }

    Bitset& flip() { // flip all bits
        for (size_t pos = 0; pos < words; ++pos) {
            _array[pos] = ~_array[pos];
        }

        trim();
        return *this;
    }

    Bitset& flip(size_t pos)    //flip bit on position
    {
        if (bits <= pos) {
            throw std::out_of_range("bitset out of range"); // pos off end
        }
        _array[pos / bits_per_word] ^= type{ 1 } << pos % bits_per_word;
        return *this;
    }

    bool all() const {
        bool zero_length = bits == 0;
        if(zero_length) { // must test for this, otherwise would count one full word
            return true;
        }

        bool no_padding = bits % bits_per_word == 0;
        for (size_t w_pos = 0; w_pos < words + no_padding - 1; ++w_pos) {
            if (_array[w_pos] != ~static_cast<type>(0)) {
                return false;
            }
        }

        return no_padding || _array[words - 1] == (static_cast<type>(1) << (bits % bits_per_word)) - 1;
    }

    size_t count() const    //number of bits that are set to true
    {  
        //function from stl
        auto popcount = [&](type _Val){
            constexpr int _Digits = std::numeric_limits<type>::digits;
            // we static_cast these bit patterns in order to truncate them to the correct size
            _Val = static_cast<type>(_Val - ((_Val >> 1) & static_cast<type>(0x5555'5555'5555'5555ull)));
            _Val = static_cast<type>((_Val & static_cast<type>(0x3333'3333'3333'3333ull))
                                    + ((_Val >> 2) & static_cast<type>(0x3333'3333'3333'3333ull)));
            _Val = static_cast<type>((_Val + (_Val >> 4)) & static_cast<type>(0x0F0F'0F0F'0F0F'0F0Full));
            // Multiply by one in each byte, so that it will have the sum of all source bytes in the highest byte
            _Val = static_cast<type>(_Val * static_cast<type>(0x0101'0101'0101'0101ull));
            // Extract highest byte
            return static_cast<size_t>(_Val >> (_Digits - 8));
        };

        size_t value = 0;
        for (int pos = words - 1; pos >= 0; --pos) {
            value += popcount(_array[pos]);
        }
        return value;
    }


    size_t size() const { return bits; }

    bool test(size_t pos) const {
        if (bits <= pos) {
            throw std::out_of_range("bitset out of range"); // pos off end
        }

        return subscript(pos);
    }

    bool any() const {
        for (size_t pos = 0; pos < words; ++pos) {
            if (_array[pos] != 0) {
                return true;
            }
        }

        return false;
    }

    bool none() const {
        return !any();
    }

    Bitset operator<<(size_t pos) const {
        Bitset tmp = *this;
        tmp <<= pos;
        return tmp;
    }

    Bitset operator>>(size_t pos) const {
        Bitset tmp = *this;
        tmp >>= pos;
        return tmp;
    }
    
    Bitset& operator&=(const Bitset& right) noexcept {
        for (size_t w_pos = 0; w_pos < words; ++w_pos) {
            _array[w_pos] &= right._array[w_pos];
        }

        return *this;
    }

    Bitset& operator|=(const Bitset& right) noexcept {
        for (size_t w_pos = 0; w_pos < words; ++w_pos) {
            _array[w_pos] |= right._array[w_pos];
        }

        return *this;
    }

    Bitset& operator^=(const Bitset& right) noexcept {
        for (size_t w_pos = 0; w_pos < words; ++w_pos) {
            _array[w_pos] ^= right._array[w_pos];
        }

        return *this;
    }


    Bitset& reset() noexcept { // set all bits false
        for (size_t i = 0; i < words; ++i) {
            _array[i] = 0;
        }
        return *this;
    }

    Bitset& reset(size_t pos) { // set bit at pos to false
        return set(pos, false);
    }

    Bitset& set(size_t pos, bool val = true) noexcept { // set all bits true 
        if (bits <= pos) {
            throw std::out_of_range("bitset out of range"); // pos off end
        }

        set_unchecked(pos, val);
        return *this;
    }

    Bitset& set() noexcept { // set all bits true
        for (size_t i = 0; i < words; ++i) {
            _array[i] = ULLONG_MAX;
        }
        trim();
        return *this;
    }

    std::string to_string() const
    {
        std::string str;
        str.reserve(bits);
        char zero = '0', one = '1';
        for (auto pos = bits; 0 < pos;) {
            str.push_back(subscript(--pos) ? one : zero);
        }

        return str;
    }

    unsigned long long to_ullong() const    //throws std::overflow_error
    {  
        if (bits == 0) return 0;
        bool _bits_large = bits > 64;

        if (_bits_large) {
            for (size_t indx = 1; indx < words; ++indx) {
                if (_array[indx] != 0) {
                    throw std::overflow_error("bitset overflow"); // fail if any high-order words are nonzero
                }
            }
        }

        return _array[0];
    }

    unsigned long to_ulong() const {
        bool _bits_large = bits > 64;

        if(bits == 0)  return 0;

        if(bits <= 32) {
            return static_cast<unsigned long>(_array[0]);
        }

        if (_bits_large) {
            for (size_t indx = 1; indx <= words; ++indx) {
                if (_array[indx] != 0) {
                    throw std::overflow_error("bitset overflow"); // fail if any high-order words are nonzero
                }
            }
        }

        if (_array[0] > ULONG_MAX) {
            throw std::overflow_error("bitset overflow");
        }

        return static_cast<unsigned long>(_array[0]);
    }

    

private:
    constexpr bool subscript(size_t pos) const {
        return (_array[pos / bits_per_word] & (type{ 1 } << pos % bits_per_word)) != 0;
    }

    void trim()  { // clear any trailing bits in last word
        bool work_to_do = bits == 0 || bits % bits_per_word != 0;
        if (work_to_do) {
            _array[words - 1] &= (type{ 1 } << bits % bits_per_word) - 1;
        }
    }

    Bitset& set_unchecked(size_t pos, bool val) { // set bit at _Pos to _Val, no checking
        auto& selected_word = _array[pos / bits_per_word];
        const auto bit = type{ 1 } << pos % bits_per_word;
        if (val) {
            selected_word |= bit;
        }
        else {
            selected_word &= ~bit;
        }

        return *this;
    }
};

template <size_t bits>
Bitset<bits> operator&(const Bitset<bits>& left, const Bitset<bits>& right) noexcept {
    Bitset<bits> answer = left;
    answer &= right;
    return answer;
}

template <size_t bits>
Bitset<bits> operator|(const Bitset<bits>& left, const Bitset<bits>& right) noexcept {
    Bitset<bits> answer = left;
    answer |= right;
    return answer;
}

template <size_t bits>
Bitset<bits> operator^(const Bitset<bits>& left, const Bitset<bits>& right) noexcept {
    Bitset<bits> answer = left;
    answer ^= right;
    return answer;
}

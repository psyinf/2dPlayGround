#pragma once
#include <cstdint>

/**
 * A type that represents a fractional amount with a fixed number of digits. This obviously has some limitations if the
 * floating point number is not representable in the given number of digits. (e.g. FractionalAmount(1.0 / 3.0) * 3 will
 * never equal 1)
 */
template <typename DigitsType, DigitsType NumDigits>
class FractionalAmount
{
public:
    friend FractionalAmount operator+(const FractionalAmount& lhs, const FractionalAmount& rhs)
    {
        return FractionalAmount(lhs._value + rhs._value);
    }

    friend FractionalAmount operator-(const FractionalAmount& lhs, const FractionalAmount& rhs)
    {
        return FractionalAmount(lhs._value - rhs._value);
    }

    friend FractionalAmount operator*(const FractionalAmount& lhs, const FractionalAmount& rhs)
    {
        return FractionalAmount(static_cast<float>(lhs) * static_cast<float>(rhs));
    }

    friend FractionalAmount operator/(const FractionalAmount& lhs, const FractionalAmount& rhs)
    {
        return FractionalAmount(static_cast<float>(lhs) / static_cast<float>(rhs));
    }

    friend bool operator==(const FractionalAmount& lhs, const FractionalAmount& rhs)
    {
        return lhs._value == rhs._value;
    }

    friend bool operator!=(const FractionalAmount& lhs, const FractionalAmount& rhs)
    {
        return lhs._value != rhs._value;
    }

    friend auto operator<=>(const FractionalAmount& lhs, const FractionalAmount& rhs)
    {
        return lhs._value <=> rhs._value;
    }

    // equality operators
    friend bool operator==(const FractionalAmount& lhs, const float& rhs) { return static_cast<float>(lhs) == rhs; }

    operator float() const { return static_cast<float>(_value) / getFactor(); }

    FractionalAmount() = default;

    // assignment operators
    FractionalAmount& operator=(const FractionalAmount& rhs)
    {
        _value = rhs._value;
        return *this;
    }

    FractionalAmount& operator=(const float& value)
    {
        _value = static_cast<std::uint32_t>(value * getFactor());
        return *this;
    }

    FractionalAmount& operator+=(const FractionalAmount& rhs)
    {
        _value += rhs._value;
        return *this;
    }

    FractionalAmount& operator-=(const FractionalAmount& rhs)
    {
        _value -= rhs._value;
        return *this;
    }

    /*explicit*/ FractionalAmount(const float& value)
      : _value(static_cast<std::uint32_t>(value * getFactor()))
    {
    }

    // ostream op
    friend std::ostream& operator<<(std::ostream& os, const FractionalAmount& fa)
    {
        os << static_cast<float>(fa);
        return os;
    }

private:
    consteval static std::uint32_t getFactor()
    {
        std::uint32_t factor = 1;
        for (DigitsType i = 0; i < NumDigits; ++i)
        {
            factor *= 10;
        }
        return factor;
    }

    std::uint32_t _value{};
};



namespace std {
template <typename DigitsType, DigitsType NumDigits>
class numeric_limits<FractionalAmount<DigitsType, NumDigits>>
{
public:
    static constexpr FractionalAmount<DigitsType, NumDigits> max() 
    {
        return FractionalAmount<DigitsType, NumDigits>(std::numeric_limits<DigitsType>::max());
    }

};
} // namespace std
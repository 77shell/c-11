
#include <stdio.h>
#include <cmath>

using u16 = unsigned short;
using s16 = short;
using u8  = unsigned char;

union u16_t {
        u16 val;
        struct {
                u8 lo;
                u8 hi;
        } byte;
	u8 ary[2];
};

union s16_t {
        s16 val;
        struct {
                u8 lo;
                u8 hi;
        } byte;
	u8 ary[2];
};

int main(int argc, char *argv[])
{
	//u16 tcurrent {0x3d8b};
	u16 tcurrent {0x57aa};
	//u16 tcurrent {0x4856};
	constexpr u16
		sign_mask       {0b0001000000000000},
		multiplier_mask {0b1110000000000000},
		base_mask       {0b0000111111111111};
	u16
		sign            {tcurrent & sign_mask},
		multiplier      {(tcurrent & multiplier_mask) >> 13};

	constexpr float multipliers[] = {
		0.001,
		0.01,
		0.1,
		1,
		10,
		100
	};

	if(multiplier > sizeof multipliers / sizeof(float))
		return std::nan("tCurrent-over-flow");

	float s;
	if(sign > 0) { // Negative
		u16 base {(~tcurrent & base_mask) + 1};
		s = base * -1 * multipliers[multiplier];
	}
	else { // Positive
		u16 base {tcurrent & base_mask};
		s = base * multipliers[multiplier];
	}
	printf("%f\n", s);
}

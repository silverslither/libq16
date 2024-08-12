// Copyright (c) 2024 silverslither.

#include "fixed.hpp"

uq16 F64_TO_UQ16(double n) {
    double res = std::round(65536.0 * n);

    if (res < 0) {
        LIBQ16_LOG_ERR("double to ufixed_16_16 conversion underflow\n");
        return UFIXED_MIN;
    }

    if (res >= 4294967296.0) {
        LIBQ16_LOG_ERR("double to ufixed_16_16 conversion overflow\n");
        return UFIXED_MAX;
    }

    return (uq16)res;
}

q16 F64_TO_Q16(double n) {
    double res = fabs(std::round(65536.0 * n));

    if (res >= 2147483648.0) {
        LIBQ16_LOG_ERR("double to sfixed_15_16 conversion overflow\n");
        return (n >= 0) ? SFIXED_MAX : SFIXED_MIN;
    }

    return (n >= 0) ? (q16)res : NABS_Q16((q16)res);
}

bool CMP_GEQ_Q16(q16 a, q16 b) {
    uint32_t sign_a = SGN_Q16(a);
    uint32_t sign_b = SGN_Q16(b);
    if (sign_a == sign_b)
        return sign_b ? a <= b : a >= b;
    return sign_b;
}

bool CMP_LEQ_Q16(q16 a, q16 b) {
    uint32_t sign_a = SGN_Q16(a);
    uint32_t sign_b = SGN_Q16(b);
    if (sign_a == sign_b)
        return sign_a ? a >= b : a <= b;
    return sign_a;
}

q16 ADD_Q16_UNSAFE(q16 a, q16 b) {
    uint32_t sign_a = SGN_Q16(a);
    uint32_t sign_b = SGN_Q16(b);
    if (sign_a == sign_b)
        return (q16)((a + b) | sign_a);

    uint32_t abs_a = ABS_Q16(a);
    uint32_t abs_b = ABS_Q16(b);
    if (abs_a == abs_b)
        return (q16)0;

    uint32_t res;
    if (abs_a > abs_b) {
        res = abs_a - abs_b;
        res |= sign_a;
    } else {
        res = abs_b - abs_a;
        res |= sign_b;
    }

    return (q16)res;
}

q16 ADD_Q16(q16 a, q16 b) {
    uint32_t res;
    uint32_t sign_a = SGN_Q16(a);
    uint32_t sign_b = SGN_Q16(b);
    if (sign_a == sign_b) {
        res = a + b;
        if (res & 0x80000000) {
            LIBQ16_LOG_ERR("sfixed_15_16 addition overflow\n");
            return (q16)(SFIXED_MAX | sign_a);
        }
        return (q16)(res | sign_a);
    }

    uint32_t abs_a = ABS_Q16(a);
    uint32_t abs_b = ABS_Q16(b);
    if (abs_a == abs_b)
        return (q16)0;

    if (abs_a > abs_b) {
        res = abs_a - abs_b;
        res |= sign_a;
    } else {
        res = abs_b - abs_a;
        res |= sign_b;
    }

    return (q16)res;
}

q16 SUB_Q16_UNSAFE(q16 a, q16 b) {
    uint32_t sign_a = SGN_Q16(a);
    uint32_t sign_b = SGN_Q16(b);
    if (sign_a != sign_b)
        return (q16)(ABS_Q16((q16)(a + b)) | sign_a);

    uint32_t abs_a = ABS_Q16(a);
    uint32_t abs_b = ABS_Q16(b);
    if (abs_a == abs_b)
        return (q16)0;

    uint32_t res;
    if (abs_a > abs_b) {
        res = abs_a - abs_b;
        res |= sign_a;
    } else {
        res = abs_b - abs_a;
        res |= NEG_Q16((q16)sign_a);
    }

    return (q16)res;
}

q16 SUB_Q16(q16 a, q16 b) {
    uint32_t res;
    uint32_t sign_a = SGN_Q16(a);
    uint32_t sign_b = SGN_Q16(b);
    if (sign_a != sign_b) {
        res = a + b;
        if (!(res & 0x80000000)) {
            LIBQ16_LOG_ERR("sfixed_15_16 subtraction overflow\n");
            return (q16)(SFIXED_MAX | sign_a);
        }
        return (q16)(ABS_Q16((q16)res) | sign_a);
    }

    uint32_t abs_a = ABS_Q16(a);
    uint32_t abs_b = ABS_Q16(b);
    if (abs_a == abs_b)
        return (q16)0;

    if (abs_a > abs_b) {
        res = abs_a - abs_b;
        res |= sign_a;
    } else {
        res = abs_b - abs_a;
        res |= NEG_Q16((q16)sign_a);
    }

    return (q16)res;
}

uq16 MUL_UQ16(uq16 a, uq16 b) {
    uint64_t res = (uint64_t)a * (uint64_t)b;

    if (res >> 48) {
        LIBQ16_LOG_ERR("ufixed_16_16 multiplication overflow\n");
        return UFIXED_MAX;
    }

    res = (res >> 16) + ((res & 0x8000) >> 15);
    return (uq16)res;
}

q16 MUL_Q16_UNSAFE(q16 a, q16 b) {
    uint32_t sign = SGN_Q16((q16)(a ^ b));
    uint64_t res = (uint64_t)ABS_Q16(a) * (uint64_t)ABS_Q16(b);
    res = (res >> 16) + ((res & 0x8000) >> 15);
    return (q16)(res | sign);
}

q16 MUL_Q16(q16 a, q16 b) {
    uint32_t sign = SGN_Q16((q16)(a ^ b));
    uint64_t res = (uint64_t)ABS_Q16(a) * (uint64_t)ABS_Q16(b);

    if (res >> 47) {
        LIBQ16_LOG_ERR("sfixed_15_16 multiplication overflow\n");
        return (q16)(SFIXED_MAX | sign);
    }

    res = (res >> 16) + ((res & 0x8000) >> 15);
    return (q16)(res | sign);
}

uq16 DIV_UQ16_UNSAFE(uq16 a, uq16 b) {
    return (uq16)((((uint64_t)a << 16) + (b >> 1)) / b);
}

uq16 DIV_UQ16(uq16 a, uq16 b) {
    if (b == 0) {
        LIBQ16_LOG_ERR("ufixed_16_16 division by zero\n");
        return UFIXED_MAX;
    }

    uint64_t res = (((uint64_t)a << 16) + (b >> 1)) / b;

    if (res >> 32) {
        LIBQ16_LOG_ERR("ufixed_16_16 division overflow\n");
        return UFIXED_MAX;
    }

    return (uq16)res;
}

q16 DIV_Q16_UNSAFE(q16 a, q16 b) {
    uint32_t sign = SGN_Q16((q16)(a ^ b));
    uint64_t abs_a = ABS_Q16(a);
    uint32_t abs_b = ABS_Q16(b);
    uint32_t res = ((abs_a << 16) + (abs_b >> 1)) / abs_b;
    return (q16)(res | sign);
}

q16 DIV_Q16(q16 a, q16 b) {
    if (ABS_Q16(b) == 0) {
        LIBQ16_LOG_ERR("sfixed_15_16 division by zero\n");
        if (ABS_Q16(a) == 0)
            return SFIXED_NAN;
        return (q16)(SFIXED_MAX | SGN_Q16(a));
    }

    uint32_t sign = SGN_Q16((q16)(a ^ b));
    uint64_t abs_a = ABS_Q16(a);
    uint32_t abs_b = ABS_Q16(b);
    uint64_t res = ((abs_a << 16) + (abs_b >> 1)) / abs_b;

    if (res >> 31) {
        LIBQ16_LOG_ERR("sfixed_15_16 division overflow\n");
        return (q16)(SFIXED_MAX | sign);
    }

    return (q16)(res | sign);
}

#if !__has_builtin(__builtin_clzg)
#define __builtin_clzg(x) \
    _Generic((x), unsigned int: __builtin_clz(x), unsigned long: __builtin_clzl(x), unsigned long long: __builtin_clzll(x))
#endif

#ifndef LIBQ16_USE_FPU

uq16 SQRT_UQ16(uq16 n) {
    // Uses a modified libfixmath (MIT-licensed) implementation that avoids 64-bit operations.
    // Copyright (c) 2011-2021 libfixmath AUTHORS

    if (n == 0)
        return (uq16)0;

    uint32_t x = n;
    uint32_t c = 0;
    uint32_t d = 1 << ((__builtin_clzg(x) ^ 0x1f) & 0x1e);

    while (d) {
        if (x >= c + d) {
            x -= c + d;
            c = (c >> 1) + d;
        } else {
            c = (c >> 1);
        }

        d >>= 2;
    }

    if (x >> 16) {
        x -= c;
        x = (x << 16) - 0x8000;
        c = (c << 16) + 0x8000;
    } else {
        x <<= 16;
        c <<= 16;
    }

    d = 1 << 14;

    while (d) {
        if (x >= c + d) {
            x -= c + d;
            c = (c >> 1) + d;
        } else {
            c = (c >> 1);
        }

        d >>= 2;
    }

    if (x > c)
        c++;

    return (uq16)c;
}

uq16 RSQRT_UQ16_UNSAFE(uq16 n) {
    uint32_t res;
    uint64_t quasi_one_q48;
    uint32_t quasi_one_half;

    if (n <= 0x6ee8) { // max value that doesn't work
        res = SQRT_UQ16((uq16)(0xffffffff / n));
    } else {
        uint32_t lz = __builtin_clzg((uint32_t)n);
        res = 0x100 << (lz >> 1);
        res = res | (0x40 << ((lz + 1) >> 1)); // dirty but accurate guess

        for (int i = 0; i < 3; i++) { // 32-bit Newton-Raphson iterations, should be unrolled by compiler
            quasi_one_q48 = (uint64_t)res * (uint64_t)res * (uint64_t)n;
            quasi_one_half = quasi_one_q48 >> 34; // Q.15
            res = (res * (0xc000 - quasi_one_half)) >> 15;
        }

        quasi_one_q48 = (uint64_t)res * (uint64_t)res * (uint64_t)n; // 64-bit Newton-Raphson iteration
        quasi_one_half = quasi_one_q48 >> 33;
        res = ((uint64_t)res * (uint64_t)(0x18000 - quasi_one_half)) >> 16;
    }

    // correct off-by-one errors
    quasi_one_q48 = (uint64_t)res * (uint64_t)res * (uint64_t)n;
    quasi_one_q48 >>= 48;

    uint64_t res_alt = res - ((quasi_one_q48 - 1) | 1);
    uint64_t res_sum = res + res_alt;
    uint64_t quasi_four_q48 = res_sum * res_sum * (uint64_t)n;

    return (quasi_four_q48 >> 50 != quasi_one_q48) ? (uq16)res : (uq16)res_alt;
}

#else

uq16 SQRT_UQ16(uq16 n) {
    double res = std::sqrt((double)((uint64_t)n << 16));
    return (uq16)(res + 0.5);
}

uq16 RSQRT_UQ16_UNSAFE(uq16 n) {
    double res = 16777216.0 / std::sqrt((double)n);
    return (uq16)(res + 0.5);
}

#endif // LIBQ16_USE_FPU

// Calculates both the sine and cosine of an angle measured in rotations.
// Angle of vector spanned by result is guaranteed to be correct.
// Length of vector may not be exactly one.
SC_Q16 SINCOS_UQ16(uq16 n) {
    int32_t _n = (int32_t)n;
    uq16 norm = (uq16)std::abs((_n & 0x2000) - (_n & 0x1fff));
    uint16_t s = n ^ (n << 1);

    // qcos(x) = d - x^2(c - x^2(b - ax^2))
    // remez minimax polynomial on [-1/8, 1/8]
    // a:q16 = 0x5397b5, b = 0x40ebd73ac5, c = 0x13bd391496, d = 0xffffff8a
    // max err = 1, occurs 10/8192 angles
    uint64_t norm_sq_q32 = norm * norm;
    uint64_t poly_q32 = 0x40ebd73ac5 - ((0x5397b5 * norm_sq_q32) >> 16);
    poly_q32 = 0x13bd391496 - ((poly_q32 * norm_sq_q32) >> 32);
    poly_q32 = 0xffffff8a - ((poly_q32 * norm_sq_q32) >> 32);
    q16 cos = (q16)((poly_q32 >> 16) + ((poly_q32 & 0x8000) >> 15));

    // qsin(x) = x(c - x^2(b - ax^2))
    // remez minimax polynomial on [-1/8, 1/8]
    // a:q16 = 0x4f880b, b = 0x295358621e, c = 0x6487cc5bd
    // max err = 1, occurs 184/8192 angles
    poly_q32 = 0x295358621e - ((0x4f880b * norm_sq_q32) >> 16);
    poly_q32 = 0x6487cc5bd - ((poly_q32 * norm_sq_q32) >> 32);
    poly_q32 *= norm;
    q16 sin = (q16)((poly_q32 >> 32) + ((poly_q32 & 0x80000000) >> 31));

    if (s & 0x4000) {
        q16 temp = cos;
        cos = sin;
        sin = temp;
    }

    if (n & 0x8000 && sin != 0)
        sin = NABS_Q16(sin);
    if (s & 0x8000 && cos != 0)
        cos = NABS_Q16(cos);

    return { cos, sin };
}

#ifndef LIBQ16_USE_FPU

static uint32_t __SQRT_NEWTON_32_R16(uint32_t n, uint32_t guess) {
    uint32_t x = guess;
    uint32_t y;

    for (int i = 0; i < 2; i++) { // should be unrolled
        y = n / x;
        x = (x + y) >> 1;
    }

    if (((x * x + x) << 1) - n < n)
        return x + 1;
    return x;
}

#else

static uint32_t __SQRT_32_R16(uint32_t n) {
    float res = std::sqrt((double)n);
    return (uq16)(res + 0.5);
}

#endif // LIBQ16_USE_FPU

// Calculates both the sine and cosine of an angle measured in rotations.
// Vector spanned by result is guaranteed to be normalized to exactly one.
// Angle of vector may not be correct.
SC_Q16 SINCOS_UQ16_NORM(uq16 n) {
    int32_t _n = (int32_t)n;
    uq16 norm = (uq16)std::abs((_n & 0x2000) - (_n & 0x1fff));
    uint16_t s = n ^ (n << 1);

    // qcos(x) = d - x^2(c - x^2(b - ax^2))
    // remez minimax polynomial on [-1/8, 1/8]
    // a:q16 = 0x5397b5, b = 0x40ebd73ac5, c = 0x13bd391496, d = 0xffffff8a
    // max err = 1, occurs 10/8192 angles
    uint64_t norm_sq_q32 = norm * norm;
    uint64_t poly_q32 = 0x40ebd73ac5 - ((0x5397b5 * norm_sq_q32) >> 16);
    poly_q32 = 0x13bd391496 - ((poly_q32 * norm_sq_q32) >> 32);
    poly_q32 = 0xffffff8a - ((poly_q32 * norm_sq_q32) >> 32);
    q16 cos = (q16)((poly_q32 >> 16) + ((poly_q32 & 0x8000) >> 15));
    q16 sin;
#ifndef LIBQ16_USE_FPU
    uint32_t sin_guess;
#endif

    if (cos == 65536) {
        sin = (q16)0;
        goto fold;
    }

#ifndef LIBQ16_USE_FPU
    sin_guess = 6 * norm;
    sin = (q16)__SQRT_NEWTON_32_R16(-(cos * cos), sin_guess);
#else
    sin = (q16)__SQRT_32_R16(-(cos * cos));
#endif

fold:
    if (s & 0x4000) {
        q16 temp = cos;
        cos = sin;
        sin = temp;
    }

    if (n & 0x8000 && sin != 0)
        sin = NABS_Q16(sin);
    if (s & 0x8000 && cos != 0)
        cos = NABS_Q16(cos);

    return { cos, sin };
}

uq16 ATAN2_Q16_UNSAFE(q16 y, q16 x) {
    uint32_t abs_y = ABS_Q16(y);
    uint32_t abs_x = ABS_Q16(x);

    uint32_t res = 0;
    uint32_t negate = 0;
    uint64_t quotient_q32;

    // abs_y > 2.5 * abs_x
    // rotate 90 degrees
    if (abs_y >> 1 > abs_x + (abs_x >> 2)) {
        res = 0x4000;
        negate = 0xffffffff;
        quotient_q32 = ((uint64_t)abs_x << 32) / abs_y;
        goto poly;
    }

    // abs_x <= 2.5 * abs_y
    // rotate 45 degrees
    if (abs_x >> 1 <= abs_y + (abs_y >> 2)) {
        res = 0x2000;
        int32_t tmp = (int32_t)abs_y - (int32_t)abs_x;
        abs_x += abs_y;
        negate = tmp >> 31;
        abs_y = (uint32_t)(tmp ^ negate) - negate;
    }

    quotient_q32 = ((uint64_t)abs_y << 32) / abs_x;

poly:
    // qatan(x) = x(c - x^2(b - ax^2))
    // remez minimax polynomial on [-3/7, 3/7]
    // a = 0x693cc22, b = 0xd72318d, c = 0x28bd9daa
    // max err = 1, occurs about 3% of the time
    uint64_t quotient_sq_q32 = (quotient_q32 * quotient_q32) >> 32;
    uint64_t poly_q32 = 0xd72318d - ((0x693cc22 * quotient_sq_q32) >> 32);
    poly_q32 = 0x28bd9daa - ((poly_q32 * quotient_sq_q32) >> 32);
    poly_q32 *= quotient_q32;

    res += (((poly_q32 >> 48) + ((poly_q32 & 0x800000000000) >> 47)) ^ negate) - negate;
    if (SGN_Q16(x))
        res = 0x8000 - res;
    if (SGN_Q16(y))
        res = 0x10000 - res;

    return (uq16)res;
}

#undef __builtin_clzg

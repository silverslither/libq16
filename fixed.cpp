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

q16 UQ16_TO_Q16(uq16 n) {
    if (n & 0x80000000) {
        LIBQ16_LOG_ERR("ufixed_16_16 to sfixed_15_16 conversion overflow\n");
        return SFIXED_MAX;
    }

    return (q16)n;
}

uq16 Q16_TO_UQ16(uq16 n) {
    if (n & 0x80000000) {
        LIBQ16_LOG_ERR("sfixed_15_16 to ufixed_16_16 conversion underflow\n");
        return UFIXED_MAX;
    }

    return (uq16)n;
}

bool CMP_GT_Q16(q16 a, q16 b) {
    if (SGN_Q16(a) < SGN_Q16(b))
        return true;
    else if (SGN_Q16(a) > SGN_Q16(b))
        return false;
    else
        return SGN_Q16(a) ? ABS_Q16(a) < ABS_Q16(b) : ABS_Q16(a) > ABS_Q16(b);
}

bool CMP_LT_Q16(q16 a, q16 b) {
    if (SGN_Q16(a) > SGN_Q16(b))
        return true;
    else if (SGN_Q16(a) < SGN_Q16(b))
        return false;
    else
        return SGN_Q16(a) ? ABS_Q16(a) > ABS_Q16(b) : ABS_Q16(a) < ABS_Q16(b);
}

bool CMP_GEQ_Q16(q16 a, q16 b) {
    if (SGN_Q16(a) < SGN_Q16(b))
        return true;
    else if (SGN_Q16(a) > SGN_Q16(b))
        return false;
    else
        return SGN_Q16(a) ? ABS_Q16(a) <= ABS_Q16(b) : ABS_Q16(a) >= ABS_Q16(b);
}

bool CMP_LEQ_Q16(q16 a, q16 b) {
    if (SGN_Q16(a) > SGN_Q16(b))
        return true;
    else if (SGN_Q16(a) < SGN_Q16(b))
        return false;
    else
        return SGN_Q16(a) ? ABS_Q16(a) >= ABS_Q16(b) : ABS_Q16(a) <= ABS_Q16(b);
}

uq16 ADD_UQ16(uq16 a, uq16 b) {
    uint32_t res;

    if (__builtin_add_overflow((uint32_t)a, (uint32_t)b, &res)) {
        LIBQ16_LOG_ERR("ufixed_16_16 addition overflow\n");
        return UFIXED_MAX;
    }

    return (uq16)res;
}

q16 ADD_Q16_UNSAFE(q16 a, q16 b) {
    uint8_t diff = (a ^ b) >> 31;

    if (diff) {
        uint32_t abs_a = ABS_Q16(a);
        uint32_t abs_b = ABS_Q16(b);

        if (abs_a >= abs_b) {
            uint32_t res = abs_a - abs_b;
            if (res != 0)
                res |= SGN_Q16(a);
            return (q16)res;
        } else {
            uint32_t res = abs_b - abs_a;
            if (res != 0)
                res |= SGN_Q16(b);
            return (q16)res;
        }
    }

    return (q16)((a + b) | SGN_Q16(a));
}

q16 ADD_Q16(q16 a, q16 b) {
    uint8_t diff = (a ^ b) >> 31;

    if (diff) {
        uint32_t abs_a = ABS_Q16(a);
        uint32_t abs_b = ABS_Q16(b);

        if (abs_a >= abs_b) {
            uint32_t res = abs_a - abs_b;
            if (res != 0)
                res |= SGN_Q16(a);
            return (q16)res;
        } else {
            uint32_t res = abs_b - abs_a;
            if (res != 0)
                res |= SGN_Q16(b);
            return (q16)res;
        }
    }

    uint32_t res = a + b;
    if (res & 0x80000000) {
        LIBQ16_LOG_ERR("sfixed_15_16 addition overflow\n");
        return (q16)(0x7fffffff | SGN_Q16(a));
    }

    return (q16)(res | SGN_Q16(a));
}

uq16 SUB_UQ16(uq16 a, uq16 b) {
    if (b > a) {
        LIBQ16_LOG_ERR("ufixed_16_16 subtraction underflow\n");
        return UFIXED_MIN;
    }

    return (uq16)(a - b);
}

q16 SUB_Q16_UNSAFE(q16 a, q16 b) {
    uint8_t diff = (a ^ b) >> 31;

    if (!diff) {
        uint32_t abs_a = ABS_Q16(a);
        uint32_t abs_b = ABS_Q16(b);

        if (abs_a >= abs_b) {
            uint32_t res = abs_a - abs_b;
            if (res != 0)
                res |= SGN_Q16(a);
            return (q16)res;
        } else {
            uint32_t res = abs_b - abs_a;
            if (res != 0)
                res |= NEG_Q16(SGN_Q16(b));
            return (q16)res;
        }
    }

    return (q16)(ABS_Q16((q16)(a + b)) | SGN_Q16(a));
}

q16 SUB_Q16(q16 a, q16 b) {
    uint8_t diff = (a ^ b) >> 31;

    if (!diff) {
        uint32_t abs_a = ABS_Q16(a);
        uint32_t abs_b = ABS_Q16(b);

        if (abs_a >= abs_b) {
            uint32_t res = abs_a - abs_b;
            if (res != 0)
                res |= SGN_Q16(a);
            return (q16)res;
        } else {
            uint32_t res = abs_b - abs_a;
            if (res != 0)
                res |= NEG_Q16(SGN_Q16(b));
            return (q16)res;
        }
    }

    uint32_t res = a + b;
    if (!(res & 0x80000000)) {
        LIBQ16_LOG_ERR("sfixed_15_16 subtraction overflow\n");
        return (q16)(0x7fffffff | SGN_Q16(a));
    }

    return (q16)(ABS_Q16((q16)res) | SGN_Q16(a));
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
    uint32_t sign = (a ^ b) & 0x80000000;
    uint64_t res = (uint64_t)ABS_Q16(a) * (uint64_t)ABS_Q16(b);
    res = (res >> 16) + ((res & 0x8000) >> 15);
    return (q16)(res | sign);
}

q16 MUL_Q16(q16 a, q16 b) {
    uint32_t sign = (a ^ b) & 0x80000000;
    uint64_t res = (uint64_t)ABS_Q16(a) * (uint64_t)ABS_Q16(b);

    if (res >> 47) {
        LIBQ16_LOG_ERR("sfixed_15_16 multiplication overflow\n");
        return (q16)(0x7fffffff | sign);
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
    uint32_t sign = (a ^ b) & 0x80000000;
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
        return (q16)(0x7fffffff | SGN_Q16(a));
    }

    uint32_t sign = (a ^ b) & 0x80000000;
    uint64_t abs_a = ABS_Q16(a);
    uint32_t abs_b = ABS_Q16(b);
    uint64_t res = ((abs_a << 16) + (abs_b >> 1)) / abs_b;

    if (res >> 31) {
        LIBQ16_LOG_ERR("sfixed_15_16 division overflow\n");
        return (q16)(0x7fffffff | sign);
    }

    return (q16)(res | sign);
}

#if !__has_builtin(__builtin_clzg)
#define __builtin_clzg(x) \
    _Generic((x), unsigned int: __builtin_clz(x), unsigned long: __builtin_clzl(x), unsigned long long: __builtin_clzll(x))
#endif

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
    // Mostly exact, except edge cases where result is right in between two integers

    uint32_t res;
    uint64_t quasi_one_q48;
    uint32_t quasi_one_half;

    if (n <= 0x71c0) { // minimum value that works
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
    int64_t quasi_diff_q48 = (int64_t)quasi_one_q48 - 0x1000000000000;

    int64_t res_alt = (int64_t)res - ((quasi_diff_q48 >> 63) | 1);
    int64_t quasi_one_q48_alt = res_alt * res_alt * (int64_t)n;

    return (std::abs(quasi_diff_q48) < std::abs(quasi_one_q48_alt - 0x1000000000000)) ? (uq16)res : (uq16)res_alt;
}

static uq16 __SQRT_32_R16(uint32_t n) {
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

    if (x > c)
        c++;

    return (uq16)c;
}

static uint32_t __MUL_32_32_R16(uint64_t a, uint64_t b) {
    uint64_t res = a * b;
    res = (res >> 48) + ((res & 0x800000000000) >> 47);
    return (uq16)res;
}

// Calculates both the sine and cosine of an angle measured in rotations.
SC_Q16 SINCOS_UQ16(uq16 n) {
    // Algorithm by SilasLock, originally developed for Kaze Emanuar's SM64 optimization project.

    int32_t _n = (int32_t)n;
    uq16 norm = (uq16)std::abs((_n & 0x2000) - (_n & 0x1fff));
    uint16_t s = n ^ (n << 1);

    // qcos(x) = 1 - (b - ax^2)x^2
    // optimal constants simulated with a ~= 63, b = a / 64 + 32(2 - sqrt(2))
    // a = 0x3f1188, b = 0x13bb095302 (lowest total err)
    // a = 0x3ef743, b = 0x13baa03f02 (lowest total err for max err = 1)
    uint64_t norm_sq_q32 = norm * norm;
    uint64_t inner_q32 = 0x13baa03f02 - ((0x3ef743 * norm_sq_q32) >> 16);
    uq16 cos = (uq16)(0x10000 - __MUL_32_32_R16(inner_q32, norm_sq_q32));
    uq16 sin = __SQRT_32_R16(-(cos * cos));

    if (s & 0x4000) {
        uq16 temp = cos;
        cos = sin;
        sin = temp;
    }

    if (n & 0x8000 && sin != 0)
        sin = NABS_Q16((q16)sin);
    if (s & 0x8000 && cos != 0)
        cos = NABS_Q16((q16)cos);

    return SC_Q16{ (q16)sin, (q16)cos };
}

uq16 ATAN2_Q16_UNSAFE(q16 y, q16 x) {
    // Max error of 1 about 3% of the time

    uint32_t abs_y = y & 0x7fffffff;
    uint32_t abs_x = x & 0x7fffffff;

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
    uint64_t quotient_sq_q32 = (quotient_q32 * quotient_q32) >> 32;
    uint64_t poly_q32 = 0xd72318d - ((0x693cc22 * quotient_sq_q32) >> 32);
    poly_q32 = 0x28bd9daa - ((poly_q32 * quotient_sq_q32) >> 32);

    res += (__MUL_32_32_R16(poly_q32, quotient_q32) ^ negate) - negate;
    if (x & 0x80000000)
        res = 0x8000 - res;
    if (y & 0x80000000)
        res = 0x10000 - res;

    return (uq16)res;
}

#undef __builtin_clzg

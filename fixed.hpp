// Copyright (c) 2024 silverslither.

#ifndef __LIBQ16_FIXED_HPP
#define __LIBQ16_FIXED_HPP

#include <bit>
#include <cmath>
#include <cstdint>
#include <cstdio>

#ifndef NDEBUG
// Placeholder logging function
#define LIBQ16_LOG_ERR(str) fprintf(stderr, str)
#else
#define LIBQ16_LOG_ERR(ignore) ((void)0)
#endif

enum ufixed_16_16 : uint32_t {};
enum sfixed_15_16 : uint32_t {};
typedef ufixed_16_16 uq16;
typedef sfixed_15_16 q16;

#define UFIXED_MIN (uq16)0
#define UFIXED_MAX (uq16)0xffffffff

#define SFIXED_MIN (q16)0xffffffff
#define SFIXED_MAX (q16)0x7fffffff
#define SFIXED_NAN (q16)0x80000000

#define __LIBQ16_ALWAYS_INLINE static inline __attribute__((always_inline))

struct SC_Q16 {
    q16 cos;
    q16 sin;
};

constexpr __LIBQ16_ALWAYS_INLINE uq16 F64_TO_UQ16_UNSAFE(double n) {
    return (uq16)(65536.0 * n + 0.5);
}
constexpr __LIBQ16_ALWAYS_INLINE q16 F64_TO_Q16_UNSAFE(double n) {
    double res = std::bit_cast<double>(std::bit_cast<uint64_t>(65536.0 * n) & 0x7fffffffffffffff) + 0.5;
    return (q16)(((uint32_t)((std::bit_cast<uint64_t>(n) & 0x8000000000000000) >> 32)) | (uint32_t)res);
}
uq16 F64_TO_UQ16(double n);
q16 F64_TO_Q16(double n);

constexpr __LIBQ16_ALWAYS_INLINE double UQ16_TO_F64(uq16 n) {
    return (double)n / 65536.0;
}
constexpr __LIBQ16_ALWAYS_INLINE double Q16_TO_F64(q16 n) {
    double res = (double)(n & 0x7fffffff) / 65536.0;
    return std::bit_cast<double>(((uint64_t)(n & 0x80000000) << 32) | std::bit_cast<uint64_t>(res));
}

__LIBQ16_ALWAYS_INLINE q16 ABS_Q16(q16 n) {
    return (q16)(n & 0x7fffffff);
}
__LIBQ16_ALWAYS_INLINE q16 NABS_Q16(q16 n) {
    return (q16)(n | 0x80000000);
}
__LIBQ16_ALWAYS_INLINE q16 NEG_Q16(q16 n) {
    return (q16)(n ^ 0x80000000);
}
__LIBQ16_ALWAYS_INLINE q16 SGN_Q16(q16 n) {
    return (q16)(n & 0x80000000);
}

constexpr __LIBQ16_ALWAYS_INLINE q16 UQ16_TO_Q16_UNSAFE(uq16 n) {
    return (q16)n;
}
constexpr __LIBQ16_ALWAYS_INLINE uq16 Q16_TO_UQ16_UNSAFE(q16 n) {
    return (uq16)n;
}
__LIBQ16_ALWAYS_INLINE q16 UQ16_TO_Q16(uq16 n) {
    if (SGN_Q16((q16)n)) {
        LIBQ16_LOG_ERR("ufixed_16_16 to sfixed_15_16 conversion overflow\n");
        return SFIXED_MAX;
    }
    return (q16)n;
}
__LIBQ16_ALWAYS_INLINE uq16 Q16_TO_UQ16(q16 n) {
    if (SGN_Q16(n)) {
        LIBQ16_LOG_ERR("sfixed_15_16 to ufixed_16_16 conversion underflow\n");
        return UFIXED_MAX;
    }
    return (uq16)n;
}

__LIBQ16_ALWAYS_INLINE bool CMP_EQ_UQ16(uq16 a, uq16 b) {
    return a == b;
}
__LIBQ16_ALWAYS_INLINE bool CMP_EQ_Q16(q16 a, q16 b) {
    return a == b;
}

__LIBQ16_ALWAYS_INLINE bool CMP_GT_UQ16(uq16 a, uq16 b) {
    return a > b;
}
__LIBQ16_ALWAYS_INLINE bool CMP_LT_UQ16(uq16 a, uq16 b) {
    return a < b;
}
__LIBQ16_ALWAYS_INLINE bool CMP_GEQ_UQ16(uq16 a, uq16 b) {
    return a >= b;
}
__LIBQ16_ALWAYS_INLINE bool CMP_LEQ_UQ16(uq16 a, uq16 b) {
    return a <= b;
}

bool CMP_GEQ_Q16(q16 a, q16 b);
bool CMP_LEQ_Q16(q16 a, q16 b);
__LIBQ16_ALWAYS_INLINE bool CMP_GT_Q16(q16 a, q16 b) {
    return !CMP_LEQ_Q16(a, b);
};
__LIBQ16_ALWAYS_INLINE bool CMP_LT_Q16(q16 a, q16 b) {
    return !CMP_GEQ_Q16(a, b);
};

__LIBQ16_ALWAYS_INLINE uq16 MIN_UQ16(uq16 a, uq16 b) {
    return (uq16)std::min((uint32_t)a, (uint32_t)b);
}
__LIBQ16_ALWAYS_INLINE uq16 MAX_UQ16(uq16 a, uq16 b) {
    return (uq16)std::max((uint32_t)a, (uint32_t)b);
}
__LIBQ16_ALWAYS_INLINE q16 MIN_Q16(q16 a, q16 b) {
    if ((a & b) >> 31)
        return (q16)std::max((uint32_t)a, (uint32_t)b);
    return (q16)std::min((int32_t)a, (int32_t)b);
}
__LIBQ16_ALWAYS_INLINE q16 MAX_Q16(q16 a, q16 b) {
    if ((a & b) >> 31)
        return (q16)std::min((uint32_t)a, (uint32_t)b);
    return (q16)std::max((int32_t)a, (int32_t)b);
}

__LIBQ16_ALWAYS_INLINE uq16 ADD_UQ16_UNSAFE(uq16 a, uq16 b) {
    return (uq16)(a + b);
}
__LIBQ16_ALWAYS_INLINE uq16 ADD_UQ16(uq16 a, uq16 b) {
    uint32_t res;
    if (__builtin_add_overflow((uint32_t)a, (uint32_t)b, &res)) {
        LIBQ16_LOG_ERR("ufixed_16_16 addition overflow\n");
        return UFIXED_MAX;
    }
    return (uq16)res;
}
q16 ADD_Q16_UNSAFE(q16 a, q16 b);
q16 ADD_Q16(q16 a, q16 b);

__LIBQ16_ALWAYS_INLINE uq16 SUB_UQ16_UNSAFE(uq16 a, uq16 b) {
    return (uq16)(a - b);
}
__LIBQ16_ALWAYS_INLINE uq16 SUB_UQ16(uq16 a, uq16 b) {
    uint32_t res;
    if (__builtin_sub_overflow((uint32_t)a, (uint32_t)b, &res)) {
        LIBQ16_LOG_ERR("ufixed_16_16 subtraction underflow\n");
        return UFIXED_MIN;
    }
    return (uq16)(a - b);
}
q16 SUB_Q16_UNSAFE(q16 a, q16 b);
q16 SUB_Q16(q16 a, q16 b);

__LIBQ16_ALWAYS_INLINE uq16 MUL_UQ16_UNSAFE(uq16 a, uq16 b) {
    uint64_t res = (uint64_t)a * (uint64_t)b;
    return (uq16)((res >> 16) + ((res & 0x8000) >> 15));
}
uq16 MUL_UQ16(uq16 a, uq16 b);
q16 MUL_Q16_UNSAFE(q16 a, q16 b);
q16 MUL_Q16(q16 a, q16 b);

uq16 DIV_UQ16_UNSAFE(uq16 a, uq16 b);
uq16 DIV_UQ16(uq16 a, uq16 b);
q16 DIV_Q16_UNSAFE(q16 a, q16 b);
q16 DIV_Q16(q16 a, q16 b);

__LIBQ16_ALWAYS_INLINE uq16 MOD_UQ16_UNSAFE(uq16 a, uq16 b) {
    return (uq16)(a % b);
}
__LIBQ16_ALWAYS_INLINE uq16 MOD_UQ16(uq16 a, uq16 b) {
    return (b == 0) ? a : (uq16)(a % b);
}
__LIBQ16_ALWAYS_INLINE q16 MOD_Q16_UNSAFE(q16 a, q16 b) {
    uint32_t res = ABS_Q16(a) % ABS_Q16(b);
    if (res == 0)
        return (q16)0;
    return (q16)(res | SGN_Q16(a));
}
__LIBQ16_ALWAYS_INLINE q16 MOD_Q16(q16 a, q16 b) {
    if (b == 0)
        return a;
    uint32_t res = ABS_Q16(a) % ABS_Q16(b);
    if (res == 0)
        return (q16)0;
    return (q16)(res | SGN_Q16(a));
}

uq16 SQRT_UQ16(uq16 n);
__LIBQ16_ALWAYS_INLINE q16 SQRT_Q16(q16 n) {
    return (q16)(SGN_Q16(n) | (uint32_t)SQRT_UQ16((uq16)ABS_Q16(n)));
}

uq16 RSQRT_UQ16_UNSAFE(uq16 n);
__LIBQ16_ALWAYS_INLINE q16 RSQRT_Q16_UNSAFE(q16 n) {
    return (q16)(SGN_Q16(n) | (uint32_t)RSQRT_UQ16_UNSAFE((uq16)ABS_Q16(n)));
}

SC_Q16 SINCOS_UQ16(uq16 n);
__LIBQ16_ALWAYS_INLINE SC_Q16 SINCOS_Q16(q16 n) {
    return SGN_Q16(n) ? SINCOS_UQ16((uq16)(0x80000000 - ABS_Q16(n))) : SINCOS_UQ16((uq16)n);
}
SC_Q16 SINCOS_UQ16_NORM(uq16 n);
__LIBQ16_ALWAYS_INLINE SC_Q16 SINCOS_Q16_NORM(q16 n) {
    return SGN_Q16(n) ? SINCOS_UQ16_NORM((uq16)(0x80000000 - ABS_Q16(n))) : SINCOS_UQ16_NORM((uq16)n);
}

uq16 ATAN2_Q16_UNSAFE(q16 y, q16 x);

#undef __LIBQ16_ALWAYS_INLINE

#endif // __LIBQ16_FIXED_HPP

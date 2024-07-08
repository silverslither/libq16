// Copyright (c) 2024 silverslither.

#ifndef __LIBQ16_FIXED_HPP
#define __LIBQ16_FIXED_HPP

#include <bit>
#include <cmath>
#include <cstdint>
#include <cstdio>

// Placeholder log function
#define LIBQ16_LOG_ERR(str) fprintf(stderr, str)

#ifndef LIBQ16_NO_OPAQUE_TYPEDEFS

// WARNING: Likely not transparent! Only for LSP type checking. Production builds should define LIBQ16_NO_OPAQUE_TYPEDEFS.
#define __INT_OPAQUE_TYPEDEF(T, D)                  \
    struct D {                                      \
        T t;                                        \
        explicit D(const T &_t) noexcept : t(_t) {} \
        D() noexcept : t(0) {}                      \
        D(const D &_d) noexcept = default;          \
        D &operator=(const D &rhs) noexcept {       \
            t = rhs.t;                              \
            return *this;                           \
        }                                           \
        D &operator=(const T &rhs) noexcept {       \
            t = rhs;                                \
            return *this;                           \
        }                                           \
        operator const T &() const { return t; }    \
        operator T &() { return t; }                \
    }

__INT_OPAQUE_TYPEDEF(uint32_t, ufixed_16_16);
__INT_OPAQUE_TYPEDEF(uint32_t, sfixed_15_16);
#undef __INT_OPAQUE_TYPEDEF

#else

typedef uint32_t ufixed_16_16;
typedef uint32_t sfixed_15_16;

#endif // LIBQ16_NO_OPAQUE_TYPEDEFS

typedef ufixed_16_16 uq16;
typedef sfixed_15_16 q16;

#define UFIXED_MIN (uq16)0
#define UFIXED_MAX (uq16)0xffffffff

#define SFIXED_MIN (q16)0xffffffff
#define SFIXED_MAX (q16)0x7fffffff
#define SFIXED_NAN (q16)0x80000000

#define __LIBQ16_ALWAYS_INLINE static inline __attribute__((always_inline))

struct SC_Q16 {
    q16 sin;
    q16 cos;
};

__LIBQ16_ALWAYS_INLINE uq16 F64_TO_UQ16_UNSAFE(double n) {
    return (uq16)std::round(65536.0 * n);
}
uq16 F64_TO_UQ16(double n);
q16 F64_TO_Q16_UNSAFE(double n);
q16 F64_TO_Q16(double n);

__LIBQ16_ALWAYS_INLINE double UQ16_TO_F64(uq16 n) {
    return (double)n / 65536.0;
}
__LIBQ16_ALWAYS_INLINE double Q16_TO_F64(q16 n) {
    double res = (double)(n & 0x7fffffff) / 65536.0;
    return std::bit_cast<double>(((uint64_t)(n & 0x80000000) << 32) | std::bit_cast<uint64_t>(res));
}

__LIBQ16_ALWAYS_INLINE q16 UQ16_TO_Q16_UNSAFE(uq16 n) {
    return (q16)n;
}
__LIBQ16_ALWAYS_INLINE uq16 Q16_TO_UQ16_UNSAFE(q16 n) {
    return (uq16)n;
}

q16 UQ16_TO_Q16(uq16 n);
uq16 Q16_TO_UQ16(q16 n);

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

bool CMP_GT_Q16(uq16 a, uq16 b);
bool CMP_LT_Q16(uq16 a, uq16 b);
bool CMP_GEQ_Q16(uq16 a, uq16 b);
bool CMP_LEQ_Q16(uq16 a, uq16 b);

__LIBQ16_ALWAYS_INLINE uq16 ADD_UQ16_UNSAFE(uq16 a, uq16 b) {
    return (uq16)(a + b);
}
uq16 ADD_UQ16(uq16 a, uq16 b);
q16 ADD_Q16_UNSAFE(q16 a, q16 b);
q16 ADD_Q16(q16 a, q16 b);

__LIBQ16_ALWAYS_INLINE uq16 SUB_UQ16_UNSAFE(uq16 a, uq16 b) {
    return (uq16)(a - b);
}
uq16 SUB_UQ16(uq16 a, uq16 b);
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
    return (q16)((ABS_Q16(a) % ABS_Q16(b)) | SGN_Q16(a));
}
__LIBQ16_ALWAYS_INLINE q16 MOD_Q16(q16 a, q16 b) {
    return (b == 0) ? a : (q16)((ABS_Q16(a) % ABS_Q16(b)) | SGN_Q16(a));
}

uq16 SQRT_UQ16(uq16 n);
__LIBQ16_ALWAYS_INLINE q16 SQRT_Q16(q16 n) {
    return (q16)(SGN_Q16(n) | SQRT_UQ16((uq16)ABS_Q16(n)));
}

uq16 RSQRT_UQ16_UNSAFE(uq16 n);
__LIBQ16_ALWAYS_INLINE q16 RSQRT_Q16_UNSAFE(q16 n) {
    return (q16)(SGN_Q16(n) | RSQRT_UQ16_UNSAFE((uq16)ABS_Q16(n)));
}

SC_Q16 SINCOS_UQ16(uq16 n);
__LIBQ16_ALWAYS_INLINE SC_Q16 SINCOS_Q16(q16 n) {
    return SGN_Q16(n) ? SINCOS_UQ16((uq16)(0x80000000 - ABS_Q16(n))) : SINCOS_UQ16((uq16)n);
}

#undef __LIBQ16_ALWAYS_INLINE

#endif // __LIBQ16_FIXED_HPP

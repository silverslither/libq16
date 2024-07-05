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

struct SC_Q16 {
    q16 sin;
    q16 cos;
};

#define F64_TO_UQ16_UNSAFE(n) \
    _Generic((n), double: ((uq16)std::round(65536.0 * n)))
uq16 F64_TO_UQ16(double n);
q16 F64_TO_Q16_UNSAFE(double n);
q16 F64_TO_Q16(double n);

#define UQ16_TO_F64(n) \
    _Generic((n), uq16: ((double)n / 65536.0))
#define Q16_TO_F64(n) \
    _Generic((n), q16: (std::bit_cast<double>(((uint64_t)(n & 0x80000000) << 32) | std::bit_cast<uint64_t>((double)(n & 0x7fffffff) / 65536.0))))

#define UQ16_TO_Q16_UNSAFE \
    _Generic((n), uq16: ((q16)n))
#define Q16_TO_UQ16_UNSAFE \
    _Generic((n), q16: ((uq16)n))

q16 UQ16_TO_Q16(uq16 n);
uq16 Q16_TO_UQ16(q16 n);

#define ABS_Q16(n) \
    _Generic((n), q16: ((q16)(n & 0x7fffffff)))
#define NABS_Q16(n) \
    _Generic((n), q16: ((q16)(n | 0x80000000)))
#define NEG_Q16(n) \
    _Generic((n), q16: ((q16)(n ^ 0x80000000)))
#define SGN_Q16(n) \
    _Generic((n), q16: ((q16)(n & 0x80000000)))

#define CMP_EQ_UQ16(a, b) \
    _Generic((a), uq16: _Generic((b), uq16: (a == b)))
#define CMP_EQ_Q16(a, b) \
    _Generic((a), q16: _Generic((b), q16: (a == b)))

#define CMP_GT_UQ16(a, b) \
    _Generic((a), uq16: _Generic((b), uq16: (a > b)))
#define CMP_LT_UQ16(a, b) \
    _Generic((a), uq16: _Generic((b), uq16: (a < b)))
#define CMP_GEQ_UQ16(a, b) \
    _Generic((a), uq16: _Generic((b), uq16: (a >= b)))
#define CMP_LEQ_UQ16(a, b) \
    _Generic((a), uq16: _Generic((b), uq16: (a <= b)))

bool CMP_GT_Q16(uq16 a, uq16 b);
bool CMP_LT_Q16(uq16 a, uq16 b);
bool CMP_GEQ_Q16(uq16 a, uq16 b);
bool CMP_LEQ_Q16(uq16 a, uq16 b);

#define ADD_UQ16_UNSAFE(a, b) \
    _Generic((a), uq16: _Generic((b), uq16: ((uq16)(a + b))))
uq16 ADD_UQ16(uq16 a, uq16 b);
q16 ADD_Q16_UNSAFE(q16 a, q16 b);
q16 ADD_Q16(q16 a, q16 b);

#define SUB_UQ16_UNSAFE(a, b) \
    _Generic((a), uq16: _Generic((b), uq16: ((uq16)(a - b))))
uq16 SUB_UQ16(uq16 a, uq16 b);
q16 SUB_Q16_UNSAFE(q16 a, q16 b);
q16 SUB_Q16(q16 a, q16 b);

#define MUL_UQ16_UNSAFE(a, b) \
    _Generic((a), uq16: _Generic((b), uq16: ((uq16)((((uint64_t)a * (uint64_t)b) >> 16) + ((((uint64_t)a * (uint64_t)b) & 0x8000) >> 15)))))
uq16 MUL_UQ16(uq16 a, uq16 b);
q16 MUL_Q16_UNSAFE(q16 a, q16 b);
q16 MUL_Q16(q16 a, q16 b);

uq16 DIV_UQ16_UNSAFE(uq16 a, uq16 b);
uq16 DIV_UQ16(uq16 a, uq16 b);
q16 DIV_Q16_UNSAFE(q16 a, q16 b);
q16 DIV_Q16(q16 a, q16 b);

#define MOD_UQ16_UNSAFE(a, b) \
    _Generic((a), uq16: _Generic((b), uq16: ((uq16)(a % b))))
#define MOD_UQ16(a, b) \
    _Generic((a), uq16: _Generic((b), uq16: ((b == 0) ? a : (uq16)(a % b))))
#define MOD_Q16_UNSAFE(a, b) \
    _Generic((a), q16: _Generic((b), q16: ((q16)((ABS_Q16(a) % ABS_Q16(b)) | SGN_Q16(a)))))
#define MOD_Q16(a, b) \
    _Generic((a), q16: _Generic((b), q16: ((b == 0) ? a : (q16)((ABS_Q16(a) % ABS_Q16(b)) | SGN_Q16(a)))))

uq16 SQRT_UQ16(uq16 n);
#define SQRT_Q16(n) \
    _Generic((n), q16: ((q16)(SGN_Q16(n) | SQRT_UQ16((uq16)ABS_Q16(n)))))

SC_Q16 SINCOS_UQ16(uq16 n);
#define SINCOS_Q16(n) \
    _Generic((n), q16: (SGN_Q16(n) ? SINCOS_UQ16((uq16)(0x80000000 - ABS_Q16(n))) : SINCOS_UQ16((uq16)n)))

#endif // __LIBQ16_FIXED_HPP

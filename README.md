# libq16

Performant, high-accuracy C++20 library for basic operations on Q.16 fixed point numbers. All exact operations are saturating and rounding. Signed values are symmetrical across zero.

### Preprocessor 

`NDEBUG` disables logging.

`LIBQ16_USE_FPU` enables floating point operations for sqrt and rsqrt.

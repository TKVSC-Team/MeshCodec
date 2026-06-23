#include "mc_Float.h"

#ifdef _MSC_VER
#include <intrin.h>
#elif defined(__GNUC__) && (defined(__x86_64__) || defined(__i386__) || defined(_M_IX86) || defined(_M_X64))
#include <xmmintrin.h>
#endif

namespace mc::detail {

u32 InitFPUState() {
#if defined(__x86_64__) || defined(_M_X64)
    u32 csr = _mm_getcsr();
#elif defined(__aarch64__) || defined(_M_ARM64)
    u32 csr;
#if defined(__GNUC__) && !defined(__clang__)
    csr = __builtin_aarch64_get_fpcr();
#else
    u64 csr64;
    __asm__("mrs %0, fpcr" : "=r"(csr64));
    csr = static_cast<u32>(csr64);
#endif
#else
    u32 csr = 0;
#endif
#if defined(__x86_64__) || defined(_M_X64)
    u32 newCsr = csr & 0xffff9fff; // rounding mode nearest
    newCsr |= 0x8040; // flush denormals (I think nan propagation is always set on x86?)
#elif defined(__aarch64__) || defined(_M_ARM64)
    u32 newCsr = csr & 0xfc3fffff; // rounding mode nearest
    newCsr |= 0x3000000; // nan propagation and flush denormals
#else
    u32 newCsr = csr;
#endif
#if defined(__x86_64__) || defined(_M_X64)
    _mm_setcsr(newCsr);
#elif defined(__aarch64__) || defined(_M_ARM64)
#if defined(__GNUC__) && !defined(__clang__)
    __builtin_aarch64_set_fpcr(newCsr);
#else
    const u64 newCsr64 = newCsr;
    __asm__("msr fpcr, %0" :: "r"(newCsr64));
#endif
#endif
    return csr;
}

void SetFPUState(u32 state) {
#if defined(__x86_64__) || defined(_M_X64)
    _mm_setcsr(state);
#elif defined(__aarch64__) || defined(_M_ARM64)
#if defined(__GNUC__) && !defined(__clang__)
    __builtin_aarch64_set_fpcr(state);
#else
    const u64 state64 = state;
    __asm__("msr fpcr, %0" :: "r"(state64));
#endif
#endif
}

} // namespace mc
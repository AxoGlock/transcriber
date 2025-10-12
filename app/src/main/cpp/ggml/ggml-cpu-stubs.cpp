// ggml-cpu-stubs.cpp
// Stub implementations for missing ggml CPU/NUMA/threading functions.

#include "ggml.h"
#include <cstdio>
#include <cstdlib>

extern "C" {

// -------------------- CPU init & traits --------------------
int ggml_cpu_init() {
    // Pretend CPU initialized successfully
    return 0;
}

void ggml_numa_init() {
    // No NUMA on Android — stub
}

bool ggml_is_numa() {
    // Always false on Android
    return false;
}

// CPU feature checks — all disabled for ARM fallback
int ggml_cpu_has_sse3() { return 0; }
int ggml_cpu_has_ssse3() { return 0; }
int ggml_cpu_has_avx() { return 0; }
int ggml_cpu_has_avx_vnni() { return 0; }
int ggml_cpu_has_avx2() { return 0; }
int ggml_cpu_has_avx512() { return 0; }
int ggml_cpu_has_avx512_vbmi() { return 0; }
int ggml_cpu_has_avx512_vnni() { return 0; }
int ggml_cpu_has_f16c() { return 0; }
int ggml_cpu_has_fma() { return 0; }
int ggml_cpu_has_bmi2() { return 0; }

// Stub for ggml_get_type_traits_cpu
const void * ggml_get_type_traits_cpu() {
    return nullptr;
}
// -------------------- CPU feature stubs --------------------
extern "C" {

// x86 features (we don't have these on ARM)
bool ggml_cpu_has_avx512_bf16() { return false; }
bool ggml_cpu_has_amx_int8()      { return false; }

// ARM features
bool ggml_cpu_has_neon()          { return true; }  // ARM64 always has NEON
bool ggml_cpu_has_arm_fma()       { return true; }
bool ggml_cpu_has_fp16_va()       { return true; }
bool ggml_cpu_has_matmul_int8()   { return false; }

// Other architectures
bool ggml_cpu_has_sve()           { return false; }
bool ggml_cpu_has_dotprod()       { return true; }
int  ggml_cpu_get_sve_cnt()       { return 0; }
bool ggml_cpu_has_sme()           { return false; }
bool ggml_cpu_has_riscv_v()       { return false; }
bool ggml_cpu_has_vsx()           { return false; }
bool ggml_cpu_has_vxe()           { return false; }
bool ggml_cpu_has_wasm_simd()     { return false; }
bool ggml_cpu_has_llamafile()     { return false; }

} // extern "C"

// -------------------- Threadpool stubs --------------------
struct ggml_threadpool {
    int dummy;
};

ggml_threadpool * ggml_threadpool_new(int /*n_threads*/) {
    ggml_threadpool * tp = (ggml_threadpool *)malloc(sizeof(ggml_threadpool));
    if (tp) tp->dummy = 1;
    return tp;
}

void ggml_threadpool_free(ggml_threadpool * tp) {
    free(tp);
}

void ggml_threadpool_pause(ggml_threadpool * /*tp*/) {}
void ggml_threadpool_unpause(ggml_threadpool * /*tp*/) {}

// -------------------- Graph compute stubs --------------------
void * ggml_graph_plan(void * /*ctx*/, const void * /*graph*/) { return nullptr; }
void ggml_graph_compute(void * /*plan*/, void * /*data*/) {}
}

#pragma once
#include "monopp/mono_jit.h"
#include "monopp/mono_domain.h"

#include <iostream>

namespace tests
{

void bind_mono();

void test_mono_jit_get_assembly_fail(mono::mono_domain& domain);
void test_mono_jit_get_assembly(mono::mono_domain& domain);
void test_mono_jit_get_class_fail(mono::mono_domain& domain);
void test_mono_jit_get_class(mono::mono_domain& domain);
void test_mono_jit_get_method(mono::mono_domain& domain);
void test_mono_jit_get_field(mono::mono_domain& domain);
void test_mono_jit_get_field_fail(mono::mono_domain& domain);

void test_mono_call_thunk(mono::mono_domain& domain);
void test_mono_call_thunk2(mono::mono_domain& domain);
void test_mono_call_thunk3(mono::mono_domain& domain);
void test_mono_call_thunk4(mono::mono_domain& domain);
void test_mono_call_thunk_with_exception(mono::mono_domain& domain);
void test_mono_call_method(mono::mono_domain& domain);
void test_mono_call_method2(mono::mono_domain& domain);
void test_mono_call_method3(mono::mono_domain& domain);
void test_mono_call_method4(mono::mono_domain& domain);

inline void test_mono(mono::mono_domain& domain)
{
    bind_mono();
    test_mono_jit_get_assembly_fail(domain);
    test_mono_jit_get_assembly(domain);
    test_mono_jit_get_class_fail(domain);
    test_mono_jit_get_class(domain);
    test_mono_jit_get_method(domain);
    test_mono_jit_get_field(domain);
    test_mono_jit_get_field_fail(domain);    
    test_mono_call_thunk(domain);
    test_mono_call_thunk2(domain);
    test_mono_call_thunk3(domain);
    test_mono_call_thunk4(domain);
    test_mono_call_thunk_with_exception(domain);
    test_mono_call_method(domain);
    test_mono_call_method2(domain);
    test_mono_call_method3(domain);
    test_mono_call_method4(domain);

}
}

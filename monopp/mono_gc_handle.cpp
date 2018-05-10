#include "mono_gc_handle.h"
#include "mono_object.h"
#include <cassert>
#include <utility>

namespace mono
{

mono_gc_handle::mono_gc_handle(mono_object& obj)
	: mono_gc_handle(obj.get_mono_object())
{
}

mono_gc_handle::mono_gc_handle(MonoObject* obj)
	: object_(obj)
{
}

mono_gc_handle::~mono_gc_handle() = default;

void mono_gc_handle::lock()
{
	assert(handle_ == 0);
	handle_ = mono_gchandle_new(object_, 1);
}

void mono_gc_handle::unlock()
{
	assert(handle_ != 0);
	mono_gchandle_free(handle_);
	handle_ = 0;
}

} // namespace mono

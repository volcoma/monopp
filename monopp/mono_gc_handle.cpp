#include "mono_gc_handle.h"

namespace mono
{

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

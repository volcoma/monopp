#pragma once

#include "mono_config.h"

#include "mono_object.h"

namespace mono
{

class mono_gc_handle : public mono_object
{
public:
	using mono_object::mono_object;

	void lock();
	void unlock();

private:
	std::uint32_t handle_ = 0;
};

class mono_scoped_gc_handle
{
public:
	mono_scoped_gc_handle(const mono_scoped_gc_handle&) noexcept = delete;
	mono_scoped_gc_handle& operator=(const mono_scoped_gc_handle&) noexcept = delete;

	explicit mono_scoped_gc_handle(mono_gc_handle& handle)
		: handle_(handle)
	{
		handle_.lock();
	}

	~mono_scoped_gc_handle()
	{
		handle_.unlock();
	}

	auto& get_handle() const
	{
		return handle_;
	}

private:
	mono_gc_handle& handle_;
};

} // namespace mono

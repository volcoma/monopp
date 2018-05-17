#pragma once

#include "mono_config.h"

#include "mono_noncopyable.h"

#include <mono/metadata/object.h>

namespace mono
{

class mono_object;

class mono_gc_handle
{
public:
	explicit mono_gc_handle(mono_object& obj);
	explicit mono_gc_handle(MonoObject* obj);
	~mono_gc_handle();

	void lock();
	void unlock();

private:
	MonoObject* object_ = nullptr;
	std::uint32_t handle_ = 0;
};

class mono_scoped_gc_handle : public common::noncopyable
{
public:
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

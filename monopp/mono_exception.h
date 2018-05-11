#pragma once

#include "mono_config.h"

#include <mono/metadata/reflection.h>

namespace mono
{

class mono_exception : public std::runtime_error
{
public:
	mono_exception();
	explicit mono_exception(const std::string& what);
};

class mono_thunk_exception : public mono_exception
{
public:
	explicit mono_thunk_exception(MonoException* ex);

	auto exception_typename() const -> const std::string&
	{
		return exception_typename_;
	}

	auto message() const -> const std::string&
	{
		return message_;
	}

	auto stacktrace() const -> const std::string&
	{
		return stacktrace_;
	}

private:
	struct mono_exception_info
	{
		std::string exception_typename;
		std::string message;
		std::string stacktrace;
	};

	explicit mono_thunk_exception(const mono_exception_info& info);

	static auto __get_exception_info(MonoException* ex) -> mono_exception_info;
	static auto __get_string_property(const char* property_name, MonoClass* cls, MonoObject* obj) -> char*;

	std::string exception_typename_;
	std::string message_;
	std::string stacktrace_;
};

} // namespace mono

#pragma once

#include "mono_config.h"

BEGIN_MONO_INCLUDE
#include <mono/metadata/reflection.h>
END_MONO_INCLUDE

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

	auto exception_typename() const -> const std::string&;

	auto message() const -> const std::string&;

	auto stacktrace() const -> const std::string&;

	struct mono_exception_info
	{
		std::string exception_typename;
		std::string message;
		std::string stacktrace;
	};

private:
	explicit mono_thunk_exception(const mono_exception_info& info);

	mono_exception_info info_;
};

} // namespace mono

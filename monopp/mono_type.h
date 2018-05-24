#pragma once

#include "mono_config.h"
#include "mono_noncopyable.h"

#include <mono/metadata/metadata.h>

namespace mono
{
enum class visibility
{
	private_,
	protected_internal_,
	internal_,
	protected_,
	public_
};

class mono_type
{
public:
	mono_type() = default;
	explicit mono_type(MonoType* type);
	auto operator=(MonoType* type) -> mono_type&;
	auto get_name() const -> const std::string&;
	auto get_fullname() const -> const std::string&;

	auto is_valuetype() const -> bool;

protected:
	non_owning_ptr<MonoType> type_ = nullptr;

	std::string name_;
	std::string fullname_;

	bool valuetype_ = false;
};

} // namespace mono

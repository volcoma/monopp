#pragma once

namespace mono
{
namespace common
{
/*!
 * Delete copy and assignment operators so that derived classes
 * can not be copied.
 */
class noncopyable
{
public:
	noncopyable() noexcept = default;
	~noncopyable() noexcept = default;
	noncopyable(const noncopyable&) noexcept = delete;
	noncopyable& operator=(const noncopyable&) noexcept = delete;
	noncopyable(noncopyable&& o) noexcept = default;
	noncopyable& operator=(noncopyable&& other) noexcept = default;
};

} // namespace common
} // namespace mono

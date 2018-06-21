#ifndef SUITE_HPP
#define SUITE_HPP

#include <cassert>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <functional>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>

#define SUITE_JOIN(str, num) str##num
#define SUITE_GLUE(str, num) SUITE_JOIN(str, num)
#define SUITE_LINE(str) SUITE_GLUE(str, __LINE__)
#define SUITE_GLOBAL static auto SUITE_LINE(sstsuite) =

#define SUITE_DECOMPOSE(...) (suite::expression_decomposer() << (__VA_ARGS__)).get_result()

#define SUITE_CHECK(...)                                                                                     \
	suite::check(#__VA_ARGS__, __FILE__, __LINE__, [&]() { return SUITE_DECOMPOSE(__VA_ARGS__); })

#define CHECK(...) SUITE_CHECK(__VA_ARGS__)
#define THROWS(...)                                                                                          \
	[&]() {                                                                                                  \
		try                                                                                                  \
		{                                                                                                    \
			__VA_ARGS__;                                                                                     \
		}                                                                                                    \
		catch(...)                                                                                           \
		{                                                                                                    \
			return true;                                                                                     \
		}                                                                                                    \
		return false;                                                                                        \
	}()

#define NOTHROWS(...) !THROWS(__VA_ARGS__)

namespace suite
{

using timer = std::chrono::high_resolution_clock;

template <typename T, typename R>
using ForNonPointer =
	typename std::enable_if<!(std::is_pointer<T>::value || std::is_null_pointer<T>::value), R>::type;

template <typename T>
inline std::string make_string(T const* ptr)
{
	// Note showbase affects the behavior of /integer/ output;
	std::ostringstream os;
	os << std::internal << std::hex << std::showbase << std::setw(2 + 2 * sizeof(T*)) << std::setfill('0')
	   << reinterpret_cast<std::ptrdiff_t>(ptr);
	return os.str();
}

inline auto to_string(const std::nullptr_t&) -> std::string
{
	return "nullptr";
}

template <typename T>
auto to_string(const T& t) -> ForNonPointer<T, std::string>
{
	std::stringstream ss;
	return (ss << std::boolalpha << t) ? ss.str() : std::string("??");
}

template <typename T>
auto to_string(const T*& ptr) -> std::string
{
	return !ptr ? to_string(nullptr) : make_string(ptr);
}

template <typename L, typename R>
auto to_string(L const& lhs, std::string op, R const& rhs) -> std::string
{
	std::ostringstream os;
	os << to_string(lhs) << " " << op << " " << to_string(rhs);
	return os.str();
}

enum test_status
{
	FAILED,
	PASSED,
	TESTNO
};
inline unsigned& get(int i)
{
	static unsigned var[TESTNO + 1] = {};
	return var[i];
}

inline void summary()
{
	std::string ss, run = to_string(get(TESTNO)), res = get(FAILED) ? "[FAIL]  " : "[ OK ]  ";
	if(get(FAILED))
		ss += res + "Failure! " + to_string(get(FAILED)) + '/' + run + " checks failed :(\n";
	else
		ss += res + "Success: " + run + " checks passed :)\n";
	fprintf(stdout, "\n%s", ss.c_str());
	if(get(FAILED))
		std::exit(int(get(FAILED)));
}

struct result
{
	bool passed = false;
	std::string decomposition;
};

template <typename L>
struct expression_lhs
{
	const L lhs;

	expression_lhs(L lhs_)
		: lhs(lhs_)
	{
	}

	result get_result() const
	{
		return {!!lhs, to_string(lhs)};
	}

#define DECOMPOSE_OP(OP)                                                                                     \
	template <typename R>                                                                                    \
	result operator OP(R const& rhs)                                                                         \
	{                                                                                                        \
		return result{lhs OP rhs, to_string(lhs, #OP, rhs)};                                                 \
	}

	DECOMPOSE_OP(<)
	DECOMPOSE_OP(<=)
	DECOMPOSE_OP(>)
	DECOMPOSE_OP(>=)
	DECOMPOSE_OP(!=)
	DECOMPOSE_OP(==)
	DECOMPOSE_OP(&&)
	DECOMPOSE_OP(||)
};

struct expression_decomposer
{
	template <typename L>
	expression_lhs<L const&> operator<<(L const& operand)
	{
		return expression_lhs<L const&>(operand);
	}
};

class check
{
public:
	check(const char* const text, const char* const file, int line, const std::function<result()>& f)
		: text_(text)
		, file_(file)
		, line_(line)
	{
		result_ = f();
		set_label(to_string(get(TESTNO)++));
	}

	check(check&&) = default;
	check& operator=(check&&) = default;
	check(const check&) = delete;
	check& operator=(const check&) = delete;

	~check()
	{
		if(text_.empty())
			return;

		using namespace std::chrono;
		auto dur = duration_cast<duration<double, std::milli>>(timer::now() - start_);

		bool ok = result_.passed;

		get(ok ? PASSED : FAILED)++;
		std::string res[] = {"[FAIL]", "[ OK ]"};

		std::string desc = res[ok] + " check (" + label_ + ")" + " (" + to_string(dur.count()) + " ms)";

		fprintf(stdout, "%s", desc.c_str());
		if(!ok)
		{
			fprintf(stdout, " at %s:%d\n", file_.c_str(), line_);
			fprintf(stdout, "\t%s\n", text_.c_str());
			fprintf(stdout, "\t%s\n", result_.decomposition.c_str());
		}
		fprintf(stdout, "%s", "\n");
	}

	check& set_label(const std::string& name)
	{
		label_ = name;
		return *this;
	}

private:
	result result_;
	timer::time_point start_ = timer::now();
	std::string text_;
	std::string label_;
	std::string file_;
	int line_ = 0;
};

inline decltype(auto) test(const std::string& text, const std::function<void()>& fn)
{
	auto title = text;
	if(title.empty())
		title = "Test";
	fprintf(stdout, "--------  %s  --------\n", title.c_str());

	auto whole_case = [&]() {
		auto fails = get(FAILED);
		fn();
		return get(FAILED) == fails;
	};
	auto c = CHECK(whole_case());
	c.set_label("total");
	return c;
}
}

#endif

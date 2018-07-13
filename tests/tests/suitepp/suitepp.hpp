#include <utility>

#ifndef SUITEPP_HPP
#define SUITEPP_HPP

#include <cassert>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <functional>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>

#define SUITEPP_STR(s) #s
#define SUITEPP_JOIN(str, num) str##num
#define SUITEPP_GLUE(str, num) SUITEPP_JOIN(str, num)
#define SUITEPP_LINE(str) SUITEPP_GLUE(str, __LINE__)

#ifdef _MSC_VER
#define SUITEPP_DO_PRAGMA(x) __pragma(x)
#define SUITEPP_PRAGMA(compiler, x) SUITEPP_DO_PRAGMA(warning(x))
#else
#define SUITEPP_DO_PRAGMA(x) _Pragma(#x)
#define SUITEPP_PRAGMA(compiler, x) SUITEPP_DO_PRAGMA(compiler diagnostic x)
#endif
#if defined(__clang__)
#define SUITEPP_PUSH_PRAGMA SUITEPP_PRAGMA(clang, push)
#define SUITEPP_DISABLE_WARNING(gcc_unused, clang_option, msvc_unused)                                       \
	SUITEPP_PRAGMA(clang, ignored clang_option)
#define SUITEPP_POP_PRAGMA SUITEPP_PRAGMA(clang, pop)
#elif defined(_MSC_VER)
#define SUITEPP_PUSH_PRAGMA SUITEPP_PRAGMA(msvc, push)
#define SUITEPP_DISABLE_WARNING(gcc_unused, clang_unused, msvc_errorcode)                                    \
	SUITEPP_PRAGMA(msvc, disable : msvc_errorcode)
#define SUITEPP_POP_PRAGMA SUITEPP_PRAGMA(msvc, pop)
#elif defined(__GNUC__)
#define SUITEPP_PUSH_PRAGMA SUITEPP_PRAGMA(GCC, push)
#define SUITEPP_DISABLE_WARNING(gcc_option, clang_unused, msvc_unused) SUITEPP_PRAGMA(GCC, ignored gcc_option)
#define SUITEPP_POP_PRAGMA SUITEPP_PRAGMA(GCC, pop)
#endif

#define SUITEPP_GLOBAL static auto SUITEPP_LINE(sstsuite) =

#define SUITEPP_DECOMPOSE(expr) suitepp::result(suitepp::expression_decomposer() << expr)

#define SUITEPP_EXPECT(expr)                                                                                 \
	suitepp::check(#expr, __FILE__, __LINE__, [&]() {                                                        \
		SUITEPP_PUSH_PRAGMA                                                                                  \
		SUITEPP_DISABLE_WARNING("-Wparentheses", "-Wparentheses", 4554)                                      \
		auto res = SUITEPP_DECOMPOSE(expr);                                                                  \
		SUITEPP_POP_PRAGMA                                                                                   \
		return res;                                                                                          \
	})

#define EXPECT(expr) SUITEPP_EXPECT(expr)

#define THROWS(expr)                                                                                         \
	[&]() {                                                                                                  \
		try                                                                                                  \
		{                                                                                                    \
			expr;                                                                                            \
		}                                                                                                    \
		catch(...)                                                                                           \
		{                                                                                                    \
			return true;                                                                                     \
		}                                                                                                    \
		return false;                                                                                        \
	}()

#define THROWS_AS(expr, exception_type)                                                                      \
	[&]() {                                                                                                  \
		try                                                                                                  \
		{                                                                                                    \
			expr;                                                                                            \
		}                                                                                                    \
		catch(const exception_type&)                                                                         \
		{                                                                                                    \
			return true;                                                                                     \
		}                                                                                                    \
		catch(...)                                                                                           \
		{                                                                                                    \
		}                                                                                                    \
		return false;                                                                                        \
	}()

#define NOTHROWS(expr) !THROWS(expr)

namespace suitepp
{

using timer = std::chrono::high_resolution_clock;

template <typename T>
struct is_container
{
	template <typename U>
	static auto test(int)
		-> decltype(std::begin(std::declval<U>()) == std::end(std::declval<U>()), std::true_type());

	template <typename>
	static auto test(...) -> std::false_type;

	static constexpr bool value =
		std::is_same<decltype(test<T>(0)), std::true_type>::value || std::is_array<T>::value;
};

template <typename T, typename R>
using ForNonContainerNonPointer = typename std::enable_if<
	!(is_container<T>::value || std::is_pointer<T>::value || std::is_null_pointer<T>::value), R>::type;

template <typename T, typename R>
using ForPointer = typename std::enable_if<std::is_pointer<T>::value, R>::type;

template <typename T, typename R>
using ForContainer = typename std::enable_if<is_container<T>::value, R>::type;

template <typename T>
inline std::string make_string(T const* ptr)
{
	// Note showbase affects the behavior of /integer/ output;
	std::ostringstream os;
	os << std::internal << std::hex << std::showbase << std::setw(2 + 2 * sizeof(T*)) << std::setfill('0')
	   << reinterpret_cast<std::ptrdiff_t>(ptr);
	return os.str();
}
inline std::string to_string(std::nullptr_t const&)
{
	return "nullptr";
}

inline std::string to_string(std::string const& txt)
{
	return "\"" + txt + "\"";
}

inline std::string to_string(char const* const& txt)
{
	return "\"" + std::string(txt) + "\"";
}

inline std::string to_string(char const& txt)
{
	return "\'" + std::string(1, txt) + "\'";
}

template <typename T>
auto to_string(const T& t) -> ForNonContainerNonPointer<T, std::string>
{
	std::stringstream ss;
	return (ss << std::boolalpha << t) ? ss.str() : std::string("??");
}

template <typename T>
auto to_string(const T& ptr) -> ForPointer<T, std::string>
{
	return !ptr ? to_string(nullptr) : make_string(ptr);
}

template <typename T1, typename T2>
auto to_string(std::pair<T1, T2> const& pair) -> std::string
{
	std::ostringstream oss;
	oss << "{ " << to_string(pair.first) << ", " << to_string(pair.second) << " }";
	return oss.str();
}

template <typename C>
auto to_string(C const& cont) -> ForContainer<C, std::string>
{
	std::ostringstream os;
	os << "{ ";
	int i = 0;
	for(auto& x : cont)
	{
		if(i++ != 0)
		{
			os << ", ";
		}
		os << to_string(x);
	}
	os << " }";
	return os.str();
}

template <typename L, typename R>
auto to_string(const L& lhs, std::string op, const R& rhs) -> std::string
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
	static unsigned var[TESTNO + 1] = {0, 0, 0};
	return var[i];
}

struct result
{
	bool passed = false;
	std::string decomposition;
};

template <typename L>
struct expression_lhs
{
	static result make_result(bool passed, const std::string& decomp)
	{
		result r;
		r.passed = passed;
		r.decomposition = decomp;
		return r;
	}

	const L lhs;

	expression_lhs(L lhs_)
		: lhs(lhs_)
	{
	}

	operator result() const
	{
		return make_result(!!lhs, to_string(lhs));
	}

#define DECOMPOSE_OP(OP)                                                                                     \
	template <typename R>                                                                                    \
	result operator OP(R const& rhs)                                                                         \
	{                                                                                                        \
		return make_result(lhs OP rhs, to_string(lhs, #OP, rhs));                                            \
	}

	DECOMPOSE_OP(<)
	DECOMPOSE_OP(<=)
	DECOMPOSE_OP(>)
	DECOMPOSE_OP(>=)
	DECOMPOSE_OP(!=)
	DECOMPOSE_OP(==)
	DECOMPOSE_OP(&&)
	DECOMPOSE_OP(||)

#undef DECOMPOSE_OP
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
	struct summary_reporter
	{
		~summary_reporter()
		{
			std::string run = to_string(get(TESTNO));
			std::string res = get(FAILED) ? "[FAIL]  " : "[ OK ]  ";
			std::string ss;
			if(get(FAILED))
				ss += res + "Failure! " + to_string(get(FAILED)) + '/' + run + " checks failed :(\n";
			else
				ss += res + "Success: " + run + " checks passed :)\n";
			fprintf(stdout, "\n%s", ss.c_str());
			if(get(FAILED))
				std::exit(int(get(FAILED)));
		}
	};

	using duration_t = std::chrono::duration<double, std::milli>;

public:
	check(const char* const text, const char* const file, int line, std::function<result()> f)
		: result_getter_(std::move(f))
		, text_(text)
		, file_(file)
		, line_(line)
	{
		static summary_reporter reporter;
		(void)reporter;

		set_label(to_string(get(TESTNO)++));
	}

	~check()
	{
		if(text_.empty() || result_getter_ == nullptr)
			return;

		auto start = timer::now();
		auto result = result_getter_();
		auto end = timer::now();
		duration_ = std::chrono::duration_cast<duration_t>(end - start);

		bool ok = result.passed;

		get(ok ? PASSED : FAILED)++;

		fprintf(stdout, "%s", ok ? "[ OK ] " : "[FAIL] ");
		fprintf(stdout, "check (%s) ", label_.c_str());
		fprintf(stdout, "(%sms) ", to_string(duration_.count()).c_str());

		if(!ok)
		{
			fprintf(stdout, "at %s:%d\n", file_.c_str(), line_);
			fprintf(stdout, "       %s\n", text_.c_str());
			fprintf(stdout, "       %s\n", result.decomposition.c_str());
		}
		fprintf(stdout, "%s", "\n");
	}

	check& set_label(const std::string& name)
	{
		label_ = name;
		return *this;
	}

private:
	std::function<result()> result_getter_;
	result result_;
	duration_t duration_;
	std::string text_;
	std::string label_;
	std::string file_;
	int line_ = 0;
};

inline auto test(const std::string& text, const std::function<void()>& fn)
{
	auto title = text;
	if(title.empty())
		title = "Test";
	fprintf(stdout, "-------------- %s --------------\n", title.c_str());

	auto whole_case = [&]() {
		auto fails_before = get(FAILED);
		fn();
		auto fails_after = get(FAILED);
		return fails_before == fails_after;
	};
	EXPECT(whole_case()).set_label("total");
	return true;
}
}

#endif

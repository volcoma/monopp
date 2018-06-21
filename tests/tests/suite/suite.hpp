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

#define SUITE_STR(s) #s
#define SUITE_JOIN(str, num) str##num
#define SUITE_GLUE(str, num) SUITE_JOIN(str, num)
#define SUITE_LINE(str) SUITE_GLUE(str, __LINE__)

#ifdef _MSC_VER
#define SUITE_DO_PRAGMA(x) __pragma(x)
#define SUITE_PRAGMA(compiler, x) SUITE_DO_PRAGMA(warning(x))
#else
#define SUITE_DO_PRAGMA(x) _Pragma(#x)
#define SUITE_PRAGMA(compiler, x) SUITE_DO_PRAGMA(compiler diagnostic x)
#endif
#if defined(__clang__)
#define SUITE_PUSH_PRAGMA SUITE_PRAGMA(clang, push)
#define SUITE_DISABLE_WARNING(gcc_unused, clang_option, msvc_unused) SUITE_PRAGMA(clang, ignored clang_option)
#define SUITE_POP_PRAGMA SUITE_PRAGMA(clang, pop)
#elif defined(_MSC_VER)
#define SUITE_PUSH_PRAGMA SUITE_PRAGMA(msvc, push)
#define SUITE_DISABLE_WARNING(gcc_unused, clang_unused, msvc_errorcode)                                      \
    SUITE_PRAGMA(msvc, disable : msvc_errorcode)
#define SUITE_POP_PRAGMA SUITE_PRAGMA(msvc, pop)
#elif defined(__GNUC__)
#define SUITE_PUSH_PRAGMA SUITE_PRAGMA(GCC, push)
#define SUITE_DISABLE_WARNING(gcc_option, clang_unused, msvc_unused) SUITE_PRAGMA(GCC, ignored gcc_option)
#define SUITE_POP_PRAGMA SUITE_PRAGMA(GCC, pop)
#endif

#define SUITE_GLOBAL static auto SUITE_LINE(sstsuite) =

#define SUITE_DECOMPOSE(expr) (suite::expression_decomposer() << expr)

#define SUITE_EXPECT(expr)                                                                                   \
    suite::check(#expr, __FILE__, __LINE__, [&]() {                                                          \
        SUITE_PUSH_PRAGMA                                                                                    \
        SUITE_DISABLE_WARNING("-Wparentheses", "-Woverloaded-shift-op-parentheses", 4554)                    \
        suite::result res(SUITE_DECOMPOSE(expr));                                                            \
        SUITE_POP_PRAGMA                                                                                     \
        return res;                                                                                          \
    })

#define EXPECT(expr) SUITE_EXPECT(expr)
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

#define THROWS_TYPE(exception_type, expr)                                                                    \
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

namespace suite
{

using timer = std::chrono::high_resolution_clock;

template <typename T, typename R>
using ForNonPointer =
    typename std::enable_if<!(std::is_pointer<T>::value || std::is_null_pointer<T>::value), R>::type;

template <typename T, typename R>
using ForPointer = typename std::enable_if<std::is_pointer<T>::value, R>::type;

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
auto to_string(const T& ptr) -> ForPointer<T, std::string>
{
    return !ptr ? to_string(nullptr) : make_string(ptr);
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
    static unsigned var[TESTNO + 1] = {};
    return var[i];
}

struct result
{
    operator bool() const
    {
        return passed;
    }

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
        return make_result(lhs OP rhs, to_string(lhs, #OP, rhs));                                                 \
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
    check(const char* const text, const char* const file, int line, const std::function<result()>& f)
        : text_(text)
        , file_(file)
        , line_(line)
    {
        static summary_reporter reporter;
        (void)reporter;

        auto start = timer::now();
        result_ = f();
        auto end = timer::now();
        duration_ = std::chrono::duration_cast<duration_t>(end - start);

        set_label(to_string(get(TESTNO)++));
    }

    ~check()
    {
        if(text_.empty())
            return;

        bool ok = result_.passed;

        get(ok ? PASSED : FAILED)++;
        std::string res[] = {"[FAIL]", "[ OK ]"};

        std::string desc = res[ok] + " check (" + label_ + ")" + " (" + to_string(duration_.count()) + " ms)";

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
    duration_t duration_;
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
    EXPECT(whole_case()).set_label("total");
    return true;
}
}

#endif

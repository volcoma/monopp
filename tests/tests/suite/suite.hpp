#ifndef TESTS_HPP
#define TESTS_HPP

#include <functional>
#include <iostream>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

namespace test
{

template <typename T>
auto to_string(T val)
{
	std::stringstream ss;
	ss << val;
	return ss.str();
}

using reset_func = std::function<void(void)>;

struct result
{
	std::string expected;
	std::string got;
	std::string message;
	bool passed = false;
};

class spec
{
public:
	spec(std::string name)
		: name_(std::move(name))
	{
	}

	template <typename T>
	void expect_equals(T expected, T got)
	{
		auto expected_str = to_string(expected);
		auto got_str = to_string(got);

		result res;
		res.passed = expected == got;
		res.expected = expected_str;
		res.got = got_str;
		res.message = expected_str + " to equal " + got_str;

		results_.emplace_back(std::move(res));
	}

	void expect_true(bool expected)
	{
		auto got = std::string{expected ? "true" : "false"};

		result res;
		res.passed = expected;
		res.expected = "true";
		res.got = got;
		res.message = "true, got " + got;

		results_.emplace_back(std::move(res));
	}

	void expect_false(bool expected)
	{
		auto got = std::string{expected ? "true" : "false"};

		result res;
		res.passed = !expected;
		res.expected = "false";
		res.got = got;
		res.message = "false, got " + got;

		results_.emplace_back(std::move(res));
	}

	template <typename F>
	void expect_nothrow(F&& expression)
	{
		result res;
		res.message = "expression nothrow";
		try
		{
			expression();
			res.passed = true;
		}
		catch(const std::exception& e)
		{
			res.passed = false;
			res.expected = "";
			res.got = std::string("exception: ") + e.what();
		}
		catch(...)
		{
			res.passed = false;
		}

		results_.emplace_back(std::move(res));
	}

	template <typename F>
	void expect_throw(F&& expression)
	{
		result res;
		res.message = "expression throws";
		try
		{
			expression();
			res.passed = false;
		}
		catch(...)
		{
			res.passed = true;
		}
		results_.emplace_back(std::move(res));
	}

	template <typename T>
	void expect_not_null(T* ptr)
	{
		result res;
		res.passed = ptr != nullptr;
		res.message = "pointer that is not null";

		results_.emplace_back(std::move(res));
	}

	auto& name() const
	{
		return name_;
	}

	auto passed() const
	{
		auto pass = true;

		for(auto& r : results_)
		{
			pass = pass && r.passed;
		}

		return pass;
	}

	auto& results() const
	{
		return results_;
	}

private:
	std::string name_;
	std::vector<result> results_;
};

using spec_function = std::function<void(spec&)>;

class suite
{
public:
	suite(std::string name)
		: name_(std::move(name))
	{
	}

	void before(const reset_func& f)
	{
		before_func_ = f;
	}

	void after(const reset_func& f)
	{
		after_func_ = f;
	}

	void it(const std::string& spec_name, const spec_function& test_spec)
	{
		specs_.emplace_back(spec_name);
		auto& s = specs_.back();
		before_func_();
		test_spec(s);
		after_func_();
	}

	void print()
	{
		std::cout << std::endl << "#  " << name_ << std::endl << std::endl;

		size_t spec_counter = 0u;
		size_t failure_counter = 0u;

		const std::string SPACE = "\t";
		const std::string SEPARATOR = "====================================================";

		for(const auto& spec : specs_)
		{
			std::cout << SEPARATOR << std::endl;
			std::cout << "it: " << spec.name() << ": " << (spec.passed() ? "PASS" : "FAIL") << std::endl;

			auto& results = spec.results();
			spec_counter += results.size();

			if(!spec.passed())
			{
				for(auto& r : results)
				{
					std::cout << SPACE << (r.passed ? "+" : "-") << " expect " << r.message << std::endl;

					if(!r.passed)
					{
						std::cout << SPACE << SPACE << "expected : " << r.expected << std::endl;
						std::cout << SPACE << SPACE << "got : " << r.got << std::endl;

						failure_counter++;
					}
				}

				std::cout << std::endl;
			}
			std::cout << SEPARATOR << std::endl;
		}

		std::cout << spec_counter << " specs, " << failure_counter << " failures" << std::endl;
		std::cout << SEPARATOR << std::endl;
	}

private:
	std::string name_;
	std::vector<spec> specs_;

	reset_func before_func_ = []() {};
	reset_func after_func_ = []() {};
};

inline void describe(const std::string& name, const std::function<void(suite&)>& test_suite)
{
	suite s(name);

	test_suite(s);

	s.print();
}
}

#endif

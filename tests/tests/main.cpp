#include "monopp/mono_jit.h"

#include "monopp/mono_assembly.h"
#include "monopp/mono_class.h"
#include "monopp/mono_class_field.h"
#include "monopp/mono_class_instance.h"
#include "monopp/mono_class_property.h"
#include "monopp/mono_domain.h"
#include "monopp/mono_method.h"
#include "monopp/mono_string.h"
#include <iostream>
#include <memory>

#include "monort/monort.h"

struct vec2f
{
	float x;
	float y;
};

namespace mono
{
namespace managed_interface
{
struct vector2f
{
	float x;
	float y;
};
template <>
inline auto converter::convert(const vec2f& v) -> vector2f
{
	return vector2f{v.x, v.y};
}

template <>
inline auto converter::convert(const vector2f& v) -> vec2f
{
	return vec2f{v.x, v.y};
}
} // namespace managed_interface

register_basic_mono_converter_for_pod(vec2f, managed_interface::vector2f);
register_basic_mono_converter_for_wrapper(std::shared_ptr<vec2f>);
} // namespace mono

void MyObject_CreateInternal(MonoObject* this_ptr, float x, std::string v)
{
	(void)this_ptr;
	(void)x;
	(void)v;
	std::cout << "FROM C++ : MyObject created." << std::endl;
}

void MyObject_DestroyInternal(MonoObject* this_ptr)
{
	(void)this_ptr;
	std::cout << "FROM C++ : MyObject deleted." << std::endl;
}

void MyObject_DoStuff(MonoObject* this_ptr, std::string value)
{
	(void)this_ptr;
	(void)value;
	std::cout << "FROM C++ : DoStuff was called with: " << value << std::endl;
}

std::string MyObject_ReturnAString(MonoObject* this_ptr, std::string value)
{
	(void)this_ptr;
	(void)value;
	std::cout << "FROM C++ : ReturnAString was called with: " << value << std::endl;
	return "The value: " + value;
}

void MyVec_CreateInternalCtor(MonoObject* this_ptr, float x, float y)
{
	std::cout << "FROM C++ : WrapperVector2f created." << std::endl;
	using vec2f_ptr = std::shared_ptr<vec2f>;
	auto p = std::make_shared<vec2f>();
	p->x = x;
	p->y = y;

	mono::managed_interface::mono_object_wrapper<vec2f_ptr>::create(this_ptr, p);
}

void MyVec_CreateInternalCopyCtor(MonoObject* this_ptr, std::shared_ptr<vec2f> rhs)
{
	std::cout << "FROM C++ : WrapperVector2f created." << std::endl;
	using vec2f_ptr = std::shared_ptr<vec2f>;
	auto p = std::make_shared<vec2f>();
	p->x = rhs->x;
	p->y = rhs->y;

	mono::managed_interface::mono_object_wrapper<vec2f_ptr>::create(this_ptr, p);
}

// Let Catch provide main():
#define CATCH_IMPL
#define CATCH_CONFIG_ALL_PARTS
#include "catch/catch.hpp"

static std::unique_ptr<mono::mono_domain> domain;

TEST_CASE("create mono domain", "[domain]")
{
	// clang-format off
	auto expression = []()
	{
		// clang-format on
		domain = std::make_unique<mono::mono_domain>("domain");
		mono::mono_domain::set_current_domain(*domain);
		// clang-format off
	};
	// clang-format on

	REQUIRE_NOTHROW(expression());
}

TEST_CASE("load invalid assembly", "[domain]")
{
	// clang-format off
	auto expression = []()
	{
		// clang-format on
		domain->get_assembly("doesnt_exist_12345.dll");
		// clang-format off
	};
	// clang-format on

	// clang-format off
    REQUIRE_THROWS_WITH(expression(), "NATIVE::Could not open assembly with path : doesnt_exist_12345.dll");
	// clang-format on
}

TEST_CASE("load valid assembly", "[domain]")
{
	// clang-format off
	auto expression = []()
	{
		// clang-format on
		auto assembly = domain->get_assembly("tests_managed.dll");
		REQUIRE(assembly.valid() == true);
		// clang-format off
	};
	// clang-format on

	REQUIRE_NOTHROW(expression());
}

TEST_CASE("load valid assembly and bind", "[domain]")
{
	// clang-format off
	auto expression = []()
	{
		// clang-format on
		auto assembly = domain->get_assembly("tests_managed.dll");
		REQUIRE(assembly.valid() == true);

		mono::add_internal_call("Tests.MyObject::CreateInternal", internal_call(MyObject_CreateInternal));
		mono::add_internal_call("Tests.MyObject::DestroyInternal", internal_call(MyObject_DestroyInternal));
		mono::add_internal_call("Tests.MyObject::DoStuff", internal_call(MyObject_DoStuff));
		mono::add_internal_call("Tests.MyObject::ReturnAString", internal_call(MyObject_ReturnAString));
		// clang-format off
	};
	// clang-format on

	REQUIRE_NOTHROW(expression());
}

TEST_CASE("init monort", "[monort]")
{
	// clang-format off
	auto expression = []()
	{
		// clang-format on
		auto core_assembly = domain->get_assembly("monort_managed.dll");
		mono::managed_interface::init(core_assembly);
		// clang-format off
	};
	// clang-format on

	REQUIRE_NOTHROW(expression());
}

TEST_CASE("bind monort", "[monort]")
{
	// clang-format off
	auto expression = []()
	{
		// clang-format on
		mono::add_internal_call("Tests.WrapperVector2f::.ctor(single,single)",
								internal_call(MyVec_CreateInternalCtor));
		mono::add_internal_call("Tests.WrapperVector2f::.ctor(Tests.WrapperVector2f)",
								internal_call(MyVec_CreateInternalCopyCtor));
		// clang-format off
	};
	// clang-format on

	REQUIRE_NOTHROW(expression());
}
TEST_CASE("get invalid class", "[assembly]")
{
	// clang-format off
	auto expression = []()
	{
		// clang-format on
		auto assembly = domain->get_assembly("tests_managed.dll");
		REQUIRE(assembly.valid() == true);

		REQUIRE_THROWS([&]() { assembly.get_class("SomeClassThatDoesntExist12345"); }());
		// clang-format off
	};
	// clang-format on

	REQUIRE_NOTHROW(expression());
}

TEST_CASE("get valid class", "[assembly]")
{
	// clang-format off
	auto expression = []()
	{
		// clang-format on
		auto assembly = domain->get_assembly("tests_managed.dll");
		REQUIRE(assembly.valid() == true);

		auto cls = assembly.get_class("ClassInstanceTest");

		REQUIRE(cls.get_internal_ptr() != nullptr);

		auto fields = cls.get_fields();
		auto props = cls.get_properties();
		auto methods = cls.get_methods();
		for(const auto& field : fields)
		{
			std::cout << field.get_full_declname() << std::endl;
		}
		for(const auto& prop : props)
		{
			std::cout << prop.get_full_declname() << std::endl;
		}
		for(const auto& method : methods)
		{
			std::cout << method.get_full_declname() << std::endl;
		}
		// clang-format off
	};
	// clang-format on

	REQUIRE_NOTHROW(expression());
}

TEST_CASE("get valid method", "[class]")
{
	// clang-format off
	auto expression = []()
	{
		// clang-format on
		auto assembly = domain->get_assembly("tests_managed.dll");
		REQUIRE(assembly.valid() == true);

		auto cls = assembly.get_class("ClassInstanceTest");
		REQUIRE(cls.get_internal_ptr() != nullptr);

		auto method1 = cls.get_method<void()>("Method");
		REQUIRE(method1.get_internal_ptr() != nullptr);
		auto method2 = cls.get_method<void(int)>("MethodWithParameter");
		REQUIRE(method2.get_internal_ptr() != nullptr);
		auto method3 = cls.get_method<std::string(std::string, int)>("MethodWithParameterAndReturnValue");
		REQUIRE(method3.get_internal_ptr() != nullptr);
		// clang-format off
	};
	// clang-format on

	REQUIRE_NOTHROW(expression());
}

TEST_CASE("get/set field", "[class]")
{
	// clang-format off
	auto expression = []()
	{
		// clang-format on
		auto assembly = domain->get_assembly("tests_managed.dll");
		REQUIRE(assembly.valid() == true);

		auto cls = assembly.get_class("ClassInstanceTest");
		REQUIRE(cls.get_internal_ptr() != nullptr);

		auto field = cls.get_field("someField");
		REQUIRE(field.get_internal_ptr() != nullptr);

		auto obj = cls.new_instance();
		REQUIRE(obj.get_internal_ptr() != nullptr);

		auto someField = field.get_value<int>(obj);
		REQUIRE(someField == 12);

		int arg = 6;
		field.set_value(obj, arg);

		someField = field.get_value<int>(obj);
		REQUIRE(someField == 6);
		// clang-format off
	};
	// clang-format on

	REQUIRE_NOTHROW(expression());
}

TEST_CASE("get invalid field", "[class]")
{
	// clang-format off
	auto expression = []()
	{
		// clang-format on
		auto assembly = domain->get_assembly("tests_managed.dll");
		REQUIRE(assembly.valid() == true);

		auto cls = assembly.get_class("ClassInstanceTest");
		REQUIRE(cls.get_internal_ptr() != nullptr);

		REQUIRE_THROWS([&]() { cls.get_field("someInvalidField"); }());
		// clang-format off
	};
	// clang-format on

	REQUIRE_NOTHROW(expression());
}

TEST_CASE("get/set property", "[class]")
{
	// clang-format off
	auto expression = []()
	{
		// clang-format on
		auto assembly = domain->get_assembly("tests_managed.dll");
		REQUIRE(assembly.valid() == true);

		auto cls = assembly.get_class("ClassInstanceTest");
		REQUIRE(cls.get_internal_ptr() != nullptr);

		auto prop = cls.get_property("someProperty");
		REQUIRE(prop.get_internal_ptr() != nullptr);

		auto obj = cls.new_instance();
		REQUIRE(obj.get_internal_ptr() != nullptr);

		auto someProp = prop.get_value<int>(obj);
		REQUIRE(someProp == 12);

		int arg = 55;
		prop.set_value(obj, arg);

		someProp = prop.get_value<int>(obj);
		REQUIRE(someProp == 55);
		// clang-format off
	};
	// clang-format on

	REQUIRE_NOTHROW(expression());
}

TEST_CASE("get invalid property", "[class]")
{
	// clang-format off
	auto expression = []()
	{
		// clang-format on
		auto assembly = domain->get_assembly("tests_managed.dll");
		REQUIRE(assembly.valid() == true);

		auto cls = assembly.get_class("ClassInstanceTest");
		REQUIRE(cls.get_internal_ptr() != nullptr);

		REQUIRE_THROWS([&]() { cls.get_property("someInvalidProperty"); }());
		// clang-format off
	};
	// clang-format on

	REQUIRE_NOTHROW(expression());
}

TEST_CASE("call static method 1", "[method]")
{
	// clang-format off
	auto expression = []()
	{
		// clang-format on
		auto assembly = domain->get_assembly("tests_managed.dll");
		auto cls = assembly.get_class("ClassInstanceTest");
		auto method_thunk = cls.get_method<int(int)>("FunctionWithIntParam");
		const auto number = 1000;
		auto result = method_thunk(number);
		REQUIRE(number + 1337 == result);
		// clang-format off
	};
	// clang-format on

	REQUIRE_NOTHROW(expression());
}

TEST_CASE("call static method 2", "[method]")
{
	// clang-format off
	auto expression = []()
	{
		// clang-format on
		auto assembly = domain->get_assembly("tests_managed.dll");
		auto cls = assembly.get_class("ClassInstanceTest");
		auto method_thunk = cls.get_method<void(float, int, float)>("VoidFunction");
		method_thunk(13.37f, 42, 9000.0f);
		// clang-format off
	};
	// clang-format on

	REQUIRE_NOTHROW(expression());
}

TEST_CASE("call static method 3", "[method]")
{
	// clang-format off
	auto expression = []()
	{
		// clang-format on
		auto assembly = domain->get_assembly("tests_managed.dll");
		auto cls = assembly.get_class("ClassInstanceTest");
		auto method_thunk = cls.get_method<void(std::string)>("FunctionWithStringParam");
		method_thunk("Hello!");
		// clang-format off
	};
	// clang-format on

	REQUIRE_NOTHROW(expression());
}
TEST_CASE("call static method 4", "[method]")
{
	// clang-format off
	auto expression = []()
	{
		// clang-format on
		auto assembly = domain->get_assembly("tests_managed.dll");
		auto cls = assembly.get_class("ClassInstanceTest");
		auto method_thunk = cls.get_method<std::string(std::string)>("StringReturnFunction");
		auto expected_string = std::string("Hello!");
		auto result = method_thunk(expected_string);
		REQUIRE(result == std::string("The string value was: " + expected_string));
		// clang-format off
	};
	// clang-format on

	REQUIRE_NOTHROW(expression());
}

TEST_CASE("call static method 5", "[method]")
{
	// clang-format off
	auto expression = []()
	{
		// clang-format on
		auto assembly = domain->get_assembly("tests_managed.dll");
		auto cls = assembly.get_class("ClassInstanceTest");
		auto method_thunk = cls.get_method<void()>("ExceptionFunction");
		method_thunk();
		// clang-format off
	};
	// clang-format on

	REQUIRE_THROWS(expression());
}

TEST_CASE("call static method 6", "[method]")
{
	// clang-format off
	auto expression = []()
	{
		// clang-format on
		auto assembly = domain->get_assembly("tests_managed.dll");
		auto cls = assembly.get_class("ClassInstanceTest");
		auto method_thunk = cls.get_method<void()>("CreateStruct");
		method_thunk();
		// clang-format off
	};
	// clang-format on

	REQUIRE_NOTHROW(expression());
}

TEST_CASE("call member method 1", "[method]")
{
	// clang-format off
	auto expression = []()
	{
		// clang-format on
		auto assembly = domain->get_assembly("tests_managed.dll");
		auto cls = assembly.get_class("ClassInstanceTest");
		auto obj = cls.new_instance();
		auto method_thunk = cls.get_method<void()>("Method");
		method_thunk(obj);
		// clang-format off
	};
	// clang-format on

	REQUIRE_NOTHROW(expression());
}

TEST_CASE("call member method 2", "[method]")
{
	// clang-format off
	auto expression = []()
	{
		// clang-format on
		auto assembly = domain->get_assembly("tests_managed.dll");
		auto cls = assembly.get_class("ClassInstanceTest");
		auto obj = cls.new_instance();
		auto method_thunk =
			cls.get_method<std::string(std::string, int)>("MethodWithParameterAndReturnValue");
		auto result = method_thunk(obj, "test", 5);
		REQUIRE("Return Value: test" == result);
		// clang-format off
	};
	// clang-format on

	REQUIRE_NOTHROW(expression());
}

TEST_CASE("call member method 3", "[method]")
{
	// clang-format off
	auto expression = []()
	{
		// clang-format on
		auto assembly = domain->get_assembly("tests_managed.dll");
		auto cls = assembly.get_class("ClassInstanceTest");
		auto obj = cls.new_instance();

		auto method_thunk = cls.get_method<vec2f(vec2f)>("MethodPodAR");
		vec2f p;
		p.x = 12;
		p.y = 15;
		auto result = method_thunk(obj, p);
		REQUIRE(165.0f == result.x);
		REQUIRE(7.0f == result.y);
		// clang-format off
	};
	// clang-format on

	REQUIRE_NOTHROW(expression());
}
TEST_CASE("call member method 4", "[method]")
{
	// clang-format off
	auto expression = []()
	{
		// clang-format on
		auto assembly = domain->get_assembly("tests_managed.dll");
		auto cls = assembly.get_class("ClassInstanceTest");
		// auto fields = cls.get_fields();
		// auto props = cls.get_properties();

		auto obj = cls.new_instance();

		using vec2f_ptr = std::shared_ptr<vec2f>;

		auto ptr = std::make_shared<vec2f>();
		ptr->x = 12;
		ptr->y = 15;

		auto method_thunk = cls.get_method<vec2f_ptr(vec2f_ptr)>("MethodPodARW");
		auto result = method_thunk(obj, ptr);

		REQUIRE(result != nullptr);
		REQUIRE(55.0f == result->x);
		REQUIRE(66.0f == result->y);
		// clang-format off
	};
	// clang-format on

	REQUIRE_NOTHROW(expression());
}

TEST_CASE("test member POD field", "[method]")
{
	// clang-format off
	auto expression = []()
	{
		// clang-format on
		auto assembly = domain->get_assembly("tests_managed.dll");
		REQUIRE(assembly.valid() == true);

		auto cls = assembly.get_class("ClassInstanceTest");
		REQUIRE(cls.get_internal_ptr() != nullptr);

		auto field = cls.get_field("someFieldPOD");
		REQUIRE(field.get_internal_ptr() != nullptr);

		auto obj = cls.new_instance();
		REQUIRE(obj.get_internal_ptr() != nullptr);

		auto someField = field.get_value<vec2f>(obj);
		REQUIRE(someField.x == 12.0f);
		REQUIRE(someField.y == 13.0f);

		vec2f arg = {6.0f, 7.0f};
		field.set_value(obj, arg);

		someField = field.get_value<vec2f>(obj);
		REQUIRE(someField.x == 6.0f);
		REQUIRE(someField.y == 7.0f);
		// clang-format off
	};
	// clang-format on

	REQUIRE_NOTHROW(expression());
}
TEST_CASE("test member POD property", "[method]")
{
	// clang-format off
	auto expression = []()
	{
		// clang-format on
		auto assembly = domain->get_assembly("tests_managed.dll");
		REQUIRE(assembly.valid() == true);

		auto cls = assembly.get_class("ClassInstanceTest");
		REQUIRE(cls.get_internal_ptr() != nullptr);

		auto prop = cls.get_property("somePropertyPOD");
		REQUIRE(prop.get_internal_ptr() != nullptr);

		auto obj = cls.new_instance();
		REQUIRE(obj.get_internal_ptr() != nullptr);

		auto someProp = prop.get_value<vec2f>(obj);
		REQUIRE(someProp.x == 12.0f);
		REQUIRE(someProp.y == 13.0f);

		vec2f arg = {55.0f, 56.0f};
		prop.set_value(obj, arg);

		someProp = prop.get_value<vec2f>(obj);
		REQUIRE(someProp.x == 55.0f);
		REQUIRE(someProp.y == 56.0f);
		// clang-format off
	};
	// clang-format on

	REQUIRE_NOTHROW(expression());
}

TEST_CASE("test static POD field", "[method]")
{
	// clang-format off
	auto expression = []()
	{
		// clang-format on
		auto assembly = domain->get_assembly("tests_managed.dll");
		REQUIRE(assembly.valid() == true);

		auto cls = assembly.get_class("ClassInstanceTest");
		REQUIRE(cls.get_internal_ptr() != nullptr);

		auto field = cls.get_field("someFieldPODStatic");
		REQUIRE(field.get_internal_ptr() != nullptr);

		auto someField = field.get_value<vec2f>();
		REQUIRE(someField.x == 12.0f);
		REQUIRE(someField.y == 13.0f);

		vec2f arg = {6.0f, 7.0f};
		field.set_value(arg);

		someField = field.get_value<vec2f>();
		REQUIRE(someField.x == 6.0f);
		REQUIRE(someField.y == 7.0f);
		// clang-format off
	};
	// clang-format on

	REQUIRE_NOTHROW(expression());
}
TEST_CASE("test static POD property", "[method]")
{
	// clang-format off
	auto expression = []()
	{
		// clang-format on
		auto assembly = domain->get_assembly("tests_managed.dll");
		REQUIRE(assembly.valid() == true);

		auto cls = assembly.get_class("ClassInstanceTest");
		REQUIRE(cls.get_internal_ptr() != nullptr);

		auto prop = cls.get_property("somePropertyPODStatic");
		REQUIRE(prop.get_internal_ptr() != nullptr);

		auto someProp = prop.get_value<vec2f>();
		REQUIRE(someProp.x == 6.0f);
		REQUIRE(someProp.y == 7.0f);

		vec2f arg = {55.0f, 56.0f};
		prop.set_value(arg);

		someProp = prop.get_value<vec2f>();
		REQUIRE(someProp.x == 55.0f);
		REQUIRE(someProp.y == 56.0f);
		// clang-format off
	};
	// clang-format on

	REQUIRE_NOTHROW(expression());
}

TEST_CASE("test static NON-POD field", "[method]")
{
	// clang-format off
	auto expression = []()
	{
		// clang-format on
		auto assembly = domain->get_assembly("tests_managed.dll");
		REQUIRE(assembly.valid() == true);

		auto cls = assembly.get_class("ClassInstanceTest");
		REQUIRE(cls.get_internal_ptr() != nullptr);

		auto field = cls.get_field("someFieldNONPODStatic");
		REQUIRE(field.get_internal_ptr() != nullptr);

		using vec2f_ptr = std::shared_ptr<vec2f>;

		auto someField = field.get_value<vec2f_ptr>();
		REQUIRE(someField->x == 12.0f);
		REQUIRE(someField->y == 13.0f);

		vec2f_ptr arg = std::make_shared<vec2f>(vec2f{6.0f, 7.0f});
		field.set_value(arg);

		someField = field.get_value<vec2f_ptr>();
		REQUIRE(someField->x == 6.0f);
		REQUIRE(someField->y == 7.0f);
		// clang-format off
	};
	// clang-format on

	REQUIRE_NOTHROW(expression());
}
TEST_CASE("test static NON-POD property", "[method]")
{
	// clang-format off
	auto expression = []()
	{
		// clang-format on
		auto assembly = domain->get_assembly("tests_managed.dll");
		REQUIRE(assembly.valid() == true);

		auto cls = assembly.get_class("ClassInstanceTest");
		REQUIRE(cls.get_internal_ptr() != nullptr);

		auto prop = cls.get_property("somePropertyNONPODStatic");
		REQUIRE(prop.get_internal_ptr() != nullptr);

		using vec2f_ptr = std::shared_ptr<vec2f>;

		auto someProp = prop.get_value<vec2f_ptr>();
		REQUIRE(someProp->x == 6.0f);
		REQUIRE(someProp->y == 7.0f);

		vec2f_ptr arg = std::make_shared<vec2f>(vec2f{55.0f, 56.0f});
		prop.set_value(arg);

		someProp = prop.get_value<vec2f_ptr>();
		REQUIRE(someProp->x == 55.0f);
		REQUIRE(someProp->y == 56.0f);
		// clang-format off
	};
	// clang-format on

	REQUIRE_NOTHROW(expression());
}

TEST_CASE("destroy mono domain", "[domain]")
{
	// clang-format off
	auto expression = []()
	{
		// clang-format on
		domain.reset();
		// clang-format off
	};
	// clang-format on

	REQUIRE_NOTHROW(expression());
}

int main(int argc, char* argv[])
{
	if(!mono::init("mono", true))
	{
		return 1;
	}

	int res = 0;
	auto&& session = Catch::Session();
	for(int i = 0; i < 10; ++i)
	{
		res = session.run(argc, argv);
		if(res != 0)
			return res;
	}

	mono::shutdown();
	return res;
}

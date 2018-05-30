#include "monopp/mono_internal_call.h"
#include "monopp/mono_jit.h"

#include "monopp/mono_assembly.h"
#include "monopp/mono_domain.h"
#include "monopp/mono_field.h"
#include "monopp/mono_method.h"
#include "monopp/mono_object.h"
#include "monopp/mono_property.h"
#include "monopp/mono_string.h"
#include "monopp/mono_type.h"
#include <iostream>
#include <memory>

#include "monort/monort.h"

// Let Catch provide main():
#define CATCH_IMPL
#define CATCH_CONFIG_ALL_PARTS
#include "catch/catch.hpp"

static std::unique_ptr<mono::mono_domain> domain;

///////////////////////////////////////////////////////////////
/// \brief MONORT TESTS
///////////////////////////////////////////////////////////////

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

TEST_CASE("create MONOPP domain", "[domain]")
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
		auto refs = assembly.dump_references();
		for(const auto& ref : refs)
		{
			std::cout << ref << std::endl;
		}
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
		mono::add_internal_call("Tests.MyObject::CreateInternal", internal_call(MyObject_CreateInternal));
		mono::add_internal_call("Tests.MyObject::DestroyInternal", internal_call(MyObject_DestroyInternal));
		mono::add_internal_call("Tests.MyObject::DoStuff", internal_call(MyObject_DoStuff));
		mono::add_internal_call("Tests.MyObject::ReturnAString", internal_call(MyObject_ReturnAString));
		// clang-format off
	};
	// clang-format on

	REQUIRE_NOTHROW(expression());
}

TEST_CASE("get invalid type", "[assembly]")
{
	// clang-format off
	auto expression = []()
	{
		// clang-format on
		auto assembly = domain->get_assembly("tests_managed.dll");

		REQUIRE_THROWS([&]() { assembly.get_type("SometypeThatDoesntExist12345"); }());
		// clang-format off
	};
	// clang-format on

	REQUIRE_NOTHROW(expression());
}

TEST_CASE("get monopp valid type", "[assembly]")
{
	// clang-format off
	auto expression = []()
	{
		// clang-format on
		auto assembly = domain->get_assembly("tests_managed.dll");
		assembly.get_type("Tests", "MonoppTest");
		// clang-format off
	};
	// clang-format on

	REQUIRE_NOTHROW(expression());
}

TEST_CASE("get valid method", "[type]")
{
	// clang-format off
	auto expression = []()
	{
		// clang-format on
		auto assembly = domain->get_assembly("tests_managed.dll");
		auto type = assembly.get_type("Tests", "MonoppTest");

		auto method1 = type.get_method<void()>("Method1");
		auto method2 = type.get_method<void(std::string)>("Method2");
		auto method3 = type.get_method<void(int)>("Method3");
		auto method4 = type.get_method<void(int, int)>("Method4");
		auto method5 = type.get_method<std::string(std::string, int)>("Method5");
		// clang-format off
	};
	// clang-format on

	REQUIRE_NOTHROW(expression());
}

TEST_CASE("get/set field", "[type]")
{
	// clang-format off
	auto expression = []()
	{
		// clang-format on
		auto assembly = domain->get_assembly("tests_managed.dll");

		auto type = assembly.get_type("Tests", "MonoppTest");

		auto field = type.get_field("someField");

		auto obj = type.new_instance();

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

TEST_CASE("get/set static field", "[type]")
{
	// clang-format off
	auto expression = []()
	{
		// clang-format on
		auto assembly = domain->get_assembly("tests_managed.dll");

		auto type = assembly.get_type("Tests", "MonoppTest");

		auto field = type.get_field("someFieldStatic");

		auto someField = field.get_value<int>();
		REQUIRE(someField == 12);

		int arg = 6;
		field.set_value(arg);

		someField = field.get_value<int>();
		REQUIRE(someField == 6);
		// clang-format off
	};
	// clang-format on

	REQUIRE_NOTHROW(expression());
}

TEST_CASE("get invalid field", "[type]")
{
	// clang-format off
	auto expression = []()
	{
		// clang-format on
		auto assembly = domain->get_assembly("tests_managed.dll");
		auto type = assembly.get_type("Tests", "MonoppTest");

		REQUIRE_THROWS([&]() { type.get_field("someInvalidField"); }());
		// clang-format off
	};
	// clang-format on

	REQUIRE_NOTHROW(expression());
}

TEST_CASE("get/set property", "[type]")
{
	// clang-format off
	auto expression = []()
	{
		// clang-format on
		auto assembly = domain->get_assembly("tests_managed.dll");
		auto type = assembly.get_type("Tests", "MonoppTest");
		auto prop = type.get_property("someProperty");
		auto obj = type.new_instance();
		REQUIRE(obj.valid());

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

TEST_CASE("get/set static property", "[type]")
{
	// clang-format off
	auto expression = []()
	{
		// clang-format on
		auto assembly = domain->get_assembly("tests_managed.dll");
		auto type = assembly.get_type("Tests", "MonoppTest");
		auto prop = type.get_property("somePropertyStatic");

		auto someProp = prop.get_value<int>();
		REQUIRE(someProp == 6);

		int arg = 55;
		prop.set_value(arg);

		someProp = prop.get_value<int>();
		REQUIRE(someProp == 55);
		// clang-format off
	};
	// clang-format on

	REQUIRE_NOTHROW(expression());
}

TEST_CASE("get invalid property", "[type]")
{
	// clang-format off
	auto expression = []()
	{
		// clang-format on
		auto assembly = domain->get_assembly("tests_managed.dll");
		auto type = assembly.get_type("Tests", "MonoppTest");
		REQUIRE_THROWS([&]() { type.get_property("someInvalidProperty"); }());
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
		auto type = assembly.get_type("Tests", "MonoppTest");
		auto method_thunk = type.get_method<int(int)>("Function1");
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
		auto type = assembly.get_type("Tests", "MonoppTest");
		auto method_thunk = type.get_method<void(float, int, float)>("Function2");
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
		auto type = assembly.get_type("Tests", "MonoppTest");
		auto method_thunk = type.get_method<void(std::string)>("Function3");
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
		auto type = assembly.get_type("Tests", "MonoppTest");
		auto method_thunk = type.get_method<std::string(std::string)>("Function4");
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
		auto type = assembly.get_type("Tests", "MonoppTest");
		auto method_thunk = type.get_method<void()>("Function5");
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
		auto type = assembly.get_type("Tests", "MonoppTest");
		auto method_thunk = type.get_method<void()>("Function6");
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
		auto type = assembly.get_type("Tests", "MonoppTest");
		auto obj = type.new_instance();
		auto method_thunk = type.get_method<void()>("Method1");
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
		auto type = assembly.get_type("Tests", "MonoppTest");
		auto obj = type.new_instance();
		auto method_thunk = type.get_method<std::string(std::string, int)>("Method5");
		auto result = method_thunk(obj, "test", 5);
		REQUIRE("Return Value: test" == result);
		// clang-format off
	};
	// clang-format on

	REQUIRE_NOTHROW(expression());
}

TEST_CASE("destroy MONOPP domain", "[domain]")
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
TEST_CASE("full example", "[monopp]")
{
	/// Create an app domain. When destructed it will
	/// unload all loaded assemblies
	mono::mono_domain my_domain("my_domain");

	/// Sets the current domain
	mono::mono_domain::set_current_domain(my_domain);

	/// Get or load an assembly
	auto assembly = my_domain.get_assembly("tests_managed.dll");

	/// Get access to a c# type
	auto type = assembly.get_type("Tests", "MonoppTest");

	/// Prints out Tests
	std::cout << type.get_namespace() << std::endl;

	/// Prints out MonoppTest
	std::cout << type.get_name() << std::endl;

	/// Prints out Tests.MonoppTest
	std::cout << type.get_fullname() << std::endl;

	/// Checks and gets the base type of Tests.MonoppTest
	if(type.has_base_type())
	{
		auto base_type = type.get_base_type();
		std::cout << base_type.get_fullname() << std::endl;
	}

	/// Create an instance of it. Default constructed.
	auto obj = type.new_instance();

	/// There are several ways of getting access to methods
	/// Way 1, name + arg count
	auto method1 = type.get_method("Method1", 0);

	/// You can invoke it by creating a thunk and calling it passing
	/// the object it belongs to as the first parameter. Not passing
	/// an object as the first parameter will treat it as a static method
	auto thunk1 = mono::mono_method_thunk<void()>(std::move(method1));
	thunk1(obj);

	/// Way 2, name + args
	auto method2 = type.get_method("Method2(string)");
	auto thunk2 = mono::mono_method_thunk<void(std::string)>(std::move(method1));
	thunk2(obj, "str_param");

	/// Way 3, use the template method
	auto method3 = type.get_method<std::string(std::string, int)>("Method5");
	auto result3 = method3(obj, "test", 5);

	/// You can also get and invoke static methods without passing
	/// an object as the first parameter
	auto method4 = type.get_method<int(int)>("Function1");
	auto result4 = method4(55);
	std::cout << result4 << std::endl;
	/// You can query various information about a method
	method1.get_name();
	method1.get_fullname();
	method1.get_full_declname();
	method1.get_visibility();
	method1.is_static();
	method1.get_param_types();
	method1.get_return_type();
	/// etc.

	/// You can catch exceptions like so
	try
	{
		type.get_method<int(int, float)>("NonExistingFunction");
	}
	catch(const mono::mono_exception& e)
	{
		std::cout << e.what() << std::endl;
	}

	/// You can access type fields by name
	auto field = type.get_field("someField");

	/// You can get their values. Not passing an
	/// object as the parameter would treat
	/// it as being static.

	auto field_value = field.get_value<int>(obj);
	// auto field_value = field.get_value<int>();
	std::cout << field_value << std::endl;

	/// You can set their values. Not passing an
	/// object as the parameter would treat
	/// it as being static.
	int field_arg = 55;
	field.set_value(obj, field_arg);
	// field.set_value(field_arg);

	/// You can query various information for a field.
	field.get_name();
	field.get_fullname();
	field.get_full_declname();
	field.get_type();
	field.get_visibility();
	field.is_static();
	/// etc..

	auto prop = type.get_property("someProperty");

	/// You can get their values. Not passing an
	/// object as the parameter would treat
	/// it as being static.
	auto prop_value = prop.get_value<int>(obj);
	// auto prop_value = prop.get_value<int>();
	std::cout << prop_value << std::endl;

	/// You can set their values. Not passing an
	/// object as the parameter would treat
	/// it as being static.
	int prop_arg = 55;
	prop.set_value(obj, prop_arg);
	// prop.set_value(prop_arg);

	/// You can get access to the get and set
	/// methods of a property
	auto getter = prop.get_get_method();
	auto setter = prop.get_set_method();
	/// You can treat these methods as you would any other method
	auto getter_thunk = mono::mono_method_thunk<int()>(std::move(getter));
	prop_value = getter_thunk(obj);

	auto setter_thunk = mono::mono_method_thunk<void(int)>(std::move(setter));
	setter_thunk(obj, 12);

	/// You can query various information for a field.
	prop.get_name();
	prop.get_fullname();
	prop.get_full_declname();
	prop.get_type();
	prop.get_visibility();
	prop.is_static();
	/// etc..

	/// Get all the fields of the type
	auto fields = type.get_fields();

	for(const auto& f : fields)
	{
		std::cout << f.get_full_declname() << std::endl;
	}

	/// Get all the properties of the type
	auto props = type.get_properties();

	for(const auto& p : props)
	{
		std::cout << p.get_full_declname() << std::endl;
	}

	/// Get All the methods of the type
	auto methods = type.get_methods();

	for(const auto& method : methods)
	{
		std::cout << method.get_full_declname() << std::endl;
	}
}
///////////////////////////////////////////////////////////////
/// \brief MONORT TESTS
///////////////////////////////////////////////////////////////

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

TEST_CASE("create MONORT domain", "[domain]")
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

TEST_CASE("get monort valid type", "[assembly]")
{
	// clang-format off
	auto expression = []()
	{
		// clang-format on
		auto assembly = domain->get_assembly("tests_managed.dll");
		auto type = assembly.get_type("Tests", "MonortTest");

		std::cout << type.get_fullname() << std::endl;
		auto fields = type.get_fields();
		auto props = type.get_properties();
		auto methods = type.get_methods();
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

TEST_CASE("call member method 3", "[method]")
{
	// clang-format off
	auto expression = []()
	{
		// clang-format on
		auto assembly = domain->get_assembly("tests_managed.dll");
		auto type = assembly.get_type("Tests", "MonortTest");
		auto obj = type.new_instance();

		auto method_thunk = type.get_method<vec2f(vec2f)>("MethodPodAR");
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
		auto type = assembly.get_type("Tests", "MonortTest");

		auto obj = type.new_instance();

		using vec2f_ptr = std::shared_ptr<vec2f>;

		auto ptr = std::make_shared<vec2f>();
		ptr->x = 12;
		ptr->y = 15;

		auto method_thunk = type.get_method<vec2f_ptr(vec2f_ptr)>("MethodPodARW");
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
		auto type = assembly.get_type("Tests", "MonortTest");
		auto field = type.get_field("someFieldPOD");
		auto obj = type.new_instance();
		REQUIRE(obj.valid());

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
		auto type = assembly.get_type("Tests", "MonortTest");
		auto prop = type.get_property("somePropertyPOD");
		auto obj = type.new_instance();
		REQUIRE(obj.valid());

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
		auto type = assembly.get_type("Tests", "MonortTest");
		auto field = type.get_field("someFieldPODStatic");
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
		auto type = assembly.get_type("Tests", "MonortTest");
		auto prop = type.get_property("somePropertyPODStatic");
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
		auto type = assembly.get_type("Tests", "MonortTest");
		auto field = type.get_field("someFieldNONPODStatic");
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
		auto type = assembly.get_type("Tests", "MonortTest");
		auto prop = type.get_property("somePropertyNONPODStatic");

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

TEST_CASE("destroy MONORT domain", "[domain]")
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

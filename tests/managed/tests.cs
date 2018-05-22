using System;
using System.Runtime.CompilerServices;
using System.IO;
using System.Text;
using System.Net;
using System.Collections.Generic;
using System.Linq;

namespace Tests
{
class MyObject
{
public
	MyObject()
	{
		CreateInternal(5.0f, "test");
	}

	~MyObject()
	{
		DestroyInternal();
	}

	[MethodImpl(MethodImplOptions.InternalCall)] private extern void CreateInternal(float x, string s);

	[MethodImpl(MethodImplOptions.InternalCall)] private extern void DestroyInternal();

	[MethodImpl(MethodImplOptions.InternalCall)] public extern void DoStuff(string value);

	[MethodImpl(MethodImplOptions.InternalCall)] public extern string ReturnAString(string value);
}

public struct Vector2f
{
public
	float x;
public
	float y;
}

public class WrapperVector2f : Monopp.Core.NativeObject
{
	[MethodImpl(MethodImplOptions.InternalCall)] public extern WrapperVector2f(float x, float y);

	[MethodImpl(MethodImplOptions.InternalCall)] public extern WrapperVector2f(WrapperVector2f rhs);

public
	void Foo()
	{
	}
}
}

public
class ClassInstanceTest
{
public
	int someField = 12;

public
	int someProperty
	{
		get
		{
			return someField;
		}
		set
		{
			Console.WriteLine("FROM C# : Setting property value to {0}", value);
			someField = value;
		}
	}

public
	Tests.Vector2f someFieldPOD;

public
	Tests.Vector2f somePropertyPOD
	{
		get
		{
			return someFieldPOD;
		}
		set
		{
			Console.WriteLine("FROM C# : Setting property value to {0}, {1}", value.x, value.y);
			someFieldPOD = value;
		}
	}

public
	ClassInstanceTest()
	{
		Console.WriteLine("FROM C# : ClassInstanceTest created.");
	}

	~ClassInstanceTest()
	{
		Console.WriteLine("FROM C# : ClassInstanceTest destroyed.");
	}

public
	void Method()
	{
		Console.WriteLine("FROM C# : Hello Mono World from instance.");
	}

public
	Tests.Vector2f MethodPodAR(Tests.Vector2f bb)
	{
		Console.WriteLine(bb.x);
		Console.WriteLine(bb.y);
		var s = new Tests.Vector2f();
		s.x = 165.0f;
		s.y = 7.0f;

		return s;
	}

	Tests.WrapperVector2f MethodPodARW(Tests.WrapperVector2f bb)
	{
		Console.WriteLine("FROM C# :");
		var s = new Tests.WrapperVector2f(55.0f, 66.0f);
		var s1 = new Tests.WrapperVector2f(s);

		s.Foo();
		s1.Foo();
		return s;
	}

	void MethodWithParameter(string s)
	{
		Console.WriteLine("FROM C# : WithParam string: " + s);
	}
	void MethodWithParameter(int s)
	{
		Console.WriteLine("FROM C# : WithParam int: " + s);
	}
	void MethodWithParameter(int s, int s1)
	{
		Console.WriteLine("FROM C# : WithParam int, int: {0}, {1}", s, s1);
	}
public
	string MethodWithParameterAndReturnValue(string s, int b)
	{
		Console.WriteLine("FROM C# : WithParam: {0}, {1}", s, b);
		return "Return Value: " + s;
	}

public
	static int FunctionWithIntParam(int a)
	{
		Console.WriteLine("FROM C# : Int value: " + a);
		return a + 1337;
	}

public
	static void VoidFunction(float a, int b, float c)
	{
		Console.WriteLine("FROM C# : VoidMethod: " + a + ", " + b + ", " + c);
	}

public
	static void CreateStruct()
	{
		Tests.MyObject obj = new Tests.MyObject();
		obj.DoStuff("blalba");
	}

public
	static void FunctionWithStringParam(string a)
	{
		Console.WriteLine("FROM C# : String value: " + a);
	}

public
	static void ExceptionFunction()
	{
		throw new Exception("Hello!");
	}

public
	static string StringReturnFunction(string str)
	{
		return "The string value was: " + str;
	}
}

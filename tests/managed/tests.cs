using System;
using System.Runtime.CompilerServices;

namespace Ethereal
{
    class MyObject
    {
        public MyObject()
        {
            CreateInternal();
        }

        ~MyObject()
        {
            DestroyInternal();
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern void CreateInternal();

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern void DestroyInternal();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void DoStuff(string value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern string ReturnAString(string value);
    }
}

public class ClassInstanceTest
{
    public int someField = 12;
        
    public ClassInstanceTest()
    {
        Console.WriteLine("FROM C# : ClassInstanceTest created.");

        Ethereal.MyObject s = new Ethereal.MyObject();
        s.DoStuff("Hello from C#!");

        Console.WriteLine("FROM C# : " + s.ReturnAString("Testing ReturnAString."));
    }

    ~ClassInstanceTest()
    {
        Console.WriteLine("FROM C# : ClassInstanceTest destroyed.");
    }

    public void Method()
    {
        Console.WriteLine("FROM C# : Hello Mono World from instance.");
    }

    public void MethodWithParameter(string s)
    {
        Console.WriteLine("FROM C# : WithParam: " + s);
    }

    public string MethodWithParameterAndReturnValue(string s)
    {
        Console.WriteLine("FROM C# : WithParam: " + s);
        return "Return Value: " + s;
    }

    public static int FunctionWithIntParam(int a)
    {
        Console.WriteLine("FROM C# : Int value: " + a);
        return a + 1337;
    }

    public static void VoidFunction(float a, int b, float c)
    {
        Console.WriteLine("FROM C# : VoidMethod: " + a + ", " + b + ", " + c);
    }

    public static void FunctionWithStringParam(string a)
    {
        Console.WriteLine("FROM C# : String value: " + a);
    }

    public static void ExceptionFunction()
    {
        throw new Exception("Hello!");
    }

    public static string StringReturnFunction(string str)
    {
        return "The string value was: " + str;
    }
}

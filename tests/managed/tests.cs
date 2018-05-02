using System;
using System.Runtime.CompilerServices;
using System.IO;
using System.Text;
using System.Net;
using System.Collections.Generic;
using System.Linq;

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
        string path = @"/home/default/MyTest.txt";

                try
                {

                    // Delete the file if it exists.
                    if (File.Exists(path))
                    {
                        // Note that no lock is put on the
                        // file and the possibility exists
                        // that another process could do
                        // something with it between
                        // the calls to Exists and Delete.
                        File.Delete(path);
                    }

                    // Create the file.
                    using (FileStream fs = File.Create(path))
                    {
                        Byte[] info = new UTF8Encoding(true).GetBytes("This is some text in the file.");
                        // Add some information to the file.
                        fs.Write(info, 0, info.Length);
                    }

                    // Open the stream and read it back.
                    using (StreamReader sr = File.OpenText(path))
                    {
                        string s = "";
                        while ((s = sr.ReadLine()) != null)
                        {
                            Console.WriteLine(s);
                        }
                    }
                }

                catch (Exception ex)
                {
                    Console.WriteLine(ex.ToString());
                }

        //string search = "lookforme";
        List<int> myList = new List<int>();
        myList.Add(1);
        myList.Add(2);
        myList.Add(3);
        myList.Add(4);
        var result = myList.Where(s => s > 2);
        foreach(var r in result)
        {
            Console.WriteLine(r);
        }



        Console.WriteLine("FROM C# : ClassInstanceTest created.");

        Ethereal.MyObject s1 = new Ethereal.MyObject();
        s1.DoStuff("Hello from C#!");

        Console.WriteLine("FROM C# : " + s1.ReturnAString("Testing ReturnAString."));
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

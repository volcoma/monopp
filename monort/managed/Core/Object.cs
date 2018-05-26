using System;
using System.Runtime.CompilerServices;

namespace Monopp.Core
{
    public abstract class NativeObject
    {
#pragma warning disable 414
        private IntPtr native_this_ = IntPtr.Zero;
#pragma warning restore 414
    
        [MethodImpl(MethodImplOptions.InternalCall)]
        extern ~NativeObject();
    }

}

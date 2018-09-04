using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;

namespace HPSocketCS
{
    public class Common
    {
        /// <summary>
        /// 
        /// </summary>
        /// <param name="ptr"></param>
        public static string PtrToAnsiString(IntPtr ptr)
        {
            string str = "";
            try
            {
                if (ptr != IntPtr.Zero)
                {
                    str = Marshal.PtrToStringAnsi(ptr);
                }
            }
            catch (Exception)
            {
            }
            return str;
        }
    }
}

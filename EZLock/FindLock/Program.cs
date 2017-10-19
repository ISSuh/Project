using System;
using System.Threading;
using System.Diagnostics;
using System.Runtime.InteropServices;

namespace FindLock
{
    class Program
    {
        [DllImport("kernel32.dll")]
        static extern IntPtr GetConsoleWindow();
        [DllImport("User32.dll")]
        static extern bool ShowWindow(IntPtr hWnd, int nCmdShow);

        const int SW_HIDE = 0;
        
        static void Main(string[] args)
        {
            var handle = GetConsoleWindow();
            ShowWindow(handle, SW_HIDE);

            ProcessStartInfo PSinfo = new ProcessStartInfo();
            Process PS = new Process();

            PSinfo.FileName = @"C:\Users\SIS\Desktop\새 폴더\테스트용 윈폼";
            PSinfo.CreateNoWindow = true;
            PSinfo.UseShellExecute = false;
            PS.StartInfo = PSinfo;

            while (true)
            {
                Process[] processList = Process.GetProcessesByName("테스트용 윈폼");
                ProcessModule objCurrentModule = Process.GetCurrentProcess().MainModule;
                if (processList.Length < 1)
                {
                    PS.Start();
                }

                Thread.Sleep(1000);
            }
        }

        private static void ProcessCreate()
        {
   
        }
    }
}

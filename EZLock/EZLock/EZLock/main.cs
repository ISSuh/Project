using System;
using System.Drawing;
using System.Windows.Forms;
using System.Diagnostics;
using System.Runtime.InteropServices;
using System.Threading;
using System.Net.NetworkInformation;
using System.Net;
using System.Net.Sockets;
using System.Text;
using System.IO;

using Gma.UserActivityMonitor;

namespace EZLock
{
    public partial class main : Form
    {
        [DllImport("User32.dll")]
        static extern IntPtr GetDC(IntPtr hwnd);

        public System.Threading.Thread Kill;
        // public System.Threading.Thread CreateFL;
        public System.Threading.Thread Unlock_SOCKET;
        System.Windows.Forms.Timer timer = new System.Windows.Forms.Timer();

        String STATE = "1";                                                                // 0일때 언락 1일때 락 

        public main()
        {
            InitializeComponent();

            Interface();
            LOCK();
        }

        private void main_Load(object sender, EventArgs e)
        {
            ProcessModule objCurrentModule = Process.GetCurrentProcess().MainModule; //프로세스리스트
        }

        private void main_FormClosing(object sender, FormClosingEventArgs e)
        {
            //CreateFL.Abort();
            //lock_SOCKET.Abort();
        }

        private void 설정ToolStripMenuItem_Click(object sender, EventArgs e)
        {
            Application.Restart();
            /*Setting set = new Setting();
            set.ShowDialog();*/
        }

        protected override bool ProcessCmdKey(ref Message msg, Keys keyData)        // 키보드 후킹
        {
            if (keyData == (Keys.Alt | Keys.F4))
            {
                MouseBlockUnlock();
                InterceptMouse.End();

                Kill.Abort();

                this.Visible = false;

                MessageBox.Show("Alt + F4 catched!");
                //MessageBox.Show(GetMacAddress() + " " + GetIPAddress());

                timer_event();
                return true;
            }

            else
                return base.ProcessCmdKey(ref msg, keyData);

        }

        private void timer_tick(object sender, EventArgs e)                         // 시간 경과시 PC잠금
        {
            Application.Restart();
        }                      

        private void timer_event()                                                  // 타이머 설정
        {
            SendtoServer_unlock();

            timer.Enabled = true;
            timer.Interval = 3000;
            timer.Tick += new EventHandler(timer_tick);
            timer.Start();
        }

        private void Interface()
        {
            int W = System.Windows.Forms.SystemInformation.VirtualScreen.Width;
            int H = System.Windows.Forms.SystemInformation.VirtualScreen.Height;

            this.BackgroundImage = Image.FromFile(@"E:\Projects\Visual Studio 2015\Projects\EZLock\EZLock\lock.jpg");
            this.BackgroundImageLayout = ImageLayout.Stretch;

            this.FormBorderStyle = FormBorderStyle.None;
            this.Size = new System.Drawing.Size(W, H);
            this.Location = new Point(0, 0);
            //this.TopMost = true;

            notifyIcon.ContextMenuStrip = contextMenuStrip;
        }                                                 // 인터페이스 설정

        private void LOCK()
        {
            InterceptMouse.Start();                                                  // 마우스 블락
            MouseBlockLock();

            SendtoServer_lock();

            ThreadTask();                                                            // 작업관리자 프로세스 끝내기 스레드
            timer.Stop();
        }                                                      //PC락

        private void ThreadTask()
        {
            Kill = new Thread(new ThreadStart(this.KillTaskMg));
            //CreateFL = new Thread(new ThreadStart(this.CreatFindlock));
            //lock_SOCKET = new Thread(new ThreadStart(this.lock_socket_tread));
            Unlock_SOCKET = new Thread(new ThreadStart (this.unlock_socket_tread));

            Kill.Start();
            //CreateFL.Start()
        
            Unlock_SOCKET.Start();
        }                                                // 쓰레드 생성

        private void KillTaskMg()
        {
            while (true)
            {
                Process[] processList = Process.GetProcessesByName("Taskmgr");
                if (processList.Length > 0)
                    //processList[0].Kill();

                    Thread.Sleep(500);
            }
        }                                                // 작업관리자 프로세스가 있다면 작업관리자종료

        private void SendtoServer_lock()
        {
            IPEndPoint ipep = new IPEndPoint(IPAddress.Parse("192.168.0.18"), 9777);                                // 클라이언트로써 서버접속
            Socket server = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
            byte[] H_S = Encoding.UTF8.GetBytes("11");

            server.Connect(ipep);

            server.Send(H_S);

            server.Close();
        }                                         // 서버에 현 상황을 알림

        private void SendtoServer_unlock()
        {
            IPEndPoint ipep = new IPEndPoint(IPAddress.Parse("192.168.0.18"), 9777);                                // 클라이언트로써 서버접속
            Socket server = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
            byte[] H_S = Encoding.UTF8.GetBytes("12");

            server.Connect(ipep);

            server.Send(H_S);

            server.Close();
        }

        private void unlock_socket_tread()
        {
            Socket Server_s = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
            IPEndPoint iplocal = new IPEndPoint(IPAddress.Any, 9988);
            Socket client_s;
            IPEndPoint newclient;
            ASCIIEncoding asen = new ASCIIEncoding();

            byte[] data = new byte[512];

            Server_s.Bind(iplocal);
            Server_s.Listen(1);
            client_s = Server_s.Accept();
            newclient = (IPEndPoint)client_s.RemoteEndPoint;

            client_s.Receive(data);

            STATE = System.Text.Encoding.UTF8.GetString(data);

            if (int.Parse(STATE) == 0)
            {
                MouseBlockUnlock();
                InterceptMouse.End();

                Kill.Abort();
                this.Invoke(new MethodInvoker(delegate () { this.Visible = false;}));
                MessageBox.Show(STATE);

                Server_s.Close();
                this.Invoke(new MethodInvoker(delegate () { this.timer_event();}));
            }

            Server_s.Close();
        }                                       // 서버로써 클라이언트에서 언락 데이터 받음

        private void MouseBlockLock()
        {
            HookManager.MouseUp_Block = true;
            HookManager.MouseDown_Block = true;
        }                                            // 마우스 블락

        private void MouseBlockUnlock()
        {
            HookManager.MouseUp_Block = false;
            HookManager.MouseDown_Block = false;
        }                                          // 마우스 블락 해제

        private string GetMacAddress()
        {
            return NetworkInterface.GetAllNetworkInterfaces()[0].GetPhysicalAddress().ToString();
        }                                           // 해당 PC의 MAC주소 검색
    
        private string GetIPAddress()
        {
            string ip = "";
            IPAddress[] host = Dns.GetHostAddresses(Dns.GetHostName());
            foreach (var item in host)
            {
                if (item.AddressFamily == System.Net.Sockets.AddressFamily.InterNetwork)
                {
                    ip = item.ToString();
                }
            }
            return ip;
        }                                            // 해당 PC의 IP주소 검색 

        /* private void CreatFindlock()
         {
             ProcessStartInfo PSinfo = new ProcessStartInfo();
             Process PS = new Process();

             PSinfo.FileName = @"C:\Users\SIS\Desktop\새 폴더 (2)\FindLock";
             PSinfo.CreateNoWindow = false;
             PSinfo.UseShellExecute = false;
             PS.StartInfo = PSinfo;

             while (true)
             {
                 Process[] processList = Process.GetProcessesByName("FindLock");

                 if (processList.Length < 1)
                     PS.Start();

                 Thread.Sleep(500);
             }
         }   */                                          // FindLock프로세스 검색

        class InterceptMouse                                                         // 마우스 후킹 모듈
        {

            private static LowLevelMouseProc _proc = HookCallback;
            private static IntPtr _hookID = IntPtr.Zero;

            public static void Start()
            {
                _hookID = SetHook(_proc);
            }

            public static void End()
            {
                UnhookWindowsHookEx(_hookID);
            }

            private static IntPtr SetHook(LowLevelMouseProc proc)
            {
                using (Process curProcess = Process.GetCurrentProcess())
                using (ProcessModule curModule = curProcess.MainModule)
                {
                    return SetWindowsHookEx(WH_MOUSE_LL, proc,
                        GetModuleHandle(curModule.ModuleName), 0);
                }
            }

            private delegate IntPtr LowLevelMouseProc(int nCode, IntPtr wParam, IntPtr lParam);

            private static IntPtr HookCallback(
                int nCode, IntPtr wParam, IntPtr lParam)
            {
                if (nCode >= 0 &&
                    MouseMessages.WM_LBUTTONDOWN == (MouseMessages)wParam)
                {
                    MSLLHOOKSTRUCT hookStruct = (MSLLHOOKSTRUCT)Marshal.PtrToStructure(lParam, typeof(MSLLHOOKSTRUCT));
                    Console.WriteLine(hookStruct.pt.x + ", " + hookStruct.pt.y);
                    return _hookID;
                }

                return CallNextHookEx(_hookID, nCode, wParam, lParam);
            }

            private const int WH_MOUSE_LL = 14;

            private enum MouseMessages
            {
                WM_LBUTTONDOWN = 0x0201,
                WM_LBUTTONUP = 0x0202,
                WM_MOUSEMOVE = 0x0200,
                WM_MOUSEWHEEL = 0x020A,
                WM_RBUTTONDOWN = 0x0204,
                WM_RBUTTONUP = 0x0205
            }

            [StructLayout(LayoutKind.Sequential)]
            private struct POINT
            {
                public int x;
                public int y;
            }

            [StructLayout(LayoutKind.Sequential)]
            private struct MSLLHOOKSTRUCT
            {
                public POINT pt;
                public uint mouseData;
                public uint flags;
                public uint time;
                public IntPtr dwExtraInfo;
            }

            [DllImport("user32.dll", CharSet = CharSet.Auto, SetLastError = true)]
            private static extern IntPtr SetWindowsHookEx(int idHook,
                LowLevelMouseProc lpfn, IntPtr hMod, uint dwThreadId);

            [DllImport("user32.dll", CharSet = CharSet.Auto, SetLastError = true)]
            [return: MarshalAs(UnmanagedType.Bool)]
            private static extern bool UnhookWindowsHookEx(IntPtr hhk);

            [DllImport("user32.dll", CharSet = CharSet.Auto, SetLastError = true)]
            private static extern IntPtr CallNextHookEx(IntPtr hhk, int nCode,
                IntPtr wParam, IntPtr lParam);

            [DllImport("kernel32.dll", CharSet = CharSet.Auto, SetLastError = true)]
            private static extern IntPtr GetModuleHandle(string lpModuleName);
        }
    }
}

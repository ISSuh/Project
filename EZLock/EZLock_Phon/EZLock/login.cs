using Android.App;
using Android.Widget;
using Android.OS;
using Android.Views;
using Android.Runtime;
using Android.Content;

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Net;
using System.Net.Sockets;
using System.Threading;

namespace EZLock
{
    [Activity(Label = "EZLock", MainLauncher = true, Icon = "@drawable/icon")]
    public class MainActivity : Activity
    {
        byte[] datalength = new byte[4];
        string Receive_Data = "TEST";

        Button Connect_B;
        AutoCompleteTextView User_ID;
        EditText User_PW;
        AutoCompleteTextView PCNum;


        protected override void OnCreate(Bundle bundle)
        {
            base.OnCreate(bundle);

            SetContentView (Resource.Layout.Main);
            
            Connect_B = FindViewById<Button>(Resource.Id.Connect_button);

            User_ID = FindViewById<AutoCompleteTextView>(Resource.Id.UserID);
            User_PW = FindViewById<EditText>(Resource.Id.UserPW);
            PCNum = FindViewById<AutoCompleteTextView>(Resource.Id.PCID);

            Connect_B.Click += Connect_Click;
            Toast.MakeText(this, Receive_Data, ToastLength.Short).Show();
            /*Button Login = FindViewById<Button>(Resource.Id.button1);
            Login.Click += delegate { StartActivity(typeof(Network)); };*/
        }


        void Connect_Click(object sender, EventArgs e)
        {

            IPEndPoint ipep = new IPEndPoint(IPAddress.Parse("192.168.0.18"), 9777);
            Socket server = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);

            string Num,flage;
            int Num_length;
            byte[] data = new byte[2];

            try
            {
                server.Connect(ipep);

                Num = PCNum.Text;
                Num_length = Num.Length;

                clientSend("2", server);

                //clientSend(Convert.ToString(Num_length));
                clientSend(Num, server);

                server.Receive(data);

                flage = Encoding.Default.GetString(data);
              
                if(flage == "1\0")
                {
                    Toast.MakeText(this, "잘못된 PC ID입니다", ToastLength.Short).Show();
                }

                else
                {
                    Toast.MakeText(this, "Connected", ToastLength.Short).Show();
                    StartActivity(typeof(Network));
                }
               
                server.Close();
            }
            catch (Exception ex)
            {
                Toast.MakeText(this, ex.Message, ToastLength.Short).Show();
                server.Close();
            }
        }

     /*   private void clientReceive()
        {
            try
            {
                // how to make a byte E.X byte[] examlpe = new byte[the size of the byte here] , i used BitConverter.ToInt32(datalength,0) cuz i received the length of the data in byte called datalength :D
                byte[] data = new byte[BitConverter.ToInt32(datalength, 0)]; // Creates a Byte for the data to be Received On
                server.Receive(data,server.Available,SocketFlags.None);
                Receive_Data = System.Text.Encoding.UTF8.GetString(data); //Receives The Real Data not the Size
            }
            catch (Exception ex)
            {
                Toast.MakeText(this, ex.Message, ToastLength.Short).Show();
            }
        }*/

        /* void buttonSend_Click(object sender, EventArgs e)
         {
             try
             {
                 if (client.Connected) // if the client is connected
                 {
                     clientSend(this.etextSend.Text); // uses the Function ClientSend and the msg as txtSend.Text
                 }
             }
             catch (Exception ex)
             {
                 Toast.MakeText(this, ex.Message, ToastLength.Short).Show();
             }
         }*/

        public void clientSend(string msg,Socket server)
        {
            try
            {
                byte[] data; // creates a new byte without mentioning the size of it cuz its a byte used for sending
                data = Encoding.Default.GetBytes(msg); // put the msg in the byte ( it automaticly uses the size of the msg )
                server.Send(data);
            }
            catch (Exception ex)
            {
                Toast.MakeText(this, ex.Message, ToastLength.Short).Show();
            }
        }

        void InitString(string res)
        {
            res = res.Replace("\0", string.Empty);
        }
    }
}


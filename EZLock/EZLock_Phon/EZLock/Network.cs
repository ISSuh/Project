using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Net;
using System.Net.Sockets;
using System.Threading;

using Android.App;
using Android.Content;
using Android.OS;
using Android.Runtime;
using Android.Views;
using Android.Widget;

namespace EZLock
{
    [Activity(Label = "Network")]
    public class Network : Activity
    {
        IPEndPoint ipep = new IPEndPoint(IPAddress.Parse("192.168.0.18"), 9777);
        Socket server = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);

        byte[] datalength = new byte[4];

        TextView textReceive;
        EditText etextSend;
        Button Connect_B;
        Button Send_B;

        protected override void OnCreate(Bundle savedInstanceState)
        {
            base.OnCreate(savedInstanceState);
            SetContentView(Resource.Layout.Network_Layout);

            Connect_B = FindViewById<Button>(Resource.Id.button1);
            Send_B = FindViewById<Button>(Resource.Id.button2);
       
            textReceive = FindViewById<TextView>(Resource.Id.textView1);
            etextSend = FindViewById<EditText>(Resource.Id.editText1);

            Send_B.Enabled = false;

            Connect_B.Click += button1_Click;
            //Send_B.Click += buttonSend_Click;
        }

        void button1_Click(object sender, EventArgs e)
        {
            try
            {
                if (Connect_B.Text == "Connect")
                {
                    server.Connect(ipep); //Trys to Connect
                    Toast.MakeText(this, "Connected", ToastLength.Short).Show();
                    Connect_B.Text = "Dis-Connect";
                    textReceive.Text = null;
                    Send_B.Enabled = true;

                    clientSend("2");
                    //clientReceive(); //Starts Receiving When Connected
                    server.Close();
                }
                else
                {
                    Connect_B.Text = "Connect";
                    Toast.MakeText(this, "Dis-Connected", ToastLength.Short).Show();
                    server.Close();
                }
            }
            catch (Exception ex)
            {
                Toast.MakeText(this, ex.Message, ToastLength.Short).Show();
            }
        }

        private void clientReceive()
        {
            try
            {
                new Thread(() => // Thread (like Timer)
                {
                    while (true)//Keeps Trying to Receive the Size of the Message or Data
                    {
                        // how to make a byte E.X byte[] examlpe = new byte[the size of the byte here] , i used BitConverter.ToInt32(datalength,0) cuz i received the length of the data in byte called datalength :D
                        byte[] data = new byte[BitConverter.ToInt32(datalength, 0)]; // Creates a Byte for the data to be Received On
                        server.Receive(data);
                        System.Text.Encoding.UTF8.GetString(data); //Receives The Real Data not the Size
                        this.RunOnUiThread(() => this.textReceive.Text = textReceive.Text + (Encoding.ASCII.GetString(data)) + System.Environment.NewLine);
                    }
                }).Start(); // Start the Thread
            }
            catch (Exception ex)
            {
                Toast.MakeText(this, ex.Message, ToastLength.Short).Show();
            }
        }

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

        public void clientSend(string msg)
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
    }
}
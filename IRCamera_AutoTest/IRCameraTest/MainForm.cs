//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************
using System.Globalization;
using System;
using System.Resources;
using System.Windows.Forms;
using System.Net.NetworkInformation;
using System.Collections.Generic;
using System.Diagnostics;
using System.Drawing;
using System.IO;
using System.Runtime.InteropServices;
using System.Threading;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Threading;
using Windows.Devices.Perception;
using Windows.Graphics.Imaging;
using Windows.Media;
using IRIQCapture;
using MyLog;
using MyXml;

namespace IRCameraTest
{
    [ComImport]
    [Guid("4ACCE8C4-3A72-4B17-8CFB-771DA0D12AFB")]
    [InterfaceType(ComInterfaceType.InterfaceIsIUnknown)]
    interface IClosableByteAccess
    {
        void Lock(out IntPtr buffer, out uint capacity);
        void Unlock();
    }

    public partial class MainForm : Form
    {

        // DllImport("user32.dll") to add hot-key register and unregister
        [System.Runtime.InteropServices.DllImport("user32.dll")] //申明API函数
        public static extern bool RegisterHotKey(
         IntPtr hWnd, // handle to window
         int id, // hot key identifier
         uint fsModifiers, // key-modifier options
         uint vk // virtual-key code
        );

        [System.Runtime.InteropServices.DllImport("user32.dll")] //申明API函数
        public static extern bool UnregisterHotKey(
         IntPtr hWnd, // handle to window
         int id // hot key identifier
        );

        private void ProcessHotkey(Message m) //按下设定的键时调用该函数
        {
            IntPtr id = m.WParam; //IntPtr用于表示指针或句柄的平台特定类型
            //MessageBox.Show(id.ToString());
            string sid = id.ToString();
            switch (sid)
            {
                // yes
                case "89":       
                    UnregisterHotKey(this.Handle, 89);
                    GetYesCmd(true);
                    break;
                // ok 
                case "79":
                    UnregisterHotKey(this.Handle, 79);
                    UnregisterHotKey(this.Handle, 78);
                    UnregisterHotKey(this.Handle, 82);
                    GetOkCmd();
                    break;
                
                // no and ng
                case "78":
                    UnregisterHotKey(this.Handle, 78);
                    UnregisterHotKey(this.Handle, 79);
                    UnregisterHotKey(this.Handle, 82);
                    GetNgCmd();
                    break;

                // retry
                case "82":
                    UnregisterHotKey(this.Handle, 78);
                    UnregisterHotKey(this.Handle, 79);
                    UnregisterHotKey(this.Handle, 82);
                    GetRetryCmd();
                    break;
            }
        }

        protected override void WndProc(ref Message m)//监视Windows消息
        {
            const int WM_HOTKEY = 0x0312;   //如果m.Msg的值为0x0312那么表示用户按下了热键
            if (m.Msg == WM_HOTKEY)         //判断热键
              {
                 ProcessHotkey(m);           //按下热键时调用ProcessHotkey()函数
              }
            base.WndProc(ref m);            //将系统消息传递自父类的WndProc
        }
       private int m_OkCount = 0;    
       private string logName = "c:\\logs\\SFTIRCameraTest";
       private LogFile Log = new LogFile();
       private XmlFile Xml = new XmlFile();       

      //  private 
        private PerceptionInfraredFrameSourceWatcher _infraredSourceWatcher;
        private List<PerceptionInfraredFrameSource> _infraredSourceList = null;
        private PerceptionInfraredFrameReader _infraredReader = null;
        private PerceptionInfraredFrameSource _infraredSource = null;

        private string Frame1 = "IRIlluminatedFrame.png";
        private string Frame2 = "IRAmbientFrame.png";
        private string SubFrame = "IRImage1.png";       // for auto program use picture
        private string mySubFrame = "IRImage.png";      // for our program show picture
        private DispatcherTimer timer;
        private Dispatcher dispatcher;
        private bool isCaptured = false;
        FrameProcessor _frameProcessor = null;
        private const int STATE_PASS = 0;
        private const int STATE_FAIL = 1;
        private const int STATE_SKIP = 2;

        #region Fields

        private static ResourceManager LocRM;
        private object illuminationEnabled;

        #endregion // Fields

        #region Constructor

        /// <summary>
        /// Initializes a new instance of the MainForm form class.
        /// </summary>
        public MainForm()
        {
            
            if (Log.LogFileCreate(logName))
            {
                Log.LogTmStart();           
            }
            else
            {
                Program.ExitApplication(255);
            }
            if (Xml.XmlFileCreate(logName))
            {
                Xml.XmlTmStart();
            }
            else
            {
                Program.ExitApplication(255);
            }

            InitializeComponent();
            try
            {
                //Load Guidence Image
                string ReferenceImage = "LEDBlink.bmp";
                System.Drawing.Image Image = System.Drawing.Image.FromFile(ReferenceImage);
                GuidImage.Image = Image;
                GuidImage.Height = Image.Height;
                GuidImage.Width = Image.Width;
                this.RetryBtn.Visible = false; 
            }
            catch(Exception e)
            { 
                 Log.LogWrite("LEDBlink.bmp not exist, quite app.");
                 Log.LogFail();
                 Log.WriteLogResult(STATE_FAIL);
                 Xml.XmlFail();
                 Xml.XmlFail();
                 Xml.WriteXmlResult(STATE_FAIL);
                 System.Environment.Exit(255);
            }           
        }

        #endregion // Constructor

        private async void Initialize()
        {
            try
            {
                var infraredAccess = await PerceptionInfraredFrameSource.RequestAccessAsync();
                if (infraredAccess == PerceptionFrameSourceAccessStatus.Allowed)
                {
                    _infraredSourceWatcher = PerceptionInfraredFrameSource.CreateWatcher();
                    EnableInfraredSourceChangedEvents();
                    _infraredSourceWatcher.Start();
                }
            }
            catch(Exception e)
            {
                QuiteApp(false);
            }

        }

        private void EnableInfraredSourceChangedEvents()
        {
            if (_infraredSourceList == null)
            {
                _infraredSourceList = new List<PerceptionInfraredFrameSource>();
            }

            _infraredSourceWatcher.SourceAdded += InfraredSourceWatcher_SourceAdded;
            _infraredSourceWatcher.SourceRemoved += InfraredSourceWatcher_SourceRemoved;
        }

        private void DisableInfraredSourceChangedEvents()
        {
            if (_infraredSourceWatcher != null)
            {
                _infraredSourceWatcher.SourceAdded -= InfraredSourceWatcher_SourceAdded;
                _infraredSourceWatcher.SourceRemoved -= InfraredSourceWatcher_SourceRemoved;
                if (_infraredSourceList.Count > 0)
                {
                    _infraredSourceList.Clear();
                }
                _infraredSourceList = null;
                _infraredSourceWatcher.Stop();
                _infraredSourceWatcher = null;
            }
        }

        private void InfraredSourceWatcher_SourceRemoved(PerceptionInfraredFrameSourceWatcher sender, PerceptionInfraredFrameSourceRemovedEventArgs args)
        {
            int removeIndex = _infraredSourceList.IndexOf(args.FrameSource);
            lbl_IRImage.ForeColor = System.Drawing.Color.Red;
            lbl_IRImage.Text = "Sensor {0} Unplugged. " + args.FrameSource.DisplayName;
            _infraredSourceList.RemoveAt(removeIndex);
        }

        private void InfraredSourceWatcher_SourceAdded(PerceptionInfraredFrameSourceWatcher sender, PerceptionInfraredFrameSourceAddedEventArgs args)
        {
            _infraredSourceList.Add(args.FrameSource);

            if (_infraredSource == null)
            {                
                if (OpenInfraredDevice(_infraredSourceList[0]))
                {                    
                    isCaptured = true;
                }
                else
                {
                    isCaptured = false;
                }
            }
        }

        private bool OpenInfraredDevice(PerceptionInfraredFrameSource infraredFrameSoruce)
        {
            bool isOpened = false;
            if (_infraredSource == null && infraredFrameSoruce != null)
            {
                try
                {
                    _infraredSource = infraredFrameSoruce;
                    _infraredReader = _infraredSource.OpenReader();
                    _infraredReader.FrameArrived += InfraredReader_FrameArrived;
                    isOpened = true;
                }
                catch
                {
                    isOpened = false;
                }
            }

            return isOpened;
        }

        private void CloseInfraredDevice()
        {
            if (_infraredReader != null)
            {
                _infraredReader.FrameArrived -= InfraredReader_FrameArrived;
                _infraredReader.Dispose();
                _infraredReader = null;
                _infraredSource = null;
            }
        }

        private void Shutdown()
        {
            CloseInfraredDevice();
            DisableInfraredSourceChangedEvents();
        }

        private void InfraredReader_FrameArrived(PerceptionInfraredFrameReader sender, PerceptionInfraredFrameArrivedEventArgs args)
        {
            ProcessInfraredFrame(/*sender,*/ args);
        }

        private void ProcessInfraredFrame(/*PerceptionInfraredFrameReader sender, */PerceptionInfraredFrameArrivedEventArgs args)
        {
            using (var frame = args.TryOpenFrame())
            {
                if (frame != null)
                {
                    using (var videoFrame = frame.VideoFrame)
                    {
                        if (videoFrame != null)
                        {
                            var IsGetIllumination = videoFrame.ExtendedProperties.TryGetValue(KnownPerceptionInfraredFrameSourceProperties.ActiveIlluminationEnabled, out illuminationEnabled);
                            
                            ProcessVideoFrame(videoFrame, Convert.ToBoolean(illuminationEnabled));
                        }
                    }
                }
            }
        }

        private void ProcessVideoFrame(VideoFrame videoFrame, Boolean ifIlluminationEnabled)
        {
            using (var originalBitmap = videoFrame.SoftwareBitmap)
            {
                if (originalBitmap != null)
                {
                    using (var softwareBitmap = SoftwareBitmap.Convert(originalBitmap, BitmapPixelFormat.Bgra8))
                    {
                        using (var convertedBuffer = softwareBitmap.LockBuffer(BitmapBufferAccessMode.Read))
                        {
                            IClosableByteAccess convertedByteAccess = (IClosableByteAccess)(Object)convertedBuffer;
                            IntPtr convertedBytes;
                            uint convertedCapacity = 0;
                            convertedByteAccess.Lock(out convertedBytes, out convertedCapacity);
                            //dispatcher = Dispatcher.CurrentDispatcher;
                            dispatcher.Invoke((Action)delegate ()
                            {
                                WriteableBitmap displayImageSourceInterimBitmap = new WriteableBitmap((int)softwareBitmap.PixelWidth,
                                    (int)softwareBitmap.PixelHeight, 96.0, 96.0, PixelFormats.Bgra32, null);

                                    displayImageSourceInterimBitmap.WritePixels(
                                        new Int32Rect(0, 0, softwareBitmap.PixelWidth, softwareBitmap.PixelHeight),
                                        (IntPtr)convertedBytes, (int)convertedCapacity, convertedBuffer.GetPlaneDescription(0).Stride);

                                    string filename = ifIlluminationEnabled ? Frame1 : Frame2;

                                    using (FileStream stream5 = new FileStream(filename, FileMode.Create))
                                    {
                                        PngBitmapEncoder encoder5 = new PngBitmapEncoder();
                                        encoder5.Frames.Add(System.Windows.Media.Imaging.BitmapFrame.Create(displayImageSourceInterimBitmap.Clone()));
                                        encoder5.Save(stream5);
                                    }
                            });
                            convertedByteAccess.Unlock();
                        }
                    }
                }
            }
        }
        private void ShowIRImage()
        {
            try
            {
                // Load the images.
                Bitmap bm1 = new Bitmap(Frame2);
                Bitmap bm2 = new Bitmap(Frame1);

                // Make a difference image.
                int wid = Math.Min(bm1.Width, bm2.Width);
                int hgt = Math.Min(bm1.Height, bm2.Height);

                // Get the differences.
                int[,] diffs = new int[wid, hgt];
                int max_diff = 0;
                for (int x = 0; x < wid; x++)
                {
                    for (int y = 0; y < hgt; y++)
                    {
                        // Calculate the pixels' difference.
                        System.Drawing.Color color1 = bm1.GetPixel(x, y);
                        System.Drawing.Color color2 = bm2.GetPixel(x, y);
                        diffs[x, y] = (int)(
                            Math.Abs(color1.R - color2.R) +
                            Math.Abs(color1.G - color2.G) +
                            Math.Abs(color1.B - color2.B));
                        if (diffs[x, y] > max_diff)
                            max_diff = diffs[x, y];
                    }
                }

                // Create the difference image.
                Bitmap bm3 = new Bitmap(wid, hgt);
                for (int x = 0; x < wid; x++)
                {
                    for (int y = 0; y < hgt; y++)
                    {
                        int clr = (int)(255.0 / max_diff * diffs[x, y]);
                        bm3.SetPixel(x, y, System.Drawing.Color.FromArgb(clr, clr, clr));
                    }
                }

                if (IRImage.Image != null)
                {
                    IRImage.Image.Dispose();
                }

                bm3.Save(SubFrame);
                bm3.Save(mySubFrame);
                
                //Display the result.
                System.Drawing.Image image = System.Drawing.Image.FromFile(mySubFrame);
                IRImage.Image = image;
                IRImage.Height = image.Height;
                IRImage.Width = image.Width;

            }
            catch (Exception e)
            {
                PassBtn.Enabled = false;
                lbl_IRImage.ForeColor = System.Drawing.Color.Red;
                lbl_IRImage.Text = e.ToString();
                Log.LogWrite("IRCamera capture function Fail: " + e.ToString());
            }

        }

        private void EnqueueTakePhoto() //new API to support FrameServer
        {
            _frameProcessor.WaitCaptureMutex();
            if (_frameProcessor.CaptureSessionPara.CaptureCount < Constant.QueueMaxCount)
            {
                _frameProcessor.CaptureSessionPara.CaptureCount++;
            }
            _frameProcessor.ReleaseCaptureMutex();
        }

        void timer_Tick(object sender, EventArgs e)
        {
            timer.Stop();
            
            if (m_OkCount == 1)
            {
                SubFrame = "IRImage1.png";
            }
            else if (m_OkCount == 2)
            {
                SubFrame = "IRImage2.png";
            }

            File.Delete(Frame1);
            File.Delete(Frame2);

           // new capture method to support FrameServer
            EnqueueTakePhoto();

            Log.LogWrite("Start Search Pic ..................");
            UInt32 m_SerachPicCount = 0;
            while (true)
            {
                System.Threading.Thread.Sleep(10);
                m_SerachPicCount++;
                if (File.Exists(Frame1) && File.Exists(Frame2))
                {
                    Log.LogWrite("find Capture Pic");
                    isCaptured = true;
                    break;
                }
                if (m_SerachPicCount > 200 )
                {
                    Log.LogWrite("find Capture Pic Time out.. exit");
                    break;
                }
            }
            Log.LogWrite("Search Pic End ....................");
                
            if (isCaptured == true)
            {
                ShowGuidStep("Capture Ok!");
                ShowIRImage();
                PassBtn.Visible = true;
                isCaptured = false;
            }
            else
            {
                PassBtn.Visible = false;
           //   Display the result.
                System.Drawing.Image image = System.Drawing.Image.FromFile("ConnectToCameraFail.jpg");
                IRImage.Image = image;
                IRImage.Height = image.Height;
                IRImage.Width = image.Width;

                ShowGuidStep("capture NG!");
                Log.LogWrite("IRCamera Fail: IR Camera capture NG!");
            }
            this.FailBtn.Enabled = false;       // use for auto
        }

        private void GetYesCmd(bool isYkey)        // auto capture
        {
            m_OkCount++;
            if (m_OkCount == 1)
            {
                Log.LogWrite("step 1: blink function test!");
            }
            else if (m_OkCount == 2)
            {
                Log.LogWrite("step 2: capture function test!");
            }

            if (isYkey)
            {
                Log.LogWrite("get Yes[Y] CMD to auto capture!");
            }          

            if (_frameProcessor == null)
            {
                Log.LogWrite("get CMD to new _frameProcessor");
                _frameProcessor = new FrameProcessor(Frame1, Frame2);
            }

            dispatcher = Dispatcher.CurrentDispatcher;
            timer = new DispatcherTimer { Interval = TimeSpan.FromSeconds(0) };
            timer.Tick += timer_Tick;
            timer.Start();  

            string ReferenceImagePath = "CapturePic.bmp";
            System.Drawing.Image Image = System.Drawing.Image.FromFile(ReferenceImagePath);
            GuidImage.Image = Image;
            GuidImage.Height = Image.Height;
            GuidImage.Width = Image.Width;

            this.RetryBtn.Text = "Retry";
            this.RetryBtn.Visible = true;
            this.PassBtn.Text = "Ok";
            this.PassBtn.Visible = true;
            this.FailBtn.Text = "NG";
            this.FailBtn.Visible = true;

            RegisterHotKey(this.Handle, 79, 0, (UInt32)Keys.O); //注册热键 Ok  
            RegisterHotKey(this.Handle, 78, 0, (UInt32)Keys.N); //注册热键 NG
            RegisterHotKey(this.Handle, 82, 0, (UInt32)Keys.R); //注册热键 Retry    
        }

        private void GetOkCmd()        // auto capture
        {
            Log.LogWrite("get Ok[O] result!");
            if(m_OkCount == 1)
            {
                ShowGuidStep("Press Yes[Y] to start capture...");
                this.RetryBtn.Visible = false;
                this.FailBtn.Visible = false;
                this.PassBtn.Text = "Yes";
                this.PassBtn.Visible = true;
                Log.LogWrite("IR Camera device blink function OK!"+ "\r\n");
                RegisterHotKey(this.Handle, 89, 0, (UInt32)Keys.Y); //wait for next capture loop  
            }
            else if (m_OkCount == 2)
            {
                Log.LogWrite("IR Camera device capture function OK!");
                QuiteApp(true);
            }
        }

        private void GetNgCmd()        // auto capture
        {
            Log.LogWrite("get Ng[N] result!");
            if (m_OkCount == 1)
            {
                Log.LogWrite("IR Camera device blink function NG!");
            }
            else if (m_OkCount == 2)
            {
                Log.LogWrite("IR Camera device capture function NG!");
            }

            QuiteApp(false);
        }

        private void GetRetryCmd()        // auto capture
        {
            try
            {
                //isCaptured = false;
                Log.LogWrite("get Retry[R] CMD to Retry!");
                ShowGuidStep("Retry[R] start...");
                Log.LogWrite("");
                Log.LogWrite("Retry!");
                m_OkCount--;
                GetYesCmd(false);
            }
            catch (Exception e)
            {
                Log.LogWrite("get R CMD error");
                QuiteApp(false);
            }
        }

        private void MainForm_Load(object sender, EventArgs e)
        {
            this.FormBorderStyle = FormBorderStyle.None;//Full screen and no title
            RegisterHotKey(this.Handle, 89, 0, (UInt32)Keys.Y); //注册热键 Yes  0x59
            try
            {               
                if (_frameProcessor == null)
                {
                    Log.LogWrite("start new _frameProcessor.........");
                    _frameProcessor = new FrameProcessor(Frame1, Frame2);
                }
                Log.LogWrite("start Search IRcameral is Open.........");
                UInt32 m_SearchCount = 0;
                while(true)
                {
                    System.Threading.Thread.Sleep(10);
                    m_SearchCount++;
                    if (_frameProcessor.isIRcameralOpen)
                    {
                        ShowGuidStep("Press Yes[Y] to start capture...");
                        Log.LogWrite("IR Camera open...........");
                        break;
                    }
                    if (m_SearchCount > 100)
                    {
                        Log.LogWrite("Search out time no IR Camera device open.. exit");
                        QuiteApp(false);
                        break;
                    }
                }           
            }
            catch (Exception ex)
            {
                Log.LogWrite("no IR Camera device available, exit");
                Shutdown();
                QuiteApp(false);
            }

        }

        private void ShowGuidStep(string guidence)
        {
            lbl_IRImage.ForeColor = System.Drawing.Color.Yellow;
            lbl_IRImage.Text = guidence;
        }

        private void QuiteApp(bool bPass)
        {
            DisableInfraredSourceChangedEvents();
            if(_frameProcessor!=null)
            {
                _frameProcessor.Dispose();
                _frameProcessor.Shutdown();
                _frameProcessor.StopIRDevice();
                _frameProcessor = null;
            }

            if(bPass)
            {
                Log.LogPass();
                Log.WriteLogResult(STATE_PASS);
                Xml.XmlPass();
                Xml.WriteXmlResult(STATE_PASS);
                Program.ExitApplication(0);
            }
            else
            {
                Log.LogFail();
                Log.WriteLogResult(STATE_FAIL);
                Xml.XmlFail();
                Xml.WriteXmlResult(STATE_FAIL);
                try
                {
                    Program.ExitApplication(255);
                }
                catch(Exception ex) 
                {
                    throw(ex);
                    Program.ExitApplication(255);
                    System.Environment.Exit(255);
                }


            }

            
        }

    }
}

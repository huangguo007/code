using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Threading;
using System.Windows;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Threading;
using Windows.Graphics.Imaging;
using MyLog;
using IRCameraTest;

namespace IRIQCapture
{
    [ComImport]
    [Guid("4ACCE8C4-3A72-4B17-8CFB-771DA0D12AFB")]
    [InterfaceType(ComInterfaceType.InterfaceIsIUnknown)]
    interface IClosableByteAccess
    {
        void Lock(out IntPtr buffer, out uint capacity);
        void Unlock();
    }

    [ComImport]
    [Guid("905a0fef-bc53-11df-8c49-001e4fc686da")]
    [InterfaceType(ComInterfaceType.InterfaceIsIUnknown)]
    interface IBufferByteAccess
    {
        unsafe void Buffer(out byte* pByte);
    }

    [ComImport]
    [Guid("5b0d3235-4dba-4d44-865e-8f1d0e4fd04d")]
    [InterfaceType(ComInterfaceType.InterfaceIsIUnknown)]
    unsafe interface IMemoryBufferByteAccess
    {
        void GetBuffer(out byte* buffer, out uint capacity);
    }

    public enum FrameType
    {
        Light = 0,
        Dark,
        Substracted,
        Unkown
    }

    public enum PreviewState
    {
        Playing = 0,
        Pause,
        Stop
    }

    public class FrameProcessor : IDisposable
    {
        private CaptureSessionParameter _captureSessionParameter;
        private PreviewParameter _previewParameter;

        //thread relative variables
        private ThreadsManager _frameProcessThread;
        private ThreadsManager _IRDeviceManagedThread;

        private FSSensorSourceManager _sensorSourceManager;

        //Image Queue defination
        private Queue<ImageFrame> _iRImageQueue;
        private AutoResetEvent _ImageAutoEvent;

        //capture times
        private static Mutex _mutexCapture = new Mutex();

        private int _selectedIRindex;

        private DispatcherTimer CaptureSucceedTimer = new DispatcherTimer();

        private AutoResetEvent _displayAutoEvent;

        private bool _IsAutoOpenIRDevice = true;

        private string _frame1;
        private string _frame2;

        public  bool isIRcameralOpen = false;

        #region Public variable

        public bool isMouseStatus
        {
            get
            {
                return _previewParameter.IsIlluminationValueEnabled;
            }
        }

        public void WaitCaptureMutex()
        {
            _mutexCapture.WaitOne();
        }

        public void ReleaseCaptureMutex()
        {
            _mutexCapture.ReleaseMutex();
        }

        public int selectedIRindex
        {
            set
            {
                _selectedIRindex = value;
            }

            get
            {
                return _selectedIRindex;
            }
        }

        public PreviewParameter PreviewParameters
        {
            set
            {
                _previewParameter = value;
            }
            get
            {
                return _previewParameter;
            }
        }

        public CaptureSessionParameter CaptureSessionPara
        {
            set
            {
                _captureSessionParameter = value;
            }
            get
            {
                return _captureSessionParameter;
            }
        }

        #endregion Public variable

        private void FrameProcessThread()
        {
            while (!_frameProcessThread.IsThreadStop)
            {
                _ImageAutoEvent.WaitOne();
                if (_previewParameter.PreviewStates == PreviewState.Playing)
                {
                    var framesCandidate = _captureSessionParameter.IsFaceMode ? Constant.FaceModeCandidates : Constant.NormalCandidates;
                    if (_iRImageQueue.Count > framesCandidate - 1)
                    {
                        ImageFrame[] imageFrameArray = new ImageFrame[framesCandidate];
                        FillArrayFromQueue(ref imageFrameArray, _iRImageQueue);
                        ProcessImageFrame(imageFrameArray, ref _previewParameter, ref _captureSessionParameter);
                        EmptyArray(ref imageFrameArray);
                    }
                }
                else
                {
                    while (_iRImageQueue.Count > 0)
                        _iRImageQueue.Dequeue().Dispose();
                }
            }
        }

        private void FillArrayFromQueue(ref ImageFrame[] imageFrameArray, Queue<ImageFrame> imageQueue)
        {
            if(imageFrameArray != null)
            {
                for (int i = 0; i < imageFrameArray.Length; i++)
                {
                    imageFrameArray[i] = imageQueue.Dequeue();
                }
            }
        }

        private void EmptyArray(ref ImageFrame[] imageFrameArray)
        {
            if(imageFrameArray != null)
            {
                for (int i = 0; i < imageFrameArray.Length; i++)
                {
                    imageFrameArray[i].Dispose();
                }
            }
        }

        private void StopThread()
        {
            _frameProcessThread.Dispose();
            _ImageAutoEvent.Set();
        }

        //****************************************************************
        //****************************************************************
        public FrameProcessor(string Frame1, string Frame2)
        {
            // give the path of Illuminated and AmbientFrame image
            _frame1 = Frame1;
            _frame2 = Frame2;

            _captureSessionParameter = new CaptureSessionParameter();
            _previewParameter = new PreviewParameter();
            //Initialize IR Image Queue
            _iRImageQueue = new Queue<ImageFrame>();

            _ImageAutoEvent = new AutoResetEvent(false);
            StartIRDeviceManagedThread();

            _sensorSourceManager = new FSSensorSourceManager(_iRImageQueue, _ImageAutoEvent, _displayAutoEvent);

            _frameProcessThread = new ThreadsManager();
            _frameProcessThread.ThreadStart(FrameProcessThread);
            _selectedIRindex = 0;

        }

        public void Shutdown()
        {
            StopThread();
            StopIRDeviceManagedThread();
            if (_iRImageQueue.Count > 0)
            {
                _iRImageQueue.Clear();
            }
        }

        public void Dispose()
        {
            Dispose(true);
            //_exitEvent.Set();
            GC.SuppressFinalize(true);
        }

        #region Frame Process

        private void ProcessImageFrame(ImageFrame[] imageFrameArray, ref PreviewParameter previewParameter, ref CaptureSessionParameter captureSessionParameter)
        {
            SoftwareBitmap cleanSoftwareBitmap = null;
            if( imageFrameArray == null )
            {
                return;
            }

            if (previewParameter.Frametype == FrameType.Substracted)
            {
                cleanSoftwareBitmap = CleanSoftwareBitmap(imageFrameArray);                
            }

            CaptureFromImageFrame(imageFrameArray, cleanSoftwareBitmap, ref captureSessionParameter/*, previewParameter*/);

            if (cleanSoftwareBitmap != null) cleanSoftwareBitmap.Dispose();
        }

        private void CaptureFromImageFrame(ImageFrame[] imageFrameArray, SoftwareBitmap cleanSoftwareBitmap, ref CaptureSessionParameter captureSessionParameter/*, PreviewParameter previewParameter*/)
        {
            if (captureSessionParameter.CaptureCount > 0)
            {
                if (cleanSoftwareBitmap == null)
                {
                    cleanSoftwareBitmap = CleanSoftwareBitmap(imageFrameArray);
                }

                CaptureBitmap(imageFrameArray[0].OriginalBitmap/*, captureSessionParameter*/, imageFrameArray[0].FrameIllumination.ToString());
                CaptureBitmap(imageFrameArray[1].OriginalBitmap/*, captureSessionParameter*/, imageFrameArray[1].FrameIllumination.ToString());

                DequeueCapturePhoto(ref captureSessionParameter);
            }
        }
        
        private void DequeueCapturePhoto( ref CaptureSessionParameter captureSessionParameter)
        {
            WaitCaptureMutex();
            captureSessionParameter.CaptureCount--;
            ReleaseCaptureMutex();
        }


        #endregion Frame Process

        #region open/close IR device 

        public void ReopenIRDevice()
        {
            _captureSessionParameter.IsFaceMode = false;
            _previewParameter.IsIlluminationValueEnabled = true;

            EnableSensorViewer();
        }

        public void StopIRDevice()
        {
            DisableSensorViewer();
        }

        //****************************************************************
        //****************************************************************
        private void EnableSensorViewer(/*int selectedidx = 0*/)
        {           
            if (_sensorSourceManager.OpenIRDevice())
            {
                isIRcameralOpen = true;
                _captureSessionParameter.CaptureCount = 0;
                _previewParameter.IsRecoveryOriginalSize = true;
                _previewParameter.IsControllerResize = true;
                if (_previewParameter.Frametype == FrameType.Unkown)
                    _previewParameter.Frametype = FrameType.Light;
                _previewParameter.IsAmbientSubtractionEnabled = _sensorSourceManager.IsAmbientSubtractionEnabled;               
                _previewParameter.ImageFrameSize.Height = 480;
                _previewParameter.ImageFrameSize.Width = 640;
                _previewParameter.PreviewStates = PreviewState.Playing;
            }
        }

        private void DisableSensorViewer()
        {
            _previewParameter.PreviewStates = PreviewState.Stop;
            Thread.Sleep(100);
            _sensorSourceManager.CloseIRDevice();
            _ImageAutoEvent.Set();
        }

        //****************************************************************
        //****************************************************************
        protected virtual void Dispose(bool disposing)
        {
            if (disposing)
            {
                Shutdown();
                _sensorSourceManager.Dispose();
            }
        }

        #endregion open/close IR device


        #region Capture Method

        private void CaptureBitmap(SoftwareBitmap softwareBitmap/*, CaptureSessionParameter captureSessionParameter*/, string tailName)
        {
            if (softwareBitmap != null)
            {        
                var fullFileName = tailName == "Light" ? _frame1 : _frame2;
                using (var frameConvertedForRendering = SoftwareBitmap.Convert(softwareBitmap, BitmapPixelFormat.Bgra8))
                {
                    try
                    {
                        // As above, easiest way to get access to the data is to lock it
                        using (var convertedBuffer = frameConvertedForRendering.LockBuffer(BitmapBufferAccessMode.Read))
                        {
                            IClosableByteAccess convertedByteAccess = (IClosableByteAccess)(Object)convertedBuffer;
                            IntPtr convertedBytes;
                            uint convertedCapacity = 0;
                            convertedByteAccess.Lock(out convertedBytes, out convertedCapacity);
                            byte[] convertedByteArray = new byte[convertedCapacity];
                            Marshal.Copy(convertedBytes, convertedByteArray, 0, (int)convertedCapacity);
                            WriteableBitmap ImageSourceInterimBitmap = new WriteableBitmap((int)softwareBitmap.PixelWidth, (int)softwareBitmap.PixelHeight, 96.0, 96.0, PixelFormats.Bgra32, null);
                            ImageSourceInterimBitmap.WritePixels(
                                    new Int32Rect(0, 0, softwareBitmap.PixelWidth, softwareBitmap.PixelHeight),
                                    (IntPtr)convertedBytes, (int)convertedCapacity, convertedBuffer.GetPlaneDescription(0).Stride);
                            Utilities.SaveImage(ImageSourceInterimBitmap, fullFileName);
                            convertedByteAccess.Unlock();
                        }
                    }
                    catch(Exception ex)
                    {
                        throw ex;
                    }
                }
            }
        } 

        private void CaptureSucceedTimer_Tick(object sender, EventArgs e)
        {
            CaptureSucceedTimer.Tick -= CaptureSucceedTimer_Tick;
            CaptureSucceedTimer.Stop();            
        }

        #endregion Capture Method


        #region Infrared Camera Device Name List Management


        private void IRDeviceManagedThread()
        {
            while (!_IRDeviceManagedThread.IsThreadStop)
            {
                _displayAutoEvent.WaitOne();                
                
                if (_IsAutoOpenIRDevice)
                {                 
                    _IsAutoOpenIRDevice = false;
                    _previewParameter.Frametype = FrameType.Light;
                    EnableSensorViewer();                    
                }
                if (!_sensorSourceManager.isWorking)
                {
                    StopIRDevice();
                    _ImageAutoEvent.Set();
                }
            }
        }

        private void StartIRDeviceManagedThread()
        {
            _displayAutoEvent = new AutoResetEvent(false);
            _IRDeviceManagedThread = new ThreadsManager();
            _IRDeviceManagedThread.ThreadStart(IRDeviceManagedThread);
        }

        private void StopIRDeviceManagedThread()
        {
            _IRDeviceManagedThread.Dispose();
            _displayAutoEvent.Set();
        }

        #endregion Infrared Camera Device Name List Management



        #region Get Clean frame

        private SoftwareBitmap CleanSoftwareBitmap(ImageFrame[] imageFrameArray)
        {
            SoftwareBitmap CleanFrameBitmap = null;
            if (imageFrameArray != null)
            {
                if (imageFrameArray.Length > 1)
                {
                    CleanFrameBitmap = new SoftwareBitmap(imageFrameArray[0].OriginalBitmap.BitmapPixelFormat, imageFrameArray[0].OriginalBitmap.PixelWidth, imageFrameArray[0].OriginalBitmap.PixelHeight);
                    imageFrameArray[0].OriginalBitmap.CopyTo(CleanFrameBitmap);

                    using (var convertedBuffer1 = CleanFrameBitmap.LockBuffer(BitmapBufferAccessMode.Read))
                    using (var convertedBuffer2 = imageFrameArray[1].OriginalBitmap.LockBuffer(BitmapBufferAccessMode.Read))
                    {
                        IClosableByteAccess[] convertedByteAccess = new IClosableByteAccess[2];
                        IntPtr[] convertedBytes = new IntPtr[2];
                        uint[] convertedCapacity = new uint[2];

                        convertedByteAccess[0] = (IClosableByteAccess)(Object)convertedBuffer1;
                        convertedByteAccess[0].Lock(out convertedBytes[0], out convertedCapacity[0]);

                        convertedByteAccess[1] = (IClosableByteAccess)(Object)convertedBuffer2;
                        convertedByteAccess[1].Lock(out convertedBytes[1], out convertedCapacity[1]);

                        byte[][] convertedByteArray = new byte[2][];
                        convertedByteArray[0] = new byte[convertedCapacity[0]];
                        convertedByteArray[1] = new byte[convertedCapacity[1]];

                        Marshal.Copy(convertedBytes[0], convertedByteArray[0], 0, (int)convertedCapacity[0]);
                        Marshal.Copy(convertedBytes[1], convertedByteArray[1], 0, (int)convertedCapacity[1]);

                        byte[] lightFrame, darkFrame;
                        if (imageFrameArray[0].FrameIllumination == LuminanceInfo.Light)
                        {
                            lightFrame = convertedByteArray[0];
                            darkFrame = convertedByteArray[1];
                        }
                        else
                        {
                            lightFrame = convertedByteArray[1];
                            darkFrame = convertedByteArray[0];
                        }
                        for (int i = 0; i < convertedCapacity[0]; i++)
                        {
                            int DestBit = (int)lightFrame[i] - (int)darkFrame[i];
                            lightFrame[i] = DestBit > 0 ? (byte)DestBit : (byte)0;
                        }
                        Marshal.Copy(lightFrame, 0, convertedBytes[0], (int)convertedCapacity[0]);

                        convertedByteAccess[0].Unlock();
                        convertedByteAccess[1].Unlock();
                    }
                }
            }

            return CleanFrameBitmap;
        }

 

        #endregion Get Clean Frame

  
    }
}

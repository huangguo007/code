using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using System.Diagnostics;
using System.Threading;
using Windows.Devices.Enumeration;
using Windows.Media.Capture;
using Windows.Media.Capture.Frames;
using Windows.Graphics.Imaging;

namespace IRIQCapture
{
    class FSSensorSourceManager : IDisposable
    {
        private readonly IDictionary<string, MediaFrameSourceGroup> _sourceGroups =
            new Dictionary<string, MediaFrameSourceGroup>();

        private DeviceWatcher _watcher;
        

        private MediaCapture _mc = new MediaCapture();
        private Queue<ImageFrame> _frameQueue;
        private const int MaxQueueCount = 12;
        private AutoResetEvent _ImageAutoEvent;
        private AutoResetEvent _displayAutoEvent; 
        public FrameSize imageFrameSize = new FrameSize();
        public bool IsAmbientSubtractionEnabled = false;
        private MediaFrameReader _iRFrameReader;

        bool _isOpened = false;

        private async Task _AddAcceptableSourceGroupAsync(string deviceId)
        {
            // Accept any user facing IR camera
            var sourceGroup = await MediaFrameSourceGroup.FromIdAsync(deviceId);
            
            if (sourceGroup != null && (sourceGroup.SourceInfos.Count > 1) && 
                sourceGroup.SourceInfos.Any(source =>
                    source.SourceKind == MediaFrameSourceKind.Infrared))
            {
                if(sourceGroup.SourceInfos.Any(source =>
                    source.SourceKind == MediaFrameSourceKind.Color))
                {
                    _sourceGroups[deviceId] = sourceGroup;
                    if(!_isOpened)
                    {
                        _displayAutoEvent.Set();
                        _isOpened = true;
                    }
                }
            }
                
        }
        public void Initialize()
        {            
            string fsselector = MediaFrameSourceGroup.GetDeviceSelector();
        //  string  dis = MediaFrameSourceGroup.GetDeviceSelector().
            _watcher = DeviceInformation.CreateWatcher(fsselector);
            
            _watcher.Added += async (sender, devInfo) =>
            {
                await _AddAcceptableSourceGroupAsync(devInfo.Id);
                Debug.WriteLine("Watcher Added " + devInfo.Name);
            };

            _watcher.Removed += (sender, devInfo) =>
            {
                _sourceGroups.Remove(devInfo.Id);
            };

            _watcher.Updated += async (sender, devInfo) =>
            {
                _sourceGroups.Remove(devInfo.Id);
                await _AddAcceptableSourceGroupAsync(devInfo.Id);
            };

            _watcher.EnumerationCompleted += (sender, sourceGroup) =>
            {
                // Update UI.
            };

            _watcher.Start();

            
        }

        private LuminanceInfo GetLuminanceInfomation(VideoMediaFrame videoFrame)
        {
            LuminanceInfo illuminationInfo = LuminanceInfo.AmbientSubtractionEnabled;
           
            try
            {
                if (videoFrame.InfraredMediaFrame.IsIlluminated)
                {
                    illuminationInfo = LuminanceInfo.Light;
                }
                else
                {
                    illuminationInfo = LuminanceInfo.Dark;
                }
            }
            catch(Exception ex)
            {
                throw ex;
            }

            return illuminationInfo;
        }

        private void ProcessVideoFrame(VideoMediaFrame videoMediaframe, TimeSpan timeStamp)
        {
            try
            {
                ImageFrame imageFrame = new ImageFrame();
                imageFrame.FrameIllumination = GetLuminanceInfomation(videoMediaframe);
                imageFrame.Timestamp = (UInt64)timeStamp.Ticks;
                
                imageFrame.OriginalBitmap = new SoftwareBitmap(videoMediaframe.SoftwareBitmap.BitmapPixelFormat, (int)videoMediaframe.VideoFormat.Width, (int)videoMediaframe.VideoFormat.Height);
                videoMediaframe.SoftwareBitmap.CopyTo(imageFrame.OriginalBitmap);
                _frameQueue.Enqueue(imageFrame);
                if (_frameQueue.Count > MaxQueueCount)
                {
                    _frameQueue.Dequeue().Dispose();
                }
                _ImageAutoEvent.Set();
            }
            catch(Exception ex)
            {
                throw ex;
            }
        }





        public void CloseIRDevice()
        {           
            if (_frameQueue.Count > 0)
                _frameQueue.Clear();
        }
       

        private void IrReader_FrameArrived(MediaFrameReader sender, MediaFrameArrivedEventArgs args)
        {
            try
            {
                using (var frame = sender.TryAcquireLatestFrame())
                {
                    var videoFrame = frame.VideoMediaFrame;
                    ProcessVideoFrame(videoFrame, videoFrame.FrameReference.Duration);
                    
                }
            }
            catch (Exception ex)
            {
                throw ex;
            }
        }

        private async void FrameReaderThread()
        {
            var list = await MediaFrameSourceGroup.FindAllAsync();
            MediaFrameSourceInfo sourceInfo = null;
            MediaFrameSourceGroup sourceGroup = null;

            foreach (var group in list)
            {
                if (group.SourceInfos.Count == 2)
                {
                    var tempSourceInfo = group.SourceInfos.FirstOrDefault(s =>
                    s.SourceKind == MediaFrameSourceKind.Infrared &&                    
                    (s.MediaStreamType == MediaStreamType.VideoPreview ||
                     s.MediaStreamType == MediaStreamType.VideoRecord));
                    if (tempSourceInfo != null)
                    {
                        sourceInfo = tempSourceInfo;
                        sourceGroup = group;
                        break;
                    }
                }
            }

            if (sourceGroup == null || sourceInfo == null)
                return;
            
            var settings = new MediaCaptureInitializationSettings();
            settings.SourceGroup = sourceGroup;
            settings.SharingMode = MediaCaptureSharingMode.SharedReadOnly;
            settings.MemoryPreference = MediaCaptureMemoryPreference.Cpu;
            await _mc.InitializeAsync(settings);

            var irSource = _mc.FrameSources[sourceInfo.Id];

            imageFrameSize.Width = (int)irSource.CurrentFormat.VideoFormat.Width;
            imageFrameSize.Height = (int)irSource.CurrentFormat.VideoFormat.Height;

            _iRFrameReader = await _mc.CreateFrameReaderAsync(irSource);
            _iRFrameReader.FrameArrived += IrReader_FrameArrived;            

            await _iRFrameReader.StartAsync();

        }



        public bool isWorking;
        private bool OpenIrCamera()//open cameral
        {

            FrameReaderThread();

            isWorking = true;
            return true;
        }

        public bool OpenIRDevice(string selectDeviceId = null)
        {
            bool bRet = false;

            string deviceId = selectDeviceId;

            if (_sourceGroups.Count > 0)
            {
                MediaFrameSourceGroup mediaFrameSource = null;
                if (deviceId != null)
                {
                    mediaFrameSource = _sourceGroups[deviceId];
                }
                else
                {
                    foreach (string k in _sourceGroups.Keys)
                    {
                        deviceId = k;
                        break;
                    }
                }

                mediaFrameSource = _sourceGroups[deviceId];
                if (mediaFrameSource != null)
                {
                    bRet = OpenIrCamera();
                }
                else
                {
                    
                }
            }
            else
            {

            }

            return bRet;
        }

        public FSSensorSourceManager(Queue<ImageFrame> frameQueue, AutoResetEvent imageAutoEvent, AutoResetEvent displayAutoEvent)
        {
            _frameQueue = frameQueue;
            _ImageAutoEvent = imageAutoEvent;
            _displayAutoEvent = displayAutoEvent;
            Initialize();

        }

        public FSSensorSourceManager()
        {
             Initialize();
        }

        public void Dispose()
        {
            Dispose(true);
        }

        protected virtual void Dispose(bool disposing)
        {
            if (disposing)
            {
                Shutdown();
            }
        }
        private void Shutdown()
        {

        }
    }
}
using System;
using Windows.Graphics.Imaging;

namespace IRIQCapture
{
    public enum LuminanceInfo
    {
        AmbientSubtractionEnabled = 0, //it's own Ambient Light Subtraction 
        Light,
        Dark
    }

    public class ImageFrame : IDisposable
    {
        public SoftwareBitmap OriginalBitmap;
        public LuminanceInfo FrameIllumination;

        public UInt64 Timestamp;

        public void Dispose()
        {
            Dispose(true);
            GC.SuppressFinalize(true);
        }

        protected virtual void Dispose(bool disposing)
        {
            if(disposing)
            {
                OriginalBitmap.Dispose();
            }
        }
    }
}

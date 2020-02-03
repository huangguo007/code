namespace IRIQCapture
{
    public class PreviewParameter
    {
        public PreviewParameter()
        {
            FrameTypeLastState = FrameType.Unkown;
            Frametype = FrameType.Unkown;
            PreviewLastState = PreviewState.Stop;
            PreviewStates = PreviewState.Stop;
            IsAmbientSubtractionEnabled = true;
            IsIlluminationValueEnabled = true;
            IsRecoveryOriginalSize = true;
            IsControllerResize = true;
            IsAutoOpenIRDevice = true;
            ImageFrameSize = new FrameSize();
            ImageFrameSize.Height = 300;
            ImageFrameSize.Width = 300;
        }

        private FrameType FrameTypeLastState;

        private bool QueryFrameTypeChanged()
        {
            bool isFrameTypeChanged = false;
            if (FrameTypeLastState != Frametype)
            {
                FrameTypeLastState = Frametype;
                isFrameTypeChanged = true;
            }

            return isFrameTypeChanged;
        }

        public bool IsFrameTypeChanged
        {
            get
            {
                return QueryFrameTypeChanged();
            }
        }

        private PreviewState PreviewLastState;
        private bool QueryPrviewStateChanged()
        {
            bool isPrviewStateChanged = false;
            if (PreviewLastState != PreviewStates)
            {
                PreviewLastState = PreviewStates;
                isPrviewStateChanged = true;
            }
            return isPrviewStateChanged;
        }

        public bool IsPrviewStateChanged
        {
            get
            {
                return QueryPrviewStateChanged();
            }
        }

        public FrameType Frametype;
        public PreviewState PreviewStates;
        public bool IsAmbientSubtractionEnabled;
        public volatile bool IsRecoveryOriginalSize;
        public bool IsControllerResize;
        public bool IsAutoOpenIRDevice;
        public bool IsIlluminationValueEnabled;
        public FrameSize ImageFrameSize;
    }
}

namespace IRIQCapture
{
    public class CaptureSessionParameter
    {
        public CaptureSessionParameter()
        {
            CaptureCount = 0;
            SelectedIndex = 0;
            RootFolder = "";
            CustomRootFolder = "";
            IsFaceMode = false;
            SavePath = "";
            FileNameTag = "";
        }

        public int SelectedIndex;
        public string RootFolder;
        public string CustomRootFolder;
        public bool IsFaceMode;
        public string SavePath;
        public string FileNameTag;
        public int CaptureCount;
    }
}

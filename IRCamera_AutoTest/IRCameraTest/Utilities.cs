using System;
using System.Drawing;
using System.IO;
using System.Text.RegularExpressions;
using System.Windows.Media.Imaging;

namespace IRIQCapture
{
    class Utilities
    {
        //****************************************************************
        //****************************************************************
        public static void SaveImage(WriteableBitmap bitmap, string filename)
        {
            MemoryStream stream = null;
            try
            {
                stream = new MemoryStream();
                BitmapEncoder encoder = new BmpBitmapEncoder();

                encoder.Frames.Add(BitmapFrame.Create((BitmapSource)bitmap));
                encoder.Save(stream);

                using (var image = new Bitmap(stream))
                {
                    stream = null;
                    var fileName = CreateFullPathFileName(filename);
                    if (!fileName.Contains(".png"))
                    {
                        fileName += ".png";
                    }

                    image.Save(fileName);
                }
            }
            finally
            {
                if (stream != null)
                {
                    stream.Dispose();
                }
            }
        }

        public static string CreateFullPathFileName(String filepath)
        {
            if (false)
            {
                string folder = Path.GetDirectoryName(filepath);
                string filename = Path.GetFileNameWithoutExtension(filepath);
                string frametype = Path.GetExtension(filename);
                
                if(frametype.Length > 0)
                {
                    filename = filename.Replace(frametype, "");
                }
                string extension = Path.GetExtension(filepath);
                int number = 0;

                Match regex = Regex.Match(filepath, @"(.+) \((\d+)\)\.\w+");

                if (regex.Success)
                {
                    filename = regex.Groups[1].Value;
                    number = int.Parse(regex.Groups[2].Value);
                }

                do
                {
                    number++;
                    if(frametype == "")
                    {
                        filepath = Path.Combine(folder, string.Format("{0}_({1}){2}", filename, number, extension));
                    }
                    else
                    {
                        filepath = Path.Combine(folder, string.Format("{0}_({1}){2}{3}", filename, number, frametype, extension));
                    }
                } while (File.Exists(filepath));
            }

            return filepath;
        }
     
        public static string[,] scenarioTable = new string[Constant.scenarioTotal, Constant.tableElement]
        {
            //"Senario Name", "Filename Tag", "Target", "Illumination", "Distance", "Sub-folder Name" "State"
            { "Uniformity", "Uniformity","Uniformity", "0", "Mid", "Misc", "incomplete" },
            { "Gamma", "Gamma", "Gamma", "0", "100", "Misc", "incomplete" },
            { "Dynamic Range", "Dynamic_range", "Dynamic", "0", "30", "Misc", "incomplete" },
            { "Distortion", "Distortion", "Distortion", "", "", "Misc", "incomplete"},
            { "MTF min range – 0lux", "MTF_close", "MTF", "0", "30", "MTF_close", "incomplete" },
            { "MTF min range – 100lux", "MTF_close", "MTF", "100", "30", "MTF_close", "incomplete" },
            { "MTF min range – 500lux", "MTF_close", "MTF", "500", "30", "MTF_close", "incomplete" },
            { "MTF Center – 0lux", "MTF_center", "MTF", "0", "75", "MTF_center", "incomplete" },
            { "MTF Center – 100lux", "MTF_center", "MTF", "100", "75", "MTF_center", "incomplete" },
            { "MTF Center – 500lux", "MTF_center", "MTF", "500", "75", "MTF_center", "incomplete" },
            { "MTF Top-left – 0lux", "MTF_topleft", "MTF", "0", "75", "MTF_corner", "incomplete" },
            { "MTF Top-left – 100lux", "MTF_topleft", "MTF", "100", "75", "MTF_corner", "incomplete" },
            { "MTF Top-left – 500lux", "MTF_topleft", "MTF", "500", "75", "MTF_corner", "incomplete" },
            { "MTF Top-right – 0lux", "MTF_topright", "MTF", "0", "75", "MTF_corner", "incomplete" },
            { "MTF Top-right – 100lux", "MTF_topright", "MTF", "100", "75", "MTF_corner", "incomplete" },
            { "MTF Top-right – 500lux", "MTF_topright", "MTF", "500", "75", "MTF_corner", "incomplete" },
            { "MTF Bottom-left 0lux", "MTF_bottomleft", "MTF", "0", "75", "MTF_corner", "incomplete" },
            { "MTF Bottom-left 100lux", "MTF_bottomleft", "MTF", "100", "75", "MTF_corner", "incomplete" },
            { "MTF Bottom-left 500lux", "MTF_bottomleft", "MTF", "500", "75", "MTF_corner", "incomplete" },
            { "MTF Bottom-right 0lux", "MTF_bottomright", "MTF", "0", "75", "MTF_corner", "incomplete" },
            { "MTF Bottom-right 100lux", "MTF_bottomright", "MTF", "100", "75", "MTF_corner", "incomplete" },
            { "MTF Bottom-right 500lux", "MTF_bottomright", "MTF", "500", "75", "MTF_corner", "incomplete" },
            { "SNR Center – 0lux", "SNR_center", "SNR", "0", "75", "SNR_center", "incomplete" },
            { "SNR Center – 100lux", "SNR_center", "SNR", "100", "75", "SNR_center", "incomplete" },
            { "SNR Center – 500lux", "SNR_center", "SNR", "500", "75", "SNR_center", "incomplete" },
            { "SNR Top-left – 0lux", "SNR_topleft", "SNR", "0", "75", "SNR_corner", "incomplete" },
            { "SNR Top-left – 100lux", "SNR_topleft", "SNR", "100", "75", "SNR_corner", "incomplete" },
            { "SNR Top-left – 500lux", "SNR_topleft", "SNR", "500", "75", "SNR_corner", "incomplete" },
            { "SNR Top-right – 0lux", "SNR_topright", "SNR", "0", "75", "SNR_corner", "incomplete" },
            { "SNR Top-right – 100lux", "SNR_topright", "SNR", "100", "75", "SNR_corner", "incomplete" },
            { "SNR Top-right – 500lux", "SNR_topright", "SNR", "500", "75", "SNR_corner", "incomplete" },
            { "SNR Bottom-left – 0lux", "SNR_bottomleft", "SNR", "0", "75", "SNR_corner", "incomplete" },
            { "SNR Bottom-left – 100lux", "SNR_bottomleft", "SNR", "100", "75", "SNR_corner", "incomplete" },
            { "SNR Bottom-left – 500lux", "SNR_bottomleft", "SNR", "500", "75", "SNR_corner", "incomplete" },
            { "SNR Bottom-right – 0lux", "SNR_bottomright", "SNR", "0", "75", "SNR_corner", "incomplete" },
            { "SNR Bottom-right – 100lux", "SNR_bottomright", "SNR", "100", "75", "SNR_corner", "incomplete" },
            { "SNR Bottom-right – 500lux", "SNR_bottomright", "SNR", "500", "75", "SNR_corner", "incomplete" },
            { "Saturation Near – 0lux Even", "Sat_near_even", "Saturation", "0", "30", "Saturation_near", "incomplete" },
            { "Saturation Near-even – 50lux", "Sat_near_even", "Saturation", "50", "30", "Saturation_near", "incomplete" },
            { "Saturation Near-even – 150lux", "Sat_near_even", "Saturation", "150", "30", "Saturation_near", "incomplete" },
            { "Saturation Near-even – 1000lux", "Sat_near_even", "Saturation", "1000", "30", "Saturation_near", "incomplete" },
            { "Saturation Near-uneven – 0lux", "Sat_near_uneven", "Saturation", "0", "75", "Saturation_near", "incomplete" },
            { "Saturation Near-uneven – 50lux", "Sat_near_uneven", "Saturation", "50", "75", "Saturation_near", "incomplete" },
            { "Saturation Near-uneven – 150lux", "Sat_near_uneven", "Saturation", "150", "75", "Saturation_near", "incomplete" },
            { "Saturation Near-uneven – 1000lux", "Sat_near_uneven", "Saturation", "1000", "75", "Saturation_near", "incomplete" },
            { "Saturation Far-even – 0lux", "Sat_far_even", "Saturation", "0", "30", "Saturation_far", "incomplete" },
            { "Saturation Far-even – 50lux", "Sat_far_even", "Saturation", "50", "30", "Saturation_far", "incomplete" },
            { "Saturation Far-even – 150lux", "Sat_far_even", "Saturation", "150", "30", "Saturation_far", "incomplete" },
            { "Saturation Far-even – 1000lux", "Sat_far_even", "Saturation", "1000", "30", "Saturation_far", "incomplete" },
            { "Saturation Far-uneven – 0lux", "Sat_far_uneven", "Saturation", "0", "75", "Saturation_far", "incomplete" },
            { "Saturation Far-uneven – 50lux", "Sat_far_uneven", "Saturation", "50", "75", "Saturation_far", "incomplete" },
            { "Saturation Far-uneven – 150lux", "Sat_far_uneven", "Saturation", "150", "75", "Saturation_far", "incomplete" },
            { "Saturation Far-uneven – 1000lux", "Sat_far_uneven", "Saturation", "1000", "75", "Saturation_far", "incomplete" },
            { "Custom", "Custom", "", "", "", "Custom", "" },
            {"FaceMode", "FaceMode", "", "", "", "FaceMode", ""},
        };
    }
}

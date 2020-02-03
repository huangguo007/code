using System;
using System.Text;
using System.IO;
using System.Xml;

namespace MyXml
{ 
     public class XmlFile    //(String LogFileName)
     {
        private XmlTextWriter writer;
        private String XmlFileName = "";
        private string exeVer = "SFTIRCameraTest.exe Auto IR Test Version V2.1.1.4";
        private  string m_XmlStartTime;
        private  string m_XmlEndTime;
        private UInt32 m_uXmlPassCnt;
        private UInt32 m_uXmlFailCnt;
        private UInt32 m_uXmlSkipCnt;
        private string m_strResult;

        public Boolean XmlFileCreate(String strXmlFileName)
        {
            Boolean bRet = false;
            XmlFileName = strXmlFileName;
            if (XmlFileName.Length > 0)
            {
                if (File.Exists(XmlFileName))
                {
                    File.Delete(XmlFileName);
                }
                try
                {
                    writer = new XmlTextWriter(strXmlFileName+".xml", null);
                    //使用自动缩进便于阅读
                    writer.Formatting = Formatting.Indented;
                    //书写根元素
                    writer.WriteStartElement("IRcamera_Test");
                    //开始一个元素
                    writer.WriteStartElement("Test_Name");
                    //向先前创建的元素中添加一个属性
                    writer.WriteAttributeString("rating", "R");
                    //添加子元素
                    writer.WriteElementString("title", "IRCamera.exe");
                    //关闭item元素
                    writer.WriteEndElement();
                    writer.WriteWhitespace("\n");
                    writer.WriteRaw(String.Format("<Test_Version>" +
                                    "<title>{0}</title>" +
                                    "</Test_Version>", exeVer));
                    writer.WriteRaw("\n  <Xml_Name>\n" +
                                    "    <title>SFTIRCameraTes.xml</title>\n" +
                                    "  </Xml_Name>\n");
                    bRet = true;
                }
                catch (System.Exception ex)
                {
                    throw (ex);
                }
            }
            return bRet;
        }

        private void XmlInit()
        {
            m_uXmlPassCnt = 0;
            m_uXmlFailCnt = 0;
            m_uXmlSkipCnt = 0;
            m_XmlStartTime = "";
            m_XmlEndTime = "";
            m_strResult = "";
        }

        public void XmlTmStart()
        {
            XmlInit();
            m_XmlStartTime = DateTime.Now.ToString("yyyy/MM/dd HH:mm:ss.fff");
        }

        public void WriteXmlResult(int m_iState)
        {
            switch (m_iState)
            {
                case 0:
                    m_strResult = "RETURN PASS";
                    break;
                case 1:
                    m_strResult = "RETURN FAIL";
                    break;
                case 2:
                    m_strResult = "RETURN SKIP";
                    break;
                default:
                    break;
            }
            XmlTmEnd();       
        }

        private void XmlTmEnd()
        {
            m_XmlEndTime = DateTime.Now.ToString("yyyy/MM/dd HH:mm:ss.fff");
            writer.WriteRaw(String.Format("\n  <FINAL_RESULT>\n" +
                "   <RETURN>{0}</RETURN>\n" +
                "   <TOTAL_PASS>{1}</TOTAL_PASS>\n" +
                "   <TOTAL_FAIL>{2}</TOTAL_FAIL>\n" +
                "   <TOTAL_SKIP>{3}</TOTAL_SKIP>\n" +
                "   <Start_Time>{4}</Start_Time>\n" +
                "   <Finsh_Time>{5}</Finsh_Time>\n" +
                "  </FINAL_RESULT>\n", m_strResult, m_uXmlPassCnt, m_uXmlFailCnt, m_uXmlSkipCnt, m_XmlStartTime, m_XmlEndTime));
            XmlClose();
        }

        public void XmlPass()
        {
            m_uXmlPassCnt++;
        }

        public void XmlFail()
        {
            m_uXmlFailCnt++;
        }

        public void XmlSkip()
        {
            m_uXmlSkipCnt++;
        }

        private void XmlClose()
        {
            try
            {
                writer.WriteFullEndElement();
                writer.Close();
            }
            catch (System.Exception ex)
            {
                throw (ex);
            }
        }
     
     }

}
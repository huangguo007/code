using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO;
using System.Xml;

namespace MyLog
{
    public class LogFile    //(String LogFileName)
    {
        private String LogFileName = "";
        private FileStream fs;
        private StreamWriter fswriter;
        private string exeVer = "SFTIRCameraTest.exe Auto IR Test Version V2.1.1.4";
        private UInt32 m_uPassCnt;
        private UInt32 m_uFailCnt;
        private UInt32 m_uSkipCnt;
        private string m_startTime;
        private string m_endTime;

        public Boolean LogFileCreate( String strLogFileName)
        {
            Boolean bRet = false;
            LogFileName = strLogFileName;
            if (LogFileName.Length > 0)
            {
                
                if (File.Exists(LogFileName))
                {
                    File.Delete(LogFileName);
                }

                try
                {
                    fs = new FileStream(LogFileName+".log", FileMode.Create);
                    fswriter = new StreamWriter(fs);
                    bRet = true;
                }
                catch (System.Exception ex)
                {
                    throw (ex);
                }
            }
            return bRet;
        }

        private void LogInit()
        {
            m_uPassCnt = 0;
            m_uFailCnt = 0;
            m_uSkipCnt = 0;
            m_startTime = "";
            m_endTime = "";
        }

        private void LogClose()
        {
            try
            {
                fswriter.Close();
                fs.Close();
            }
            catch (System.Exception ex)
            {
                throw (ex);
            }
        }

        public void LogWrite(String strLogContent)
        {
            try
            {
                if (fswriter != null)
                    fswriter.WriteLine(DateTime.Now.ToString("yyyy/MM/dd HH:mm:ss.fff") + "  " + strLogContent);
            }
            catch(System.Exception ex)
            {
                throw(ex);
            }           
        }

        public void LogTmStart()
        {
            LogInit();
            LogApend(exeVer + "\r\n");    
            m_startTime = DateTime.Now.ToString("yyyy/MM/dd HH:mm:ss.fff");
        }

        public void WriteLogResult(int m_iState)
        {
            LogApend("\r\nFINAL RESULT");
            switch (m_iState)
            { 
                case 0:
                    LogApend("RETURN PASS");
                    break;
                case 1:
                    LogApend("RETURN FAIL");
                    break;
                case 2:
                    LogApend("RETURN SKIP");
                    break;
                default:
                    break;
            }
            LogTmEnd();
        }

        private void LogTmEnd()
        {
            m_endTime = DateTime.Now.ToString("yyyy/MM/dd HH:mm:ss.fff");
            LogApend("Start Time: " + m_startTime);
            LogApend("End Time: " + m_endTime);
            LogApend(String.Format(("TOTAL PASS {0}, FAIL {1}, SKIP {2} \r\n"), m_uPassCnt, m_uFailCnt, m_uSkipCnt));
            LogClose();
        }

        public void LogPass()
        {
            m_uPassCnt++;
            LogApend("\r\nPASS");
        }

        public void LogFail()
        {
            m_uFailCnt++;
            LogApend("\r\nFAIL");
        }

        public void LogSkip()
        {
            m_uSkipCnt++;
            LogApend("\r\nSKIP");
        }

        private void LogApend(String strLogContent)
        {
            try
            {
                if (fswriter != null)
                    fswriter.WriteLine(strLogContent);
            }
            catch (System.Exception ex)
            {
                throw (ex);
            }
        }

    }
}

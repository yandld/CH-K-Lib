using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.IO;
using System.Windows.Forms;

namespace serial
{
    public partial class frm_download : Form
    {
        public frm_download()
        {
            InitializeComponent();
        }

        private void sys_log(string log)
        {
            listBox1.Items.Add(log);
        }

        private void btn_ping_Click(object sender, EventArgs e)
        {
            kboot kb = new kboot(CHConn.ConnObject);
            if (CHConn.isConnected == true)
            {
                if (kb.Ping())
                {
                    sys_log("Ping OK");
                    sys_log("FlashSize:" + (kb.GetFlashSizeInBytes() / 1024).ToString() + "KB");
                    sys_log("RAMSize:" + (kb.GetRAMSizeInBytes() / 1024).ToString() + "KB");
                    sys_log("Device: 0x" + System.Convert.ToString(kb.GetSystemDeviceId(), 16));
                }
                else
                {
                    sys_log("Ping ERROR");
                }
            }
            else
            {
                sys_log("Select a connection first!");
            }

        }

        private void btn_OpenFile_Click(object sender, EventArgs e)
        {
            OpenFileDialog opd = new OpenFileDialog();

            if (opd.ShowDialog() == DialogResult.OK)
            {
                FileInfo fileInfo = new FileInfo(opd.FileName);
                FileStream fs = fileInfo.OpenRead();
                sys_log(opd.SafeFileName);
                sys_log(fs.Length + " Bytes");
                byte[] FileData = new byte[fs.Length];

                if (fs.Length > (1024 * 1024 * 4))
                {
                    sys_log("File too large!");
                    fs.Close();
                    return;
                }
                fs.Read(FileData, 0, (int)fs.Length);
                fs.Close();

                sys_log("File loaded.");
                sys_log(FileData[0].ToString());
                sys_log(FileData[1].ToString());
                sys_log(FileData[2].ToString());
                sys_log(FileData[3].ToString());
                sys_log(FileData[4].ToString());
            }
        }

        private void btn_ClearLog_Click(object sender, EventArgs e)
        {
            listBox1.Items.Clear();
        }

        private void textBox1_KeyPress(object sender, KeyPressEventArgs e)
        {
            //如果输入的不是数字键，也不是回车键、Backspace键，则取消该输入
            if (!(Char.IsNumber(e.KeyChar)) && e.KeyChar != (char)13 && e.KeyChar != (char)8)
            {
                e.Handled = true;
            }
        }

        private void textBox1_TextChanged(object sender, EventArgs e)
        {

        }

        private void button2_Click(object sender, EventArgs e)
        {
            kboot kb = new kboot(CHConn.ConnObject);

            if (CHConn.isConnected == true)
            {
                kb.Ping();
                if (kb.FlashEraseAllUnsecure())
                {
                    sys_log("FlashEraseAllUnsecure OK");
                }
                else
                {
                    sys_log("FlashEraseAllUnsecure ERROR");
                }
            }
            else
            {
                sys_log("Select a connection first!");
            }

        }

        private void button1_Click(object sender, EventArgs e)
        {
            kboot kb = new kboot(CHConn.ConnObject);
            OpenFileDialog opd = new OpenFileDialog();

            if (opd.ShowDialog() == DialogResult.OK)
            {
                FileInfo fileInfo = new FileInfo(opd.FileName);
                FileStream fs = fileInfo.OpenRead();
                sys_log(opd.SafeFileName);
                sys_log(fs.Length + " Bytes");
                byte[] FileData = new byte[fs.Length];

                if (fs.Length > (1024 * 1024 * 4))
                {
                    sys_log("File too large!");
                    fs.Close();
                    return;
                }
                fs.Read(FileData, 0, (int)fs.Length);
                fs.Close();

                if(kb.WriteMemory(FileData, 0x00000700, (int)FileData.Length))
                {
                    sys_log("All OK!");
                }
                else
                {
                    sys_log("Dowload ERROR");
                }
            }
        }

        private void button4_Click(object sender, EventArgs e)
        {
            kboot kb = new kboot(CHConn.ConnObject);
            if (kb.FlashEraseAllUnsecure())
            {
                sys_log("FlashEraseAll OK");
            }
            else
            {
                sys_log("FlashEraseAll ERROR");
            }
        }
    }
}

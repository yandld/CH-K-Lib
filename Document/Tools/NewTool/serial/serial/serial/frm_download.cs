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
        kboot kb;
        FileInfo fileInfo;

        public frm_download()
        {
            InitializeComponent();
        }

        private void sys_log(string log)
        {
            listBox1.Items.Add(log);
            this.listBox1.SelectedIndex = this.listBox1.Items.Count - 1;
        }

        private void btn_ping_Click(object sender, EventArgs e)
        {
            if (kb == null)
            {
                kb = new kboot(CHConn.ConnObject);
                kb.log += new kboot.OnSystemlog(sys_log);
            }

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


        private void btn_ClearLog_Click(object sender, EventArgs e)
        {
            listBox1.Items.Clear();
        }

        private void button2_Click(object sender, EventArgs e)
        {
                if (kb.FlashEraseAllUnsecure())
                {
                    sys_log("FlashEraseAllUnsecure OK");
                }
                else
                {
                    sys_log("FlashEraseAllUnsecure ERROR");
                }
        }

        private void button4_Click(object sender, EventArgs e)
        {
            if (kb.FlashEraseAll())
            {
                sys_log("FlashEraseAll OK");
            }
            else
            {
                sys_log("FlashEraseAll ERROR");
            }
        }

        private void txt_StartAddr_KeyPress(object sender, KeyPressEventArgs e)
        {
            //如果输入的不是数字键，也不是回车键、Backspace键，则取消该输入
            if (!(Char.IsNumber(e.KeyChar)) && e.KeyChar != (char)13 && e.KeyChar != (char)8)
            {
                e.Handled = true;
            }
        }

        private void frm_download_Load(object sender, EventArgs e)
        {
            this.StartPosition = FormStartPosition.CenterScreen;
        }

        private void btn_Download_Click(object sender, EventArgs e)
        {
            if (fileInfo == null)
            {
                sys_log("Please select file!");
                OpenFileDialog opd = new OpenFileDialog();
                if (opd.ShowDialog() == DialogResult.OK)
                {
                    fileInfo = new FileInfo(opd.FileName);
                    sys_log("File:  " + fileInfo.Name);
                    sys_log("Size:  " + fileInfo.Length);
                }
            }

            FileStream fs = new FileStream(fileInfo.FullName, FileMode.Open);
            byte[] data = new byte[fileInfo.Length];
            if (data.Length > 1024 * 1024 * 2)
            {
                sys_log("File too large!");
                return;
            }

            fs.Read (data, 0, data.Length);
            fs.Close();

            if (kb == null)
            {
                kb = new kboot(CHConn.ConnObject);
                kb.log += new kboot.OnSystemlog(sys_log);
            }

            if (kb.Ping() == false)
            {
                sys_log("Ping ERROR");
                return;
            }
            kb.FlashEraseAllUnsecure();
            kb.Ping();
            kb.FlashEraseAll();
            kb.WriteMemory(data, (int)Convert.ToUInt32(txt_StartAddr.Text), data.Length);
            kb.Execute((UInt32)BitConverter.ToUInt32(data.Skip(4).Take(4).ToArray(), 0), 0, (UInt32)BitConverter.ToUInt32(data.Skip(0).Take(4).ToArray(), 0));
        }
    }
}

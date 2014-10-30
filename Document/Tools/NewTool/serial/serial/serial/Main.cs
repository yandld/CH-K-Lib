using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace serial
{
    public partial class Main : Form
    {
        public Main()
        {
            InitializeComponent();
            LoadPanelForm(panel1, new frm_consult());
            LoadPanelForm(panel2, new frm_download());
            LoadPanelForm(panel3, new frm_IMU());
        }

        private void system_log(string log)
      {
        //  listBox1.Items.Add(log);
      }
        private void Form1_Load(object sender, EventArgs e)
        {
            system_log("Loading system...");
            this.StartPosition = FormStartPosition.CenterScreen;
        }

        private void 端口ToolStripMenuItem_Click(object sender, EventArgs e)
        {
            conDialog frm_conDialog1 = new conDialog();
            DialogResult r = frm_conDialog1.ShowDialog();
            this.Text = "CHBox - " + CHConn.linkInfoString;
        }

        private void LoadPanelForm(Panel Panel, Form  form)
        {
            form.TopLevel = false;
            form.Size = Panel.Size;
            Panel.Controls.Add(form);
            form.Show();
        }

        private void timer1_Tick(object sender, EventArgs e)
        {
            toolStripStatusLabel1.Text = System.DateTime.Now.ToString();
        }

        private void backgroundWorker1_DoWork(object sender, DoWorkEventArgs e)
        {
        //    this.Text = "ASD";
            Button btn = (Button)e.Argument;
            btn.Text = "ASDD";
        //    serialPort1.Write("asdasd");
           System.Threading.Thread.Sleep(100);
        }

        private void button1_Click(object sender, EventArgs e)
        {
            backgroundWorker1.RunWorkerAsync(sender);
        }


    }
}

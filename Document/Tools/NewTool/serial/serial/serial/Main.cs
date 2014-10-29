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
        }

        private void Form1_Load(object sender, EventArgs e)
        {
            this.StartPosition = FormStartPosition.CenterScreen;
        }

        private void 端口ToolStripMenuItem_Click(object sender, EventArgs e)
        {
            conDialog frm_conDialog1 = new conDialog();


            DialogResult r = frm_conDialog1.ShowDialog();
            MessageBox.Show(r.ToString());
            if (r == DialogResult.OK)
            {
                conDialog.ConnResult cr1 = new conDialog.ConnResult();
                cr1 = frm_conDialog1.GetResult(sender, e);
                
            }
            frm_conDialog1.Close();
        }
    }
}

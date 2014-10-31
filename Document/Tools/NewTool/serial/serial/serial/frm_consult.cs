using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Diagnostics;
namespace serial
{
   
    public partial class frm_consult : Form
    {
        public frm_consult()
        {
            InitializeComponent();
            
        }

        private void frm_consult_Load(object sender, EventArgs e)
        {
            this.Text = sender.ToString();
        }

        private void timer1_Tick(object sender, EventArgs e)
        {
        }
    }
}

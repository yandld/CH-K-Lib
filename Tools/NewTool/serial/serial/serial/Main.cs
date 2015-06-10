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
          //  LoadPanelForm(panel1, new frm_help());
            LoadPanelForm(panel2, new frm_download());
            conDialog frm_conDialog = new conDialog();
            frm_conDialog.ShowDialog();
        }


        private void LoadPanelForm(Panel Panel, Form  form)
        {
            form.TopLevel = false;
            form.Size = Panel.Size;
            Panel.Controls.Add(form);
            form.Show();
        }

    }
}

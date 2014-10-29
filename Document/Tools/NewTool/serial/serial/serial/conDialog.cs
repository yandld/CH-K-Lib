using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.IO.Ports;
using System.Drawing;
using System.Collections;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace serial
{

    public partial class conDialog : Form
    {

        public class ConnResult
        {
            public string name;
            public int a;
        };

        private ConnResult cr1 = new ConnResult();

        public conDialog()
        {
            InitializeComponent();
            this.StartPosition = FormStartPosition.CenterParent;
        }
        public ConnResult GetResult(object sender, EventArgs e)
        {

            cr1.name = "Yandld";
            return cr1;
        }
        private void btn_open_Click(object sender, EventArgs e)
        {

       //     this.DialogResult = System.Windows.Forms.DialogResult.OK;
            
        }

        private void btn_cancel_Click(object sender, EventArgs e)
        {
            this.DialogResult = System.Windows.Forms.DialogResult.Cancel;

        }

        private void conDialog_Load(object sender, EventArgs e)
        {
            btn_Refresh_Click(null, null);


        }

        private void btn_Refresh_Click(object sender, EventArgs e)
        {
            // get serial
            ArrayList PortNames = new ArrayList();
            lbl_AvailPorts.Text = "Available Ports:";
            foreach (string Port in SerialPort.GetPortNames())
            {
                PortNames.Add(Port);
                lbl_AvailPorts.Text += "  " + Port;
            }

            if (PortNames.Capacity != 0) txt_Port.Text = (string)PortNames[0];
            
        }


    }
}

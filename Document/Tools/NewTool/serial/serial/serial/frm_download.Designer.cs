﻿namespace serial
{
    partial class frm_download
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.btn_Download = new System.Windows.Forms.Button();
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.button4 = new System.Windows.Forms.Button();
            this.button2 = new System.Windows.Forms.Button();
            this.label1 = new System.Windows.Forms.Label();
            this.txt_StartAddr = new System.Windows.Forms.TextBox();
            this.btn_ping = new System.Windows.Forms.Button();
            this.listBox1 = new System.Windows.Forms.ListBox();
            this.btn_ClearLog = new System.Windows.Forms.Button();
            this.groupBox1.SuspendLayout();
            this.SuspendLayout();
            // 
            // btn_Download
            // 
            this.btn_Download.Location = new System.Drawing.Point(6, 54);
            this.btn_Download.Name = "btn_Download";
            this.btn_Download.Size = new System.Drawing.Size(81, 27);
            this.btn_Download.TabIndex = 0;
            this.btn_Download.Text = "2. Download";
            this.btn_Download.TextAlign = System.Drawing.ContentAlignment.BottomLeft;
            this.btn_Download.UseVisualStyleBackColor = true;
            this.btn_Download.Click += new System.EventHandler(this.btn_Download_Click);
            // 
            // groupBox1
            // 
            this.groupBox1.Controls.Add(this.button4);
            this.groupBox1.Controls.Add(this.button2);
            this.groupBox1.Controls.Add(this.label1);
            this.groupBox1.Controls.Add(this.txt_StartAddr);
            this.groupBox1.Controls.Add(this.btn_Download);
            this.groupBox1.Controls.Add(this.btn_ping);
            this.groupBox1.Font = new System.Drawing.Font("Times New Roman", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.groupBox1.Location = new System.Drawing.Point(3, 13);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(184, 138);
            this.groupBox1.TabIndex = 1;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "Operation";
            // 
            // button4
            // 
            this.button4.Location = new System.Drawing.Point(80, 108);
            this.button4.Name = "button4";
            this.button4.Size = new System.Drawing.Size(62, 24);
            this.button4.TabIndex = 4;
            this.button4.Text = "EraseAll";
            this.button4.UseVisualStyleBackColor = true;
            this.button4.Click += new System.EventHandler(this.button4_Click);
            // 
            // button2
            // 
            this.button2.Location = new System.Drawing.Point(9, 108);
            this.button2.Name = "button2";
            this.button2.Size = new System.Drawing.Size(65, 24);
            this.button2.TabIndex = 4;
            this.button2.Text = "Unsecure";
            this.button2.UseVisualStyleBackColor = true;
            this.button2.Click += new System.EventHandler(this.button2_Click);
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.label1.ForeColor = System.Drawing.Color.Red;
            this.label1.Location = new System.Drawing.Point(9, 84);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(81, 13);
            this.label1.TabIndex = 3;
            this.label1.Text = "StartAddr: 0x";
            // 
            // txt_StartAddr
            // 
            this.txt_StartAddr.Location = new System.Drawing.Point(93, 81);
            this.txt_StartAddr.Name = "txt_StartAddr";
            this.txt_StartAddr.Size = new System.Drawing.Size(76, 21);
            this.txt_StartAddr.TabIndex = 3;
            this.txt_StartAddr.Text = "00000000";
            this.txt_StartAddr.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.txt_StartAddr_KeyPress);
            // 
            // btn_ping
            // 
            this.btn_ping.Location = new System.Drawing.Point(6, 23);
            this.btn_ping.Name = "btn_ping";
            this.btn_ping.Size = new System.Drawing.Size(81, 27);
            this.btn_ping.TabIndex = 2;
            this.btn_ping.Text = "1. Ping";
            this.btn_ping.TextAlign = System.Drawing.ContentAlignment.TopLeft;
            this.btn_ping.UseVisualStyleBackColor = true;
            this.btn_ping.Click += new System.EventHandler(this.btn_ping_Click);
            // 
            // listBox1
            // 
            this.listBox1.ForeColor = System.Drawing.Color.Red;
            this.listBox1.FormattingEnabled = true;
            this.listBox1.HorizontalScrollbar = true;
            this.listBox1.ItemHeight = 14;
            this.listBox1.Items.AddRange(new object[] {
            "Reset Target board"});
            this.listBox1.Location = new System.Drawing.Point(193, 14);
            this.listBox1.Name = "listBox1";
            this.listBox1.Size = new System.Drawing.Size(204, 186);
            this.listBox1.TabIndex = 2;
            // 
            // btn_ClearLog
            // 
            this.btn_ClearLog.Location = new System.Drawing.Point(129, 176);
            this.btn_ClearLog.Name = "btn_ClearLog";
            this.btn_ClearLog.Size = new System.Drawing.Size(58, 24);
            this.btn_ClearLog.TabIndex = 3;
            this.btn_ClearLog.Text = "ClearLog";
            this.btn_ClearLog.UseVisualStyleBackColor = true;
            this.btn_ClearLog.Click += new System.EventHandler(this.btn_ClearLog_Click);
            // 
            // frm_download
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 14F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(421, 216);
            this.Controls.Add(this.btn_ClearLog);
            this.Controls.Add(this.listBox1);
            this.Controls.Add(this.groupBox1);
            this.Font = new System.Drawing.Font("Times New Roman", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.None;
            this.Name = "frm_download";
            this.Text = "frm_download";
            this.Load += new System.EventHandler(this.frm_download_Load);
            this.groupBox1.ResumeLayout(false);
            this.groupBox1.PerformLayout();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.Button btn_Download;
        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.Button btn_ping;
        private System.Windows.Forms.ListBox listBox1;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.TextBox txt_StartAddr;
        private System.Windows.Forms.Button btn_ClearLog;
        private System.Windows.Forms.Button button2;
        private System.Windows.Forms.Button button4;
    }
}
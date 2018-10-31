namespace TcpClient_PFM
{
    partial class frmClient
    {
        /// <summary>
        /// 必需的设计器变量。
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// 清理所有正在使用的资源。
        /// </summary>
        /// <param name="disposing">如果应释放托管资源，为 true；否则为 false。</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows 窗体设计器生成的代码

        /// <summary>
        /// 设计器支持所需的方法 - 不要
        /// 使用代码编辑器修改此方法的内容。
        /// </summary>
        private void InitializeComponent()
        {
            this.label1 = new System.Windows.Forms.Label();
            this.label2 = new System.Windows.Forms.Label();
            this.cbxTestTime = new System.Windows.Forms.ComboBox();
            this.cbxTestInterv = new System.Windows.Forms.ComboBox();
            this.cbxThreadInterv = new System.Windows.Forms.ComboBox();
            this.cbxThreadCount = new System.Windows.Forms.ComboBox();
            this.label3 = new System.Windows.Forms.Label();
            this.label4 = new System.Windows.Forms.Label();
            this.cbxDataLength = new System.Windows.Forms.ComboBox();
            this.label5 = new System.Windows.Forms.Label();
            this.label6 = new System.Windows.Forms.Label();
            this.lbxMsg = new System.Windows.Forms.ListBox();
            this.txtPort = new System.Windows.Forms.TextBox();
            this.txtIpAddress = new System.Windows.Forms.TextBox();
            this.btnStop = new System.Windows.Forms.Button();
            this.btnStart = new System.Windows.Forms.Button();
            this.SuspendLayout();
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(25, 15);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(71, 12);
            this.label1.TabIndex = 0;
            this.label1.Text = "Test Times:";
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(19, 41);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(77, 12);
            this.label2.TabIndex = 1;
            this.label2.Text = "Test Interv:";
            // 
            // cbxTestTime
            // 
            this.cbxTestTime.FormattingEnabled = true;
            this.cbxTestTime.Items.AddRange(new object[] {
            "1",
            "5",
            "10",
            "30",
            "50",
            "100",
            "300",
            "500",
            "1000"});
            this.cbxTestTime.Location = new System.Drawing.Point(101, 11);
            this.cbxTestTime.Name = "cbxTestTime";
            this.cbxTestTime.Size = new System.Drawing.Size(82, 20);
            this.cbxTestTime.TabIndex = 2;
            // 
            // cbxTestInterv
            // 
            this.cbxTestInterv.FormattingEnabled = true;
            this.cbxTestInterv.Items.AddRange(new object[] {
            "0",
            "1",
            "3",
            "5",
            "10",
            "20",
            "30",
            "60",
            "100"});
            this.cbxTestInterv.Location = new System.Drawing.Point(101, 38);
            this.cbxTestInterv.Name = "cbxTestInterv";
            this.cbxTestInterv.Size = new System.Drawing.Size(82, 20);
            this.cbxTestInterv.TabIndex = 3;
            // 
            // cbxThreadInterv
            // 
            this.cbxThreadInterv.FormattingEnabled = true;
            this.cbxThreadInterv.Items.AddRange(new object[] {
            "0",
            "1",
            "3",
            "5",
            "10",
            "20",
            "30",
            "60",
            "100"});
            this.cbxThreadInterv.Location = new System.Drawing.Point(303, 38);
            this.cbxThreadInterv.Name = "cbxThreadInterv";
            this.cbxThreadInterv.Size = new System.Drawing.Size(82, 20);
            this.cbxThreadInterv.TabIndex = 7;
            // 
            // cbxThreadCount
            // 
            this.cbxThreadCount.FormattingEnabled = true;
            this.cbxThreadCount.Items.AddRange(new object[] {
            "1",
            "5",
            "10",
            "30",
            "50",
            "100",
            "300",
            "500",
            "1000"});
            this.cbxThreadCount.Location = new System.Drawing.Point(303, 11);
            this.cbxThreadCount.Name = "cbxThreadCount";
            this.cbxThreadCount.Size = new System.Drawing.Size(82, 20);
            this.cbxThreadCount.TabIndex = 6;
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(209, 41);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(89, 12);
            this.label3.TabIndex = 5;
            this.label3.Text = "Thread Interv:";
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Location = new System.Drawing.Point(215, 15);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(83, 12);
            this.label4.TabIndex = 4;
            this.label4.Text = "Thread Count:";
            // 
            // cbxDataLength
            // 
            this.cbxDataLength.FormattingEnabled = true;
            this.cbxDataLength.Items.AddRange(new object[] {
            "1",
            "10",
            "100",
            "1000",
            "3000",
            "5000",
            "10000"});
            this.cbxDataLength.Location = new System.Drawing.Point(500, 11);
            this.cbxDataLength.Name = "cbxDataLength";
            this.cbxDataLength.Size = new System.Drawing.Size(82, 20);
            this.cbxDataLength.TabIndex = 9;
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Location = new System.Drawing.Point(418, 15);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(77, 12);
            this.label5.TabIndex = 8;
            this.label5.Text = "Data Length:";
            // 
            // label6
            // 
            this.label6.AutoSize = true;
            this.label6.Location = new System.Drawing.Point(399, 41);
            this.label6.Name = "label6";
            this.label6.Size = new System.Drawing.Size(191, 12);
            this.label6.TabIndex = 10;
            this.label6.Text = "\'Interv\' is to slow down speed ";
            // 
            // lbxMsg
            // 
            this.lbxMsg.FormattingEnabled = true;
            this.lbxMsg.ItemHeight = 12;
            this.lbxMsg.Location = new System.Drawing.Point(21, 67);
            this.lbxMsg.Name = "lbxMsg";
            this.lbxMsg.Size = new System.Drawing.Size(561, 352);
            this.lbxMsg.TabIndex = 11;
            this.lbxMsg.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.lbxMsg_KeyPress);
            // 
            // txtPort
            // 
            this.txtPort.Location = new System.Drawing.Point(331, 430);
            this.txtPort.Name = "txtPort";
            this.txtPort.Size = new System.Drawing.Size(40, 21);
            this.txtPort.TabIndex = 16;
            this.txtPort.Text = "5555";
            // 
            // txtIpAddress
            // 
            this.txtIpAddress.Location = new System.Drawing.Point(227, 430);
            this.txtIpAddress.Name = "txtIpAddress";
            this.txtIpAddress.Size = new System.Drawing.Size(98, 21);
            this.txtIpAddress.TabIndex = 15;
            this.txtIpAddress.Text = "127.0.0.1";
            // 
            // btnStop
            // 
            this.btnStop.Anchor = System.Windows.Forms.AnchorStyles.Top;
            this.btnStop.Location = new System.Drawing.Point(476, 426);
            this.btnStop.Name = "btnStop";
            this.btnStop.Size = new System.Drawing.Size(106, 26);
            this.btnStop.TabIndex = 14;
            this.btnStop.Text = "Stop && Check";
            this.btnStop.UseVisualStyleBackColor = true;
            this.btnStop.Click += new System.EventHandler(this.btnStop_Click);
            // 
            // btnStart
            // 
            this.btnStart.Anchor = System.Windows.Forms.AnchorStyles.Top;
            this.btnStart.Location = new System.Drawing.Point(397, 426);
            this.btnStart.Name = "btnStart";
            this.btnStart.Size = new System.Drawing.Size(73, 26);
            this.btnStart.TabIndex = 13;
            this.btnStart.Text = "Start";
            this.btnStart.UseVisualStyleBackColor = true;
            this.btnStart.Click += new System.EventHandler(this.btnStart_Click);
            // 
            // frmClient
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(600, 458);
            this.Controls.Add(this.txtPort);
            this.Controls.Add(this.txtIpAddress);
            this.Controls.Add(this.btnStop);
            this.Controls.Add(this.btnStart);
            this.Controls.Add(this.lbxMsg);
            this.Controls.Add(this.label6);
            this.Controls.Add(this.cbxDataLength);
            this.Controls.Add(this.label5);
            this.Controls.Add(this.cbxThreadInterv);
            this.Controls.Add(this.cbxThreadCount);
            this.Controls.Add(this.label3);
            this.Controls.Add(this.label4);
            this.Controls.Add(this.cbxTestInterv);
            this.Controls.Add(this.cbxTestTime);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.label1);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle;
            this.Name = "frmClient";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.Text = "Echo-PFM Client [ \'C\' - clear list box ]";
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.frmClient_FormClosing);
            this.Load += new System.EventHandler(this.frmClient_Load);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.ComboBox cbxTestTime;
        private System.Windows.Forms.ComboBox cbxTestInterv;
        private System.Windows.Forms.ComboBox cbxThreadInterv;
        private System.Windows.Forms.ComboBox cbxThreadCount;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.ComboBox cbxDataLength;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.Label label6;
        private System.Windows.Forms.ListBox lbxMsg;
        private System.Windows.Forms.TextBox txtPort;
        private System.Windows.Forms.TextBox txtIpAddress;
        private System.Windows.Forms.Button btnStop;
        private System.Windows.Forms.Button btnStart;
    }
}


namespace TcpPullClientNS
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
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.cbxAsyncConn = new System.Windows.Forms.CheckBox();
            this.txtPort = new System.Windows.Forms.TextBox();
            this.txtIpAddress = new System.Windows.Forms.TextBox();
            this.btnStop = new System.Windows.Forms.Button();
            this.btnStart = new System.Windows.Forms.Button();
            this.lbxMsg = new System.Windows.Forms.ListBox();
            this.txtSend = new System.Windows.Forms.TextBox();
            this.btnSend = new System.Windows.Forms.Button();
            this.groupBox1.SuspendLayout();
            this.SuspendLayout();
            // 
            // groupBox1
            // 
            this.groupBox1.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.groupBox1.Controls.Add(this.cbxAsyncConn);
            this.groupBox1.Controls.Add(this.txtPort);
            this.groupBox1.Controls.Add(this.txtIpAddress);
            this.groupBox1.Controls.Add(this.btnStop);
            this.groupBox1.Controls.Add(this.btnStart);
            this.groupBox1.Location = new System.Drawing.Point(12, 2);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(464, 47);
            this.groupBox1.TabIndex = 11;
            this.groupBox1.TabStop = false;
            // 
            // cbxAsyncConn
            // 
            this.cbxAsyncConn.AutoSize = true;
            this.cbxAsyncConn.Checked = true;
            this.cbxAsyncConn.CheckState = System.Windows.Forms.CheckState.Checked;
            this.cbxAsyncConn.Location = new System.Drawing.Point(134, 20);
            this.cbxAsyncConn.Name = "cbxAsyncConn";
            this.cbxAsyncConn.Size = new System.Drawing.Size(84, 16);
            this.cbxAsyncConn.TabIndex = 13;
            this.cbxAsyncConn.Text = "Async Conn";
            this.cbxAsyncConn.UseVisualStyleBackColor = true;
            // 
            // txtPort
            // 
            this.txtPort.Location = new System.Drawing.Point(88, 18);
            this.txtPort.Name = "txtPort";
            this.txtPort.Size = new System.Drawing.Size(40, 21);
            this.txtPort.TabIndex = 12;
            this.txtPort.Text = "5555";
            // 
            // txtIpAddress
            // 
            this.txtIpAddress.Location = new System.Drawing.Point(11, 18);
            this.txtIpAddress.Name = "txtIpAddress";
            this.txtIpAddress.Size = new System.Drawing.Size(71, 21);
            this.txtIpAddress.TabIndex = 11;
            this.txtIpAddress.Text = "127.0.0.1";
            // 
            // btnStop
            // 
            this.btnStop.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.btnStop.Location = new System.Drawing.Point(407, 15);
            this.btnStop.Name = "btnStop";
            this.btnStop.Size = new System.Drawing.Size(47, 26);
            this.btnStop.TabIndex = 10;
            this.btnStop.Text = "Stop";
            this.btnStop.UseVisualStyleBackColor = true;
            this.btnStop.Click += new System.EventHandler(this.btnStop_Click);
            // 
            // btnStart
            // 
            this.btnStart.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.btnStart.Location = new System.Drawing.Point(354, 15);
            this.btnStart.Name = "btnStart";
            this.btnStart.Size = new System.Drawing.Size(47, 26);
            this.btnStart.TabIndex = 9;
            this.btnStart.Text = "Start";
            this.btnStart.UseVisualStyleBackColor = true;
            this.btnStart.Click += new System.EventHandler(this.btnStart_Click);
            // 
            // lbxMsg
            // 
            this.lbxMsg.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.lbxMsg.FormattingEnabled = true;
            this.lbxMsg.ItemHeight = 12;
            this.lbxMsg.Location = new System.Drawing.Point(11, 62);
            this.lbxMsg.Name = "lbxMsg";
            this.lbxMsg.Size = new System.Drawing.Size(465, 256);
            this.lbxMsg.TabIndex = 10;
            this.lbxMsg.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.lbxMsg_KeyPress);
            // 
            // txtSend
            // 
            this.txtSend.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.txtSend.Location = new System.Drawing.Point(12, 324);
            this.txtSend.Name = "txtSend";
            this.txtSend.Size = new System.Drawing.Size(411, 21);
            this.txtSend.TabIndex = 12;
            this.txtSend.Text = "text to be sent";
            // 
            // btnSend
            // 
            this.btnSend.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.btnSend.Location = new System.Drawing.Point(429, 321);
            this.btnSend.Name = "btnSend";
            this.btnSend.Size = new System.Drawing.Size(47, 26);
            this.btnSend.TabIndex = 13;
            this.btnSend.Text = "Send";
            this.btnSend.UseVisualStyleBackColor = true;
            this.btnSend.Click += new System.EventHandler(this.btnSend_Click);
            // 
            // frmClient
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(485, 350);
            this.Controls.Add(this.btnSend);
            this.Controls.Add(this.txtSend);
            this.Controls.Add(this.groupBox1);
            this.Controls.Add(this.lbxMsg);
            this.Name = "frmClient";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.Text = "Echo-TcpPullClient [ \'C\' - clear list box ]";
            this.FormClosed += new System.Windows.Forms.FormClosedEventHandler(this.frmClient_FormClosed);
            this.Load += new System.EventHandler(this.frmClient_Load);
            this.groupBox1.ResumeLayout(false);
            this.groupBox1.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.TextBox txtPort;
        private System.Windows.Forms.TextBox txtIpAddress;
        private System.Windows.Forms.Button btnStop;
        private System.Windows.Forms.Button btnStart;
        private System.Windows.Forms.ListBox lbxMsg;
        private System.Windows.Forms.CheckBox cbxAsyncConn;
        private System.Windows.Forms.TextBox txtSend;
        private System.Windows.Forms.Button btnSend;

    }
}


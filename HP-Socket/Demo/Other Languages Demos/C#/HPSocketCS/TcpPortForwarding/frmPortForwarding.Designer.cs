namespace TcpPortForwarding
{
    partial class frmTcpPortForwarding
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
            this.lbxMsg = new System.Windows.Forms.ListBox();
            this.label1 = new System.Windows.Forms.Label();
            this.label2 = new System.Windows.Forms.Label();
            this.txtBindAddr = new System.Windows.Forms.TextBox();
            this.txtTargetAddr = new System.Windows.Forms.TextBox();
            this.txtBindPort = new System.Windows.Forms.TextBox();
            this.txtTargetPort = new System.Windows.Forms.TextBox();
            this.label3 = new System.Windows.Forms.Label();
            this.txtDisConn = new System.Windows.Forms.TextBox();
            this.btnDisconn = new System.Windows.Forms.Button();
            this.btnStart = new System.Windows.Forms.Button();
            this.btnStop = new System.Windows.Forms.Button();
            this.SuspendLayout();
            // 
            // lbxMsg
            // 
            this.lbxMsg.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.lbxMsg.FormattingEnabled = true;
            this.lbxMsg.ItemHeight = 12;
            this.lbxMsg.Location = new System.Drawing.Point(1, 1);
            this.lbxMsg.Name = "lbxMsg";
            this.lbxMsg.Size = new System.Drawing.Size(474, 304);
            this.lbxMsg.TabIndex = 5;
            this.lbxMsg.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.lbxMsg_KeyPress);
            // 
            // label1
            // 
            this.label1.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(12, 319);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(41, 12);
            this.label1.TabIndex = 6;
            this.label1.Text = "Local:";
            // 
            // label2
            // 
            this.label2.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(6, 341);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(47, 12);
            this.label2.TabIndex = 7;
            this.label2.Text = "Target:";
            // 
            // txtBindAddr
            // 
            this.txtBindAddr.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.txtBindAddr.Location = new System.Drawing.Point(60, 316);
            this.txtBindAddr.Name = "txtBindAddr";
            this.txtBindAddr.Size = new System.Drawing.Size(100, 21);
            this.txtBindAddr.TabIndex = 8;
            this.txtBindAddr.Text = "0.0.0.0";
            // 
            // txtTargetAddr
            // 
            this.txtTargetAddr.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.txtTargetAddr.Location = new System.Drawing.Point(60, 343);
            this.txtTargetAddr.Name = "txtTargetAddr";
            this.txtTargetAddr.Size = new System.Drawing.Size(100, 21);
            this.txtTargetAddr.TabIndex = 9;
            this.txtTargetAddr.Text = "127.0.0.1";
            // 
            // txtBindPort
            // 
            this.txtBindPort.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.txtBindPort.Location = new System.Drawing.Point(166, 316);
            this.txtBindPort.Name = "txtBindPort";
            this.txtBindPort.Size = new System.Drawing.Size(48, 21);
            this.txtBindPort.TabIndex = 10;
            this.txtBindPort.Text = "7000";
            // 
            // txtTargetPort
            // 
            this.txtTargetPort.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.txtTargetPort.Location = new System.Drawing.Point(166, 343);
            this.txtTargetPort.Name = "txtTargetPort";
            this.txtTargetPort.Size = new System.Drawing.Size(48, 21);
            this.txtTargetPort.TabIndex = 11;
            this.txtTargetPort.Text = "5555";
            // 
            // label3
            // 
            this.label3.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(221, 336);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(47, 12);
            this.label3.TabIndex = 12;
            this.label3.Text = "CONNID:";
            // 
            // txtDisConn
            // 
            this.txtDisConn.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.txtDisConn.Location = new System.Drawing.Point(274, 332);
            this.txtDisConn.Name = "txtDisConn";
            this.txtDisConn.Size = new System.Drawing.Size(47, 21);
            this.txtDisConn.TabIndex = 13;
            this.txtDisConn.TextChanged += new System.EventHandler(this.txtDisConn_TextChanged);
            // 
            // btnDisconn
            // 
            this.btnDisconn.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.btnDisconn.Location = new System.Drawing.Point(328, 331);
            this.btnDisconn.Name = "btnDisconn";
            this.btnDisconn.Size = new System.Drawing.Size(61, 23);
            this.btnDisconn.TabIndex = 14;
            this.btnDisconn.Text = "Dis conn";
            this.btnDisconn.UseVisualStyleBackColor = true;
            this.btnDisconn.Click += new System.EventHandler(this.btnDisconn_Click);
            // 
            // btnStart
            // 
            this.btnStart.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.btnStart.Location = new System.Drawing.Point(412, 314);
            this.btnStart.Name = "btnStart";
            this.btnStart.Size = new System.Drawing.Size(61, 23);
            this.btnStart.TabIndex = 15;
            this.btnStart.Text = "Start";
            this.btnStart.UseVisualStyleBackColor = true;
            this.btnStart.Click += new System.EventHandler(this.btnStart_Click);
            // 
            // btnStop
            // 
            this.btnStop.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.btnStop.Location = new System.Drawing.Point(412, 343);
            this.btnStop.Name = "btnStop";
            this.btnStop.Size = new System.Drawing.Size(61, 23);
            this.btnStop.TabIndex = 16;
            this.btnStop.Text = "Stop";
            this.btnStop.UseVisualStyleBackColor = true;
            this.btnStop.Click += new System.EventHandler(this.btnStop_Click);
            // 
            // frmTcpPortForwarding
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(475, 370);
            this.Controls.Add(this.btnStop);
            this.Controls.Add(this.btnStart);
            this.Controls.Add(this.btnDisconn);
            this.Controls.Add(this.txtDisConn);
            this.Controls.Add(this.label3);
            this.Controls.Add(this.txtTargetPort);
            this.Controls.Add(this.txtBindPort);
            this.Controls.Add(this.txtTargetAddr);
            this.Controls.Add(this.txtBindAddr);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.lbxMsg);
            this.Name = "frmTcpPortForwarding";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.Text = "TcpPortForwarding  [ \'C\' - clear list box ]";
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.frmProxyServer_FormClosing);
            this.Load += new System.EventHandler(this.Form1_Load);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.ListBox lbxMsg;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.TextBox txtBindAddr;
        private System.Windows.Forms.TextBox txtTargetAddr;
        private System.Windows.Forms.TextBox txtBindPort;
        private System.Windows.Forms.TextBox txtTargetPort;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.TextBox txtDisConn;
        private System.Windows.Forms.Button btnDisconn;
        private System.Windows.Forms.Button btnStart;
        private System.Windows.Forms.Button btnStop;
    }
}


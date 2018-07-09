namespace WindowsFormsTest
{
    partial class FormMain
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
        /// 设计器支持所需的方法 - 不要修改
        /// 使用代码编辑器修改此方法的内容。
        /// </summary>
        private void InitializeComponent()
        {
            this.components = new System.ComponentModel.Container();
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(FormMain));
            this.button_playpause = new System.Windows.Forms.Button();
            this.button_open = new System.Windows.Forms.Button();
            this.button_close = new System.Windows.Forms.Button();
            this.label_time = new System.Windows.Forms.Label();
            this.label_file = new System.Windows.Forms.Label();
            this.timerPlay = new System.Windows.Forms.Timer(this.components);
            this.openFileDialog1 = new System.Windows.Forms.OpenFileDialog();
            this.lable_vol = new System.Windows.Forms.Label();
            this.tkb_vol = new WindowsFormsTest.BTrackBar();
            this.tkb = new WindowsFormsTest.BTrackBar();
            this.SuspendLayout();
            // 
            // button_playpause
            // 
            this.button_playpause.Location = new System.Drawing.Point(225, 216);
            this.button_playpause.Name = "button_playpause";
            this.button_playpause.Size = new System.Drawing.Size(75, 23);
            this.button_playpause.TabIndex = 1;
            this.button_playpause.Text = "播放/暂停";
            this.button_playpause.UseVisualStyleBackColor = true;
            this.button_playpause.Click += new System.EventHandler(this.button_playpause_Click);
            // 
            // button_open
            // 
            this.button_open.Location = new System.Drawing.Point(438, 12);
            this.button_open.Name = "button_open";
            this.button_open.Size = new System.Drawing.Size(75, 23);
            this.button_open.TabIndex = 2;
            this.button_open.Text = "打开音乐";
            this.button_open.UseVisualStyleBackColor = true;
            this.button_open.Click += new System.EventHandler(this.button_open_Click);
            // 
            // button_close
            // 
            this.button_close.Location = new System.Drawing.Point(438, 41);
            this.button_close.Name = "button_close";
            this.button_close.Size = new System.Drawing.Size(75, 23);
            this.button_close.TabIndex = 3;
            this.button_close.Text = "关闭音乐";
            this.button_close.UseVisualStyleBackColor = true;
            this.button_close.Click += new System.EventHandler(this.button_close_Click);
            // 
            // label_time
            // 
            this.label_time.AutoSize = true;
            this.label_time.Location = new System.Drawing.Point(439, 179);
            this.label_time.Name = "label_time";
            this.label_time.Size = new System.Drawing.Size(71, 12);
            this.label_time.TabIndex = 4;
            this.label_time.Text = "00:00/00:00";
            // 
            // label_file
            // 
            this.label_file.AutoEllipsis = true;
            this.label_file.Location = new System.Drawing.Point(12, 12);
            this.label_file.Name = "label_file";
            this.label_file.Size = new System.Drawing.Size(420, 52);
            this.label_file.TabIndex = 5;
            this.label_file.Text = "没有打开的文件";
            // 
            // timerPlay
            // 
            this.timerPlay.Interval = 1000;
            this.timerPlay.Tick += new System.EventHandler(this.timerPlay_Tick);
            // 
            // openFileDialog1
            // 
            this.openFileDialog1.Filter = "音乐文件|*.mp3;*.mp2;*.mp1;*.wav;*.mid;*.midi;*.wma;*.ogg;*.oga;*.flac;*.pcm|所有文件|*.*" +
    "";
            this.openFileDialog1.Title = "打开音乐";
            // 
            // lable_vol
            // 
            this.lable_vol.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.lable_vol.Location = new System.Drawing.Point(453, 80);
            this.lable_vol.Name = "lable_vol";
            this.lable_vol.Size = new System.Drawing.Size(60, 57);
            this.lable_vol.TabIndex = 8;
            this.lable_vol.Text = "音量：100";
            this.lable_vol.TextAlign = System.Drawing.ContentAlignment.TopRight;
            // 
            // tkb_vol
            // 
            this.tkb_vol.Direction = WindowsFormsTest.UITrackBarDirection.Vertical;
            this.tkb_vol.DrawMode = WindowsFormsTest.UITrackBarDrawMode.Color;
            this.tkb_vol.ImageCutValue = new System.Windows.Forms.Padding(5, 0, 0, 0);
            this.tkb_vol.LineWidth = 2;
            this.tkb_vol.Location = new System.Drawing.Point(431, 70);
            this.tkb_vol.Name = "tkb_vol";
            this.tkb_vol.PointDisabledImage = ((System.Drawing.Image)(resources.GetObject("tkb_vol.PointDisabledImage")));
            this.tkb_vol.PointHoverImage = ((System.Drawing.Image)(resources.GetObject("tkb_vol.PointHoverImage")));
            this.tkb_vol.PointNormalImage = ((System.Drawing.Image)(resources.GetObject("tkb_vol.PointNormalImage")));
            this.tkb_vol.PointPressedImage = ((System.Drawing.Image)(resources.GetObject("tkb_vol.PointPressedImage")));
            this.tkb_vol.ProgressBackColor = System.Drawing.Color.OrangeRed;
            this.tkb_vol.ProgressBackImage = null;
            this.tkb_vol.ProgressColor = System.Drawing.Color.FromArgb(((int)(((byte)(150)))), ((int)(((byte)(150)))), ((int)(((byte)(150)))));
            this.tkb_vol.ProgressImage = null;
            this.tkb_vol.Size = new System.Drawing.Size(16, 92);
            this.tkb_vol.TabIndex = 6;
            this.tkb_vol.Text = "bTrackBar1";
            this.tkb_vol.Value = 0D;
            this.tkb_vol.ValueChanged += new System.EventHandler(this.tkb_vol_ValueChanged);
            // 
            // tkb
            // 
            this.tkb.Direction = WindowsFormsTest.UITrackBarDirection.Horizontal;
            this.tkb.DrawMode = WindowsFormsTest.UITrackBarDrawMode.Color;
            this.tkb.ImageCutValue = new System.Windows.Forms.Padding(5, 0, 0, 0);
            this.tkb.LineWidth = 2;
            this.tkb.Location = new System.Drawing.Point(24, 174);
            this.tkb.Name = "tkb";
            this.tkb.PointDisabledImage = ((System.Drawing.Image)(resources.GetObject("tkb.PointDisabledImage")));
            this.tkb.PointHoverImage = ((System.Drawing.Image)(resources.GetObject("tkb.PointHoverImage")));
            this.tkb.PointNormalImage = ((System.Drawing.Image)(resources.GetObject("tkb.PointNormalImage")));
            this.tkb.PointPressedImage = ((System.Drawing.Image)(resources.GetObject("tkb.PointPressedImage")));
            this.tkb.ProgressBackColor = System.Drawing.Color.Gray;
            this.tkb.ProgressBackImage = null;
            this.tkb.ProgressColor = System.Drawing.Color.DeepSkyBlue;
            this.tkb.ProgressImage = null;
            this.tkb.Size = new System.Drawing.Size(409, 23);
            this.tkb.TabIndex = 0;
            this.tkb.Text = "bTrackBar1";
            this.tkb.Value = 0D;
            this.tkb.MouseDown += new System.Windows.Forms.MouseEventHandler(this.tkb_MouseDown);
            this.tkb.MouseUp += new System.Windows.Forms.MouseEventHandler(this.tkb_MouseUp);
            // 
            // FormMain
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(525, 251);
            this.Controls.Add(this.tkb_vol);
            this.Controls.Add(this.label_file);
            this.Controls.Add(this.label_time);
            this.Controls.Add(this.button_close);
            this.Controls.Add(this.button_open);
            this.Controls.Add(this.button_playpause);
            this.Controls.Add(this.tkb);
            this.Controls.Add(this.lable_vol);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle;
            this.MaximizeBox = false;
            this.Name = "FormMain";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.Text = "Form1";
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.FormMain_FormClosing);
            this.Load += new System.EventHandler(this.FormMain_Load);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private BTrackBar tkb;
        private System.Windows.Forms.Button button_playpause;
        private System.Windows.Forms.Button button_open;
        private System.Windows.Forms.Button button_close;
        private System.Windows.Forms.Label label_time;
        private System.Windows.Forms.Label label_file;
        private System.Windows.Forms.Timer timerPlay;
        private System.Windows.Forms.OpenFileDialog openFileDialog1;
        private BTrackBar tkb_vol;
        private System.Windows.Forms.Label lable_vol;
    }
}


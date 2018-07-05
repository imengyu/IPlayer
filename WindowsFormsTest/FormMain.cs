using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace WindowsFormsTest
{
    public partial class FormMain : Form
    {
        public FormMain()
        {
            InitializeComponent();
        }

        private void button_open_Click(object sender, EventArgs e)
        {
            if (openFileDialog1.ShowDialog() == DialogResult.OK)
            {
                if (IPlayer.OpenMusic(openFileDialog1.FileName))
                {
                    IPlayer.PlayMusic();
                    timerPlay.Start();
                    label_time.Text = "00:00/00:00";
                    label_file.Text = openFileDialog1.FileName;
                }
                else MessageBox.Show("打开失败：" + IPlayer.ptrtostr(IPlayer.GetPlayerError()));
            }
        }
        private void button_close_Click(object sender, EventArgs e)
        {
            label_file.Text = "没有打开的文件";
            label_time.Text = "00:00/00:00";
            timerPlay.Stop();
            IPlayer.StopMusic();
            IPlayer.CloseMusic();
        }

        private void button_playpause_Click(object sender, EventArgs e)
        {
            IPlayer.PausePlayMusic();
            if (IPlayer.GetMusicPlayState() == TPlayerStatus.Playing)
                timerPlay.Start();
        }

        private bool tkb_press = false;
        private void tkb_MouseDown(object sender, MouseEventArgs e)
        {
            tkb_press = true;
        }
        private void tkb_MouseUp(object sender, MouseEventArgs e)
        {
            tkb_press = false;
            if (IPlayer.GetMusicPlayState() != TPlayerStatus.NotOpen)
            {
                IPlayer.SetMusicPos(IPlayer.GetMusicLength() * tkb.Value);
            }
        }

        private void timerPlay_Tick(object sender, EventArgs e)
        {
            TPlayerStatus playerStatus = IPlayer.GetMusicPlayState();
            switch (playerStatus)
            {
                case TPlayerStatus.Playing:
                    if (!tkb_press)
                        tkb.Value = IPlayer.GetMusicPos() / IPlayer.GetMusicLength();
                    string s = IPlayer.ptrtostr( IPlayer.GetMusicTimeString());
                    if (!string.IsNullOrEmpty(s))
                        label_time.Text = s;
                    break;
                case TPlayerStatus.NotOpen:
                case TPlayerStatus.PlayEnd:
                    label_file.Text = "没有打开的文件";
                    label_time.Text = "00:00/00:00";
                    timerPlay.Stop();
                    IPlayer.StopMusic();
                    IPlayer.CloseMusic();
                    break;
                default:
                case TPlayerStatus.Opened:
                case TPlayerStatus.Paused:
                    timerPlay.Stop();
                    break;
            }
        }

        private void FormMain_Load(object sender, EventArgs e)
        {
            IPlayer.IPlayerInit(Handle);
        }
        private void FormMain_FormClosing(object sender, FormClosingEventArgs e)
        {
            IPlayer.IPlayerDestroy();
        }

        private void tkb_vol_ValueChanged(object sender, EventArgs e)
        {
            IPlayer.SetVolume((int)((1 - tkb_vol.Value) * 100));
            lable_vol.Text = "音量：" + (int)((1 - tkb_vol.Value) * 100);
        }

        private void open(string file)
        {
            if (IPlayer.OpenMusic(file))
            {
                IPlayer.PlayMusic();
                timerPlay.Start();
                label_time.Text = "00:00/00:00";
                label_file.Text = openFileDialog1.FileName;
            }
            else MessageBox.Show("打开失败：" + IPlayer.ptrtostr(IPlayer.GetPlayerError()));
        }

        private void button1_Click(object sender, EventArgs e)
        {
            open(@"E:\主数据库\音乐\声效\Windows XP 启动(2).wav");
        }
        private void button2_Click(object sender, EventArgs e)
        {
            open(@"C:\Users\15010\Music\aaaaaaa.mp3");
        }
        private void button3_Click(object sender, EventArgs e)
        {
            open(@"E:\主数据库\音乐\徐秉龙 - 千禧.flac");
        }

        private void FormMain_Paint(object sender, PaintEventArgs e)
        {
            
        }


    }
}

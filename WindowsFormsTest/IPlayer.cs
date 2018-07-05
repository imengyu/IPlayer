using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace WindowsFormsTest
{
    /// <summary>
    /// 播放器状态
    /// </summary>
    public enum TPlayerStatus
    {
        /// <summary>
        /// 默认
        /// </summary>
        Unknow,
        /// <summary>
        /// 就绪
        /// </summary>
        NotOpen,
        /// <summary>
        /// 已打开文件
        /// </summary>
        Opened,
        /// <summary>
        /// 正在播放
        /// </summary>
        Playing,
        /// <summary>
        /// 已暂停
        /// </summary>
        Paused,
        /// <summary>
        /// 已经播放至文件结尾
        /// </summary>
        PlayEnd,
    };
    /// <summary>
    /// IPlayer Audio 播放器
    /// </summary>
    public class IPlayer
    {
        public static string ptrtostr(IntPtr strptr)
        {
            if (strptr == IntPtr.Zero) return null;
            return Marshal.PtrToStringAuto(strptr);
        }

        /// <summary>
        /// 打开音乐
        /// </summary>
        /// <param name="file">文件路径</param>
        /// <returns></returns>
        [DllImport("iplayer.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern bool OpenMusic([MarshalAs(UnmanagedType.LPWStr)]string file);
        /// <summary>
        /// 停止音乐
        /// </summary>
        /// <returns></returns>
        [DllImport("iplayer.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern bool StopMusic();
        /// <summary>
        /// 关闭已打开的音乐
        /// </summary>
        /// <returns></returns>
        [DllImport("iplayer.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern bool CloseMusic();
        /// <summary>
        /// 播放或暂停音乐
        /// 如果音乐正在播放，则暂停音乐；如果音乐暂停，则开始播放音乐（相当于播放/暂停按钮）。
        /// </summary>
        /// <returns></returns>
        [DllImport("iplayer.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern bool PausePlayMusic();
        /// <summary>
        /// 调用本库的API时，请先调用此方法初始化播放器
        /// </summary>
        /// <param name="hWnd">调用者窗口句柄</param>
        /// <returns></returns>
        [DllImport("iplayer.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern bool IPlayerInit(IntPtr hWnd);
        /// <summary>
        /// 从头开始播放当前音乐
        /// </summary>
        /// <returns></returns>
        [DllImport("iplayer.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern bool ReplayMusic();
        /// <summary>
        /// 开始播放
        /// </summary>
        /// <returns></returns>
        [DllImport("iplayer.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern bool PlayMusic();
        /// <summary>
        /// 暂停播放
        /// </summary>
        /// <returns></returns>
        [DllImport("iplayer.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern bool PauseMusic();
        /// <summary>
        /// 设置当前音乐播放位置
        /// </summary>
        /// <param name="val">当前音乐播放位置，单位为 秒</param>
        /// <returns></returns>
        [DllImport("iplayer.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern bool SetMusicPos(double val);
        /// <summary>
        /// 获取当前音乐长度
        /// </summary>
        /// <returns>当前音乐长度，单位为 秒</returns>
        [DllImport("iplayer.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern double GetMusicLength();    
        /// <summary>
        /// 获取当前音乐播放位置
        /// </summary>
        /// <returns>当前音乐播放位置，单位为 秒</returns>
        [DllImport("iplayer.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern double GetMusicPos();
        /// <summary>
        /// 获取音乐是否已打开
        /// </summary>
        /// <returns>音乐是否已打开</returns>
        [DllImport("iplayer.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern bool GetOpened();
        /// <summary>
        /// 获取音乐正在播放的时间字符串
        /// 格式是：00:00/00:00 正在播放时间/音乐时长
        /// </summary>
        /// <returns></returns>
        [DllImport("iplayer.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr GetMusicTimeString();
        /// <summary>
        /// 获取播放器上一个错误信息
        /// </summary>
        /// <returns>播放器上一个错误信息</returns>
        [DllImport("iplayer.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr GetPlayerError();
        /// <summary>
        /// 销毁播放器
        /// 请在程序退出时使用此方法
        /// </summary>
        /// <returns></returns>
        [DllImport("iplayer.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern bool IPlayerDestroy();
        /// <summary>
        /// 音乐淡出
        /// </summary>
        /// <param name="currvol">当前音量</param>
        /// <returns></returns>
        [DllImport("iplayer.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern bool MusicTweedOut(int currvol);
        /// <summary>
        /// 音乐淡入
        /// </summary>
        /// <param name="currvol">当前音量</param>
        /// <returns></returns>
        [DllImport("iplayer.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern bool MusicTweedIn(int currvol);
        [DllImport("iplayer.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern bool SetFFTHDC(IntPtr hdc);
        [DllImport("iplayer.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern bool DrawFFTOnHDC(IntPtr hdc);
        [DllImport("iplayer.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern bool DrawFFTOnHDCSmall(IntPtr hdc);
        /// <summary>
        /// 获取播放器状态
        /// </summary>
        /// <returns></returns>
        [DllImport("iplayer.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern TPlayerStatus GetMusicPlayState();
        /// <summary>
        /// 获取播放器音量
        /// </summary>
        /// <returns>返回音量数值 ：(1-100)</returns>
        [DllImport("iplayer.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern int GetVolume();
        /// <summary>
        /// 设置播放器音量
        /// </summary>
        /// <param name="vol">音量数值 ：(1-100)</param>
        /// <returns></returns>
        [DllImport("iplayer.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern bool SetVolume(int vol);

        /// <summary>
        /// 设置音乐播放的位置 (以 样本 sample 为单位)
        /// </summary>
        /// <returns></returns>
        [DllImport("iplayer.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern ulong GetMusicPosSample();

        /// <summary>
        /// 获取音乐播放的样本位置 (以 样本 sample 为单位)，
        /// 使用 GetMusicSampleRate 获取 音乐采样率
        /// </summary>
        /// <param name="sample"></param>
        /// <returns></returns>
        [DllImport("iplayer.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern ulong SetMusicPosSample(ulong sample);

        /// <summary>
        /// 获取音乐文件的 采样率
        /// </summary>
        /// <returns></returns>
        [DllImport("iplayer.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern ulong GetMusicSampleRate();
        /// <summary>
        /// 获取音乐文件的 量化位数 （8/16/32）
        /// </summary>
        /// <returns></returns>
        [DllImport("iplayer.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern int GetMusicBitPerSample();
        /// <summary>
        /// 获取音乐文件的 声道数
        /// </summary>
        /// <returns>声道数</returns>
        [DllImport("iplayer.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern int GetMusicChannelsCount();
    }
}

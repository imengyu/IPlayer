using System;
using System.Drawing;
using System.Windows.Forms;
using WindowsFormsTest.Properties;

namespace WindowsFormsTest
{
    public enum CutPalaceType
    {
        Full,
        NS,
        WE
    }
    public enum UITrackBarDirection
    {
        Horizontal,
        Vertical
    }
    public enum UITrackBarDrawMode
    {
        Image,
        Color
    }
    public class BTrackBar : Control
    {
        private UITrackBarDirection dre;

        private UITrackBarDrawMode draw = UITrackBarDrawMode.Color;

        private Image bgimg;

        private Image progimg;

        private Image ptnorimg = Resources.pt_white_n;

        private Image pthovimg = Resources.pt_white_h;

        private Image ptpreimg = Resources.pt_white_p;

        private Image ptdisimg = Resources.pt_white_p;

        private double val;

        private Padding cutval = new Padding(5, 0, 0, 0);

        private Color bgcolor = Color.Gray;

        private Color progcolor = Color.FromArgb(150, 150, 150);

        private bool mouseenter;

        private bool mousedown;

        private int linewidth = 2;

        private Rectangle curt;

        private int drawLeft = 8;

        public event EventHandler ValueChanged;

        public int LineWidth
        {
            get
            {
                return this.linewidth;
            }
            set
            {
                this.linewidth = value;
                if (this.draw == UITrackBarDrawMode.Color)
                {
                    base.Invalidate();
                }
            }
        }

        public double Value
        {
            get
            {
                return this.val;
            }
            set
            {
                if (this.val != value && value >= 0.0 && value <= 1.0)
                {
                    this.val = value;
                    if (this.dre == UITrackBarDirection.Vertical)
                    {
                        this.curt = new Rectangle(base.Width / 2 - this.ptnorimg.Width / 2, (int)(this.val * (double)this.GetTrackReatHeight()), this.ptnorimg.Width, this.ptnorimg.Height);
                    }
                    else
                    {
                        this.curt = new Rectangle((int)(this.val * (double)this.GetTrackReatHeight()), base.Height / 2 - this.ptnorimg.Height / 2, this.ptnorimg.Width, this.ptnorimg.Height);
                    }
                    base.Invalidate();
                    if (this.ValueChanged != null)
                    {
                        this.ValueChanged(this, new EventArgs());
                    }
                }
            }
        }

        public UITrackBarDirection Direction
        {
            get
            {
                return this.dre;
            }
            set
            {
                this.dre = value;
                base.Invalidate();
            }
        }

        public UITrackBarDrawMode DrawMode
        {
            get
            {
                return this.draw;
            }
            set
            {
                this.draw = value;
                base.Invalidate();
            }
        }

        public Padding ImageCutValue
        {
            get
            {
                return this.cutval;
            }
            set
            {
                this.cutval = value;
            }
        }

        public Color ProgressBackColor
        {
            get
            {
                return this.bgcolor;
            }
            set
            {
                this.bgcolor = value;
                if (this.draw == UITrackBarDrawMode.Color)
                {
                    base.Invalidate();
                }
            }
        }

        public Color ProgressColor
        {
            get
            {
                return this.progcolor;
            }
            set
            {
                this.progcolor = value;
                if (this.draw == UITrackBarDrawMode.Color)
                {
                    base.Invalidate();
                }
            }
        }

        public Image ProgressBackImage
        {
            get
            {
                return this.bgimg;
            }
            set
            {
                this.bgimg = value;
                if (this.draw == UITrackBarDrawMode.Image)
                {
                    base.Invalidate();
                }
            }
        }

        public Image ProgressImage
        {
            get
            {
                return this.progimg;
            }
            set
            {
                this.progimg = value;
                if (this.draw == UITrackBarDrawMode.Image)
                {
                    base.Invalidate();
                }
            }
        }

        public Image PointNormalImage
        {
            get
            {
                return this.ptnorimg;
            }
            set
            {
                this.ptnorimg = value;
                if (value == null)
                {
                    this.ptnorimg = Resources.pt_white_n;
                }
                if (this.draw == UITrackBarDrawMode.Image)
                {
                    base.Invalidate();
                }
                if (value != null)
                {
                    this.drawLeft = value.Width / 2;
                }
            }
        }

        public Image PointHoverImage
        {
            get
            {
                return this.pthovimg;
            }
            set
            {
                this.pthovimg = value;
                if (this.draw == UITrackBarDrawMode.Image)
                {
                    base.Invalidate();
                }
            }
        }

        public Image PointPressedImage
        {
            get
            {
                return this.ptpreimg;
            }
            set
            {
                this.ptpreimg = value;
                if (this.draw == UITrackBarDrawMode.Image)
                {
                    base.Invalidate();
                }
            }
        }

        public Image PointDisabledImage
        {
            get
            {
                return this.ptdisimg;
            }
            set
            {
                this.ptdisimg = value;
                if (this.draw == UITrackBarDrawMode.Image)
                {
                    base.Invalidate();
                }
            }
        }

        public static Bitmap GetPalaceImage(Image i, CutPalaceType type, Padding p, Size size, bool cutcenter = false)
        {
            Bitmap expr_18 = new Bitmap(size.Width, size.Height, System.Drawing.Imaging.PixelFormat.Format32bppArgb);
            Graphics graphics = Graphics.FromImage(expr_18);
            if (type == CutPalaceType.Full)
            {
                graphics.DrawImage(i, new Rectangle(0, 0, p.Left, p.Top), new Rectangle(0, 0, p.Left, p.Top), GraphicsUnit.Pixel);
                graphics.DrawImage(i, new Rectangle(p.Left, 0, size.Width - p.Left - p.Right, p.Top), new Rectangle(p.Left, 0, i.Width - p.Right - p.Left, p.Top), GraphicsUnit.Pixel);
                graphics.DrawImage(i, new Rectangle(size.Width - p.Right, 0, p.Right, p.Top), new Rectangle(i.Width - p.Right, 0, p.Right, p.Top), GraphicsUnit.Pixel);
                graphics.DrawImage(i, new Rectangle(size.Width - p.Right, p.Top, p.Right, size.Height - p.Top - p.Bottom), new Rectangle(i.Width - p.Right, p.Top, p.Right, i.Height - p.Top - p.Bottom), GraphicsUnit.Pixel);
                if (!cutcenter)
                {
                    graphics.DrawImage(i, new Rectangle(p.Left, p.Top, size.Width - p.Left - p.Right, size.Height - p.Top - p.Bottom), new Rectangle(p.Left, p.Top, i.Width - p.Left - p.Right, i.Height - p.Bottom - p.Top), GraphicsUnit.Pixel);
                }
                graphics.DrawImage(i, new Rectangle(size.Width - p.Right, size.Height - p.Bottom, p.Right, p.Bottom), new Rectangle(i.Width - p.Right, i.Height - p.Bottom, p.Right, p.Bottom), GraphicsUnit.Pixel);
                graphics.DrawImage(i, new Rectangle(p.Left, size.Height - p.Bottom, size.Width - p.Left - p.Right, p.Bottom), new Rectangle(p.Left, i.Height - p.Bottom, i.Width - p.Right - p.Left, p.Bottom), GraphicsUnit.Pixel);
                graphics.DrawImage(i, new Rectangle(0, size.Height - p.Bottom, p.Left, p.Bottom), new Rectangle(0, i.Height - p.Bottom, p.Left, p.Bottom), GraphicsUnit.Pixel);
                graphics.DrawImage(i, new Rectangle(0, p.Top, p.Left, size.Height - p.Top - p.Bottom), new Rectangle(0, p.Top, p.Left, i.Height - p.Top - p.Bottom), GraphicsUnit.Pixel);
                graphics.Dispose();
                return expr_18;
            }
            if (type == CutPalaceType.NS)
            {
                graphics.DrawImage(i, new Rectangle(0, 0, size.Width, p.Top), new Rectangle(0, 0, i.Width, p.Top), GraphicsUnit.Pixel);
                if (!cutcenter)
                {
                    graphics.DrawImage(i, new Rectangle(0, p.Top, size.Width, size.Height - p.Top - p.Bottom), new Rectangle(0, p.Top, i.Width, i.Height - p.Bottom - p.Top), GraphicsUnit.Pixel);
                }
                graphics.DrawImage(i, new Rectangle(0, size.Height - p.Bottom, size.Width, p.Bottom), new Rectangle(0, i.Height - p.Bottom, i.Width, p.Bottom), GraphicsUnit.Pixel);
                graphics.Dispose();
                return expr_18;
            }
            if (type == CutPalaceType.WE)
            {
                graphics.DrawImage(i, new Rectangle(0, 0, p.Left, size.Height), new Rectangle(0, 0, p.Left, i.Height), GraphicsUnit.Pixel);
                if (!cutcenter)
                {
                    graphics.DrawImage(i, new Rectangle(p.Left, 0, size.Width - p.Left - p.Right, size.Height), new Rectangle(p.Left, 0, i.Width - p.Left - p.Right, i.Height), GraphicsUnit.Pixel);
                }
                graphics.DrawImage(i, new Rectangle(size.Width - p.Right, 0, p.Right, size.Height), new Rectangle(i.Width - p.Right, 0, p.Right, i.Height), GraphicsUnit.Pixel);
                graphics.Dispose();
            }
            return expr_18;
        }

        public BTrackBar()
        {
            base.SetStyle(ControlStyles.OptimizedDoubleBuffer, true);
            this.curt = new Rectangle((int)(this.val * (double)this.GetTrackReatHeight()), base.Height / 2 + 4, this.ptnorimg.Width, this.ptnorimg.Height);
        }

        private int GetTrackReatLeft()
        {
            if (this.dre == UITrackBarDirection.Vertical)
            {
                return this.ptnorimg.Width / 2;
            }
            return this.ptnorimg.Height / 2;
        }

        private int GetTrackReatHeight()
        {
            if (this.dre == UITrackBarDirection.Vertical)
            {
                return base.Height - this.ptnorimg.Width;
            }
            return base.Width - this.ptnorimg.Height;
        }

        protected override void OnMouseMove(MouseEventArgs e)
        {
            if (this.mousedown)
            {
                if (this.dre == UITrackBarDirection.Horizontal)
                {
                    this.Value = (double)e.X / (double)base.Width;
                }
                else
                {
                    this.Value = (double)e.Y / (double)base.Height;
                }
            }
            else if (this.IsInRect(e.Location, this.curt))
            {
                if (!this.mouseenter)
                {
                    this.mouseenter = true;
                    base.Invalidate();
                }
            }
            else if (this.mouseenter)
            {
                this.mouseenter = false;
                base.Invalidate();
            }
            base.OnMouseMove(e);
        }

        private bool IsInRect(Point pos, Rectangle rect)
        {
            return rect.Contains(pos);
        }

        protected override void OnMouseUp(MouseEventArgs e)
        {
            this.mousedown = false;
            base.Invalidate();
            base.OnMouseUp(e);
        }

        protected override void OnMouseDown(MouseEventArgs e)
        {
            this.mousedown = true;
            base.Invalidate();
            if (this.dre == UITrackBarDirection.Horizontal)
            {
                if (this.Value >= 0.0 && this.Value <= 1.0)
                {
                    this.Value = (double)e.X / (double)base.Width;
                }
            }
            else if (this.Value >= 0.0 && this.Value <= 1.0)
            {
                this.Value = (double)e.Y / (double)base.Height;
            }
            base.OnMouseDown(e);
        }

        protected override void OnPaint(PaintEventArgs e)
        {
            Graphics graphics = e.Graphics;
            if (this.draw == UITrackBarDrawMode.Color)
            {
                if (this.dre == UITrackBarDirection.Vertical)
                {
                    graphics.FillRectangle(new SolidBrush(this.bgcolor), new Rectangle(base.Width / 2 - this.linewidth / 2, this.GetTrackReatLeft(), this.linewidth, this.GetTrackReatHeight()));
                    graphics.FillRectangle(new SolidBrush(this.progcolor), new Rectangle(base.Width / 2 - this.linewidth / 2, this.GetTrackReatLeft(), this.linewidth, (int)((double)this.GetTrackReatHeight() * this.val)));
                }
                else
                {
                    graphics.FillRectangle(new SolidBrush(this.bgcolor), new Rectangle(this.GetTrackReatLeft(), base.Height / 2 - this.linewidth / 2, this.GetTrackReatHeight(), this.linewidth));
                    graphics.FillRectangle(new SolidBrush(this.progcolor), new Rectangle(this.GetTrackReatLeft(), base.Height / 2 - this.linewidth / 2, (int)((double)this.GetTrackReatHeight() * this.val), this.linewidth));
                }
            }
            else if (this.dre == UITrackBarDirection.Vertical)
            {
                if (this.bgimg != null)
                {
                    graphics.DrawImage(BTrackBar.GetPalaceImage(this.bgimg, CutPalaceType.NS, this.cutval, new Size(this.bgimg.Width, this.GetTrackReatHeight()), false), new Point(base.Width / 2 - this.bgimg.Width / 2, this.GetTrackReatLeft()));
                }
                if (this.progimg != null)
                {
                    graphics.DrawImage(BTrackBar.GetPalaceImage(this.progimg, CutPalaceType.NS, this.cutval, new Size(this.progimg.Width, (int)((double)this.GetTrackReatHeight() * this.val)), false), new Point(base.Width / 2 - this.progimg.Width / 2, this.GetTrackReatLeft()));
                }
            }
            else
            {
                if (this.bgimg != null)
                {
                    graphics.DrawImage(BTrackBar.GetPalaceImage(this.bgimg, CutPalaceType.NS, this.cutval, new Size(this.GetTrackReatHeight(), this.bgimg.Height), false), new Point(this.GetTrackReatLeft(), base.Height / 2 - this.bgimg.Height / 2));
                }
                if (this.progimg != null)
                {
                    graphics.DrawImage(BTrackBar.GetPalaceImage(this.progimg, CutPalaceType.NS, this.cutval, new Size((int)((double)this.GetTrackReatHeight() * this.val), this.progimg.Height), false), new Point(this.GetTrackReatLeft(), base.Height / 2 - this.progimg.Height / 2));
                }
            }
            if (base.Enabled)
            {
                if (this.mousedown)
                {
                    graphics.DrawImage(this.ptpreimg, this.curt);
                }
                else if (this.mouseenter)
                {
                    graphics.DrawImage(this.pthovimg, this.curt);
                }
                else
                {
                    graphics.DrawImage(this.ptnorimg, this.curt);
                }
            }
            else
            {
                graphics.DrawImage(this.ptdisimg, this.curt);
            }
            base.OnPaint(e);
        }

        protected override void Dispose(bool disposing)
        {
            if (disposing)
            {
                if (this.ptnorimg != null)
                {
                    this.ptnorimg.Dispose();
                    this.ptnorimg = null;
                }
                if (this.pthovimg != null)
                {
                    this.pthovimg.Dispose();
                    this.pthovimg = null;
                }
                if (this.ptpreimg != null)
                {
                    this.ptpreimg.Dispose();
                    this.ptpreimg = null;
                }
                if (this.ptdisimg != null)
                {
                    this.ptdisimg.Dispose();
                    this.ptdisimg = null;
                }
                if (this.bgimg != null)
                {
                    this.bgimg.Dispose();
                    this.bgimg = null;
                }
                if (this.progimg != null)
                {
                    this.progimg.Dispose();
                    this.progimg = null;
                }
            }
            base.Dispose(disposing);
        }
    }
}

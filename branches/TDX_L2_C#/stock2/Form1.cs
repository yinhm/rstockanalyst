using System.Text.RegularExpressions;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Collections;
using System.Net.Sockets;
using System.IO;
using System.Threading;
using System.Runtime.InteropServices;
using System.Net;
using System.Diagnostics;

public partial class Form1 : Form
{

    public Form1()
    {
        InitializeComponent();
    }
    // 接收数据头
    [StructLayout(LayoutKind.Sequential, Pack = 1)]
    struct RecvDataHeader
    {
        public UInt32 CheckSum;
        public byte EncodeMode;
        [MarshalAs(UnmanagedType.ByValArray, SizeConst = 5)]
        public byte[] tmp;         //char tmp[5];
        public ushort msgid;
        public ushort Size;
        public ushort DePackSize;
    };
    // 公司资料原始数据
    struct TGPNAME // 初始化数据 29字节
    {
        [MarshalAs(UnmanagedType.ByValArray, SizeConst = 6)]
        public byte[] code;         //char tmp[5];
        public ushort rate;// 实时盘口中的成交量除去的除数？1手=n股？
        [MarshalAs(UnmanagedType.ByValArray, SizeConst = 8)]
        public byte[] name;//名称
        public ushort w1, w2;
        public byte PriceMag;//小数点位数
        public float YClose;//昨收
        public ushort w3, w4;
        public byte mark;//市场 0=深圳 1=上海
        public int no;
    };

    // 公司资料分类数据
    struct tdx_infostyle
    {
        public string name;
        public string code;
        public int start, len;
    };

    //权息
    struct QuanInfo
    {
       public byte style;
       public uint day;
        public double money; 
        public double peimoney;
        public double number;
        public double peinumber;
    };

    //财务
    struct CaiWu
    {
        //public byte Mark;
        //[MarshalAs(UnmanagedType.ByValArray, SizeConst = 6)]
        //public byte[] code;
        public double LTG;//流通股数量
        public ushort t1, t2;
        public uint day1, day2;
        [MarshalAs(UnmanagedType.ByValArray, SizeConst = 30)]
        public double[] zl;
    };


    class Stock
    {
        public QuanInfo[] qu=new QuanInfo[80] ;
        public CaiWu cw;
     };

    RecvDataHeader hd = new RecvDataHeader();
    byte[] DataHeader = new byte[16];
    MemoryStream DataMs;
    BinaryReader DataBr;

    uint DateTime1 = 0;
    Boolean rc;
    Socket m_hSocket = null;
    tdx_infostyle[] info = new tdx_infostyle[16];

    byte[] Login = new byte[6000];//验证信息

    byte[] buf = new byte[2048 * 1024];
    byte[] RecvBuffer = new byte[2048 * 1024];
    MemoryStream RecvMs;
    BinaryReader RecvBr;

    TGPNAME[] GP = new TGPNAME[8000];
    int stkcount = 0;

    Stock[] STK=new Stock[8000];

    String[] CaiWuStr = {"总股本", "2", "3", "4", "B股", "H股", "7",
    "总资产", "流动资产", "固定资产", "无形资产", "12", "流动负债", "长期负债", "公积金", "净资产", "主营收入",
    "18", "19", "营业利润", "投资收益", "经营现金流", "总现金流", "存货", "利润总额", "税后利润", "净利润",
    "未分利润", "29", "30"};
    String[] DaStr = { "主力卖出", "主力买入", "主力买卖", "加速拉升", "加速下跌", "低位反弹", "高位回落", "撑杆跳高", "平台跳水", "单笔冲击", "区间放量" };


    int DEBUG = 0;

    private void Form1_Load(object sender, EventArgs e)
    {

        DataMs = new MemoryStream(DataHeader);
        DataBr = new BinaryReader(DataMs);

        RecvMs = new MemoryStream(RecvBuffer);
        RecvBr = new BinaryReader(RecvMs);
        for (int i = 0; i < GP.Length; i++)
        {
            GP[i] = new TGPNAME();
            STK[i] = new Stock();
            STK[i].cw.zl = new double[30];
        }
        for (int i = 0; i < 16; i++)
            info[i] = new tdx_infostyle();
    }
    public Boolean Connect(string ip, int port)
    {
        Boolean bb = false;
        if (m_hSocket != null)
        {
            MessageBox.Show("网络已连接");
            return false;
        }
        try
        {
            m_hSocket = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
            IPEndPoint localEndPoint = new IPEndPoint(IPAddress.Parse(ip), port);
            m_hSocket.Connect(localEndPoint);
            bb = true;
        }
        catch (Exception Ex)
        {
            List1.Items.Add(Ex.ToString());
            bb = false;
            m_hSocket.Close();
            m_hSocket = null;
        }
        return bb;
    }

    public int Decompress(byte[] dd, int Len, ref byte[] rv)
    {
        int rLen = -1;
        flat inflater = new flat(false);
        try
        {
            inflater.SetInput(dd);
            inflater.Inflate(rv);
            rLen = (int)inflater.TotalOut;
        }
        catch (Exception Ex)
        {
            List1.Items.Add(Ex.ToString());
            rLen = -1;
        }
        return rLen;
    }
    public void SaveRecvData()
    {

        string s1 = String.Format("C:\\C#_{0:X}.dat", hd.msgid);
        FileStream ss = new FileStream(s1, FileMode.Create);
        ss.Write(RecvBuffer, 0, hd.DePackSize);
        ss.Flush();
        ss.Close();
        List1.Items.Add(s1);
    }
    public Boolean Senddata(byte[] v, int Len)
    {
        Boolean bb = false;
        if (m_hSocket == null)
        {
            MessageBox.Show("网络尚未连接");
            return false;
        }

        try
        {
            m_hSocket.Send(v, Len, SocketFlags.None);
            bb = true;
        }
        catch (Exception Ex)
        {
            List1.Items.Add(Ex.ToString());
            bb = false;
        }
        return bb;
    }
    public Boolean Command(byte[] v, int Len)
    {
        if (Senddata(v, Len))
        {
            return RecvData();
        }
        else
            return false;
    }

    public Boolean RecvData()
    {
        string s1;
        int Len;
        if (m_hSocket == null)
        {
            MessageBox.Show("服务器未连接或者中断,请重新连接");
            return false;
        }
        try
        {
            Len = m_hSocket.Receive(DataHeader, 16, SocketFlags.None);
            if (Len != 16)
                return false;
            DataMs.Seek(0, SeekOrigin.Begin);
            hd.CheckSum = DataBr.ReadUInt32();
            if (hd.CheckSum != 7654321)
                return false;
            hd.EncodeMode = DataBr.ReadByte();
            DataMs.Seek(5, SeekOrigin.Current);
            hd.msgid = DataBr.ReadUInt16();
            hd.Size = DataBr.ReadUInt16();
            hd.DePackSize = DataBr.ReadUInt16();


            int elen = hd.Size;
            int fcur = 0;
            int Len1, min1 = 1024;
            while (fcur < elen)
            {
                min1 = Math.Min(1024, elen - fcur);
                Len1 = m_hSocket.Receive(buf, fcur, min1, SocketFlags.None);
                if (Len1 > 0)
                    fcur += Len1;
            }
            if (fcur != elen)
                return false;
            if ((hd.EncodeMode & 0x10) == 0x10)
            {
                int LL = Decompress(buf, hd.DePackSize, ref RecvBuffer);
                if (LL != hd.DePackSize)
                {
                    s1 = "解压出错:长度不同=depacksize=" + hd.DePackSize.ToString() + " 解压长度:=" + LL.ToString();
                    List1.Items.Add(s1);
                    return false;
                }
            }
            else
            {
                buf.CopyTo(RecvBuffer, 0);
            }
            int t = 0;
            switch (hd.msgid)
            {
                case 0x526:
                case 0x527: t = 0x39; break;
                case 0x551: t = 0x49; break;
                case 0x556: t = 0x69; break;
                case 0x56e:
                case 0x573: t = 0x77; break;
            }
            if (t > 0)
            {
                for (int i = 0; i < hd.DePackSize; i++)
                    RecvBuffer[i] = (byte)(RecvBuffer[i] ^ t);
            }
            RecvMs.Seek(0, SeekOrigin.Begin);
            return true;
        }
        catch (Exception e)
        {
            List1.Items.Add(e.ToString());
        }
        return false;

    }
    //解包数据
    int TDXDecode(byte[] buf, int start, ref int next)
    {
        int num, num3, num2, num4, num5, num6, num7, num8;
        byte cc;
        num = 0;
        num3 = 0;
        num2 = 0;
        while (num2 < 0x20)
        {
            cc = buf[start + num2];
            num4 = cc;
            num5 = (num4 & 0x80) / 0x80;
            if (num2 == 0)
            {
                num3 = 1 - (((num4 & 0x40) / 0x40) * 2);
                num6 = num4 & 0x3F;
                num = num + num6;
            }
            else if (num2 == 1)
            {
                num7 = (num4 & 0x7F) * (1 << (num2 * 6));
                num = num + num7;
            }
            else
            {
                num8 = (num4 & 0x7F) * (1 << (num2 * 7 - 1));
                num = num + num8;
            }
            if (num5 == 0)
            {
                num = num * num3;
                break;
            }
            num2++;
        }
        next = start + num2 + 1;
        return num;
    }
    //读取16位数据
    ushort TDXGetInt16(byte[] buf, int start, ref int next)
    {

        ushort Num = BitConverter.ToUInt16(buf, start);//buf[start+1]*256+buf[start];// (short *)&buf[start];
        next = start + 2;
        return Num;
    }
    //读取32位数据
    int TDXGetInt32(byte[] buf, int start, ref int next)
    {
        int Num = BitConverter.ToInt32(buf, start);//(long int *)&buf[start];
        next = start + 4;
        return Num;
    }
    //读取浮点数据float
    float TDXGetDouble(byte[] buf, int start, ref int next)
    {
        float d1 = BitConverter.ToSingle(buf, start);// (float*)&buf[start];
        next = start + 4;
        return d1;
    }
    //读取时间：HHMM
    int TDXGetTime(byte[] buf, int start, ref int next)
    {
        int i = (int)BitConverter.ToInt16(buf, start);
        next = start + 2;
        int ri, mm, ss;
        mm = (i / 60);
        ss = (i % 60);
        if (ss > 59)
        {
            ss = ss - 60;
            mm++;
        }
        ri = mm * 100 + ss;
        return ri;
    }
    //v 解包成年月日时分
    void TDXGetDate(int v, ref int yy, ref int mm, ref int dd, ref int hhh, ref int mmm)
    {
        yy = 2012;
        mm = 1;
        dd = 1;
        hhh = 9;
        mmm = 30;
        if (v > 21000000)
        {
            yy = 2004 + ((v & 0xF800) >> 11);
            int d1 = v & 0x7FF;
            mm = d1 / 100;
            dd = d1 % 100;
            int d2 = v >> 16;
            hhh = d2 / 60;
            mmm = d2 % 60;
        }
        else
        {
            yy = v / 10000;
            mm = (v - yy * 10000) / 100;
            dd = v % 100;
            hhh = 9;
            mmm = 30;
        }
    }
    public int gptime(byte mark, int v)
    {
        int i, h, m;
        i = v;
        if ((mark == 0) || (mark == 1))
        {
            i = v + 9 * 60 + 30;
            if (v > 120)
            {
                i = i + 90;
            }
        }
        if ((mark == 0x41) || (mark == 0x42) || (mark == 0x43))
        {
            i = v + 9 * 60;
            if (v > 75)
            {
                i = i + 15;
            };
            if (v > 135)
            {
                i = i + 120;
            }
        }
        if (mark == 0x47)
        {
            i = v + 9 * 60 + 15;
            if (v > 135)
            {
                i = i + 90;
            }
        }
        h = i / 60;
        m = i % 60;
        return h * 100 + m;
    }
    private void LinkButton_Click(object sender, EventArgs e)
    {
        if (m_hSocket != null)
        {
            MessageBox.Show("网络已连接");
            return;
        }
        Socket sk = null;
        Login[0] = 2;
        try
        {
            sk = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
            IPEndPoint localEndPoint = new IPEndPoint(IPAddress.Parse("61.152.168.228"), 7715);
            sk.Connect(localEndPoint);
            byte[] bb2 ={0x00,0x00,0x00,0x00,0x00,0x00,0x52,0x00,0x52,0x00,0xBA,0x26,0x77,0x8B,0x83,0xE5,
			0x87,0x10,0x66,0x01,0x01,0x3F,0x7E,0x73,0xF2,0x90,0xA4,0x4E,0x40,0x21,0x61,0x1F,
			0x17,0x1C,0x79,0x24,0x1F,0x40,0x71,0x5D,0x27,0xDD,0x36,0x19,0x1F,0x32,0xC6,0xE5,
			0xD5,0x3D,0xFB,0x41,0xD8,0xFA,0xC7,0x65,0x9C,0xBE,0x63,0xA5,0xFA,0x3D,0x23,0x2B,
			0x5E,0x6E,0x81,0xF3,0x5D,0x61,0x99,0x34,0x02,0x70,0x92,0x87,0x06,0x7F,0x67,0xBA,
			0xFD,0xA4,0x4B,0x47,0x2D,0x87,0x8B,0x00,0x3D,0xE3,0xBB,0xDB};
            sk.Send(bb2, bb2.Length, SocketFlags.None);

            int Len = sk.Receive(DataHeader, 16, SocketFlags.None);
            if (Len == 16)
            {

                DataMs.Seek(0, SeekOrigin.Begin);
                hd.CheckSum = DataBr.ReadUInt32();
                if (hd.CheckSum == 7654321)
                {
                    hd.EncodeMode = DataBr.ReadByte();
                    DataMs.Seek(5, SeekOrigin.Current);
                    hd.msgid = DataBr.ReadUInt16();
                    hd.Size = DataBr.ReadUInt16();
                    hd.DePackSize = DataBr.ReadUInt16();

                    int elen = hd.Size;
                    int fcur = 0;
                    int Len1, min1 = 1024;
                    while (fcur < elen)
                    {
                        min1 = Math.Min(1024, elen - fcur);
                        Len1 = sk.Receive(buf, fcur, min1, SocketFlags.None);
                        if (Len1 > 0)
                            fcur += Len1;
                    }
                    if (fcur == elen)
                    {

                        if ((hd.EncodeMode & 0x10) == 0x10)
                        {
                            int LL = Decompress(buf, hd.DePackSize, ref RecvBuffer);
                            if (LL == hd.DePackSize)
                            {
                                RecvMs.Seek(0, SeekOrigin.Begin);
                                Login = RecvBr.ReadBytes(hd.DePackSize);
                                List1.Items.Add("帐号成功登录!");
                            }
                        }
                    }
                }
            }

            sk.Close();
        }
        catch (Exception Ex)
        {
            List1.Items.Add("连接验证服务器失败:" + Ex.ToString());
            Application.DoEvents();
            return;
        }
        if (Login[0] == 0)
        {
            List1.Items.Add("帐号过期!");
            Application.DoEvents();
            return;
        }

        if (Login[0] != 1)
        {
            List1.Items.Add("服务器出错!");
            Application.DoEvents();
            return;
        }

        string s1;
        rc = false;
        if (op1.Checked)
            rc = Connect("119.147.86.172", 443);
        if (op2.Checked)
            rc = Connect("123.129.245.202", 80);
        if (rc)
        {
            List1.Items.Add("成功连接服务器");
            Application.DoEvents();
            byte[] bb1 ={0x0C,0x01,0x18,0x7B,0x00,0x01,0x20,0x00,0x20,0x00,0x1E,0x00,0x00,0x00,0x00,0x00,
			0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
			0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
            string username = "cfqs0292568";
            string password = "53839081";
            byte[] b = Encoding.Default.GetBytes(username);
            b.CopyTo(bb1, 12);
            b = Encoding.Default.GetBytes(password);
            b.CopyTo(bb1, 25);
            if (Command(bb1, bb1.Length))
            {

                List1.Items.Add("帐号成功登录行情服务器");
            }
            else
            {
                List1.Items.Add("帐号登录错误");
                return;
            }

            byte[] a = { 0xC, 0x2, 0x18, 0x93, 0x0, 0x1, 0x3, 0x0, 0x3, 0x0, 0xD, 0x0, 0x1 };
            a[12] = Login[2741];
            if (Command(a, a.Length))
            {
                s1 = "成功登录服务器";
                List1.Items.Add(s1);
                RecvMs.Seek(42, SeekOrigin.Begin);
                DateTime1 = RecvBr.ReadUInt32();
                s1 = "日期:" + DateTime1.ToString();
                List1.Items.Add(s1);
                Application.DoEvents();
            }
            //Button20_Click(sender, e)
        }
        else
            List1.Items.Add("登录失败,稍候再试");
        Application.DoEvents();
    }

    private void Form1_FormClosed(object sender, FormClosedEventArgs e)
    {
        if (m_hSocket != null)
            m_hSocket.Close();
    }
    //取股票代码
    private void Button20_Click(object sender, EventArgs e)
    {
        //string s2;
        string s1, names, codes;
        byte[] code = new byte[6];
        byte[] name = new byte[8];
        int i, j, k, count, n;

        if (m_hSocket == null)
        {
            MessageBox.Show("网络尚未连接");
            return;
        }
        if (stkcount > 0)
        {
            MessageBox.Show("代码已经初始化");
            return;
        }
        s1 = "初始化深圳股票代码";
        List1.Items.Add(s1);
        Application.DoEvents();

        byte[] a1 = { 0xC, 0xC, 0x18, 0x6C, 0x0, 0x1, 0x8, 0x0, 0x8, 0x0, 0x4E, 0x4, 0x0, 0x0, 0x1, 0x2, 0x3, 0x4 };
        byte[] b1;
        b1 = BitConverter.GetBytes(DateTime1);
        a1[14] = b1[0];
        a1[15] = b1[1];
        a1[16] = b1[2];
        a1[17] = b1[3];
        if (Command(a1, a1.Length))
        {
            count = RecvBr.ReadUInt16();
            s1 = "深圳股票数量:" + count.ToString();
            List1.Items.Add(s1);
            Application.DoEvents();
            i = 0;
            while (i < count)
            {
                byte[] a2 = { 0xC, 0x1, 0x18, 0x64, 0x1, 0x1, 0x6, 0x0, 0x6, 0x0, 0x50, 0x4, 0x0, 0x0, 0xF2, 0xF3 };
                byte[] b2;
                b2 = BitConverter.GetBytes(i);
                a2[14] = b2[0];
                a2[15] = b2[1];
                if (Command(a2, a2.Length))
                {
                    if (DEBUG == 1) SaveRecvData();

                    n = RecvBr.ReadUInt16();

                    for (j = 0; j < n; j++)
                    {
                        GP[stkcount].code = RecvBr.ReadBytes(6);
                        GP[stkcount].rate = RecvBr.ReadUInt16();
                        GP[stkcount].name = RecvBr.ReadBytes(8);
                        for (k = 0; k < 8; k++)
                        {
                            if (GP[stkcount].name[k] == 0)
                                GP[stkcount].name[k] = 32;
                        }
                        names = System.Text.Encoding.GetEncoding("GB2312").GetString(GP[stkcount].name);
                        codes = System.Text.Encoding.GetEncoding("GB2312").GetString(GP[stkcount].code);
                        GP[stkcount].w1 = RecvBr.ReadUInt16();
                        GP[stkcount].w2 = RecvBr.ReadUInt16();
                        GP[stkcount].PriceMag = RecvBr.ReadByte();
                        GP[stkcount].YClose = RecvBr.ReadSingle();
                        GP[stkcount].w3 = RecvBr.ReadUInt16();
                        GP[stkcount].w4 = RecvBr.ReadUInt16();
                        GP[stkcount].mark = 0;
                        GP[stkcount].no = Convert.ToInt32(codes); 
                        //s2 = String.Format("{0:F2}", GP[stkcount].YClose);
                        //s1 = stkcount.ToString() + ":" + codes + " " + names + " 昨收=" + s2;
                        //List1.Items.Add(s1);
                        s1 = codes.ToUpper() + ":" + names;
                        cb.Items.Add(s1);
                        if (codes.Equals("159901"))
                            cb.SelectedIndex = stkcount;
                        Application.DoEvents();
                        stkcount = stkcount + 1;
                    }
                    i = i + n;
                }
                else
                    break;
            }
        }
        if (cb.SelectedIndex == -1)
            cb.SelectedIndex = 0;
        s1 = "深圳股票代码完成";
        List1.Items.Add(s1);
        s1 = "初始化上海股票代码";
        List1.Items.Add(s1);
        Application.DoEvents();

        byte[] a3 = { 0xC, 0xC, 0x18, 0x6C, 0x0, 0x1, 0x8, 0x0, 0x8, 0x0, 0x4E, 0x4, 0x1, 0x0, 0x1, 0x2, 0x3, 0x4 };
        byte[] b3 = BitConverter.GetBytes(DateTime1);
        a3[14] = b3[0];
        a3[15] = b3[1];
        a3[16] = b3[2];
        a3[17] = b3[3];
        if (Command(a3, a3.Length))
        {
            count = RecvBr.ReadUInt16();
            s1 = "上海股票数量:" + count.ToString();
            List1.Items.Add(s1);
            Application.DoEvents();

            i = 0;
            while (i < count)
            {
                byte[] a2 = { 0xC, 0x1, 0x18, 0x64, 0x1, 0x1, 0x6, 0x0, 0x6, 0x0, 0x50, 0x4, 0x1, 0x0, 0xF2, 0xF3 };
                byte[] b2;
                b2 = BitConverter.GetBytes(i);
                a2[14] = b2[0];
                a2[15] = b2[1];
                if (Command(a2, a2.Length))
                {
                    if (DEBUG == 1) SaveRecvData();

                    n = RecvBr.ReadUInt16();

                    for (j = 0; j < n; j++)
                    {
                        GP[stkcount].code = RecvBr.ReadBytes(6);
                        GP[stkcount].rate = RecvBr.ReadUInt16();
                        GP[stkcount].name = RecvBr.ReadBytes(8);
                        for (k = 0; k < 8; k++)
                        {
                            if (GP[stkcount].name[k] == 0)
                                GP[stkcount].name[k] = 32;
                        }
                        names = System.Text.Encoding.GetEncoding("GB2312").GetString(GP[stkcount].name);
                        codes = System.Text.Encoding.GetEncoding("GB2312").GetString(GP[stkcount].code);
                        GP[stkcount].w1 = RecvBr.ReadUInt16();
                        GP[stkcount].w2 = RecvBr.ReadUInt16();
                        GP[stkcount].PriceMag = RecvBr.ReadByte();
                        GP[stkcount].YClose = RecvBr.ReadSingle();
                        GP[stkcount].w3 = RecvBr.ReadUInt16();
                        GP[stkcount].w4 = RecvBr.ReadUInt16();
                        GP[stkcount].mark = 1;
                        GP[stkcount].no =100000+ Convert.ToInt32(codes); 
                        s1 = codes.ToUpper() + ":" + names;
                        cb.Items.Add(s1);
                        Application.DoEvents();

                        stkcount = stkcount + 1;
                    }
                    i = i + n;
                }
                else
                    break;
            }
        }
        s1 = "上海股票代码完成";
        List1.Items.Add(s1);
        Application.DoEvents();

        if (cb.SelectedIndex == -1)
        {
            if (cb.Items.Count > 0)
                cb.SelectedIndex = 0;
        }

    }
    //下载日线 StkCode=股票代码 mark=市场(0=深圳 1=上海) start=为开始值(0表示最后一个周期） Count=周期数(最大800) zq=周期 取数据从后往前，类似于读文件从后往前的方式
    //取各周期数据
    public Boolean GetDayLine(byte[] StkCode, byte Fmark, int ZhouQi, int start, int Count)
    {
        string s1, codes;
        byte[] code = new byte[6];
        byte[] name = new byte[8];
        int i, j, n;

        if (m_hSocket == null)
        {
            MessageBox.Show("网络尚未连接");
            return false;
        }
        codes = System.Text.Encoding.GetEncoding("GB2312").GetString(StkCode);
        int CodeNum = Convert.ToInt32(codes);
        Boolean zs = false;
        if ((Fmark == 0) && (CodeNum >= 390000) && (CodeNum < 400000))
            zs = true;
        if ((Fmark == 1) && (CodeNum >= 930000) && (CodeNum < 999999))
            zs = true;
        if ((Fmark == 1) && (CodeNum >= 1) && (CodeNum < 1000))
            zs = true;
        byte[] a = { 0xC, 0x1, 0x8, 0x64, 0x1, 0x1, 0x12, 0x0, 0x12, 0x0, 0x29, 0x5, 0xFF, 0x0, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0xFE, 0x0, 0x1, 0x0, 0xF1, 0xF2, 0xE1, 0xE2 };
        a[12] = Fmark;
        a[20] = (byte)ZhouQi;
        byte[] sb = BitConverter.GetBytes(start);
        a[24] = sb[0];
        a[25] = sb[1];
        byte[] sb1 = BitConverter.GetBytes(Count);
        a[26] = sb1[0];
        a[27] = sb1[1];
        StkCode.CopyTo(a, 14);
        if (Command(a, a.Length))
        {
            if (DEBUG == 1) SaveRecvData();
            n = RecvBr.ReadUInt16();
            if (n > 0)
            {
                int yy1 = 0, mm1 = 0, dd1 = 0, hhh = 0, mmm = 0;
                double open, high, low, close, vol, amount;
                double upcount, downcount;
                int num6, num7, num8, num9, num10;
                int date1, time1;
                num9 = 0;
                i = 2;
                List1.Items.Clear();
                List1.BeginUpdate();
                for (j = 0; j < n; j++)
                {
                    TDXGetDate(TDXGetInt32(RecvBuffer, i, ref i), ref yy1, ref mm1, ref dd1, ref hhh, ref mmm);
                    date1 = yy1 * 10000 + mm1 * 100 + dd1;
                    time1 = hhh * 100 + mmm;
                    num10 = TDXDecode(RecvBuffer, i, ref i);
                    open = (num9 + num10) / 1000.0;

                    num6 = TDXDecode(RecvBuffer, i, ref i);
                    close = (num9 + num10 + num6) / 1000.0;
                    num7 = TDXDecode(RecvBuffer, i, ref i);
                    high = (num9 + num10 + num7) / 1000.0;
                    num8 = TDXDecode(RecvBuffer, i, ref i);
                    low = (num9 + num10 + num8) / 1000.0;
                    vol = TDXDecode(RecvBuffer, i, ref i);
                    amount = TDXGetDouble(RecvBuffer, i, ref i);
                    num9 = num9 + num10 + num6;
                    s1 = date1.ToString() + ":" + time1.ToString();
                    s1 = s1 + " 开盘:" + String.Format("{0:F2}", open);
                    s1 = s1 + " 最高:" + String.Format("{0:F2}", high);
                    s1 = s1 + " 最低:" + String.Format("{0:F2}", low);
                    s1 = s1 + " 收盘:" + String.Format("{0:F2}", close);
                    s1 = s1 + " 成交量:" + String.Format("{0:F0}", vol);
                    s1 = s1 + " 成交额:" + String.Format("{0:F0}", amount);
                    List1.Items.Add(s1);
                    if (zs)
                    {
                        upcount = TDXGetInt16(RecvBuffer, i, ref i);
                        downcount = TDXGetInt16(RecvBuffer, i, ref i);
                    }
                }
                List1.EndUpdate();

            }
            return true;
        }
        return false;

    }
    private void Button10_Click(object sender, EventArgs e)
    {
        Button bt = (Button)sender;
        if (m_hSocket == null)
        {
            MessageBox.Show("网络尚未连接");
            return;
        }
        int k = cb.SelectedIndex;
        if (k == -1)
        {
            MessageBox.Show("请先初始化代码");
            return;
        }
        String t1 = bt.Tag.ToString();
        int zq = Convert.ToInt32(t1);
        rc = GetDayLine(GP[k].code, GP[k].mark, zq, 0, 25);

    }
    //取分笔数据
    public Boolean GetFenBiLine(byte[] StkCode, byte Fmark, int start, int Count)
    {
        string s1;
        int n, i, j;
        byte[] a = { 0xC, 0x17, 0x8, 0x1, 0x1, 0x1, 0xE, 0x0, 0xE, 0x0, 0xC5, 0xF, 0xFF, 0x0, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0xF1, 0xF2, 0xE1, 0xE2 };
        a[12] = Fmark;
        StkCode.CopyTo(a, 14);

        byte[] sb = BitConverter.GetBytes(start);
        a[20] = sb[0];
        a[21] = sb[1];
        byte[] sb1 = BitConverter.GetBytes(Count);
        a[22] = sb1[0];
        a[23] = sb1[1];
        if (Command(a, a.Length))
        {
            n = RecvBr.ReadUInt16();
            if (n > 0)
            {
                double close, vol;
                int time1;
                int num4, num5, dealcount, sellorbuy;
                num4 = 0;
                i = 2;
                List1.Items.Clear();
                List1.BeginUpdate();
                for (j = 0; j < n; j++)
                {
                    time1 = TDXGetTime(RecvBuffer, i, ref i);
                    num5 = TDXDecode(RecvBuffer, i, ref i);
                    close = (num4 + num5) / 100.0;
                    vol = TDXDecode(RecvBuffer, i, ref i);
                    dealcount = TDXDecode(RecvBuffer, i, ref i);
                    sellorbuy = TDXDecode(RecvBuffer, i, ref i);
                    num4 = num5 + num4;
                    i = i + 1;
                    s1 = time1.ToString();
                    s1 = s1 + " 价格:" + String.Format("{0:F2}", close);
                    s1 = s1 + " 成交量:" + String.Format("{0:F2}", vol);
                    s1 = s1 + " 单笔量:" + String.Format("{0:F2}", dealcount);
                    s1 = s1 + " 买卖标志:";
                    if (sellorbuy == 1)
                        s1 = s1 + " B";
                    if (sellorbuy == 0)
                        s1 = s1 + " S";
                    List1.Items.Add(s1);
                }
                List1.EndUpdate();
            }
            return true;
        }
        else
            return false;
    }
    //取分笔
    private void Button18_Click(object sender, EventArgs e)
    {
        if (m_hSocket == null)
        {
            MessageBox.Show("网络尚未连接");
            return;
        }
        int k = cb.SelectedIndex;
        if (k == -1)
        {
            MessageBox.Show("请先初始化代码");
            return;
        }
        rc = GetFenBiLine(GP[k].code, GP[k].mark, 0, 18);
    }
    //取报价
    private void Button110_Click(object sender, EventArgs e)
    {
        if (m_hSocket == null)
        {
            MessageBox.Show("网络尚未连接");
            return;
        }
        int k = cb.SelectedIndex;
        if (k == -1)
        {
            MessageBox.Show("请先初始化代码");
            return;
        }

        string s1;
        byte[] code = new byte[6];
        int n, i, j;
        byte[] a = { 0xC, 0x1, 0x20, 0x63, 0x0, 0x2, 0x13, 0x0, 0x13, 0x0, 0x3E, 0x5, 0x5, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x1, 0x0, 0x0, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36 };
        a[22] = GP[k].mark;
        GP[k].code.CopyTo(a, 23);
        if (Command(a, a.Length))
        {
            RecvBr.ReadUInt16();
            n = RecvBr.ReadUInt16();
            if (n > 0)
            {
                i = 4;
                List1.Items.Clear();
                List1.BeginUpdate();
                int dd,t=0;

                for (j = 0; j < n; j++)
                {
                    byte m = RecvBuffer[i];
                    Array.Copy(RecvBuffer, i + 1, code, 0, 6);
                    i = i + 7;
                    dd =TDXGetInt16(RecvBuffer, i, ref i);
                    List1.Items.Add("d1:" + dd.ToString());

                    double prize = TDXDecode(RecvBuffer, i, ref i) / 100.0;
                    double last = prize + (((double)TDXDecode(RecvBuffer, i, ref i)) / 100.0);
                    double open = prize + (((double)TDXDecode(RecvBuffer, i, ref i)) / 100);
                    double high = prize + (((double)TDXDecode(RecvBuffer, i, ref i)) / 100);
                    double low = prize + (((double)TDXDecode(RecvBuffer, i, ref i)) / 100);
                    int Time = TDXDecode(RecvBuffer, i, ref i);
                    dd = TDXDecode(RecvBuffer, i, ref i);
                    List1.Items.Add("d0:" + dd.ToString()); ;
                    int volume = TDXDecode(RecvBuffer, i, ref i);
                    int tradeQTY = TDXDecode(RecvBuffer, i, ref i);// '现量;
                    double amount = TDXGetDouble(RecvBuffer, i, ref i);
                    int b = TDXDecode(RecvBuffer, i, ref i);
                    int s = TDXDecode(RecvBuffer, i, ref i);

                    dd = TDXDecode(RecvBuffer, i, ref i);
                    List1.Items.Add("d1:" + dd.ToString()); ;
                    dd = TDXDecode(RecvBuffer, i, ref i);
                    List1.Items.Add("d2:" + dd.ToString()); ;

                    double buy1 = prize + (((double)TDXDecode(RecvBuffer, i, ref i)) / 100);
                    double sell1 = prize + (((double)TDXDecode(RecvBuffer, i, ref i)) / 100);
                    int buyQTY1 = TDXDecode(RecvBuffer, i, ref  i);
                    int sellQTY1 = TDXDecode(RecvBuffer, i, ref  i);

                    double buy2 = prize + (((double)TDXDecode(RecvBuffer, i, ref i)) / 100);
                    double sell2 = prize + (((double)TDXDecode(RecvBuffer, i, ref i)) / 100);
                    int buyQTY2 = TDXDecode(RecvBuffer, i, ref  i);
                    int sellQTY2 = TDXDecode(RecvBuffer, i, ref  i);

                    double buy3 = prize + (((double)TDXDecode(RecvBuffer, i, ref i)) / 100);
                    double sell3 = prize + (((double)TDXDecode(RecvBuffer, i, ref i)) / 100);
                    int buyQTY3 = TDXDecode(RecvBuffer, i, ref  i);
                    int sellQTY3 = TDXDecode(RecvBuffer, i, ref  i);
                    double buy4 = prize + (((double)TDXDecode(RecvBuffer, i, ref i)) / 100);
                    double sell4 = prize + (((double)TDXDecode(RecvBuffer, i, ref i)) / 100);
                    int buyQTY4 = TDXDecode(RecvBuffer, i, ref  i);
                    int sellQTY4 = TDXDecode(RecvBuffer, i, ref  i);
                    double buy5 = prize + (((double)TDXDecode(RecvBuffer, i, ref i)) / 100);
                    double sell5 = prize + (((double)TDXDecode(RecvBuffer, i, ref i)) / 100);
                    int buyQTY5 = TDXDecode(RecvBuffer, i, ref  i);
                    int sellQTY5 = TDXDecode(RecvBuffer, i, ref  i);
                    dd = TDXDecode(RecvBuffer, i, ref t);
                    List1.Items.Add("d21:" + dd.ToString()+" i:="+i.ToString()+" t="+t.ToString());
                    dd = TDXDecode(RecvBuffer,t, ref t);
                    List1.Items.Add("d21:" + dd.ToString() + " i:=" + i.ToString() + " t=" + t.ToString());
                    dd = TDXDecode(RecvBuffer, t, ref t);
                    List1.Items.Add("d21:" + dd.ToString() + " i:=" + i.ToString() + " t=" + t.ToString());
                    i = i + 3;
                    dd = TDXDecode(RecvBuffer, i, ref i);
                    List1.Items.Add("d3:" + dd.ToString()); ;
                    dd = TDXDecode(RecvBuffer, i, ref i);
                    List1.Items.Add("d4:" + dd.ToString()); ;
                    dd = TDXDecode(RecvBuffer, i, ref i);
                    List1.Items.Add("d5:" + dd.ToString()); ;
                    double speed = TDXGetInt16(RecvBuffer, i, ref i) / 100;
                    TDXGetInt16(RecvBuffer, i, ref i);

                    s1 = GP[k].PriceMag.ToString()+ " 时间:" + Time.ToString() + " 代码:" + System.Text.Encoding.GetEncoding("GB2312").GetString(code);
                    List1.Items.Add(s1);
                    s1 = "──────────────";
                    List1.Items.Add(s1); ;
                    s1 = " 卖五:" + String.Format("{0:F4}", sell5) + "  " + String.Format("{0:d2}", sellQTY5);
                    List1.Items.Add(s1); ;
                    s1 = " 卖四:" + String.Format("{0:F4}", sell4) + "  " + String.Format("{0:d2}", sellQTY4);
                    List1.Items.Add(s1); ;
                    s1 = " 卖三:" + String.Format("{0:F4}", sell3) + "  " + String.Format("{0:d2}", sellQTY3);
                    List1.Items.Add(s1); ;
                    s1 = " 卖二:" + String.Format("{0:F4}", sell2) + "  " + String.Format("{0:d2}", sellQTY2);
                    List1.Items.Add(s1); ;
                    s1 = " 卖一:" + String.Format("{0:F4}", sell1) + "  " + String.Format("{0:d2}", sellQTY1);
                    List1.Items.Add(s1); ;
                    s1 = "──────────────";
                    List1.Items.Add(s1); ;
                    s1 = " 买一:" + String.Format("{0:F4}", buy1) + "  " + String.Format("{0:d2}", buyQTY1);
                    List1.Items.Add(s1); ;
                    s1 = " 买二:" + String.Format("{0:F4}", buy2) + "  " + String.Format("{0:d2}", buyQTY2);
                    List1.Items.Add(s1); ;
                    s1 = " 买三:" + String.Format("{0:F4}", buy3) + "  " + String.Format("{0:d2}", buyQTY3);
                    List1.Items.Add(s1); ;
                    s1 = " 买四:" + String.Format("{0:F4}", buy4) + "  " + String.Format("{0:d2}", buyQTY4);
                    List1.Items.Add(s1); ;
                    s1 = " 买五:" + String.Format("{0:F4}", buy5) + "  " + String.Format("{0:d2}", buyQTY5);
                    List1.Items.Add(s1); ;
                    s1 = "──────────────";
                    List1.Items.Add(s1); ;

                    s1 = " 昨收:" + String.Format("{0:F4}", last);
                    List1.Items.Add(s1); ;
                    s1 = " 今开:" + String.Format("{0:F4}", open);
                    List1.Items.Add(s1); ;
                    s1 = " 最高:" + String.Format("{0:F4}", high);
                    List1.Items.Add(s1); ;
                    s1 = " 最低:" + String.Format("{0:F4}", low);
                    List1.Items.Add(s1); ;
                    s1 = " 收盘:" + String.Format("{0:F4}", prize);
                    List1.Items.Add(s1); ;
                    ; s1 = " 成交量:" + String.Format("{0:F4}", volume);
                    List1.Items.Add(s1); ;
                    s1 = " 成交额:" + String.Format("{0:F4}", amount);
                    List1.Items.Add(s1); ;
                    s1 = " 现量:" + String.Format("{0:F4}", tradeQTY);
                    List1.Items.Add(s1); ;
                    s1 = " 买盘:" + String.Format("{0:F4}", b);
                    List1.Items.Add(s1); ;
                    s1 = " 卖盘:" + String.Format("{0:F4}", s);
                    List1.Items.Add(s1);
                }
                List1.EndUpdate();
            }
        }
    }
    public void LoadFile(string fn)
    {
        FileStream ss = new FileStream(fn, FileMode.Open);
        long Len = ss.Seek(0, SeekOrigin.End);
        ss.Seek(0, SeekOrigin.Begin);
        ss.Read(RecvBuffer, 0, (int)Len);
        ss.Close();
        RecvMs.Seek(0, SeekOrigin.Begin);
    }
    //取当日分时线
    private void Button111_Click(object sender, EventArgs e)
    {
        byte[] code = new byte[6];
        int n, i, j;
        string s1;
        if (m_hSocket == null)
        {
            MessageBox.Show("网络尚未连接");
            return;
        }
        int k = cb.SelectedIndex;
        if (k == -1)
        {
            MessageBox.Show("请先初始化代码");
            return;
        }

        byte[] a = { 0xC, 0x1B, 0x8, 0x0, 0x1, 0x1, 0xE, 0x0, 0xE, 0x0, 0x1D, 0x5, 0xFF, 0x0, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x0, 0x0, 0x0, 0x0 };
        a[12] = GP[k].mark;
        GP[k].code.CopyTo(a, 14);
        if (Command(a, a.Length))
        {
            if (DEBUG == 1) SaveRecvData();
            n = RecvBr.ReadUInt16();
            if (n > 0)
            {
                i = 4;
                int num4;
                double close, vol, res;
                num4 = 0;
                List1.Items.Clear();
                List1.BeginUpdate();
                for (j = 0; j < n; j++)
                {
                    num4 = num4 + TDXDecode(RecvBuffer, i, ref i);
                    close = (double)(num4 / 100.0);
                    res = TDXDecode(RecvBuffer, i, ref i);
                    vol = TDXDecode(RecvBuffer, i, ref i);
                    s1 = gptime(0, j).ToString();
                    s1 = s1 + " 价格:" + String.Format("{0:F2}", close);
                    s1 = s1 + " 成交量:" + String.Format("{0:F2}", vol);
                    List1.Items.Add(s1);
                }
                List1.EndUpdate();
            }
        }

    }
    //历史分时
    private void Button6_Click(object sender, EventArgs e)
    {
        byte[] code = new byte[6];
        int n, i, j;
        string s1;
        if (m_hSocket == null)
        {
            MessageBox.Show("网络尚未连接");
            return;
        }
        int k = cb.SelectedIndex;
        if (k == -1)
        {
            MessageBox.Show("请先初始化代码");
            return;
        }
        int HisDate = 20130128;
        byte[] a = { 0xC, 0x1, 0x30, 0x0, 0x1, 0x1, 0xD, 0x0, 0xD, 0x0, 0xB4, 0xF, 0xD1, 0xD2, 0xD3, 0xD4, 0xFF, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36 };
        byte[] sb = BitConverter.GetBytes(HisDate);
        a[12] = sb[0];
        a[13] = sb[1];
        a[14] = sb[2];
        a[15] = sb[3];
        a[16] = GP[k].mark;
        GP[k].code.CopyTo(a, 17);
        if (Command(a, a.Length))
        {
            if (DEBUG == 1) SaveRecvData();
            n = RecvBr.ReadUInt16();

            if (n > 0)
            {
                i = 6;
                int num4;
                double close, vol, res;
                num4 = 0;
                List1.Items.Clear();
                List1.BeginUpdate();
                for (j = 0; j < n; j++)
                {
                    num4 = num4 + TDXDecode(RecvBuffer, i, ref i);
                    close = (double)(num4 / 100.0);
                    res = TDXDecode(RecvBuffer, i, ref i);
                    vol = TDXDecode(RecvBuffer, i, ref i);
                    s1 = gptime(0, j).ToString();
                    s1 = s1 + " 价格:" + String.Format("{0:F2}", close);
                    s1 = s1 + " 成交量:" + String.Format("{0:F2}", vol);
                    List1.Items.Add(s1);
                }
                List1.EndUpdate();
            }
        }
    }
    //历史分笔
    private void Button5_Click(object sender, EventArgs e)
    {
        byte[] code = new byte[6];
        int n, i, j;
        string s1;

        if (m_hSocket == null)
        {
            MessageBox.Show("网络尚未连接");
            return;
        }
        int k = cb.SelectedIndex;
        if (k == -1)
        {
            MessageBox.Show("请先初始化代码");
            return;
        }
        int HisDate = 20130128;
        int start = 0;
        int Count = 25;
        byte[] a = { 0xC, 0x1, 0x30, 0x0, 0x1, 0x1, 0x12, 0x0, 0x12, 0x0, 0xB5, 0xF, 0xD1, 0xD2, 0xD3, 0xD4, 0xFF, 0x0, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0xF1, 0xF2, 0xE1, 0xE2 };
        byte[] sb = BitConverter.GetBytes(HisDate);
        a[12] = sb[0];
        a[13] = sb[1];
        a[14] = sb[2];
        a[15] = sb[3];
        a[16] = GP[k].mark;
        GP[k].code.CopyTo(a, 18);

        byte[] sb2 = BitConverter.GetBytes(start);
        a[24] = sb2[0];
        a[25] = sb2[1];
        byte[] sb1 = BitConverter.GetBytes(Count);
        a[26] = sb1[0];
        a[27] = sb1[1];
        if (Command(a, a.Length))
        {
            if (DEBUG == 1) SaveRecvData();
            n = RecvBr.ReadUInt16();

            if (n > 0)
            {
                i = 6;
                double close, vol;
                int time1;
                int num4, num5, sellorbuy;
                num4 = 0;
                List1.Items.Clear();
                List1.BeginUpdate();
                for (j = 0; j < n; j++)
                {
                    time1 = TDXGetTime(RecvBuffer, i, ref i);
                    num5 = TDXDecode(RecvBuffer, i, ref i);
                    close = (double)((num4 + num5) / 100.0);
                    vol = TDXDecode(RecvBuffer, i, ref i);
                    sellorbuy = TDXDecode(RecvBuffer, i, ref i);
                    num4 = num5 + num4;
                    i = i + 1;
                    s1 = time1.ToString();
                    s1 = s1 + " 价格:" + String.Format("{0:F2}", close);
                    s1 = s1 + " 成交量:" + String.Format("{0:F2}", vol);
                    s1 = s1 + " 买卖标志:";
                    if (sellorbuy == 1)
                        s1 = s1 + " B";
                    if (sellorbuy == 0)
                        s1 = s1 + " S";
                    List1.Items.Add(s1);
                }
                List1.EndUpdate();
            }
        }
    }
    //权息
    private void Button8_Click(object sender, EventArgs e)
    {
        string codes, names;
        byte[] code = new byte[6];
        byte[] name = new byte[8];
        int n, i, j;
        string s1;
        if (m_hSocket == null)
        {
            MessageBox.Show("网络尚未连接");
            return;
        }
        int k = cb.SelectedIndex;
        if (k == -1)
        {
            MessageBox.Show("请先初始化代码");
            return;
        }
        byte[] a = { 0xC, 0xF, 0x10, 0x9B, 0x0, 0x1, 0xE, 0x0, 0xE, 0x0, 0xCF, 0x2, 0x0, 0xFF, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x0, 0x0, 0x0, 0x0 };
        a[13] = GP[k].mark;
        GP[k].code.CopyTo(a, 14);
        if (Command(a, a.Length))
        {
            if (DEBUG == 1) SaveRecvData();
            n = RecvBr.ReadUInt16();

            if (n > 0)
            {
                i = 2;
                List1.Items.Clear();
                List1.BeginUpdate();
                for (j = 0; j < n; j++)
                {
                    Array.Copy(RecvBuffer, i, name, 0, 8);
                    i = i + 64;
                    Array.Copy(RecvBuffer, i, code, 0, 6);
                    i = i + 80;
                    int start = TDXGetInt32(RecvBuffer, i, ref i);
                    int Len = TDXGetInt32(RecvBuffer, i, ref i);
                    names = System.Text.Encoding.GetEncoding("GB2312").GetString(name);
                    codes = System.Text.Encoding.GetEncoding("GB2312").GetString(code);
                    s1 = (j + 1).ToString() + " 名称:" + names;
                    s1 = s1 + " 代码:" + codes;
                    s1 = s1 + " start:" + start.ToString();
                    s1 = s1 + " Len:" + Len.ToString();
                    info[j].name = names;
                    info[j].code = codes;
                    info[j].start = start;
                    info[j].len = Len;
                    List1.Items.Add(s1);
                }
                List1.EndUpdate();
            }
        }
    }
    //分类资料
    private void Button7_Click(object sender, EventArgs e)
    {
        string codes;
        byte[] code = new byte[6];
        byte[] name = new byte[8];
        int i;
        string s1;

        if (m_hSocket == null)
        {
            MessageBox.Show("网络尚未连接");
            return;
        }
        int k = cb.SelectedIndex;
        if (k == -1)
        {
            MessageBox.Show("请先初始化代码");
            return;
        }
        int Style = 0;//资料分类 -序号0~15
        codes = System.Text.Encoding.GetEncoding("GB2312").GetString(GP[k].code);
        if (codes.Equals(info[Style].code) == false)
        {
            MessageBox.Show("请先取得资料分类");
            return;
        }

        byte[] a = {0xC, 0x7, 0x10, 0x9C, 0x0, 0x1, 0x68, 0x0, 0x68, 0x0, 0xD0, 0x2, 0x0, 0xFF, 0x31, 0x32,
               0x33, 0x34, 0x35, 0x36, 0xAA, 0x0, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x2E, 0x74, 0x78, 0x74,
               0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 
               0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
               0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
               0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
               0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD7, 0xD8, 0x0, 0x0, 0x0, 0x0, 0x0};
        a[13] = GP[k].mark;
        a[20] = (Byte)Style; //资料分类 -序号0~15
        GP[k].code.CopyTo(a, 14);
        GP[k].code.CopyTo(a, 22);
        byte[] sb = BitConverter.GetBytes(info[Style].start);
        a[102] = sb[0];
        a[103] = sb[1];
        a[104] = sb[2];
        a[105] = sb[3];
        byte[] sb1 = BitConverter.GetBytes(info[Style].len);
        a[106] = sb1[0];
        a[107] = sb1[1];
        a[108] = sb1[2];
        a[109] = sb1[3];
        if (Command(a, a.Length))
        {
            if (DEBUG == 1) SaveRecvData();
            RecvMs.Seek(12, SeekOrigin.Begin);
            int t = hd.DePackSize - 12;
            byte[] txt = new byte[t];
            txt = RecvBr.ReadBytes(t);
            s1 = System.Text.Encoding.GetEncoding("GB2312").GetString(txt);// ' GetStr(text, 0, t)
            string[] sss;
            sss = s1.Split(new char[2] { '\xd', '\xa' });
            List1.Items.Clear();
            List1.BeginUpdate();
            for (i = 0; i < sss.Length; i++)
                List1.Items.Add(sss[i]);
            List1.EndUpdate();
        }
    }
    //权息
    private void button1_Click(object sender, EventArgs e)
    {
        string codes;
        byte[] code = new byte[6];
        byte[] name = new byte[8];
        int n, i, j, k;
        string s1;
        if (m_hSocket == null)
        {
            MessageBox.Show("网络尚未连接");
            return;
        }
        k = cb.SelectedIndex;
        if (k == -1)
        {
            MessageBox.Show("请先初始化代码");
            return;
        }
        byte[] a = { 0xC, 0x1F, 0x18, 0x76, 0x0, 0x1, 0xB, 0x0, 0xB, 0x0, 0xF, 0x0, 0xDD, 0x0, 0xFF, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36 };
        a[12] = 1;//数量
        a[14] = GP[k].mark;
        GP[k].code.CopyTo(a, 15);
        if (Command(a, a.Length))
        {
            if (DEBUG == 1) SaveRecvData();
            n = RecvBr.ReadUInt16();
            if (n > 0)
            {
                i = 2;
                int num5;
                List1.Items.Clear();
                List1.BeginUpdate();
                for (j = 0; j < n; j++)
                {
                    byte m = RecvBuffer[i];
                    i = i + 7;
                    k = TDXGetInt16(RecvBuffer, i, ref i);
                    for (int t = 0; t < k; t++)
                    {
                        m = RecvBuffer[i];
                        Array.Copy(RecvBuffer, i + 1, code, 0, 6);
                        codes = System.Text.Encoding.GetEncoding("GB2312").GetString(code);
                        num5 = i + 8;
                        int date1 = TDXGetInt32(RecvBuffer, num5, ref  num5);
                        byte t1 = RecvBuffer[i + 12];
                        if (t1 == 1)
                        {
                            num5 = i + 13;
                            double d1 = TDXGetDouble(RecvBuffer, num5, ref num5);
                            double d2 = TDXGetDouble(RecvBuffer, num5, ref  num5);
                            double d3 = TDXGetDouble(RecvBuffer, num5, ref  num5);
                            double d4 = TDXGetDouble(RecvBuffer, num5, ref  num5);
                            s1 = "代码:" + codes + " 时间:" + date1.ToString();
                            s1 = s1 + " 送现金:" + String.Format("{0:F2}", d1);
                            s1 = s1 + " 配股价:" + String.Format("{0:F2}", d2);
                            s1 = s1 + " 送股数:" + String.Format("{0:F2}", d3);
                            s1 = s1 + " 配股比例:" + String.Format("{0:F2}", d4);
                            List1.Items.Add(s1);
                        }
                        i = i + 29;
                    }
                    List1.EndUpdate();
                }
            }
        }

    }
    //财务
    private void Button4_Click(object sender, EventArgs e)
    {
        string codes;
        byte[] code = new byte[6];
        byte[] name = new byte[8];
        int n, j, k;
        string s1;
        if (m_hSocket == null)
        {
            MessageBox.Show("网络尚未连接");
            return;
        }
        k = cb.SelectedIndex;
        if (k == -1)
        {
            MessageBox.Show("请先初始化代码");
            return;
        }
        byte[] a = { 0xC, 0x1F, 0x18, 0x76, 0x0, 0x1, 0xB, 0x0, 0xB, 0x0, 0x10, 0x0, 0xDD, 0x0, 0xFF, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36 };
        a[12] = 1;//数量
        a[14] = GP[k].mark;
        GP[k].code.CopyTo(a, 15);
        if (Command(a, a.Length))
        {
            if (DEBUG == 1) SaveRecvData();
            n = RecvBr.ReadUInt16();
            if (n > 0)
            {
                List1.Items.Clear();
                List1.BeginUpdate();
                for (j = 0; j < n; j++)
                {
                    byte m = RecvBr.ReadByte();
                    code = RecvBr.ReadBytes(6);
                    codes = System.Text.Encoding.GetEncoding("GB2312").GetString(code);
                    double LTG = RecvBr.ReadSingle();
                    ushort t1 = RecvBr.ReadUInt16();
                    ushort t2 = RecvBr.ReadUInt16();
                    uint day1 = RecvBr.ReadUInt32();
                    uint day2 = RecvBr.ReadUInt32();
                    s1 = "代码:" + codes + " 流通股本:" + LTG.ToString();
                    List1.Items.Add(s1);
                    s1 = " 财务更新:" + day1.ToString();
                    List1.Items.Add(s1);
                    s1 = " 上市日期:" + day2.ToString();
                    List1.Items.Add(s1);
                    s1 = " 类型一:" + t1.ToString();
                    List1.Items.Add(s1);
                    s1 = " 类型二:" + t2.ToString();
                    List1.Items.Add(s1);
                    for (k = 0; k < 30; k++)
                    {
                        double d1 = RecvBr.ReadSingle();
                        s1 = CaiWuStr[k] + ":" + String.Format("{0:F2}", d1);
                        List1.Items.Add(s1);
                    }
                }
                List1.EndUpdate();
            }
        }
    }

    private void ToolStripStatusLabel1_Click(object sender, EventArgs e)
    {
        Process myProcess = new Process();
        myProcess.StartInfo.FileName = "iexplore.exe";
        myProcess.StartInfo.Arguments = "http://mystock.taobao.com";
        myProcess.Start();
    }
    //十档
    private void button21_Click(object sender, EventArgs e)
    {
        if (m_hSocket == null)
        {
            MessageBox.Show("网络尚未连接");
            return;
        }
        int k = cb.SelectedIndex;
        if (k == -1)
        {
            MessageBox.Show("请先初始化代码");
            return;
        }
        string s1;
        byte[] code = new byte[6];
        int n, i, j;

        byte[] a = { 0x0C, 0x01, 0x08, 0x00, 0x02, 0x01, 0x0F, 0x00, 0x0F, 0x00, 0x26, 0x05, 0x01, 0x00, 0x00, 0x30, 0x30, 0x30, 0x30, 0x30, 0x31, 0x00, 0x00, 0x00, 0x00 };
        a[14] = GP[k].mark;
        GP[k].code.CopyTo(a, 15);
        if (Command(a, a.Length))
        {
            //if (DEBUG == 1)
                SaveRecvData();
            n = RecvBr.ReadUInt16();
            if (n > 0)
            {
                i = 2;
                List1.Items.Clear();
                List1.BeginUpdate();
                for (j = 0; j < n; j++)
                {
                    byte m = RecvBuffer[i];
                    Array.Copy(RecvBuffer, i + 1, code, 0, 6);
                    i = i + 9;
                    double prize = TDXDecode(RecvBuffer, i, ref i) / 100.0;
                    double last = prize + (((double)TDXDecode(RecvBuffer, i, ref i)) / 100.0);
                    double open = prize + (((double)TDXDecode(RecvBuffer, i, ref i)) / 100);
                    double high = prize + (((double)TDXDecode(RecvBuffer, i, ref i)) / 100);
                    double low = prize + (((double)TDXDecode(RecvBuffer, i, ref i)) / 100);
                    int Time = TDXGetInt32(RecvBuffer, i, ref i);
                    TDXDecode(RecvBuffer, i, ref i);
                    int volume = TDXDecode(RecvBuffer, i, ref i);
                    int tradeQTY = TDXDecode(RecvBuffer, i, ref i);// '现量;
                    double amount = TDXGetDouble(RecvBuffer, i, ref i);
                    int b = TDXDecode(RecvBuffer, i, ref i);
                    int s = TDXDecode(RecvBuffer, i, ref i);

                    TDXDecode(RecvBuffer, i, ref i);
                    TDXDecode(RecvBuffer, i, ref i);

                    double buy1 = prize + (((double)TDXDecode(RecvBuffer, i, ref i)) / 100);
                    double sell1 = prize + (((double)TDXDecode(RecvBuffer, i, ref i)) / 100);
                    int buyQTY1 = TDXDecode(RecvBuffer, i, ref  i);
                    int sellQTY1 = TDXDecode(RecvBuffer, i, ref  i);

                    double buy2 = prize + (((double)TDXDecode(RecvBuffer, i, ref i)) / 100);
                    double sell2 = prize + (((double)TDXDecode(RecvBuffer, i, ref i)) / 100);
                    int buyQTY2 = TDXDecode(RecvBuffer, i, ref  i);
                    int sellQTY2 = TDXDecode(RecvBuffer, i, ref  i);

                    double buy3 = prize + (((double)TDXDecode(RecvBuffer, i, ref i)) / 100);
                    double sell3 = prize + (((double)TDXDecode(RecvBuffer, i, ref i)) / 100);
                    int buyQTY3 = TDXDecode(RecvBuffer, i, ref  i);
                    int sellQTY3 = TDXDecode(RecvBuffer, i, ref  i);
                    double buy4 = prize + (((double)TDXDecode(RecvBuffer, i, ref i)) / 100);
                    double sell4 = prize + (((double)TDXDecode(RecvBuffer, i, ref i)) / 100);
                    int buyQTY4 = TDXDecode(RecvBuffer, i, ref  i);
                    int sellQTY4 = TDXDecode(RecvBuffer, i, ref  i);
                    double buy5 = prize + (((double)TDXDecode(RecvBuffer, i, ref i)) / 100);
                    double sell5 = prize + (((double)TDXDecode(RecvBuffer, i, ref i)) / 100);
                    int buyQTY5 = TDXDecode(RecvBuffer, i, ref  i);
                    int sellQTY5 = TDXDecode(RecvBuffer, i, ref  i);
                    TDXGetInt16(RecvBuffer, i, ref i);
                    double BiCount =TDXGetInt16(RecvBuffer, i, ref i);//逐笔 笔数
                    TDXDecode(RecvBuffer, i, ref  i);
                    TDXDecode(RecvBuffer, i, ref  i);
                    TDXDecode(RecvBuffer, i, ref  i);
                    TDXDecode(RecvBuffer, i, ref  i);
                    TDXDecode(RecvBuffer, i, ref  i);
                    TDXDecode(RecvBuffer, i, ref  i);


                    double buy6 = prize + (((double)TDXDecode(RecvBuffer, i, ref i)) / 100);
                    double sell6 = prize + (((double)TDXDecode(RecvBuffer, i, ref i)) / 100);
                    int buyQTY6 = TDXDecode(RecvBuffer, i, ref  i);
                    int sellQTY6 = TDXDecode(RecvBuffer, i, ref  i);

                    double buy7 = prize + (((double)TDXDecode(RecvBuffer, i, ref i)) / 100);
                    double sell7 = prize + (((double)TDXDecode(RecvBuffer, i, ref i)) / 100);
                    int buyQTY7 = TDXDecode(RecvBuffer, i, ref  i);
                    int sellQTY7 = TDXDecode(RecvBuffer, i, ref  i);

                    double buy8 = prize + (((double)TDXDecode(RecvBuffer, i, ref i)) / 100);
                    double sell8 = prize + (((double)TDXDecode(RecvBuffer, i, ref i)) / 100);
                    int buyQTY8 = TDXDecode(RecvBuffer, i, ref  i);
                    int sellQTY8 = TDXDecode(RecvBuffer, i, ref  i);
                    double buy9 = prize + (((double)TDXDecode(RecvBuffer, i, ref i)) / 100);
                    double sell9 = prize + (((double)TDXDecode(RecvBuffer, i, ref i)) / 100);
                    int buyQTY9 = TDXDecode(RecvBuffer, i, ref  i);
                    int sellQTY9 = TDXDecode(RecvBuffer, i, ref  i);
                    double buy10 = prize + (((double)TDXDecode(RecvBuffer, i, ref i)) / 100);
                    double sell10 = prize + (((double)TDXDecode(RecvBuffer, i, ref i)) / 100);
                    int buyQTY10 = TDXDecode(RecvBuffer, i, ref  i);
                    int sellQTY10 = TDXDecode(RecvBuffer, i, ref  i);

                    double buyall = prize + (((double)TDXDecode(RecvBuffer, i, ref i)) / 100);//买均
                    double sellall = prize + (((double)TDXDecode(RecvBuffer, i, ref i)) / 100); ;//卖均
                    int buyQTYall = TDXDecode(RecvBuffer, i, ref i);//总买
                    int sellQTYall = TDXDecode(RecvBuffer, i, ref i);//总卖

                    s1 = "时间:" + Time.ToString() + " 代码:" + System.Text.Encoding.GetEncoding("GB2312").GetString(code);
                    List1.Items.Add(s1);
                    s1 = "──────────────";
                    List1.Items.Add(s1); ;
                    s1 = " 卖十:" + String.Format("{0:F2}", sell10) + "  " + String.Format("{0:d2}", sellQTY10);
                    List1.Items.Add(s1); ;
                    s1 = " 卖九:" + String.Format("{0:F2}", sell9) + "  " + String.Format("{0:d2}", sellQTY9);
                    List1.Items.Add(s1); ;
                    s1 = " 卖八:" + String.Format("{0:F2}", sell8) + "  " + String.Format("{0:d2}", sellQTY8);
                    List1.Items.Add(s1); ;
                    s1 = " 卖七:" + String.Format("{0:F2}", sell7) + "  " + String.Format("{0:d2}", sellQTY7);
                    List1.Items.Add(s1); ;
                    s1 = " 卖六:" + String.Format("{0:F2}", sell6) + "  " + String.Format("{0:d2}", sellQTY6);
                    List1.Items.Add(s1); ;
                    s1 = " 卖五:" + String.Format("{0:F2}", sell5) + "  " + String.Format("{0:d2}", sellQTY5);
                    List1.Items.Add(s1); ;
                    s1 = " 卖四:" + String.Format("{0:F2}", sell4) + "  " + String.Format("{0:d2}", sellQTY4);
                    List1.Items.Add(s1); ;
                    s1 = " 卖三:" + String.Format("{0:F2}", sell3) + "  " + String.Format("{0:d2}", sellQTY3);
                    List1.Items.Add(s1); ;
                    s1 = " 卖二:" + String.Format("{0:F2}", sell2) + "  " + String.Format("{0:d2}", sellQTY2);
                    List1.Items.Add(s1); ;
                    s1 = " 卖一:" + String.Format("{0:F2}", sell1) + "  " + String.Format("{0:d2}", sellQTY1);
                    List1.Items.Add(s1); ;
                    s1 = "──────────────";
                    List1.Items.Add(s1); ;
                    s1 = " 买一:" + String.Format("{0:F2}", buy1) + "  " + String.Format("{0:d2}", buyQTY1);
                    List1.Items.Add(s1); ;
                    s1 = " 买二:" + String.Format("{0:F2}", buy2) + "  " + String.Format("{0:d2}", buyQTY2);
                    List1.Items.Add(s1); ;
                    s1 = " 买三:" + String.Format("{0:F2}", buy3) + "  " + String.Format("{0:d2}", buyQTY3);
                    List1.Items.Add(s1); ;
                    s1 = " 买四:" + String.Format("{0:F2}", buy4) + "  " + String.Format("{0:d2}", buyQTY4);
                    List1.Items.Add(s1); ;
                    s1 = " 买五:" + String.Format("{0:F2}", buy5) + "  " + String.Format("{0:d2}", buyQTY5);
                    List1.Items.Add(s1); ;

                    s1 = " 买六:" + String.Format("{0:F2}", buy6) + "  " + String.Format("{0:d2}", buyQTY6);
                    List1.Items.Add(s1); ;
                    s1 = " 买七:" + String.Format("{0:F2}", buy7) + "  " + String.Format("{0:d2}", buyQTY7);
                    List1.Items.Add(s1); ;
                    s1 = " 买八:" + String.Format("{0:F2}", buy8) + "  " + String.Format("{0:d2}", buyQTY8);
                    List1.Items.Add(s1); ;
                    s1 = " 买九:" + String.Format("{0:F2}", buy9) + "  " + String.Format("{0:d2}", buyQTY9);
                    List1.Items.Add(s1); ;
                    s1 = " 买十:" + String.Format("{0:F2}", buy10) + "  " + String.Format("{0:d2}", buyQTY10);
                    List1.Items.Add(s1); ;

                    s1 = "──────────────";
                    List1.Items.Add(s1); ;

                    s1 = " 昨收:" + String.Format("{0:F2}", last);
                    List1.Items.Add(s1); ;
                    s1 = " 今开:" + String.Format("{0:F2}", open);
                    List1.Items.Add(s1); ;
                    s1 = " 最高:" + String.Format("{0:F2}", high);
                    List1.Items.Add(s1); ;
                    s1 = " 最低:" + String.Format("{0:F2}", low);
                    List1.Items.Add(s1); ;
                    s1 = " 收盘:" + String.Format("{0:F2}", prize);
                    List1.Items.Add(s1); ;
                    ; s1 = " 成交量:" + String.Format("{0:F2}", volume);
                    List1.Items.Add(s1); ;
                    s1 = " 成交额:" + String.Format("{0:F2}", amount);
                    List1.Items.Add(s1); ;
                    s1 = " 现量:" + String.Format("{0:F2}", tradeQTY);
                    List1.Items.Add(s1); ;
                    s1 = " 买盘:" + String.Format("{0:F2}", b);
                    List1.Items.Add(s1); ;
                    s1 = " 卖盘:" + String.Format("{0:F2}", s);
                    List1.Items.Add(s1);

                    s1 = " 买均:" + String.Format("{0:F2}", buyall);
                    List1.Items.Add(s1);
                    s1 = " 卖均:" + String.Format("{0:F2}", sellall);
                    List1.Items.Add(s1);
                    s1 = " 总买:" + String.Format("{0:F2}", buyQTYall);
                    List1.Items.Add(s1);
                    s1 = " 总卖:" + String.Format("{0:F2}", sellQTYall);
                    List1.Items.Add(s1);
                    s1 = " 总笔:" + String.Format("{0:F0}",BiCount);
                    List1.Items.Add(s1);

                }
                List1.EndUpdate();
            }


        }
    }
    //逐笔
    private void button22_Click(object sender, EventArgs e)
    {
        if (m_hSocket == null)
        {
            MessageBox.Show("网络尚未连接");
            return;
        }
        int k = cb.SelectedIndex;
        if (k == -1)
        {
            MessageBox.Show("请先初始化代码");
            return;
        }
        string s1;
        byte[] code = new byte[6];
        int n, i, j;

        byte[] a = { 0x0C, 0x02, 0x08, 0x00, 0x03, 0x01, 0x10, 0x00, 0x10, 0x00, 0x51, 0x05, 0x00, 0x00, 0x30, 0x30, 0x30, 0x30, 0x30, 0x31, 0x00, 0x00, 0x00, 0x00, 0xD0, 0x07 };
        a[13] = GP[k].mark;
        GP[k].code.CopyTo(a, 14);
        int start = 0;//开始位置
        byte[] b1 = BitConverter.GetBytes(start);
        b1.CopyTo(a, 20);
        ushort count = 30;//数量 最大为2000
        b1 = BitConverter.GetBytes(count);
        b1.CopyTo(a, 24);
        if (Command(a, a.Length))
        {
            if (DEBUG == 1)
                SaveRecvData();
            n = RecvBr.ReadUInt16();
            if (n > 0)
            {
                double close, dealcount;
                int time1;
                int num4, num5, sellorbuy;
                num4 = 0;
                i = 6;
                List1.Items.Clear();
                List1.BeginUpdate();
                for (j = 0; j < n; j++)
                {
                    time1 = TDXGetInt16(RecvBuffer, i, ref i);
                    num5 = TDXDecode(RecvBuffer, i, ref i);
                    close = (num4 + num5) / 10000.0;
                    dealcount = TDXDecode(RecvBuffer, i, ref i);
                    sellorbuy = TDXDecode(RecvBuffer, i, ref i);
                    num4 = num5 + num4;
                    int ss = time1 % 60;
                    int hh = 6 + (time1 / 3600);
                    int mm = ((time1 - ss) % 3600) / 60;
                    int w1 = sellorbuy & 0xff;
                    int w2 = sellorbuy & 0xff00;
                    if (w1 > 0)
                        dealcount = dealcount + w1 / 100.0;

                    s1 = "时间:" + hh.ToString() + ":" + mm.ToString() + ":" + ss.ToString();
                    s1 = s1 + " 价格:" + String.Format("{0:F2}", close);
                    s1 = s1 + " 单笔量:" + String.Format("{0:F2}", dealcount);
                    s1 = s1 + " 买卖标志:";
                    if (w2 == 0)
                        s1 += " B";
                    if (w2 == 256)
                        s1 += " S";
                    List1.Items.Add(s1);
                }
                List1.EndUpdate();
            }
        }

    }

    private void button23_Click(object sender, EventArgs e)
    {
        if (m_hSocket == null)
        {
            MessageBox.Show("网络尚未连接");
            return;
        }
        int k = cb.SelectedIndex;
        if (k == -1)
        {
            MessageBox.Show("请先初始化代码");
            return;
        }
        string s1;
        byte[] code = new byte[6];
        int i;
        byte[] a = { 0x0C, 0x13, 0x08, 0x00, 0x05, 0x01, 0x10, 0x00, 0x10, 0x00, 0x27, 0x05, 0x01, 0x00, 0x00, 0x30, 0x30, 0x30, 0x30, 0x30, 0x31, 0x00, 0x00, 0x00, 0x00, 0xA0 };
        a[14] = GP[k].mark;
        GP[k].code.CopyTo(a, 15);
        if (Command(a, a.Length))
        {
            if (DEBUG == 1)
                SaveRecvData();
            List1.Items.Clear();
            i = 9;
            int time = TDXGetInt32(RecvBuffer, i, ref i);
            int num4 = TDXDecode(RecvBuffer, i, ref i);
            int num5 = TDXDecode(RecvBuffer, i, ref i);
            int num6 = TDXDecode(RecvBuffer, i, ref i);
            int num7 = TDXDecode(RecvBuffer, i, ref i);
            s1 = String.Format(" 买1:{0:F3}  {1:d}笔", num4 / 100.0, num6);
            List1.Items.Add(s1);
            for (int j = 0; j < Math.Min(num6, 50); j++)
            {
                int t = TDXDecode(RecvBuffer, i, ref i);
                s1 = String.Format(" {0:d}  {1:d}", j + 1, t);
                List1.Items.Add(s1);
            }
            s1 = String.Format(" 卖1:{0:F3}  {1:d}笔", num5 / 100.0, num7);
            List1.Items.Add(s1);
            for (int j = 0; j < Math.Min(num7, 50); j++)
            {
                int t = TDXDecode(RecvBuffer, i, ref i);
                s1 = String.Format(" {0:d}  {1:d}", j + 1, t);
                List1.Items.Add(s1);
            }

        }

    }

    private void button26_Click(object sender, EventArgs e)
    {
        if (m_hSocket == null)
        {
            MessageBox.Show("网络尚未连接");
            return;
        }
        int k = cb.SelectedIndex;
        if (k == -1)
        {
            MessageBox.Show("请先初始化代码");
            return;
        }
        string s1;
        byte[] code = new byte[6];
        int n, i;
        byte[] aa = new byte[600];

        byte[] a = { 0x0C, 0x01, 0x08, 0x00, 0x02, 0x01, 0x0F, 0x00, 0x0F, 0x00, 0x26, 0x05, 0x01, 0x00}; 
        a.CopyTo(aa, 0);
        short j=(short)a.Length;
        for (i = 0; i < 50; i++)
        {
            aa[j + i * 11] = GP[295 + i].mark;
            GP[295 + i].code.CopyTo(aa, j + i * 11 + 1);
        }
        aa[12] = 50;//数量
        j = 50 * 11 + 4;
        byte[] b1 = BitConverter.GetBytes(j);
        b1.CopyTo(aa, 6);
        b1.CopyTo(aa, 8);
        if (Command(aa, 50*11+a.Length))
        {
            if (DEBUG == 1)
                SaveRecvData();
            n = RecvBr.ReadUInt16();
            if (n > 0)
            {
                i = 2;
                List1.Items.Clear();
                List1.BeginUpdate();
                for (j = 0; j < n; j++)
                {
                    byte m = RecvBuffer[i];
                    Array.Copy(RecvBuffer, i + 1, code, 0, 6);
                    i = i + 9;
                    double prize = TDXDecode(RecvBuffer, i, ref i) / 100.0;
                    double last = prize + (((double)TDXDecode(RecvBuffer, i, ref i)) / 100.0);
                    double open = prize + (((double)TDXDecode(RecvBuffer, i, ref i)) / 100);
                    double high = prize + (((double)TDXDecode(RecvBuffer, i, ref i)) / 100);
                    double low = prize + (((double)TDXDecode(RecvBuffer, i, ref i)) / 100);
                    int Time = TDXGetInt32(RecvBuffer, i, ref i);
                    TDXDecode(RecvBuffer, i, ref i);
                    int volume = TDXDecode(RecvBuffer, i, ref i);
                    int tradeQTY = TDXDecode(RecvBuffer, i, ref i);// '现量;
                    double amount = TDXGetDouble(RecvBuffer, i, ref i);
                    int b = TDXDecode(RecvBuffer, i, ref i);
                    int s = TDXDecode(RecvBuffer, i, ref i);

                    TDXDecode(RecvBuffer, i, ref i);
                    TDXDecode(RecvBuffer, i, ref i);

                    double buy1 = prize + (((double)TDXDecode(RecvBuffer, i, ref i)) / 100);
                    double sell1 = prize + (((double)TDXDecode(RecvBuffer, i, ref i)) / 100);
                    int buyQTY1 = TDXDecode(RecvBuffer, i, ref  i);
                    int sellQTY1 = TDXDecode(RecvBuffer, i, ref  i);

                    double buy2 = prize + (((double)TDXDecode(RecvBuffer, i, ref i)) / 100);
                    double sell2 = prize + (((double)TDXDecode(RecvBuffer, i, ref i)) / 100);
                    int buyQTY2 = TDXDecode(RecvBuffer, i, ref  i);
                    int sellQTY2 = TDXDecode(RecvBuffer, i, ref  i);

                    double buy3 = prize + (((double)TDXDecode(RecvBuffer, i, ref i)) / 100);
                    double sell3 = prize + (((double)TDXDecode(RecvBuffer, i, ref i)) / 100);
                    int buyQTY3 = TDXDecode(RecvBuffer, i, ref  i);
                    int sellQTY3 = TDXDecode(RecvBuffer, i, ref  i);
                    double buy4 = prize + (((double)TDXDecode(RecvBuffer, i, ref i)) / 100);
                    double sell4 = prize + (((double)TDXDecode(RecvBuffer, i, ref i)) / 100);
                    int buyQTY4 = TDXDecode(RecvBuffer, i, ref  i);
                    int sellQTY4 = TDXDecode(RecvBuffer, i, ref  i);
                    double buy5 = prize + (((double)TDXDecode(RecvBuffer, i, ref i)) / 100);
                    double sell5 = prize + (((double)TDXDecode(RecvBuffer, i, ref i)) / 100);
                    int buyQTY5 = TDXDecode(RecvBuffer, i, ref  i);
                    int sellQTY5 = TDXDecode(RecvBuffer, i, ref  i);
                    TDXGetInt16(RecvBuffer, i, ref i);
                    double BiCount = TDXGetInt16(RecvBuffer, i, ref i);//逐笔 笔数
                    TDXDecode(RecvBuffer, i, ref  i);
                    TDXDecode(RecvBuffer, i, ref  i);
                    TDXDecode(RecvBuffer, i, ref  i);
                    TDXDecode(RecvBuffer, i, ref  i);
                    TDXDecode(RecvBuffer, i, ref  i);
                    TDXDecode(RecvBuffer, i, ref  i);


                    double buy6 = prize + (((double)TDXDecode(RecvBuffer, i, ref i)) / 100);
                    double sell6 = prize + (((double)TDXDecode(RecvBuffer, i, ref i)) / 100);
                    int buyQTY6 = TDXDecode(RecvBuffer, i, ref  i);
                    int sellQTY6 = TDXDecode(RecvBuffer, i, ref  i);

                    double buy7 = prize + (((double)TDXDecode(RecvBuffer, i, ref i)) / 100);
                    double sell7 = prize + (((double)TDXDecode(RecvBuffer, i, ref i)) / 100);
                    int buyQTY7 = TDXDecode(RecvBuffer, i, ref  i);
                    int sellQTY7 = TDXDecode(RecvBuffer, i, ref  i);

                    double buy8 = prize + (((double)TDXDecode(RecvBuffer, i, ref i)) / 100);
                    double sell8 = prize + (((double)TDXDecode(RecvBuffer, i, ref i)) / 100);
                    int buyQTY8 = TDXDecode(RecvBuffer, i, ref  i);
                    int sellQTY8 = TDXDecode(RecvBuffer, i, ref  i);
                    double buy9 = prize + (((double)TDXDecode(RecvBuffer, i, ref i)) / 100);
                    double sell9 = prize + (((double)TDXDecode(RecvBuffer, i, ref i)) / 100);
                    int buyQTY9 = TDXDecode(RecvBuffer, i, ref  i);
                    int sellQTY9 = TDXDecode(RecvBuffer, i, ref  i);
                    double buy10 = prize + (((double)TDXDecode(RecvBuffer, i, ref i)) / 100);
                    double sell10 = prize + (((double)TDXDecode(RecvBuffer, i, ref i)) / 100);
                    int buyQTY10 = TDXDecode(RecvBuffer, i, ref  i);
                    int sellQTY10 = TDXDecode(RecvBuffer, i, ref  i);

                    double buyall = prize + (((double)TDXDecode(RecvBuffer, i, ref i)) / 100);//买均
                    double sellall = prize + (((double)TDXDecode(RecvBuffer, i, ref i)) / 100); ;//卖均
                    int buyQTYall = TDXDecode(RecvBuffer, i, ref i);//总买
                    int sellQTYall = TDXDecode(RecvBuffer, i, ref i);//总卖

                    s1 = "序号:"+j.ToString()+ " 时间:" + Time.ToString() + " 代码:" + System.Text.Encoding.GetEncoding("GB2312").GetString(code);
                    List1.Items.Add(s1);
                    s1 = "======================================";
                    List1.Items.Add(s1); ;
                    s1 = " 卖十:" + String.Format("{0:F2}", sell10) + "  " + String.Format("{0:d2}", sellQTY10);
                    List1.Items.Add(s1); ;
                    s1 = " 卖九:" + String.Format("{0:F2}", sell9) + "  " + String.Format("{0:d2}", sellQTY9);
                    List1.Items.Add(s1); ;
                    s1 = " 卖八:" + String.Format("{0:F2}", sell8) + "  " + String.Format("{0:d2}", sellQTY8);
                    List1.Items.Add(s1); ;
                    s1 = " 卖七:" + String.Format("{0:F2}", sell7) + "  " + String.Format("{0:d2}", sellQTY7);
                    List1.Items.Add(s1); ;
                    s1 = " 卖六:" + String.Format("{0:F2}", sell6) + "  " + String.Format("{0:d2}", sellQTY6);
                    List1.Items.Add(s1); ;
                    s1 = " 卖五:" + String.Format("{0:F2}", sell5) + "  " + String.Format("{0:d2}", sellQTY5);
                    List1.Items.Add(s1); ;
                    s1 = " 卖四:" + String.Format("{0:F2}", sell4) + "  " + String.Format("{0:d2}", sellQTY4);
                    List1.Items.Add(s1); ;
                    s1 = " 卖三:" + String.Format("{0:F2}", sell3) + "  " + String.Format("{0:d2}", sellQTY3);
                    List1.Items.Add(s1); ;
                    s1 = " 卖二:" + String.Format("{0:F2}", sell2) + "  " + String.Format("{0:d2}", sellQTY2);
                    List1.Items.Add(s1); ;
                    s1 = " 卖一:" + String.Format("{0:F2}", sell1) + "  " + String.Format("{0:d2}", sellQTY1);
                    List1.Items.Add(s1); ;
                    s1 = "==================";
                    List1.Items.Add(s1); ;
                    s1 = " 买一:" + String.Format("{0:F2}", buy1) + "  " + String.Format("{0:d2}", buyQTY1);
                    List1.Items.Add(s1); ;
                    s1 = " 买二:" + String.Format("{0:F2}", buy2) + "  " + String.Format("{0:d2}", buyQTY2);
                    List1.Items.Add(s1); ;
                    s1 = " 买三:" + String.Format("{0:F2}", buy3) + "  " + String.Format("{0:d2}", buyQTY3);
                    List1.Items.Add(s1); ;
                    s1 = " 买四:" + String.Format("{0:F2}", buy4) + "  " + String.Format("{0:d2}", buyQTY4);
                    List1.Items.Add(s1); ;
                    s1 = " 买五:" + String.Format("{0:F2}", buy5) + "  " + String.Format("{0:d2}", buyQTY5);
                    List1.Items.Add(s1); ;
                    s1 = " 买六:" + String.Format("{0:F2}", buy6) + "  " + String.Format("{0:d2}", buyQTY6);
                    List1.Items.Add(s1); ;
                    s1 = " 买七:" + String.Format("{0:F2}", buy7) + "  " + String.Format("{0:d2}", buyQTY7);
                    List1.Items.Add(s1); ;
                    s1 = " 买八:" + String.Format("{0:F2}", buy8) + "  " + String.Format("{0:d2}", buyQTY8);
                    List1.Items.Add(s1); ;
                    s1 = " 买九:" + String.Format("{0:F2}", buy9) + "  " + String.Format("{0:d2}", buyQTY9);
                    List1.Items.Add(s1); ;
                    s1 = " 买十:" + String.Format("{0:F2}", buy10) + "  " + String.Format("{0:d2}", buyQTY10);
                    List1.Items.Add(s1); ;
                    s1 = "==================";
                    List1.Items.Add(s1); ;
                    s1 = " 昨收:" + String.Format("{0:F2}", last);
                    List1.Items.Add(s1); ;
                    s1 = " 今开:" + String.Format("{0:F2}", open);
                    List1.Items.Add(s1); ;
                    s1 = " 最高:" + String.Format("{0:F2}", high);
                    List1.Items.Add(s1); ;
                    s1 = " 最低:" + String.Format("{0:F2}", low);
                    List1.Items.Add(s1); ;
                    s1 = " 收盘:" + String.Format("{0:F2}", prize);
                    List1.Items.Add(s1); ;
                    ; s1 = " 成交量:" + String.Format("{0:F2}", volume);
                    List1.Items.Add(s1); ;
                    s1 = " 成交额:" + String.Format("{0:F2}", amount);
                    List1.Items.Add(s1); ;
                    s1 = " 现量:" + String.Format("{0:F2}", tradeQTY);
                    List1.Items.Add(s1); ;
                    s1 = " 买盘:" + String.Format("{0:F2}", b);
                    List1.Items.Add(s1); ;
                    s1 = " 卖盘:" + String.Format("{0:F2}", s);
                    List1.Items.Add(s1);
                    s1 = " 买均:" + String.Format("{0:F2}", buyall);
                    List1.Items.Add(s1);
                    s1 = " 卖均:" + String.Format("{0:F2}", sellall);
                    List1.Items.Add(s1);
                    s1 = " 总买:" + String.Format("{0:F2}", buyQTYall);
                    List1.Items.Add(s1);
                    s1 = " 总卖:" + String.Format("{0:F2}", sellQTYall);
                    List1.Items.Add(s1);
                    s1 = " 总笔:" + String.Format("{0:F0}", BiCount);
                    List1.Items.Add(s1);
                    s1 = "======================================";
                    List1.Items.Add(s1); ;

                }
                List1.EndUpdate();
            }
        }
    }
    public int GetStock(int mark, string StkCode)
    {
        int k = -1;
        int no = 0;
        try
        {
            no = mark * 1000000 + Convert.ToInt32(StkCode);
            for (int i = 0; i < stkcount; i++)
            {
                if (no == GP[i].no)
                {
                    return i;
                }
            }

        }
        catch
        {
            return -1;
        }

        return k;
    }
    //初始化权息
    private void button24_Click(object sender, EventArgs e)
    {
        if (m_hSocket == null)
        {
            MessageBox.Show("网络尚未连接");
            return;
        }
        if (stkcount == -1)
        {
            MessageBox.Show("请先初始化代码");
            return;
        }
        string codes;
        byte[] code = new byte[6];
        byte[] a = new byte[2000];
        int Len, i, t, n, k, ii, kk, tt;
        int j;
        Len = 100;
        byte[] bb = { 0xC, 0x1F, 0x18, 0x75, 0x0, 0x1, 0xB, 0x0, 0xB, 0x0, 0xF, 0x0, 0x0, 0x0 };
        bb.CopyTo(a, 0);
        i = 100;
        SP.Minimum = 0;
        SP.Maximum = stkcount;
        while (i < stkcount)
        {
            j = bb.Length;
            Len = Math.Min(100, stkcount - i);
            for (t = i; t < i + Len; t++)
            {
                a[j] = GP[t].mark;
                GP[t].code.CopyTo(a, j + 1);
                j = j + 7;
            }
            byte[] b1;
            ushort jj = (ushort)(j-10);
            b1 = BitConverter.GetBytes(jj);
            b1.CopyTo(a, 6);
            b1.CopyTo(a, 8);
            a[12] = (byte)Len;
            if (Command(a, j))
            {
                if (DEBUG == 1) SaveRecvData();
                n = RecvBr.ReadUInt16();
                if (n > 0)
                {
                    ii = 2;
                    int num5;
                    for (j = 0; j < n; j++)
                    {
                        byte m = RecvBuffer[ii];
                        Array.Copy(RecvBuffer, ii + 1, code, 0, 6);
                        codes = System.Text.Encoding.GetEncoding("GB2312").GetString(code);
                        kk = GetStock(m, codes);
                        if (kk == -1)
                        {
                            break;
                        }

                        ii = ii + 7;
                        k = TDXGetInt16(RecvBuffer, ii, ref ii);
                        if (k == 0)
                            continue;
                        tt = 0;
                        for (t = 0; t < k; t++)
                        {
                            m = RecvBuffer[ii];
                            num5 = ii + 8;
                            int date1 = TDXGetInt32(RecvBuffer, num5, ref  num5);
                            byte t1 = RecvBuffer[ii + 12];
                            if (t1 == 1)
                            {
                                STK[kk].qu[tt].day = (uint)date1;
                                STK[kk].qu[tt].style = t1;
                                num5 = ii + 13;
                                STK[kk].qu[tt].money = TDXGetDouble(RecvBuffer, num5, ref num5);
                                STK[kk].qu[tt].peimoney = TDXGetDouble(RecvBuffer, num5, ref  num5);
                                STK[kk].qu[tt].number = TDXGetDouble(RecvBuffer, num5, ref  num5);
                                STK[kk].qu[tt].peinumber = TDXGetDouble(RecvBuffer, num5, ref  num5);
                                tt = tt + 1;
                            }
                            ii = ii + 29;
                        }

                    }
                }
            }
            else
            {
                break;
            }
            i = i + Len;
            SP.Value = i;
            Application.DoEvents();
        }
        List1.Items.Add("权息初始化完成");
    }
    //财务初始化
    private void button25_Click(object sender, EventArgs e)
    {
        if (m_hSocket == null)
        {
            MessageBox.Show("网络尚未连接");
            return;
        }
        if (stkcount == -1)
        {
            MessageBox.Show("请先初始化代码");
            return;
        }
        string  codes;
        byte[] code = new byte[6];
        byte[] a = new byte[2000];
        int Len, i, t, n, k, kk;
        int j;
        Len = 100;
        byte[] bb = { 0xC, 0x1F, 0x18, 0x75, 0x0, 0x1, 0xB, 0x0, 0xB, 0x0, 0x10, 0x0, 0x0, 0x0 };
        bb.CopyTo(a, 0);
        i = 100;
        SP.Minimum = 0;
        SP.Maximum = stkcount;
        while (i < stkcount)
        {
            j = bb.Length;
            Len = Math.Min(100, stkcount - i);
            for (t = i; t < i + Len; t++)
            {
                a[j] = GP[t].mark;
                GP[t].code.CopyTo(a, j + 1);
                j = j + 7;
            }
            byte[] b1;
            ushort jj = (ushort)(j - 10);
            b1 = BitConverter.GetBytes(jj);
            b1.CopyTo(a, 6);
            b1.CopyTo(a, 8);
            a[12] = (byte)Len;
            if (Command(a, j))
            {
                if (DEBUG == 1) SaveRecvData();
                n = RecvBr.ReadUInt16();
                if (n > 0)
                {
                    for (j = 0; j < n; j++)
                    {
                        byte m = RecvBr.ReadByte();
                        code = RecvBr.ReadBytes(6);
                        codes = System.Text.Encoding.GetEncoding("GB2312").GetString(code);
                        kk = GetStock(m, codes);
                        if (kk == -1)
                            break;
                        STK[kk].cw.LTG= RecvBr.ReadSingle();
                        STK[kk].cw.t1 = RecvBr.ReadUInt16();
                        STK[kk].cw.t2 = RecvBr.ReadUInt16();
                        STK[kk].cw.day1 = RecvBr.ReadUInt32();
                        STK[kk].cw.day2 = RecvBr.ReadUInt32();
                        for (k = 0; k < 30; k++)
                            STK[kk].cw.zl[k]= RecvBr.ReadSingle();
                    }
                }
            }
            else
            {
                break;
            }
            i = i + Len;
            SP.Value = i;
            Application.DoEvents();
        }
        List1.Items.Add("财务初始化完成");
    }

    private void button9_Click(object sender, EventArgs e)
    {
        byte[] a = {0x0C,0x25,0x08,0x00,0x02,0x01,0x0F,0x00,0x0F,0x00,0x26,0x05,0x01,0x00,0x01,0x35,0x31,0x30,0x32,0x32,0x30,0x00,0x00,0x00,0x00};
        if (Command(a,a.Length))
        {
            SaveRecvData();
        }
    }

}

using System.Diagnostics;
using System;
using System.Collections.Generic;
using System.Text;
using System.Runtime.Serialization;
public class flat
{
    #region Constants/Readonly
    static readonly int[] CPLENS =
                {
                 3, 4, 5, 6, 7, 8, 9, 10, 11, 13, 15, 17, 19, 23, 27, 31,
                 35, 43, 51, 59, 67, 83, 99, 115, 131, 163, 195, 227, 258
                };
    static readonly int[] CPLEXT =
                {
                 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2,
                 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 0
                };
    static readonly int[] CPDIST =
                {
                 1, 2, 3, 4, 5, 7, 9, 13, 17, 25, 33, 49, 65, 97, 129, 193,
                 257, 385, 513, 769, 1025, 1537, 2049, 3073, 4097, 6145,
                 8193, 12289, 16385, 24577
                };
    static readonly int[] CPDEXT =
                {
                 0, 0, 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6,
                 7, 7, 8, 8, 9, 9, 10, 10, 11, 11,
                 12, 12, 13, 13
                };
    const int DECODE_HEADER = 0;
    const int DECODE_DICT = 1;
    const int DECODE_BLOCKS = 2;
    const int DECODE_STORED_LEN1 = 3;
    const int DECODE_STORED_LEN2 = 4;
    const int DECODE_STORED = 5;
    const int DECODE_DYN_HEADER = 6;
    const int DECODE_HUFFMAN = 7;
    const int DECODE_HUFFMAN_LENBITS = 8;
    const int DECODE_HUFFMAN_DIST = 9;
    const int DECODE_HUFFMAN_DISTBITS = 10;
    const int DECODE_CHKSUM = 11;
    const int FINISHED = 12;
    #endregion
    #region Instance Fields
    int mode;
    int readAdler;
    int neededBits;
    int repLength;
    int repDist;
    int uncomprLen;
    bool isLastBlock;
    long totalOut;
    long totalIn;
    bool noHeader;
    StreamManipulator input;
    OutputWindow outputWindow;
    InflaterDynHeader dynHeader;
    InflaterHuffmanTree litlenTree, distTree;
    Adler32 adler;
    #endregion
    #region Constructors
    public flat()
        : this(false)
    {
    }
    public flat(bool noHeader)
    {
        this.noHeader = noHeader;
        this.adler = new Adler32();
        input = new StreamManipulator();
        outputWindow = new OutputWindow();
        mode = noHeader ? DECODE_BLOCKS : DECODE_HEADER;
    }
    #endregion
    public void Reset()
    {
        mode = noHeader ? DECODE_BLOCKS : DECODE_HEADER;
        totalIn = 0;
        totalOut = 0;
        input.Reset();
        outputWindow.Reset();
        dynHeader = null;
        litlenTree = null;
        distTree = null;
        isLastBlock = false;
        adler.Reset();
    }
    private bool DecodeHeader()
    {
        int header = input.PeekBits(16);
        if (header < 0)
        {
            return false;
        }
        input.DropBits(16);
        header = ((header << 8) | (header >> 8)) & 0xffff;
        if (header % 31 != 0)
        {
            throw new SharpZipBaseException("Header checksum illegal");
        }
        if ((header & 0x0f00) != (8<< 8))
        {
            throw new SharpZipBaseException("Compression Method unknown");
        }
        if ((header & 0x0020) == 0)
        {
            mode = DECODE_BLOCKS;
        }
        else
        {
            mode = DECODE_DICT;
            neededBits = 32;
        }
        return true;
    }
    private bool DecodeDict()
    {
        while (neededBits > 0)
        {
            int dictByte = input.PeekBits(8);
            if (dictByte < 0)
            {
                return false;
            }
            input.DropBits(8);
            readAdler = (readAdler << 8) | dictByte;
            neededBits -= 8;
        }
        return false;
    }
    private bool DecodeHuffman()
    {
        int free = outputWindow.GetFreeSpace();
        while (free >= 258)
        {
            int symbol;
            switch (mode)
            {
                case DECODE_HUFFMAN:
                    while (((symbol = litlenTree.GetSymbol(input)) & ~0xff) == 0)
                    {
                        outputWindow.Write(symbol);
                        if (--free < 258)
                        {
                            return true;
                        }
                    }
                    if (symbol < 257)
                    {
                        if (symbol < 0)
                        {
                            return false;
                        }
                        else
                        {
                            distTree = null;
                            litlenTree = null;
                            mode = DECODE_BLOCKS;
                            return true;
                        }
                    }
                    try
                    {
                        repLength = CPLENS[symbol - 257];
                        neededBits = CPLEXT[symbol - 257];
                    }
                    catch (Exception)
                    {
                        throw new SharpZipBaseException("Illegal rep length code");
                    }
                    goto case DECODE_HUFFMAN_LENBITS;
                case DECODE_HUFFMAN_LENBITS:
                    if (neededBits > 0)
                    {
                        mode = DECODE_HUFFMAN_LENBITS;
                        int i = input.PeekBits(neededBits);
                        if (i < 0)
                        {
                            return false;
                        }
                        input.DropBits(neededBits);
                        repLength += i;
                    }
                    mode = DECODE_HUFFMAN_DIST;
                    goto case DECODE_HUFFMAN_DIST;
                case DECODE_HUFFMAN_DIST:
                    symbol = distTree.GetSymbol(input);
                    if (symbol < 0)
                    {
                        return false;
                    }
                    try
                    {
                        repDist = CPDIST[symbol];
                        neededBits = CPDEXT[symbol];
                    }
                    catch (Exception)
                    {
                        throw new SharpZipBaseException("Illegal rep dist code");
                    }
                    goto case DECODE_HUFFMAN_DISTBITS;
                case DECODE_HUFFMAN_DISTBITS:
                    if (neededBits > 0)
                    {
                        mode = DECODE_HUFFMAN_DISTBITS;
                        int i = input.PeekBits(neededBits);
                        if (i < 0)
                        {
                            return false;
                        }
                        input.DropBits(neededBits);
                        repDist += i;
                    }
                    outputWindow.Repeat(repLength, repDist);
                    free -= repLength;
                    mode = DECODE_HUFFMAN;
                    break;
                default:
                    throw new SharpZipBaseException("Inflater unknown mode");
            }
        }
        return true;
    }
    private bool DecodeChksum()
    {
        while (neededBits > 0)
        {
            int chkByte = input.PeekBits(8);
            if (chkByte < 0)
            {
                return false;
            }
            input.DropBits(8);
            readAdler = (readAdler << 8) | chkByte;
            neededBits -= 8;
        }
        if ((int)adler.Value != readAdler)
        {
            throw new SharpZipBaseException("Adler chksum doesn't match: " + (int)adler.Value + " vs. " + readAdler);
        }
        mode = FINISHED;
        return false;
    }
    private bool Decode()
    {
        switch (mode)
        {
            case DECODE_HEADER:
                return DecodeHeader();
            case DECODE_DICT:
                return DecodeDict();
            case DECODE_CHKSUM:
                return DecodeChksum();
            case DECODE_BLOCKS:
                if (isLastBlock)
                {
                    if (noHeader)
                    {
                        mode = FINISHED;
                        return false;
                    }
                    else
                    {
                        input.SkipToByteBoundary();
                        neededBits = 32;
                        mode = DECODE_CHKSUM;
                        return true;
                    }
                }
                int type = input.PeekBits(3);
                if (type < 0)
                {
                    return false;
                }
                input.DropBits(3);
                if ((type & 1) != 0)
                {
                    isLastBlock = true;
                }
                switch (type >> 1)
                {
                    case DeflaterConstants.STORED_BLOCK:
                        input.SkipToByteBoundary();
                        mode = DECODE_STORED_LEN1;
                        break;
                    case DeflaterConstants.STATIC_TREES:
                        litlenTree = InflaterHuffmanTree.defLitLenTree;
                        distTree = InflaterHuffmanTree.defDistTree;
                        mode = DECODE_HUFFMAN;
                        break;
                    case DeflaterConstants.DYN_TREES:
                        dynHeader = new InflaterDynHeader();
                        mode = DECODE_DYN_HEADER;
                        break;
                    default:
                        throw new SharpZipBaseException("Unknown block type " + type);
                }
                return true;
            case DECODE_STORED_LEN1:
                {
                    if ((uncomprLen = input.PeekBits(16)) < 0)
                    {
                        return false;
                    }
                    input.DropBits(16);
                    mode = DECODE_STORED_LEN2;
                }
                goto case DECODE_STORED_LEN2;
            case DECODE_STORED_LEN2:
                {
                    int nlen = input.PeekBits(16);
                    if (nlen < 0)
                    {
                        return false;
                    }
                    input.DropBits(16);
                    if (nlen != (uncomprLen ^ 0xffff))
                    {
                        throw new SharpZipBaseException("broken uncompressed block");
                    }
                    mode = DECODE_STORED;
                }
                goto case DECODE_STORED;
            case DECODE_STORED:
                {
                    int more = outputWindow.CopyStored(input, uncomprLen);
                    uncomprLen -= more;
                    if (uncomprLen == 0)
                    {
                        mode = DECODE_BLOCKS;
                        return true;
                    }
                    return !input.IsNeedingInput;
                }
            case DECODE_DYN_HEADER:
                if (!dynHeader.Decode(input))
                {
                    return false;
                }
                litlenTree = dynHeader.BuildLitLenTree();
                distTree = dynHeader.BuildDistTree();
                mode = DECODE_HUFFMAN;
                goto case DECODE_HUFFMAN;
            case DECODE_HUFFMAN:
            case DECODE_HUFFMAN_LENBITS:
            case DECODE_HUFFMAN_DIST:
            case DECODE_HUFFMAN_DISTBITS:
                return DecodeHuffman();
            case FINISHED:
                return false;
            default:
                throw new SharpZipBaseException("Inflater.Decode unknown mode");
        }
    }
    public void SetDictionary(byte[] buffer)
    {
        SetDictionary(buffer, 0, buffer.Length);
    }
    public void SetDictionary(byte[] buffer, int index, int count)
    {
        if (buffer == null)
        {
            throw new ArgumentNullException("buffer");
        }
        if (index < 0)
        {
            throw new ArgumentOutOfRangeException("index");
        }
        if (count < 0)
        {
            throw new ArgumentOutOfRangeException("count");
        }
        if (!IsNeedingDictionary)
        {
            throw new InvalidOperationException("Dictionary is not needed");
        }
        adler.Update(buffer, index, count);
        if ((int)adler.Value != readAdler)
        {
            throw new SharpZipBaseException("Wrong adler checksum");
        }
        adler.Reset();
        outputWindow.CopyDict(buffer, index, count);
        mode = DECODE_BLOCKS;
    }
    public void SetInput(byte[] buffer)
    {
        SetInput(buffer, 0, buffer.Length);
    }
    public void SetInput(byte[] buffer, int index, int count)
    {
        input.SetInput(buffer, index, count);
        totalIn += (long)count;
    }
    public int Inflate(byte[] buffer)
    {
        if (buffer == null)
        {
            throw new ArgumentNullException("buffer");
        }
        return Inflate(buffer, 0, buffer.Length);
    }
    public int Inflate(byte[] buffer, int offset, int count)
    {
        if (buffer == null)
        {
            throw new ArgumentNullException("buffer");
        }
        if (count < 0)
        {
            throw new ArgumentOutOfRangeException("count", "count cannot be negative");
        }
        if (offset < 0)
        {
            throw new ArgumentOutOfRangeException("offset", "offset cannot be negative");
        }
        if (offset + count > buffer.Length)
        {
            throw new ArgumentException("count exceeds buffer bounds");
        }
        if (count == 0)
        {
            if (!IsFinished)
            {
                Decode();
            }
            return 0;
        }
        int bytesCopied = 0;
        do
        {
            if (mode != DECODE_CHKSUM)
            {
                int more = outputWindow.CopyOutput(buffer, offset, count);
                if (more > 0)
                {
                    adler.Update(buffer, offset, more);
                    offset += more;
                    bytesCopied += more;
                    totalOut += (long)more;
                    count -= more;
                    if (count == 0)
                    {
                        return bytesCopied;
                    }
                }
            }
        } while (Decode() || ((outputWindow.GetAvailable() > 0) && (mode != DECODE_CHKSUM)));
        return bytesCopied;
    }
    public bool IsNeedingInput
    {
        get
        {
            return input.IsNeedingInput;
        }
    }
    public bool IsNeedingDictionary
    {
        get
        {
            return mode == DECODE_DICT && neededBits == 0;
        }
    }
    public bool IsFinished
    {
        get
        {
            return mode == FINISHED && outputWindow.GetAvailable() == 0;
        }
    }
    public int Adler
    {
        get
        {
            return IsNeedingDictionary ? readAdler : (int)adler.Value;
        }
    }
    public long TotalOut
    {
        get
        {
            return totalOut;
        }
    }
    public long TotalIn
    {
        get
        {
            return totalIn - (long)RemainingInput;
        }
    }
    public int RemainingInput
    {
        get
        {
            return input.AvailableBytes;
        }
    }
    public class StreamManipulator
    {
        #region Constructors
        public StreamManipulator()
        {
        }
        #endregion
        public int PeekBits(int bitCount)
        {
            if (bitsInBuffer_ < bitCount)
            {
                if (windowStart_ == windowEnd_)
                {
                    return -1;
                }
                buffer_ |= (uint)((window_[windowStart_++] & 0xff |
                (window_[windowStart_++] & 0xff) << 8) << bitsInBuffer_);
                bitsInBuffer_ += 16;
            }
            return (int)(buffer_ & ((1 << bitCount) - 1));
        }
        public void DropBits(int bitCount)
        {
            buffer_ >>= bitCount;
            bitsInBuffer_ -= bitCount;
        }
        public int GetBits(int bitCount)
        {
            int bits = PeekBits(bitCount);
            if (bits >= 0)
            {
                DropBits(bitCount);
            }
            return bits;
        }
        public int AvailableBits
        {
            get
            {
                return bitsInBuffer_;
            }
        }
        public int AvailableBytes
        {
            get
            {
                return windowEnd_ - windowStart_ + (bitsInBuffer_ >> 3);
            }
        }
        public void SkipToByteBoundary()
        {
            buffer_ >>= (bitsInBuffer_ & 7);
            bitsInBuffer_ &= ~7;
        }
        public bool IsNeedingInput
        {
            get
            {
                return windowStart_ == windowEnd_;
            }
        }
        public int CopyBytes(byte[] output, int offset, int length)
        {
            if (length < 0)
            {
                throw new ArgumentOutOfRangeException("length");
            }
            if ((bitsInBuffer_ & 7) != 0)
            {
                throw new InvalidOperationException("Bit buffer is not byte aligned!");
            }
            int count = 0;
            while ((bitsInBuffer_ > 0) && (length > 0))
            {
                output[offset++] = (byte)buffer_;
                buffer_ >>= 8;
                bitsInBuffer_ -= 8;
                length--;
                count++;
            }
            if (length == 0)
            {
                return count;
            }
            int avail = windowEnd_ - windowStart_;
            if (length > avail)
            {
                length = avail;
            }
            System.Array.Copy(window_, windowStart_, output, offset, length);
            windowStart_ += length;
            if (((windowStart_ - windowEnd_) & 1) != 0)
            {
                buffer_ = (uint)(window_[windowStart_++] & 0xff);
                bitsInBuffer_ = 8;
            }
            return count + length;
        }
        public void Reset()
        {
            buffer_ = 0;
            windowStart_ = windowEnd_ = bitsInBuffer_ = 0;
        }
        public void SetInput(byte[] buffer, int offset, int count)
        {
            if (buffer == null)
            {
                throw new ArgumentNullException("buffer");
            }
            if (offset < 0)
            {
                throw new ArgumentOutOfRangeException("offset", "Cannot be negative");
            }
            if (count < 0)
            {
                throw new ArgumentOutOfRangeException("count", "Cannot be negative");
            }
            if (windowStart_ < windowEnd_)
            {
                throw new InvalidOperationException("Old input was not completely processed");
            }
            int end = offset + count;
            if ((offset > end) || (end > buffer.Length))
            {
                throw new ArgumentOutOfRangeException("count");
            }
            if ((count & 1) != 0)
            {
                buffer_ |= (uint)((buffer[offset++] & 0xff) << bitsInBuffer_);
                bitsInBuffer_ += 8;
            }
            window_ = buffer;
            windowStart_ = offset;
            windowEnd_ = end;
        }
        #region Instance Fields
        private byte[] window_;
        private int windowStart_;
        private int windowEnd_;
        private uint buffer_;
        private int bitsInBuffer_;
        #endregion
    }
    public class OutputWindow
    {
        #region Constants
        const int WindowSize = 1 << 15;
        const int WindowMask = WindowSize - 1;
        #endregion
        #region Instance Fields
        byte[] window = new byte[WindowSize];
        int windowEnd;
        int windowFilled;
        #endregion
        public void Write(int value)
        {
            if (windowFilled++ == WindowSize)
            {
                throw new InvalidOperationException("Window full");
            }
            window[windowEnd++] = (byte)value;
            windowEnd &= WindowMask;
        }
        private void SlowRepeat(int repStart, int length, int distance)
        {
            while (length-- > 0)
            {
                window[windowEnd++] = window[repStart++];
                windowEnd &= WindowMask;
                repStart &= WindowMask;
            }
        }
        public void Repeat(int length, int distance)
        {
            if ((windowFilled += length) > WindowSize)
            {
                throw new InvalidOperationException("Window full");
            }
            int repStart = (windowEnd - distance) & WindowMask;
            int border = WindowSize - length;
            if ((repStart <= border) && (windowEnd < border))
            {
                if (length <= distance)
                {
                    System.Array.Copy(window, repStart, window, windowEnd, length);
                    windowEnd += length;
                }
                else
                {
                    while (length-- > 0)
                    {
                        window[windowEnd++] = window[repStart++];
                    }
                }
            }
            else
            {
                SlowRepeat(repStart, length, distance);
            }
        }
        public int CopyStored(StreamManipulator input, int length)
        {
            length = Math.Min(Math.Min(length, WindowSize - windowFilled), input.AvailableBytes);
            int copied;
            int tailLen = WindowSize - windowEnd;
            if (length > tailLen)
            {
                copied = input.CopyBytes(window, windowEnd, tailLen);
                if (copied == tailLen)
                {
                    copied += input.CopyBytes(window, 0, length - tailLen);
                }
            }
            else
            {
                copied = input.CopyBytes(window, windowEnd, length);
            }
            windowEnd = (windowEnd + copied) & WindowMask;
            windowFilled += copied;
            return copied;
        }
        public void CopyDict(byte[] dictionary, int offset, int length)
        {
            if (dictionary == null)
            {
                throw new ArgumentNullException("dictionary");
            }
            if (windowFilled > 0)
            {
                throw new InvalidOperationException();
            }
            if (length > WindowSize)
            {
                offset += length - WindowSize;
                length = WindowSize;
            }
            System.Array.Copy(dictionary, offset, window, 0, length);
            windowEnd = length & WindowMask;
        }
        public int GetFreeSpace()
        {
            return WindowSize - windowFilled;
        }
        public int GetAvailable()
        {
            return windowFilled;
        }
        public int CopyOutput(byte[] output, int offset, int len)
        {
            int copyEnd = windowEnd;
            if (len > windowFilled)
            {
                len = windowFilled;
            }
            else
            {
                copyEnd = (windowEnd - windowFilled + len) & WindowMask;
            }
            int copied = len;
            int tailLen = len - copyEnd;
            if (tailLen > 0)
            {
                System.Array.Copy(window, WindowSize - tailLen, output, offset, tailLen);
                offset += tailLen;
                len = copyEnd;
            }
            System.Array.Copy(window, copyEnd - len, output, offset, len);
            windowFilled -= copied;
            if (windowFilled < 0)
            {
                throw new InvalidOperationException();
            }
            return copied;
        }
        public void Reset()
        {
            windowFilled = windowEnd = 0;
        }
    }
    public sealed class Adler32 : IChecksum
    {
        const uint BASE = 65521;
        public long Value
        {
            get
            {
                return checksum;
            }
        }
        public Adler32()
        {
            Reset();
        }
        public void Reset()
        {
            checksum = 1;
        }
        public void Update(int value)
        {
            uint s1 = checksum & 0xFFFF;
            uint s2 = checksum >> 16;
            s1 = (s1 + ((uint)value & 0xFF)) % BASE;
            s2 = (s1 + s2) % BASE;
            checksum = (s2 << 16) + s1;
        }
        public void Update(byte[] buffer)
        {
            if (buffer == null)
            {
                throw new ArgumentNullException("buffer");
            }
            Update(buffer, 0, buffer.Length);
        }
        public void Update(byte[] buffer, int offset, int count)
        {
            if (buffer == null)
            {
                throw new ArgumentNullException("buffer");
            }
            if (offset < 0)
            {
                throw new ArgumentOutOfRangeException("offset", "cannot be negative");
            }
            if (count < 0)
            {
                throw new ArgumentOutOfRangeException("count", "cannot be negative");
            }
            if (offset >= buffer.Length)
            {
                throw new ArgumentOutOfRangeException("offset", "not a valid index into buffer");
            }
            if (offset + count > buffer.Length)
            {
                throw new ArgumentOutOfRangeException("count", "exceeds buffer size");
            }
            uint s1 = checksum & 0xFFFF;
            uint s2 = checksum >> 16;
            while (count > 0)
            {
                int n = 3800;
                if (n > count)
                {
                    n = count;
                }
                count -= n;
                while (--n >= 0)
                {
                    s1 = s1 + (uint)(buffer[offset++] & 0xff);
                    s2 = s2 + s1;
                }
                s1 %= BASE;
                s2 %= BASE;
            }
            checksum = (s2 << 16) | s1;
        }
        #region Instance Fields
        uint checksum;
        #endregion
    }
    public interface IChecksum
    {
        long Value
        {
            get;
        }
        void Reset();
        void Update(int value);
        void Update(byte[] buffer);
        void Update(byte[] buffer, int offset, int count);
    }
    class InflaterDynHeader
    {
        #region Constants
        const int LNUM = 0;
        const int DNUM = 1;
        const int BLNUM = 2;
        const int BLLENS = 3;
        const int LENS = 4;
        const int REPS = 5;
        static readonly int[] repMin =
               {
                3, 3, 11
               };
        static readonly int[] repBits =
               {
                2, 3,  7
               };
        static readonly int[] BL_ORDER =
               {
                16, 17, 18, 0, 8, 7, 9, 6, 10, 5, 11, 4, 12, 3, 13, 2, 14, 1, 15
               };
        #endregion
        #region Constructors
        public InflaterDynHeader()
        {
        }
        #endregion
        public bool Decode(StreamManipulator input)
        {
        decode_loop:
            for (; ; )
            {
                switch (mode)
                {
                    case LNUM:
                        lnum = input.PeekBits(5);
                        if (lnum < 0)
                        {
                            return false;
                        }
                        lnum += 257;
                        input.DropBits(5);
                        mode = DNUM;
                        goto case DNUM;
                    case DNUM:
                        dnum = input.PeekBits(5);
                        if (dnum < 0)
                        {
                            return false;
                        }
                        dnum++;
                        input.DropBits(5);
                        num = lnum + dnum;
                        litdistLens = new byte[num];
                        mode = BLNUM;
                        goto case BLNUM;
                    case BLNUM:
                        blnum = input.PeekBits(4);
                        if (blnum < 0)
                        {
                            return false;
                        }
                        blnum += 4;
                        input.DropBits(4);
                        blLens = new byte[19];
                        ptr = 0;
                        mode = BLLENS;
                        goto case BLLENS;
                    case BLLENS:
                        while (ptr < blnum)
                        {
                            int len = input.PeekBits(3);
                            if (len < 0)
                            {
                                return false;
                            }
                            input.DropBits(3);
                            blLens[BL_ORDER[ptr]] = (byte)len;
                            ptr++;
                        }
                        blTree = new InflaterHuffmanTree(blLens);
                        blLens = null;
                        ptr = 0;
                        mode = LENS;
                        goto case LENS;
                    case LENS:
                        {
                            int symbol;
                            while (((symbol = blTree.GetSymbol(input)) & ~15) == 0)
                            {
                                /* Normal case: symbol in [0..15] */
                                litdistLens[ptr++] = lastLen = (byte)symbol;
                                if (ptr == num)
                                {
                                    /* Finished */
                                    return true;
                                }
                            }
                            /* need more input ? */
                            if (symbol < 0)
                            {
                                return false;
                            }
                            /* otherwise repeat code */
                            if (symbol >= 17)
                            {
                                /* repeat zero */
                                lastLen = 0;
                            }
                            else
                            {
                                if (ptr == 0)
                                {
                                    throw new SharpZipBaseException();
                                }
                            }
                            repSymbol = symbol - 16;
                        }
                        mode = REPS;
                        goto case REPS;
                    case REPS:
                        {
                            int bits = repBits[repSymbol];
                            int count = input.PeekBits(bits);
                            if (count < 0)
                            {
                                return false;
                            }
                            input.DropBits(bits);
                            count += repMin[repSymbol];
                            if (ptr + count > num)
                            {
                                throw new SharpZipBaseException();
                            }
                            while (count-- > 0)
                            {
                                litdistLens[ptr++] = lastLen;
                            }
                            if (ptr == num)
                            {
                                /* Finished */
                                return true;
                            }
                        }
                        mode = LENS;
                        goto decode_loop;
                }
            }
        }
        public InflaterHuffmanTree BuildLitLenTree()
        {
            byte[] litlenLens = new byte[lnum];
            Array.Copy(litdistLens, 0, litlenLens, 0, lnum);
            return new InflaterHuffmanTree(litlenLens);
        }
        public InflaterHuffmanTree BuildDistTree()
        {
            byte[] distLens = new byte[dnum];
            Array.Copy(litdistLens, lnum, distLens, 0, dnum);
            return new InflaterHuffmanTree(distLens);
        }
        #region Instance Fields
        byte[] blLens;
        byte[] litdistLens;
        InflaterHuffmanTree blTree;
        int mode;
        int lnum, dnum, blnum, num;
        int repSymbol;
        byte lastLen;
        int ptr;
        #endregion
    }
    public class InflaterHuffmanTree
    {
        #region Constants
        const int MAX_BITLEN = 15;
        #endregion
        #region Instance Fields
        short[] tree;
        #endregion
        public static InflaterHuffmanTree defLitLenTree;
        public static InflaterHuffmanTree defDistTree;
        static InflaterHuffmanTree()
        {
            try
            {
                byte[] codeLengths = new byte[288];
                int i = 0;
                while (i < 144)
                {
                    codeLengths[i++] = 8;
                }
                while (i < 256)
                {
                    codeLengths[i++] = 9;
                }
                while (i < 280)
                {
                    codeLengths[i++] = 7;
                }
                while (i < 288)
                {
                    codeLengths[i++] = 8;
                }
                defLitLenTree = new InflaterHuffmanTree(codeLengths);
                codeLengths = new byte[32];
                i = 0;
                while (i < 32)
                {
                    codeLengths[i++] = 5;
                }
                defDistTree = new InflaterHuffmanTree(codeLengths);
            }
            catch (Exception)
            {
                throw new SharpZipBaseException("InflaterHuffmanTree: static tree length illegal");
            }
        }
        #region Constructors
        public InflaterHuffmanTree(byte[] codeLengths)
        {
            BuildTree(codeLengths);
        }
        #endregion
        void BuildTree(byte[] codeLengths)
        {
            int[] blCount = new int[MAX_BITLEN + 1];
            int[] nextCode = new int[MAX_BITLEN + 1];
            for (int i = 0; i < codeLengths.Length; i++)
            {
                int bits = codeLengths[i];
                if (bits > 0)
                {
                    blCount[bits]++;
                }
            }
            int code = 0;
            int treeSize = 512;
            for (int bits = 1; bits <= MAX_BITLEN; bits++)
            {
                nextCode[bits] = code;
                code += blCount[bits] << (16 - bits);
                if (bits >= 10)
                {
                    /* We need an extra table for bit lengths >= 10. */
                    int start = nextCode[bits] & 0x1ff80;
                    int end = code & 0x1ff80;
                    treeSize += (end - start) >> (16 - bits);
                }
            }
            tree = new short[treeSize];
            int treePtr = 512;
            for (int bits = MAX_BITLEN; bits >= 10; bits--)
            {
                int end = code & 0x1ff80;
                code -= blCount[bits] << (16 - bits);
                int start = code & 0x1ff80;
                for (int i = start; i < end; i += 1 << 7)
                {
                    tree[DeflaterHuffman.BitReverse(i)] = (short)((-treePtr << 4) | bits);
                    treePtr += 1 << (bits - 9);
                }
            }
            for (int i = 0; i < codeLengths.Length; i++)
            {
                int bits = codeLengths[i];
                if (bits == 0)
                {
                    continue;
                }
                code = nextCode[bits];
                int revcode = DeflaterHuffman.BitReverse(code);
                if (bits <= 9)
                {
                    do
                    {
                        tree[revcode] = (short)((i << 4) | bits);
                        revcode += 1 << bits;
                    } while (revcode < 512);
                }
                else
                {
                    int subTree = tree[revcode & 511];
                    int treeLen = 1 << (subTree & 15);
                    subTree = -(subTree >> 4);
                    do
                    {
                        tree[subTree | (revcode >> 9)] = (short)((i << 4) | bits);
                        revcode += 1 << bits;
                    } while (revcode < treeLen);
                }
                nextCode[bits] = code + (1 << (16 - bits));
            }
        }
        public int GetSymbol(StreamManipulator input)
        {
            int lookahead, symbol;
            if ((lookahead = input.PeekBits(9)) >= 0)
            {
                if ((symbol = tree[lookahead]) >= 0)
                {
                    input.DropBits(symbol & 15);
                    return symbol >> 4;
                }
                int subtree = -(symbol >> 4);
                int bitlen = symbol & 15;
                if ((lookahead = input.PeekBits(bitlen)) >= 0)
                {
                    symbol = tree[subtree | (lookahead >> 9)];
                    input.DropBits(symbol & 15);
                    return symbol >> 4;
                }
                else
                {
                    int bits = input.AvailableBits;
                    lookahead = input.PeekBits(bits);
                    symbol = tree[subtree | (lookahead >> 9)];
                    if ((symbol & 15) <= bits)
                    {
                        input.DropBits(symbol & 15);
                        return symbol >> 4;
                    }
                    else
                    {
                        return -1;
                    }
                }
            }
            else
            {
                int bits = input.AvailableBits;
                lookahead = input.PeekBits(bits);
                symbol = tree[lookahead];
                if (symbol >= 0 && (symbol & 15) <= bits)
                {
                    input.DropBits(symbol & 15);
                    return symbol >> 4;
                }
                else
                {
                    return -1;
                }
            }
        }
    }
    public class DeflaterConstants
    {
        public const bool DEBUGGING = false;
        public const int STORED_BLOCK = 0;
        public const int STATIC_TREES = 1;
        public const int DYN_TREES = 2;
        public const int PRESET_DICT = 0x20;
        public const int DEFAULT_MEM_LEVEL = 8;
        public const int MAX_MATCH = 258;
        public const int MIN_MATCH = 3;
        public const int MAX_WBITS = 15;
        public const int WSIZE = 1 << MAX_WBITS;
        public const int WMASK = WSIZE - 1;
        public const int HASH_BITS = DEFAULT_MEM_LEVEL + 7;
        public const int HASH_SIZE = 1 << HASH_BITS;
        public const int HASH_MASK = HASH_SIZE - 1;
        public const int HASH_SHIFT = (HASH_BITS + MIN_MATCH - 1) / MIN_MATCH;
        public const int MIN_LOOKAHEAD = MAX_MATCH + MIN_MATCH + 1;
        public const int MAX_DIST = WSIZE - MIN_LOOKAHEAD;
        public const int PENDING_BUF_SIZE = 1 << (DEFAULT_MEM_LEVEL + 8);
        public static int MAX_BLOCK_SIZE = Math.Min(65535, PENDING_BUF_SIZE - 5);
        public const int DEFLATE_STORED = 0;
        public const int DEFLATE_FAST = 1;
        public const int DEFLATE_SLOW = 2;
        public static int[] GOOD_LENGTH =
                {
                 0, 4,  4,  4,  4,  8,   8,   8,   32,   32
                };
        public static int[] MAX_LAZY =
                {
                 0, 4,  5,  6,  4, 16,  16,  32,  128,  258
                };
        public static int[] NICE_LENGTH =
                {
                 0, 8, 16, 32, 16, 32, 128, 128,  258,  258
                };
        public static int[] MAX_CHAIN =
                {
                 0, 4,  8, 32, 16, 32, 128, 256, 1024, 4096
                };
        public static int[] COMPR_FUNC =
                {
                 0, 1,  1,  1,  1,  2,   2,   2,    2,    2
                };
    }
    public class DeflaterHuffman
    {
        const int BUFSIZE = 1 << (DeflaterConstants.DEFAULT_MEM_LEVEL + 6);
        const int LITERAL_NUM = 286;
        const int DIST_NUM = 30;
        const int BITLEN_NUM = 19;
        const int REP_3_6 = 16;
        const int REP_3_10 = 17;
        const int REP_11_138 = 18;
        const int EOF_SYMBOL = 256;
        static readonly int[] BL_ORDER =
                {
                 16, 17, 18, 0, 8, 7, 9, 6, 10, 5, 11, 4, 12, 3, 13, 2, 14, 1, 15
                };
        static readonly byte[] bit4Reverse =
                {
                 0,
                 8,
                 4,
                 12,
                 2,
                 10,
                 6,
                 14,
                 1,
                 9,
                 5,
                 13,
                 3,
                 11,
                 7,
                 15
                };
        static short[] staticLCodes;
        static byte[] staticLLength;
        static short[] staticDCodes;
        static byte[] staticDLength;
        class Tree
        {
            #region Instance Fields
            public short[] freqs;
            public byte[] length;
            public int minNumCodes;
            public int numCodes;
            short[] codes;
            int[] bl_counts;
            int maxLength;
            DeflaterHuffman dh;
            #endregion
            #region Constructors
            public Tree(DeflaterHuffman dh, int elems, int minCodes, int maxLength)
            {
                this.dh = dh;
                this.minNumCodes = minCodes;
                this.maxLength = maxLength;
                freqs = new short[elems];
                bl_counts = new int[maxLength];
            }
            #endregion
            public void Reset()
            {
                for (int i = 0; i < freqs.Length; i++)
                {
                    freqs[i] = 0;
                }
                codes = null;
                length = null;
            }
            public void WriteSymbol(int code)
            {
                dh.pending.WriteBits(codes[code] & 0xffff, length[code]);
            }
            public void CheckEmpty()
            {
                bool empty = true;
                for (int i = 0; i < freqs.Length; i++)
                {
                    if (freqs[i] != 0)
                    {
                        empty = false;
                    }
                }
                if (!empty)
                {
                    throw new SharpZipBaseException("!Empty");
                }
            }
            public void SetStaticCodes(short[] staticCodes, byte[] staticLengths)
            {
                codes = staticCodes;
                length = staticLengths;
            }
            public void BuildCodes()
            {
                int numSymbols = freqs.Length;
                int[] nextCode = new int[maxLength];
                int code = 0;
                codes = new short[freqs.Length];
                for (int bits = 0; bits < maxLength; bits++)
                {
                    nextCode[bits] = code;
                    code += bl_counts[bits] << (15 - bits);
                }
                for (int i = 0; i < numCodes; i++)
                {
                    int bits = length[i];
                    if (bits > 0)
                    {
                        codes[i] = BitReverse(nextCode[bits - 1]);
                        nextCode[bits - 1] += 1 << (16 - bits);
                    }
                }
            }
            public void BuildTree()
            {
                int numSymbols = freqs.Length;
                int[] heap = new int[numSymbols];
                int heapLen = 0;
                int maxCode = 0;
                for (int n = 0; n < numSymbols; n++)
                {
                    int freq = freqs[n];
                    if (freq != 0)
                    {
                        int pos = heapLen++;
                        int ppos;
                        while (pos > 0 && freqs[heap[ppos = (pos - 1) / 2]] > freq)
                        {
                            heap[pos] = heap[ppos];
                            pos = ppos;
                        }
                        heap[pos] = n;
                        maxCode = n;
                    }
                }
                while (heapLen < 2)
                {
                    int node = maxCode < 2 ? ++maxCode : 0;
                    heap[heapLen++] = node;
                }
                numCodes = Math.Max(maxCode + 1, minNumCodes);
                int numLeafs = heapLen;
                int[] childs = new int[4 * heapLen - 2];
                int[] values = new int[2 * heapLen - 1];
                int numNodes = numLeafs;
                for (int i = 0; i < heapLen; i++)
                {
                    int node = heap[i];
                    childs[2 * i] = node;
                    childs[2 * i + 1] = -1;
                    values[i] = freqs[node] << 8;
                    heap[i] = i;
                }
                do
                {
                    int first = heap[0];
                    int last = heap[--heapLen];
                    int ppos = 0;
                    int path = 1;
                    while (path < heapLen)
                    {
                        if (path + 1 < heapLen && values[heap[path]] > values[heap[path + 1]])
                        {
                            path++;
                        }
                        heap[ppos] = heap[path];
                        ppos = path;
                        path = path * 2 + 1;
                    }
                    int lastVal = values[last];
                    while ((path = ppos) > 0 && values[heap[ppos = (path - 1) / 2]] > lastVal)
                    {
                        heap[path] = heap[ppos];
                    }
                    heap[path] = last;
                    int second = heap[0];
                    last = numNodes++;
                    childs[2 * last] = first;
                    childs[2 * last + 1] = second;
                    int mindepth = Math.Min(values[first] & 0xff, values[second] & 0xff);
                    values[last] = lastVal = values[first] + values[second] - mindepth + 1;
                    ppos = 0;
                    path = 1;
                    while (path < heapLen)
                    {
                        if (path + 1 < heapLen && values[heap[path]] > values[heap[path + 1]])
                        {
                            path++;
                        }
                        heap[ppos] = heap[path];
                        ppos = path;
                        path = ppos * 2 + 1;
                    }
                    while ((path = ppos) > 0 && values[heap[ppos = (path - 1) / 2]] > lastVal)
                    {
                        heap[path] = heap[ppos];
                    }
                    heap[path] = last;
                } while (heapLen > 1);
                if (heap[0] != childs.Length / 2 - 1)
                {
                    throw new SharpZipBaseException("Heap invariant violated");
                }
                BuildLength(childs);
            }
            public int GetEncodedLength()
            {
                int len = 0;
                for (int i = 0; i < freqs.Length; i++)
                {
                    len += freqs[i] * length[i];
                }
                return len;
            }
            public void CalcBLFreq(Tree blTree)
            {
                int max_count;               /* max repeat count */
                int min_count;               /* min repeat count */
                int count;                   /* repeat count of the current code */
                int curlen = -1;             /* length of current code */
                int i = 0;
                while (i < numCodes)
                {
                    count = 1;
                    int nextlen = length[i];
                    if (nextlen == 0)
                    {
                        max_count = 138;
                        min_count = 3;
                    }
                    else
                    {
                        max_count = 6;
                        min_count = 3;
                        if (curlen != nextlen)
                        {
                            blTree.freqs[nextlen]++;
                            count = 0;
                        }
                    }
                    curlen = nextlen;
                    i++;
                    while (i < numCodes && curlen == length[i])
                    {
                        i++;
                        if (++count >= max_count)
                        {
                            break;
                        }
                    }
                    if (count < min_count)
                    {
                        blTree.freqs[curlen] += (short)count;
                    }
                    else if (curlen != 0)
                    {
                        blTree.freqs[REP_3_6]++;
                    }
                    else if (count <= 10)
                    {
                        blTree.freqs[REP_3_10]++;
                    }
                    else
                    {
                        blTree.freqs[REP_11_138]++;
                    }
                }
            }
            public void WriteTree(Tree blTree)
            {
                int max_count;
                int min_count;
                int count;
                int curlen = -1;
                int i = 0;
                while (i < numCodes)
                {
                    count = 1;
                    int nextlen = length[i];
                    if (nextlen == 0)
                    {
                        max_count = 138;
                        min_count = 3;
                    }
                    else
                    {
                        max_count = 6;
                        min_count = 3;
                        if (curlen != nextlen)
                        {
                            blTree.WriteSymbol(nextlen);
                            count = 0;
                        }
                    }
                    curlen = nextlen;
                    i++;
                    while (i < numCodes && curlen == length[i])
                    {
                        i++;
                        if (++count >= max_count)
                        {
                            break;
                        }
                    }
                    if (count < min_count)
                    {
                        while (count-- > 0)
                        {
                            blTree.WriteSymbol(curlen);
                        }
                    }
                    else if (curlen != 0)
                    {
                        blTree.WriteSymbol(REP_3_6);
                        dh.pending.WriteBits(count - 3, 2);
                    }
                    else if (count <= 10)
                    {
                        blTree.WriteSymbol(REP_3_10);
                        dh.pending.WriteBits(count - 3, 3);
                    }
                    else
                    {
                        blTree.WriteSymbol(REP_11_138);
                        dh.pending.WriteBits(count - 11, 7);
                    }
                }
            }
            void BuildLength(int[] childs)
            {
                this.length = new byte[freqs.Length];
                int numNodes = childs.Length / 2;
                int numLeafs = (numNodes + 1) / 2;
                int overflow = 0;
                for (int i = 0; i < maxLength; i++)
                {
                    bl_counts[i] = 0;
                }
                int[] lengths = new int[numNodes];
                lengths[numNodes - 1] = 0;
                for (int i = numNodes - 1; i >= 0; i--)
                {
                    if (childs[2 * i + 1] != -1)
                    {
                        int bitLength = lengths[i] + 1;
                        if (bitLength > maxLength)
                        {
                            bitLength = maxLength;
                            overflow++;
                        }
                        lengths[childs[2 * i]] = lengths[childs[2 * i + 1]] = bitLength;
                    }
                    else
                    {
                        int bitLength = lengths[i];
                        bl_counts[bitLength - 1]++;
                        this.length[childs[2 * i]] = (byte)lengths[i];
                    }
                }
                if (overflow == 0)
                {
                    return;
                }
                int incrBitLen = maxLength - 1;
                do
                {
                    while (bl_counts[--incrBitLen] == 0)
                        ;
                    do
                    {
                        bl_counts[incrBitLen]--;
                        bl_counts[++incrBitLen]++;
                        overflow -= 1 << (maxLength - 1 - incrBitLen);
                    } while (overflow > 0 && incrBitLen < maxLength - 1);
                } while (overflow > 0);
                /* We may have overshot above.  Move some nodes from maxLength to
                * maxLength-1 in that case.
            */
                bl_counts[maxLength - 1] += overflow;
                bl_counts[maxLength - 2] -= overflow;
                /* Now recompute all bit lengths, scanning in increasing
                * frequency.  It is simpler to reconstruct all lengths instead of
                * fixing only the wrong ones. This idea is taken from 'ar'
                * written by Haruhiko Okumura.
                *
                * The nodes were inserted with decreasing frequency into the childs
                * array.
            */
                int nodePtr = 2 * numLeafs;
                for (int bits = maxLength; bits != 0; bits--)
                {
                    int n = bl_counts[bits - 1];
                    while (n > 0)
                    {
                        int childPtr = 2 * childs[nodePtr++];
                        if (childs[childPtr + 1] == -1)
                        {
                            length[childs[childPtr]] = (byte)bits;
                            n--;
                        }
                    }
                }
            }
        }
        #region Instance Fields
        public DeflaterPending pending;
        Tree literalTree;
        Tree distTree;
        Tree blTree;
        short[] d_buf;
        byte[] l_buf;
        int last_lit;
        int extra_bits;
        #endregion
        static DeflaterHuffman()
        {
            staticLCodes = new short[LITERAL_NUM];
            staticLLength = new byte[LITERAL_NUM];
            int i = 0;
            while (i < 144)
            {
                staticLCodes[i] = BitReverse((0x030 + i) << 8);
                staticLLength[i++] = 8;
            }
            while (i < 256)
            {
                staticLCodes[i] = BitReverse((0x190 - 144 + i) << 7);
                staticLLength[i++] = 9;
            }
            while (i < 280)
            {
                staticLCodes[i] = BitReverse((0x000 - 256 + i) << 9);
                staticLLength[i++] = 7;
            }
            while (i < LITERAL_NUM)
            {
                staticLCodes[i] = BitReverse((0x0c0 - 280 + i) << 8);
                staticLLength[i++] = 8;
            }
            staticDCodes = new short[DIST_NUM];
            staticDLength = new byte[DIST_NUM];
            for (i = 0; i < DIST_NUM; i++)
            {
                staticDCodes[i] = BitReverse(i << 11);
                staticDLength[i] = 5;
            }
        }
        public DeflaterHuffman(DeflaterPending pending)
        {
            this.pending = pending;
            literalTree = new Tree(this, LITERAL_NUM, 257, 15);
            distTree = new Tree(this, DIST_NUM, 1, 15);
            blTree = new Tree(this, BITLEN_NUM, 4, 7);
            d_buf = new short[BUFSIZE];
            l_buf = new byte[BUFSIZE];
        }
        public void Reset()
        {
            last_lit = 0;
            extra_bits = 0;
            literalTree.Reset();
            distTree.Reset();
            blTree.Reset();
        }
        public void SendAllTrees(int blTreeCodes)
        {
            blTree.BuildCodes();
            literalTree.BuildCodes();
            distTree.BuildCodes();
            pending.WriteBits(literalTree.numCodes - 257, 5);
            pending.WriteBits(distTree.numCodes - 1, 5);
            pending.WriteBits(blTreeCodes - 4, 4);
            for (int rank = 0; rank < blTreeCodes; rank++)
            {
                pending.WriteBits(blTree.length[BL_ORDER[rank]], 3);
            }
            literalTree.WriteTree(blTree);
            distTree.WriteTree(blTree);
#if DebugDeflation
                 if (DeflaterConstants.DEBUGGING)
                    {
                     blTree.CheckEmpty();
                    }
#endif
        }
        public void CompressBlock()
        {
            for (int i = 0; i < last_lit; i++)
            {
                int litlen = l_buf[i] & 0xff;
                int dist = d_buf[i];
                if (dist-- != 0)
                {
                    int lc = Lcode(litlen);
                    literalTree.WriteSymbol(lc);
                    int bits = (lc - 261) / 4;
                    if (bits > 0 && bits <= 5)
                    {
                        pending.WriteBits(litlen & ((1 << bits) - 1), bits);
                    }
                    int dc = Dcode(dist);
                    distTree.WriteSymbol(dc);
                    bits = dc / 2 - 1;
                    if (bits > 0)
                    {
                        pending.WriteBits(dist & ((1 << bits) - 1), bits);
                    }
                }
                else
                {
                    literalTree.WriteSymbol(litlen);
                }
            }
#if DebugDeflation
                 if (DeflaterConstants.DEBUGGING)
                    {
                     Console.Write("EOF: ");
                    }
#endif
            literalTree.WriteSymbol(EOF_SYMBOL);
#if DebugDeflation
                 if (DeflaterConstants.DEBUGGING)
                    {
                     literalTree.CheckEmpty();
                     distTree.CheckEmpty();
                    }
#endif
        }
        public void FlushStoredBlock(byte[] stored, int storedOffset, int storedLength, bool lastBlock)
        {
#if DebugDeflation
#endif
            pending.WriteBits((DeflaterConstants.STORED_BLOCK << 1) + (lastBlock ? 1 : 0), 3);
            pending.AlignToByte();
            pending.WriteShort(storedLength);
            pending.WriteShort(~storedLength);
            pending.WriteBlock(stored, storedOffset, storedLength);
            Reset();
        }
        public void FlushBlock(byte[] stored, int storedOffset, int storedLength, bool lastBlock)
        {
            literalTree.freqs[EOF_SYMBOL]++;
            literalTree.BuildTree();
            distTree.BuildTree();
            literalTree.CalcBLFreq(blTree);
            distTree.CalcBLFreq(blTree);
            blTree.BuildTree();
            int blTreeCodes = 4;
            for (int i = 18; i > blTreeCodes; i--)
            {
                if (blTree.length[BL_ORDER[i]] > 0)
                {
                    blTreeCodes = i + 1;
                }
            }
            int opt_len = 14 + blTreeCodes * 3 + blTree.GetEncodedLength() +
            literalTree.GetEncodedLength() + distTree.GetEncodedLength() +
            extra_bits;
            int static_len = extra_bits;
            for (int i = 0; i < LITERAL_NUM; i++)
            {
                static_len += literalTree.freqs[i] * staticLLength[i];
            }
            for (int i = 0; i < DIST_NUM; i++)
            {
                static_len += distTree.freqs[i] * staticDLength[i];
            }
            if (opt_len >= static_len)
            {
                opt_len = static_len;
            }
            if (storedOffset >= 0 && storedLength + 4 < opt_len >> 3)
            {
                FlushStoredBlock(stored, storedOffset, storedLength, lastBlock);
            }
            else if (opt_len == static_len)
            {
                pending.WriteBits((DeflaterConstants.STATIC_TREES << 1) + (lastBlock ? 1 : 0), 3);
                literalTree.SetStaticCodes(staticLCodes, staticLLength);
                distTree.SetStaticCodes(staticDCodes, staticDLength);
                CompressBlock();
                Reset();
            }
            else
            {
                pending.WriteBits((DeflaterConstants.DYN_TREES << 1) + (lastBlock ? 1 : 0), 3);
                SendAllTrees(blTreeCodes);
                CompressBlock();
                Reset();
            }
        }
        public bool IsFull()
        {
            return last_lit >= BUFSIZE;
        }
        public bool TallyLit(int literal)
        {
            d_buf[last_lit] = 0;
            l_buf[last_lit++] = (byte)literal;
            literalTree.freqs[literal]++;
            return IsFull();
        }
        public bool TallyDist(int distance, int length)
        {
            d_buf[last_lit] = (short)distance;
            l_buf[last_lit++] = (byte)(length - 3);
            int lc = Lcode(length - 3);
            literalTree.freqs[lc]++;
            if (lc >= 265 && lc < 285)
            {
                extra_bits += (lc - 261) / 4;
            }
            int dc = Dcode(distance - 1);
            distTree.freqs[dc]++;
            if (dc >= 4)
            {
                extra_bits += dc / 2 - 1;
            }
            return IsFull();
        }
        public static short BitReverse(int toReverse)
        {
            return (short)(bit4Reverse[toReverse & 0xF] << 12 |
            bit4Reverse[(toReverse >> 4) & 0xF] << 8 |
            bit4Reverse[(toReverse >> 8) & 0xF] << 4 |
            bit4Reverse[toReverse >> 12]);
        }
        static int Lcode(int length)
        {
            if (length == 255)
            {
                return 285;
            }
            int code = 257;
            while (length >= 8)
            {
                code += 4;
                length >>= 1;
            }
            return code + length;
        }
        static int Dcode(int distance)
        {
            int code = 0;
            while (distance >= 4)
            {
                code += 2;
                distance >>= 1;
            }
            return code + distance;
        }
    }
    public class PendingBuffer
    {
        #region Instance Fields
        byte[] buffer_;
        int start;
        int end;
        uint bits;
        int bitCount;
        #endregion
        #region Constructors
        public PendingBuffer(): this(4096)
        {
        }
        public PendingBuffer(int bufferSize)
        {
            buffer_ = new byte[bufferSize];
        }
        #endregion
        public void Reset()
        {
            start = end = bitCount = 0;
        }
        public void WriteByte(int value)
        {
            buffer_[end++] = unchecked((byte)value);
        }
        public void WriteShort(int value)
        {
            buffer_[end++] = unchecked((byte)value);
            buffer_[end++] = unchecked((byte)(value >> 8));
        }
        public void WriteInt(int value)
        {
            buffer_[end++] = unchecked((byte)value);
            buffer_[end++] = unchecked((byte)(value >> 8));
            buffer_[end++] = unchecked((byte)(value >> 16));
            buffer_[end++] = unchecked((byte)(value >> 24));
        }
        public void WriteBlock(byte[] block, int offset, int length)
        {
            System.Array.Copy(block, offset, buffer_, end, length);
            end += length;
        }
        public int BitCount
        {
            get
            {
                return bitCount;
            }
        }
        public void AlignToByte()
        {
            if (bitCount > 0)
            {
                buffer_[end++] = unchecked((byte)bits);
                if (bitCount > 8)
                {
                    buffer_[end++] = unchecked((byte)(bits >> 8));
                }
            }
            bits = 0;
            bitCount = 0;
        }
        public void WriteBits(int b, int count)
        {
            bits |= (uint)(b << bitCount);
            bitCount += count;
            if (bitCount >= 16)
            {
                buffer_[end++] = unchecked((byte)bits);
                buffer_[end++] = unchecked((byte)(bits >> 8));
                bits >>= 16;
                bitCount -= 16;
            }
        }
        public void WriteShortMSB(int s)
        {
            buffer_[end++] = unchecked((byte)(s >> 8));
            buffer_[end++] = unchecked((byte)s);
        }
        public bool IsFlushed
        {
            get
            {
                return end == 0;
            }
        }
        public int Flush(byte[] output, int offset, int length)
        {
            if (bitCount >= 8)
            {
                buffer_[end++] = unchecked((byte)bits);
                bits >>= 8;
                bitCount -= 8;
            }
            if (length > end - start)
            {
                length = end - start;
                System.Array.Copy(buffer_, start, output, offset, length);
                start = 0;
                end = 0;
            }
            else
            {
                System.Array.Copy(buffer_, start, output, offset, length);
                start += length;
            }
            return length;
        }
        public byte[] ToByteArray()
        {
            byte[] result = new byte[end - start];
            System.Array.Copy(buffer_, start, result, 0, result.Length);
            start = 0;
            end = 0;
            return result;
        }
    }
    public class DeflaterPending : PendingBuffer
    {
        public DeflaterPending():base(DeflaterConstants.PENDING_BUF_SIZE)
        {
        }
    }
    public class SharpZipBaseException : ApplicationException
    {
        protected SharpZipBaseException(SerializationInfo info, StreamingContext context)
            : base(info, context)
        {
        }
        public SharpZipBaseException()
        {
        }
        public SharpZipBaseException(string message)
            : base(message)
        {
        }
        public SharpZipBaseException(string message, Exception innerException)
            : base(message, innerException)
        {
        }
    }
}

 
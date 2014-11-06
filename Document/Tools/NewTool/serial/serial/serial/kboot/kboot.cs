using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;
using System.IO.Ports;

namespace serial
{

    public class kboot
    {
        SerialPort sp;
        //callback typedef
        public delegate void OnSystemlog(string log);
        //callback pointer
        public event OnSystemlog log;

        private enum PacketType
        {
            kFramingPacketType_Ack = 0xA1,
            kFramingPacketType_Nak = 0xA2,
            kFramingPacketType_AckAbort = 0xA3,
            kFramingPacketType_Command = 0xA4,
            kFramingPacketType_Data = 0xA5,
            kFramingPacketType_Ping = 0xA6,
            kFramingPacketType_PingResponse = 0xA7,
        };

        private enum CommandTag
        {
            FlashEraseAll = 0x01,
            FlashEraseRegion = 0x02,
            ReadMemory = 0x03,
            WriteMemory = 0x04,
            FlashSecurityDisable = 0x06,
            GetProperty = 0x07,
            Execute = 0x09,
            Reset = 0x0B,
            SetProperty = 0x0C,
            FlashEraseAllUnsecure = 0x0D,
        };

        public kboot(object ConnObject)
        {
          //  Thread kBootThread = new Thread(new ThreadStart(kBootMain));
            sp = (SerialPort)ConnObject;
          //  kBootThread.Start();
        }

        private void _WriteData(byte[] buffer, int count)
        {
            sp.DiscardOutBuffer();
            sp.Write(buffer, 0, count);
        }

        public bool Ping()
        {
            bool r = false;
            byte[] outBuffer = new byte[2];
            byte[] inBuffer;
            outBuffer[0] = 0x5A;
            outBuffer[1] = (byte)PacketType.kFramingPacketType_Ping;

            sp.DiscardInBuffer();
            _WriteData(outBuffer, 2);

            r = WaitForAck(10, 10);

            if (r == false)
            {
                Console.WriteLine("Ping ERROR");
            }
           //inBuffer = GetData(10, 20);

           //if (inBuffer.Length == 10)
           //{
           //    if ((inBuffer[0] == 0x5A) && (inBuffer[1] == 0xA7))
           //    {
           //        Console.WriteLine("Cmd-Ping Receive Succ!");
           //        r = true;
           //    }
           //}
            return r;
        }

        public bool FlashEraseAll()
        {

            bool r = false;
            List<byte> List = new List<byte>();
            List.Add(0x5A);
            List.Add(0xA4);
            List.Add(0x04);
            List.Add(0x00);
            List.Add(0x01);
            List.Add(0x00);
            List.Add(0x00);
            List.Add(0x00);
            // Insert CRC
            byte[] crc = BitConverter.GetBytes(crc16(List.ToArray(), 0, List.Count, 0x1021));
            List.Insert(4, crc[0]);
            List.Insert(5, crc[1]);

            //send
            sp.DiscardInBuffer();
            _WriteData(List.ToArray(), List.Count);

            // read in data 
            if(WaitForAck(200, 20))
            {
                r = true;
            }

            //ack
            byte[] crc3 = new byte[2];
            crc3[0] = 0x5A;
            crc3[1] = 0xA1;
            sp.Write(crc, 0, 2);

            return r;
        }

        private bool WaitForAck(int timeOut, UInt32 len)
        {
            while (timeOut != 0)
            {
                Thread.Sleep(1);
                System.Windows.Forms.Application.DoEvents();
                if (sp.BytesToRead == len)
                {
                    byte[] dat = new byte[sp.BytesToRead];
                    sp.Read(dat, 0, dat.Length);
                    return true;
                }
                timeOut--;
            }
            Console.WriteLine("Time out " + sp.BytesToRead.ToString());
            return false;
        }

        private byte[] GetData(UInt32 len, UInt32 timeOut)
        {
            byte[] data = null;
            while (timeOut != 0)
            {
                Thread.Sleep(1);
                if (sp.BytesToRead == len)
                {
                    data = new byte[sp.BytesToRead];
                    sp.Read(data, 0, data.Length);
                    return data;
                }
                timeOut--;
            }
            return data;
        }

        public bool FlashEraseAllUnsecure()
        {
            bool r = false;
            List<byte> List = new List<byte>();
            List.Clear();
            List.Add(0x5A);
            List.Add(0xA4);
            List.Add(0x04);
            List.Add(0x00);
            List.Add(0x0D);
            List.Add(0x00);
            List.Add(0xCC);
            List.Add(0x00);

            // Insert CRC
            byte[] crc = BitConverter.GetBytes(crc16(List.ToArray(), 0, List.Count, 0x1021));
            List.Insert(4, crc[0]);
            List.Insert(5, crc[1]);

            // write data
            sp.DiscardOutBuffer();
            sp.DiscardInBuffer();
            sp.Write(List.ToArray(), 0, List.Count);

            // read in data 
            if (WaitForAck(100, 20) == true)
            {
                //ack
                byte[] crc3 = new byte[2];
                crc3[0] = 0x5A;
                crc3[1] = 0xA1;
                sp.Write(crc, 0, crc3.Length);
                r = true;
            }
            log("Inject Command:FlashEraseAllUnsecure " + r.ToString());
            return r;
        }

        #region GetProperty
        private UInt32 GetProperty(int tag)
        {
            List<byte> List = new List<byte>();
            List.Add(0x5A);
            List.Add( (byte)PacketType.kFramingPacketType_Command);
            List.Add(0x08);
            List.Add(0x00);
            List.Add((byte)CommandTag.GetProperty);
            List.Add(0x00);
            List.Add(0x00);
            List.Add(0x01);
            List.Add( (byte)tag);
            List.Add(0x00);
            List.Add(0x00);
            List.Add(0x00);

            // Insert CRC
            byte[] crc = BitConverter.GetBytes(crc16(List.ToArray(), 0, List.Count, 0x1021));
            List.Insert(4, crc[0]);
            List.Insert(5, crc[1]);
            // Send data
            sp.DiscardInBuffer();
            _WriteData(List.ToArray(), List.Count);

            //Receive
            System.Threading.Thread.Sleep(10);
            int len = sp.BytesToRead;
            byte[] RevData = new byte[len];
            sp.Read(RevData, 0, len);
           
            //ack
            crc[0] = 0x5A;
            crc[1] = 0xA1;
            sp.Write(crc, 0, 2);

            if (RevData.Length == 20)
            {
                return BitConverter.ToUInt32(RevData, 16);
            }
            return 0;
        }
        #endregion

        public UInt32 GetCurrentVersion()
        {
             return GetProperty(1);
        }
        public UInt32 GetFlashSizeInBytes()
        {
            return GetProperty(4);
        }
        public UInt32 GetRAMSizeInBytes()
        {
            return GetProperty(0x0F);
        }
        public UInt32 GetSystemDeviceId()
        {
            return GetProperty(0x10);
        }




        public bool Reset()
        {
            List<byte> List = new List<byte>();
            List.Add(0x5A);
            List.Add((byte)PacketType.kFramingPacketType_Command);
            //frame len
            List.Add(0x04);
            List.Add(0x00);
            //tag flag reseved
            List.Add((byte)CommandTag.Reset);
            List.Add(0x00);
            List.Add(0x00);
            //param count
            List.Add(0x00);
            // Insert CRC
            byte[] crc = BitConverter.GetBytes(crc16(List.ToArray(), 0, List.Count, 0x1021));
            List.Insert(4, crc[0]);
            List.Insert(5, crc[1]);

            // Send data
            sp.DiscardInBuffer();
            _WriteData(List.ToArray(), List.Count);

            //Receive
            if (WaitForAck(30, 20) == false)
            {
                log("Inject Command Reset Error:");
                return false;
            }
            //ack
            _WriteData(crc, 2);

            log("Inject Command Reset OK");
            return true;
        }

        public bool Execute(UInt32 JumpAddr, UInt32 ArgumentWord, UInt32 StackPointerAddr)
        {
            // some time, StackPointer cannot excess Limit
            StackPointerAddr -= 1;

            List<byte> List = new List<byte>();
            List.Add(0x5A);
            List.Add((byte)PacketType.kFramingPacketType_Command);
            //frame len
            List.Add(0x10);
            List.Add(0x00);
            //tag flag reseved
            List.Add((byte)CommandTag.Execute);
            List.Add(0x00);
            List.Add(0x00);
            //param count
            List.Add(0x03);
            // JumpAddr
            List.AddRange(BitConverter.GetBytes(JumpAddr));
            //ArgumentWord
            List.AddRange(BitConverter.GetBytes(ArgumentWord));
            //StackPointerAddr
            List.AddRange(BitConverter.GetBytes(StackPointerAddr));
            // Insert CRC
            byte[] crc = BitConverter.GetBytes(crc16(List.ToArray(), 0, List.Count, 0x1021));
            List.Insert(4, crc[0]);
            List.Insert(5, crc[1]);

            // Send data
            sp.DiscardInBuffer();
            _WriteData(List.ToArray(), List.Count);

            //Receive
            if (WaitForAck(30, 20) == false)
            {
                log("Inject Command Execute Error:");
                return false;
            }

            //ack
            sp.DiscardOutBuffer();
            sp.DiscardInBuffer();
            crc[0] = 0x5A;
            crc[1] = 0xA1;
            sp.Write(crc, 0, 2);

            log("Inject Command Execute OK");

            return true;
        }

        public bool WriteMemory(byte[] data, int startAddr, int byteCount)
        {

            List<byte> List = new List<byte>();
            List.Add(0x5A);
            List.Add((byte)PacketType.kFramingPacketType_Command);
            //frame len
            List.Add(0x0C);
            List.Add(0x00);
            //tag flag reseved
            List.Add((byte)CommandTag.WriteMemory);
            List.Add(0x00);
            List.Add(0x00);
            //param count
            List.Add(0x02); 
            // StartAddr
            List.AddRange( BitConverter.GetBytes(startAddr));
            //byteCount
            List.AddRange(BitConverter.GetBytes(byteCount));

            // Insert CRC
            byte[] crc = BitConverter.GetBytes(crc16(List.ToArray(), 0, List.Count, 0x1021));
            List.Insert(4, crc[0]);
            List.Insert(5, crc[1]);

            // Send data
            sp.DiscardInBuffer();
            _WriteData(List.ToArray(), List.Count);

            //Receive
            if (!WaitForAck(400, 20))
            {
                log("Inject Command WriteMemory Ack Error:");
                return false;
            }

            sp.DiscardInBuffer();
            sp.DiscardOutBuffer();

            //ack
           crc[0] = 0x5A;
            crc[1] = 0xA1;
            sp.Write(crc, 0, 2);

            Thread.Sleep(10);
            sp.DiscardInBuffer();
            sp.DiscardOutBuffer();

            UInt32 LoopCounter = (UInt32)byteCount;
            UInt32 PacketDataSize;
            uint p = 0;
            while (LoopCounter != 0)
            {
                List.Clear();

                if (LoopCounter > 32) PacketDataSize = 32; else PacketDataSize = LoopCounter;
                List.Add(0x5A); List.Add(0xA5);
                List.AddRange(BitConverter.GetBytes((UInt16)PacketDataSize));
                byte[] payload = data.Skip((int)p).Take((int)PacketDataSize).ToArray();
                List.AddRange(payload);

                // Insert CRC
                byte[] crc1 = BitConverter.GetBytes(crc16(List.ToArray(), 0, List.Count, 0x1021));
                List.Insert(4, crc1[0]);
                List.Insert(5, crc1[1]);

                #region SendData
                // Send data
                sp.DiscardInBuffer();
                sp.DiscardOutBuffer();
                sp.Write(List.ToArray(), 0, List.Count);
                if (List.Count != 38)
                {
                    log("kboot: Last packet");
                    Thread.Sleep(20);
                    if (sp.BytesToRead == 20)
                    {
                        crc[0] = 0x5A;
                        crc[1] = 0xA1;
                        sp.Write(crc, 0, 2);
                        log("Kboot: Download OK");
                        return true;
                    }
                    return false;
                }

                //Wait ack
                if (WaitForAck(1000, 2) == false)
                {
                    log("Kboot Write Memory No Respond");
                    return false;
                }

                if ((p % 1024) == 0)
                {
                    log((p).ToString() + "Byte Transfered");
                }
                
                #endregion
                p += PacketDataSize;
                LoopCounter -= PacketDataSize;
            }
            return true;
        }

        ushort crc16(byte[] data, int start, int length, ushort poly)
        {
            ushort crc = 0;
            while (length-- > 0)
            {
                byte bt = data[start++];
                for (int i = 0; i < 8; i++)
                {
                    bool b1 = (crc & 0x8000U) != 0;
                    bool b2 = (bt & 0x80U) != 0;
                    if (b1 != b2) crc = (ushort)((crc << 1) ^ poly);
                    else crc <<= 1;
                    bt <<= 1;
                }
            }
            return crc;
        }

    }
}

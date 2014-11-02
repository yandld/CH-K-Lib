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


        public struct Properties
        {
            UInt32 AvailablePeripherals;
            UInt32 FlashStartAddress;
            UInt32 FlashSizeInBytes;
            UInt32 FlashSectorSize;
            UInt32 FlashBlockCount;
            UInt32 AvailableCommands;
            UInt32 VerifyWrites;
            UInt32 MaxPacketSize;
            UInt32 RAMStartAddress;
            UInt32 RAMSizeInBytes;
            UInt32 SystemDeviceId;
            UInt32 FlashSecurityState;
        }


        public kboot(object ConnObject)
        {
          //  Thread kBootThread = new Thread(new ThreadStart(kBootMain));
            sp = (SerialPort)ConnObject;
          //  kBootThread.Start();

        }


        #region Ping
        public bool Ping()
        {
            byte[] PingData = new byte[64];
            PingData[0] = 0x5A;
            PingData[1] = (byte)PacketType.kFramingPacketType_Ping;
            sp.DiscardOutBuffer();
            sp.DiscardInBuffer();
            sp.Write(PingData, 0, 2);
            System.Threading.Thread.Sleep(60);
            int len = sp.BytesToRead;
            if (len > 10)
            {
                return false;
            }
            sp.Read(PingData, 0, len);
            if (len == 10)
            {
                if (PingData[1] == (byte)PacketType.kFramingPacketType_PingResponse)
                {
                    return true;
                }
                // sys_log("PING OK! Version:" + PingData[4].ToString() + "." + PingData[3].ToString());
            }
            return false;
        }
        #endregion

        public bool FlashEraseAll()
        {
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

            // read in data 
            sp.DiscardInBuffer();
            System.Threading.Thread.Sleep(100);
            int len = sp.BytesToRead;
            byte[] rev = new byte[len];
            sp.Read(rev, 0, len);

            //return 
            if (len == 20)
            {
                //ack
                byte[] crc3 = new byte[2];
                sp.DiscardOutBuffer();
                crc3[0] = 0x5A;
                crc3[1] = 0xA1;
                sp.Write(crc, 0, 2);
                Thread.Sleep(10);
                return true;
            }
            return false;
        }

        #region FlashEraseAllUnsecure
        public bool FlashEraseAllUnsecure()
        {
            List<byte> List = new List<byte>();
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
            sp.Write(List.ToArray(), 0, List.Count);

            // read in data 
            sp.DiscardInBuffer();
            System.Threading.Thread.Sleep(100);
            int len = sp.BytesToRead;
            byte[] rev = new byte[len];
            sp.Read(rev, 0, len);

            //return 
            if (len == 20)
            {
                //ack
                byte[] crc3 = new byte[2];
                sp.DiscardOutBuffer();
                crc3[0] = 0x5A;
                crc3[1] = 0xA1;
                sp.Write(crc, 0, 2);
                Thread.Sleep(10);
                return true;
            }
            return false;
        }
        #endregion

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
            sp.DiscardOutBuffer();
            sp.Write(List.ToArray(), 0, List.ToArray().Length);

            //Receive
            sp.DiscardInBuffer();
            System.Threading.Thread.Sleep(20);
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


        private bool WaitForAck(int timeOut)
        {

            while (timeOut != 0)
            {
                Thread.Sleep(1);
                if (sp.BytesToRead == 2)
                {
                    byte[] r = new byte[sp.BytesToRead]; 
                    sp.Read(r, 0 , sp.BytesToRead );
                    if (r[1] == 0xA1)
                    {
                        return true;
                    }
                    Console.WriteLine("Wrong Return:" + Convert.ToString(r[1], 16));
                }
                timeOut--;
            }
            Console.WriteLine("Time out ");
            return false;
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
            sp.DiscardOutBuffer();
            sp.Write(List.ToArray(), 0, List.ToArray().Length);

            //Receive
            sp.DiscardInBuffer();
            System.Threading.Thread.Sleep(30);
            int len = sp.BytesToRead;
            byte[] RevData = new byte[len];
            sp.Read(RevData, 0, len);

            if (len != 20)
            {
                Console.WriteLine("Ack Error");
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
                    Console.WriteLine("Last Frame !!!");
                    sp.DiscardInBuffer();
                    Thread.Sleep(20);
                    if (sp.BytesToRead == 20)
                    {
                        Console.WriteLine("Download OK");
                        return true;
                    }
                    return false;
                }

                //Wait ack
                if (WaitForAck(200) == false)
                {
                    return false;
                }

                Console.WriteLine((p).ToString() + "Transfered");
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

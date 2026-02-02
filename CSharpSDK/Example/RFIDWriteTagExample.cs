
using DeviceAPI.Emums.RFID;
using DeviceAPI.Emums.UHF;
using DeviceAPI.Enums.Device;
using DeviceAPI.Instance;
using DeviceAPI.Models.Params;
using DeviceAPI.Models.RFID;
using DeviceAPI.Utils;
using System;
using System.Collections.Generic;

namespace ConsoleDemo
{
    class RFIDWriteTagExample
    {
        //static String link = "COM3:115200";
        static String link = "192.168.1.201:8090";

        static void Main(string[] args)
        {
            DeviceInstance inst = new DeviceInstance();
            if (!inst.Open(link))
            {
                Console.WriteLine($"Open link {link} failed!");
                return;
            }
            Console.WriteLine($"Open link {link} success!");

            inst.RFIDStopEvent += Inst_RFIDStopEvent;

            // enable antenna 1 and antenna 2
            RFIDAntennaModel antennaModel = new RFIDAntennaModel();
            antennaModel.AntennaMask = (int)(ERFIDAntennaMask.ANT1 | ERFIDAntennaMask.ANT2);

            var rt = inst.SetRFIDParam(ERFIDParam.RFID_ANTENNA, antennaModel);
            if (rt != EDeviceStatus.SUCCESS)
            {
                Console.WriteLine($"Set antenna enable failed: {rt}!");
                return;
            }

            Console.WriteLine("Do you want select tag before write? Y/N");
            ConsoleKeyInfo key = Console.ReadKey();

            bool select_en = false;
            if ((key.KeyChar == 'Y') || (key.KeyChar == 'y'))
            {
                select_en = true;
                Console.WriteLine("Begin inventory tag");
                List<RFIDTagModel> tagList = new List<RFIDTagModel>();
                rt = inst.RFIDInventory(2000, tagList);
                if (rt != EDeviceStatus.SUCCESS)
                {
                    Console.WriteLine($"Start inventory failed: {rt}!");
                    return;
                }

                if (tagList.Count <= 0)
                {
                    Console.WriteLine("No tag found");
                    return;
                }

                RFIDTagModel tag = tagList[0];
                Console.WriteLine($"Select Tag: EPC={BytesConverter.ToHexString(tag.EPC)}, Antenna={tag.Antenna}");

                rt = inst.SetUHFParam(EUHFParam.TAG_SELECT1, new UHFSelectModel()
                {
                    Bank = EUHFBank.ISO18K6C_EPC,
                    BitStart = 0x20,
                    BitLen = (byte)(tag.EPC.Length * 8),
                    MatchValue = tag.EPC
                });
                if (rt != EDeviceStatus.SUCCESS)
                {
                    Console.WriteLine($"Set select param failed: {rt}!");
                    return;
                }
            }

            Random random = new Random();
            byte[] value = new byte[8];
            random.NextBytes(value);

            Console.WriteLine("Start write");
            Console.WriteLine($"User: {BytesConverter.ToHexString(value)}");
            rt = inst.RFIDWriteTag(new RFIDWriteTagModel()
            {
                Password = new byte[] { 0x00, 0x01, 0x00, 0x11 },
                EnablePassword = true,

                SelectEnableMask = (1 << 0),
                EnableTagSelect = select_en,

                Bank = (short)EUHFBank.ISO18K6C_USER,
                WordAddress = 0,
                WordLength = 4,
                //BlockSize = 2,
                Data = value
            });
            if (rt != EDeviceStatus.SUCCESS)
            {
                Console.WriteLine($"Write failed: {rt}!");
                return;
            }
            Console.WriteLine("Write success");

            inst.Close();
        }

        private static void Inst_RFIDStopEvent(object sender, DeviceAPI.Events.RFID.RFIDStopEventArgs e)
        {
            Console.WriteLine($"RFIDStopEvent: Reason={e.Reason}");
        }
    }
}

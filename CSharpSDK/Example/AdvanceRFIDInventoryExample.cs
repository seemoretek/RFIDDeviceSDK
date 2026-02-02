using DeviceAPI.Emums.RFID;
using DeviceAPI.Emums.UHF;
using DeviceAPI.Enums.Device;
using DeviceAPI.Instance;
using DeviceAPI.Models.Params;
using DeviceAPI.Models.RFID;
using DeviceAPI.Utils;
using System;

namespace ConsoleDemo
{
    class AdvanceRFIDInventoryExample
    {
        static String link = "COM3:115200";
        //static String link = "192.168.1.201:8090";

        static void Main(string[] args)
        {
            DeviceInstance inst = new DeviceInstance();
            if (!inst.Open(link))
            {
                Console.WriteLine($"Open link {link} failed!");
                return;
            }
            Console.WriteLine($"Open link {link} success!");


            inst.RFIDTagArrivedEvent += Inst_RFIDTagArrivedEvent;
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

            // set antenna 1 and antenna 2  read power to 30dBm;
            RFIDPowerModel power1 = new RFIDPowerModel();
            RFIDPowerModel power2 = new RFIDPowerModel();

            rt = inst.GetRFIDParam(ERFIDParam.RFID_ANT1_POWER, power1);
            if (rt != EDeviceStatus.SUCCESS)
            {
                Console.WriteLine($"Get antenna1 power failed: {rt}!");
                return;
            }
            rt = inst.GetRFIDParam(ERFIDParam.RFID_ANT2_POWER, power2);
            if (rt != EDeviceStatus.SUCCESS)
            {
                Console.WriteLine($"Get antenna2 power failed: {rt}!");
                return;
            }

            power1.RdPower = 3000; // antenna 1
            power2.RdPower = 2500; // antenna 2

            rt = inst.SetRFIDParam(ERFIDParam.RFID_ANT1_POWER, power1);
            if (rt != EDeviceStatus.SUCCESS)
            {
                Console.WriteLine($"Set antenna1 power failed: {rt}!");
                return;
            }
            rt = inst.SetRFIDParam(ERFIDParam.RFID_ANT2_POWER, power2);
            if (rt != EDeviceStatus.SUCCESS)
            {
                Console.WriteLine($"Set antenna2 power failed: {rt}!");
                return;
            }

            Console.WriteLine($"Set antenna power success: {rt}!");


            RFIDInventoryModel mode = new RFIDInventoryModel();
            mode.InventoryTime = 10000;//ms, inventory time, 0->infinite
            mode.RequestTID = true;
            mode.RequestRSSI = true;

            mode.EnableFastID = true;
            mode.EnableTagFocus = true;

            if (mode.EnableFastID || mode.EnableTagFocus)
            {
                // Impinj Feature need Session 1 Target A
                UHFEPCBaseBandModel baseband = new UHFEPCBaseBandModel();

                rt = inst.GetUHFParam(EUHFParam.UHF_EPC_BASEBAND, baseband);
                if (rt != EDeviceStatus.SUCCESS)
                {
                    Console.WriteLine($"get baseband failed: {rt}!");
                    return;
                }
                baseband.Sessoin = 1;
                baseband.Target = 0;

                rt = inst.SetUHFParam(EUHFParam.UHF_EPC_BASEBAND, baseband);
                if (rt != EDeviceStatus.SUCCESS)
                {
                    Console.WriteLine($"Set baseband failed: {rt}!");
                    return;
                }
            }

            rt = inst.RFIDInventory(mode);
            if (rt != EDeviceStatus.SUCCESS)
            {
                Console.WriteLine($"Start inventory failed: {rt}!");
                return;
            }
            Console.WriteLine("Start inventory success");
            Console.WriteLine("Press any key to stop inventory");
            Console.ReadKey();

            inst.RFIDStop();
            Console.WriteLine("Stop inventory");
            inst.Close();
        }

        private static void Inst_RFIDStopEvent(object sender, DeviceAPI.Events.RFID.RFIDStopEventArgs e)
        {
            Console.WriteLine($"RFIDStopEvent: Reason={e.Reason}");
        }

        private static void Inst_RFIDTagArrivedEvent(object sender, DeviceAPI.Events.RFID.RFIDTagArrivedEventArgs e)
        {
            Console.WriteLine($"RFIDTagArrivedEvent: EPC={BytesConverter.ToHexString(e.Tag.EPC)}, TID={BytesConverter.ToHexString(e.Tag.TID)}, Antenna={e.Tag.Antenna}, RSSI={e.Tag.RSSI}");
        }
    }
}

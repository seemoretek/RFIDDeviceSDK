
using DeviceAPI.Emums.RFID;
using DeviceAPI.Enums.Device;
using DeviceAPI.Instance;
using DeviceAPI.Models.Params;
using DeviceAPI.Utils;
using System;

namespace ConsoleDemo
{
    class BaseRFIDInventoryExample
    {
        static String link = "COM3:115200";
        //static String link = "192.168.1.201:8090";

        static void Main(string[] args)
        {
            DeviceInstance inst = new DeviceInstance();
            inst.RFIDTagArrivedEvent += Inst_RFIDTagArrivedEvent;
            inst.RFIDStopEvent += Inst_RFIDStopEvent;

            if (!inst.Open(link))
            {
                Console.WriteLine($"Open link {link} failed!");
                return;
            }
            Console.WriteLine($"Open link {link} success!");

            // enable antenna 1
            RFIDAntennaModel antennaModel = new RFIDAntennaModel();
            antennaModel.AntennaMask = 0x01;

            var rt = inst.SetRFIDParam(ERFIDParam.RFID_ANTENNA, antennaModel);
            if (rt != EDeviceStatus.SUCCESS)
            {
                Console.WriteLine($"Set antenna enable failed: {rt}!");
                return;
            }

            rt = inst.RFIDInventory();
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
            Console.WriteLine($"RFIDTagArrivedEvent: EPC={BytesConverter.ToHexString(e.Tag.EPC)}, Antenna={e.Tag.Antenna}");
        }

    }
}

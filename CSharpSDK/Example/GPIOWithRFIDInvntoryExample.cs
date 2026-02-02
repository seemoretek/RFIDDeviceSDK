using DeviceAPI.Emums.RFID;
using DeviceAPI.Enums.Device;
using DeviceAPI.Instance;
using DeviceAPI.Models.Params;
using DeviceAPI.Models.RFID;
using DeviceAPI.Utils;
using System;
using System.Collections.Generic;

namespace ConsoleDemo
{
    class GPIOWithRFIDInvntoryExample
    {
        //static String link = "COM3:115200";
        static String link = "192.168.1.201:8090";
        static DeviceInstance inst;
        static EDeviceGPIOIndex GPI_IN = EDeviceGPIOIndex.GPIO1; // in door GPI
        static EDeviceGPIOIndex GPI_OUT = EDeviceGPIOIndex.GPIO2; // out door GPI
        static bool GPI_TRIGGER_LEVEL = true;


        static DateTime? lastTriggerIn;
        static DateTime? lastTriggerOut;
        static readonly TimeSpan validInterval = TimeSpan.FromMilliseconds(5000);

        static event EventHandler OnStart;
        static event EventHandler OnInFinish;
        static event EventHandler OnOutFinish;

        static Dictionary<String, RFIDTagModel> dicTag = new Dictionary<string, RFIDTagModel>();

        static void Main(string[] args)
        {
            inst = new DeviceInstance();
            if (!inst.Open(link))
            {
                Console.WriteLine($"Open link {link} failed!");
                return;
            }
            Console.WriteLine($"Open link {link} success!");

            inst.DeviceGPIOEvent += Inst_DeviceGPIOEvent;
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

            //
            OnInFinish += GPIOExample_OnInFinish;
            OnOutFinish += GPIOExample_OnOutFinish;
            OnStart += GPIOExample_OnStart;

            Dictionary<EDeviceGPIOIndex, EDeviceGPIOMode> gpioMode = new Dictionary<EDeviceGPIOIndex, EDeviceGPIOMode>();
            inst.GetGPIOMode(gpioMode);
            if (rt != EDeviceStatus.SUCCESS)
            {
                Console.WriteLine($"Get GPIO mode failed: {rt}!");
                return;
            }

            Dictionary<EDeviceGPIOIndex, bool> gpioValue = new Dictionary<EDeviceGPIOIndex, bool>();
            inst.GetGPIOState(gpioValue);
            if (rt != EDeviceStatus.SUCCESS)
            {
                Console.WriteLine($"Get GPIO state failed: {rt}!");
                return;
            }

            foreach (var item in gpioValue)
            {
                Console.WriteLine($"{item.Key} {item.Value}!");
            }

            Console.WriteLine("Press any key to stop inventory");
            Console.ReadKey();

            inst.RFIDStop();
            Console.WriteLine("Stop inventory");
            inst.Close();
        }

        private static void GPIOExample_OnStart(object sender, EventArgs e)
        {
            dicTag.Clear();
            var rt = inst.RFIDInventory(new RFIDInventoryModel()
            {
                InventoryTime = 5000,
                RequestRSSI = true
            });

            if (rt != EDeviceStatus.SUCCESS)
            {
                Console.WriteLine($"Start inventory failed: {rt}!");
                return;
            }
            Console.WriteLine("Start inventory success");
        }

        private static void GPIOExample_OnOutFinish(object sender, EventArgs e)
        {
            inst.RFIDStop();
            Console.WriteLine($"Out Finish, EPC List:");

            inst.SetGPIOState(new Dictionary<EDeviceGPIOIndex, bool>
            {
                { EDeviceGPIOIndex.GPIO5, false },
                { EDeviceGPIOIndex.GPIO6, true }
            });

            foreach (var item in dicTag.Keys)
            {
                Console.WriteLine($"\t{item}");
            }
        }

        private static void GPIOExample_OnInFinish(object sender, EventArgs e)
        {
            inst.RFIDStop();
            Console.WriteLine($"In Finish, EPC List:");

            inst.SetGPIOState(new Dictionary<EDeviceGPIOIndex, bool>
            {
                { EDeviceGPIOIndex.GPIO5, true },
                { EDeviceGPIOIndex.GPIO6, false }
            });

            foreach (var item in dicTag.Keys)
            {
                Console.WriteLine($"\t{item}");
            }
        }

        static void HandleTrigger(ref DateTime? lastTriggerTime, EDeviceGPIOIndex triggeredSensor)
        {
            DateTime now = DateTime.Now;
            // cant trigger in work time
            if (lastTriggerTime.HasValue && (now - lastTriggerTime.Value) < validInterval)
            {
                return;
            }

            // retriger
            lastTriggerTime = now;
            CheckDirection(triggeredSensor);
        }

        static void CheckDirection(EDeviceGPIOIndex triggeredSensor)
        {
            EDeviceGPIOIndex otherSensor = (triggeredSensor == GPI_IN) ? GPI_OUT : GPI_IN;
            DateTime? otherLastTrigger = (triggeredSensor == GPI_IN) ? lastTriggerOut : lastTriggerIn;

            if (otherLastTrigger.HasValue) // A-B / B-A
            {
                TimeSpan difference = (DateTime.Now - otherLastTrigger.Value);
                if (difference <= validInterval) // valid in/out
                {
                    if (triggeredSensor == GPI_IN && otherSensor == GPI_OUT) // last is in, for out
                    {
                        OnInFinish?.Invoke(null, null);
                    }
                    else if (triggeredSensor == GPI_OUT && otherSensor == GPI_IN)
                    {
                        OnOutFinish?.Invoke(null, null);
                    }
                    // reset
                    lastTriggerIn = null;
                    lastTriggerOut = null;
                    return;
                }
                else
                {
                    if (otherSensor == GPI_IN)
                    {
                        lastTriggerIn = null;
                    }
                    else
                    {
                        lastTriggerOut = null;
                    }
                    // need restart
                }
            }
            //else // start A/B
            {
                OnStart?.Invoke(null, null);
            }
        }


        private static void Inst_RFIDStopEvent(object sender, DeviceAPI.Events.RFID.RFIDStopEventArgs e)
        {
            Console.WriteLine($"RFIDStopEvent: Reason={e.Reason}");
        }

        private static void Inst_RFIDTagArrivedEvent(object sender, DeviceAPI.Events.RFID.RFIDTagArrivedEventArgs e)
        {
            dicTag[BytesConverter.ToHexString(e.Tag.EPC)] = e.Tag;
            Console.WriteLine($"RFIDTagArrivedEvent: EPC={BytesConverter.ToHexString(e.Tag.EPC)}, Antenna={e.Tag.Antenna}, RSSI={e.Tag.RSSI}");
        }

        private static void Inst_DeviceGPIOEvent(object sender, DeviceAPI.Events.Device.DeviceGPIOEventArgs e)
        {
            Console.WriteLine($"DeviceGPIOEvent: Index={e.GPI.Index} Level={e.GPI.Level}");
            if (e.GPI.Level != GPI_TRIGGER_LEVEL)
            {
                return;
            }

            if (e.GPI.Index == GPI_IN)
            {
                HandleTrigger(ref lastTriggerIn, e.GPI.Index);
            }
            else if (e.GPI.Index == GPI_OUT)
            {
                HandleTrigger(ref lastTriggerOut, e.GPI.Index);
            }
            else
            {
                return;
            }
        }
    }
}

package com.demo;

import com.deviceapi.enums.device.EDeviceGPIOIndex;
import com.deviceapi.enums.device.EDeviceGPIOMode;
import com.deviceapi.enums.device.EDeviceStatus;
import com.deviceapi.enums.rfid.ERFIDAntennaMask;
import com.deviceapi.enums.rfid.ERFIDParam;
import com.deviceapi.events.DeviceEventArgs;
import com.deviceapi.events.DeviceEventHandler;
import com.deviceapi.events.devcie.DeviceGPIOEventArgs;
import com.deviceapi.events.rfid.RFIDStopEventArgs;
import com.deviceapi.events.rfid.RFIDTagArrivedEventArgs;
import com.deviceapi.instance.DeviceInstance;
import com.deviceapi.models.params.RFIDAntennaModel;
import com.deviceapi.models.rfid.RFIDInventoryModel;
import com.deviceapi.models.rfid.RFIDTagModel;
import com.deviceapi.utils.BytesConverter;

import java.util.Date;
import java.util.HashMap;
import java.util.Map;
import java.util.Scanner;


public class GPIOWithRFIDInventoryExample {

    private static final String link = "COM2:115200";
    //private static String link = "192.168.1.201:8090";
    private static DeviceInstance inst;
    private static final EDeviceGPIOIndex GPI_IN = EDeviceGPIOIndex.GPIO1; // in door GPI
    private static final EDeviceGPIOIndex GPI_OUT = EDeviceGPIOIndex.GPIO2; // out door GPI
    private static final boolean GPI_TRIGGER_LEVEL = true;

    private static Date lastTriggerIn;
    private static Date lastTriggerOut;
    private static final long validInterval = 5000; // milliseconds

    // Event handlers
    private static DeviceEventHandler<DeviceEventArgs> onStartHandler;
    private static DeviceEventHandler<DeviceEventArgs> onInFinishHandler;
    private static DeviceEventHandler<DeviceEventArgs> onOutFinishHandler;

    private static final Map<String, RFIDTagModel> dicTag = new HashMap<>();

    public static void main(String[] args) {
        inst = new DeviceInstance();
        if (!inst.open(link)) {
            System.out.printf("Open link %s failed!%n", link);
            return;
        }
        System.out.printf("Open link %s success!%n", link);

        // Register event handlers
        inst.addDeviceGPIOHandler(GPIOWithRFIDInventoryExample::onDeviceGPIOEvent);
        inst.addRFIDTagArrivedHandler(GPIOWithRFIDInventoryExample::onRFIDTagArrivedEvent);
        inst.addRFIDStopHandler(GPIOWithRFIDInventoryExample::onRFIDStopEvent);

        // Enable antenna 1 and antenna 2
        RFIDAntennaModel antennaModel = new RFIDAntennaModel();
        antennaModel.setAntennaMask(ERFIDAntennaMask.ANT1.getValue() | ERFIDAntennaMask.ANT2.getValue());

        EDeviceStatus rt = inst.setRFIDParam(ERFIDParam.RFID_ANTENNA, antennaModel);
        if (rt != EDeviceStatus.SUCCESS) {
            System.out.printf("Set antenna enable failed: %s!%n", rt);
            inst.close();
            return;
        }

        // Set up custom event handlers
        onInFinishHandler = (sender, e) -> onInFinish();
        onOutFinishHandler = (sender, e) -> onOutFinish();
        onStartHandler = (sender, e) -> onStart();

        Map<EDeviceGPIOIndex, EDeviceGPIOMode> gpioMode = new HashMap<>();
        rt = inst.getGPIOMode(gpioMode);
        if (rt != EDeviceStatus.SUCCESS) {
            System.out.printf("Get GPIO mode failed: %s!%n", rt);
            inst.close();
            return;
        }

        Map<EDeviceGPIOIndex, Boolean> gpioValue = new HashMap<>();
        rt = inst.getGPIOState(gpioValue);
        if (rt != EDeviceStatus.SUCCESS) {
            System.out.printf("Get GPIO state failed: %s!%n", rt);
            inst.close();
            return;
        }

        for (Map.Entry<EDeviceGPIOIndex, Boolean> item : gpioValue.entrySet()) {
            System.out.printf("%s %s!%n", item.getKey(), item.getValue());
        }

        System.out.println("Press any key to stop inventory");
        Scanner scanner = new Scanner(System.in);
        scanner.nextLine();
        scanner.close();

        inst.rfidStop();
        System.out.println("Stop inventory");
        inst.close();
    }

    private static void onStart() {
        dicTag.clear();
        RFIDInventoryModel model = new RFIDInventoryModel();
        model.setInventoryTime(5000);
        model.setRequestRSSI(true);

        EDeviceStatus rt = inst.rfidInventory(model);

        if (rt != EDeviceStatus.SUCCESS) {
            System.out.printf("Start inventory failed: %s!%n", rt);
            return;
        }
        System.out.println("Start inventory success");
    }

    private static void onOutFinish() {
        inst.rfidStop();
        System.out.println("Out Finish, EPC List:");

        Map<EDeviceGPIOIndex, Boolean> gpioState = new HashMap<>();
        gpioState.put(EDeviceGPIOIndex.GPIO5, false);
        gpioState.put(EDeviceGPIOIndex.GPIO6, true);
        inst.setGPIOState(gpioState);

        for (String epc : dicTag.keySet()) {
            System.out.printf("\t%s%n", epc);
        }
    }

    private static void onInFinish() {
        inst.rfidStop();
        System.out.println("In Finish, EPC List:");

        Map<EDeviceGPIOIndex, Boolean> gpioState = new HashMap<>();
        gpioState.put(EDeviceGPIOIndex.GPIO5, true);
        gpioState.put(EDeviceGPIOIndex.GPIO6, false);
        inst.setGPIOState(gpioState);

        for (String epc : dicTag.keySet()) {
            System.out.printf("\t%s%n", epc);
        }
    }

    private static void handleTrigger(Date[] lastTriggerTimeHolder, EDeviceGPIOIndex triggeredSensor) {
        Date now = new Date();
        Date lastTriggerTime = lastTriggerTimeHolder[0];

        // Can't trigger in work time
        if (lastTriggerTime != null && (now.getTime() - lastTriggerTime.getTime()) < validInterval) {
            return;
        }

        // Re-trigger
        lastTriggerTimeHolder[0] = now;
        checkDirection(triggeredSensor);
    }

    private static void checkDirection(EDeviceGPIOIndex triggeredSensor) {
        EDeviceGPIOIndex otherSensor = (triggeredSensor == GPI_IN) ? GPI_OUT : GPI_IN;
        Date[] otherLastTriggerHolder = (triggeredSensor == GPI_IN)
                ? new Date[]{lastTriggerOut}
                : new Date[]{lastTriggerIn};

        Date otherLastTrigger = otherLastTriggerHolder[0];

        if (otherLastTrigger != null) { // A-B / B-A
            long difference = new Date().getTime() - otherLastTrigger.getTime();
            if (difference <= validInterval) { // Valid in/out
                if (triggeredSensor == GPI_IN && otherSensor == GPI_OUT) { // Last is in, for out
                    if (onInFinishHandler != null) {
                        onInFinishHandler.handle(null, null);
                    }
                } else if (triggeredSensor == GPI_OUT && otherSensor == GPI_IN) {
                    if (onOutFinishHandler != null) {
                        onOutFinishHandler.handle(null, null);
                    }
                }
                // Reset
                lastTriggerIn = null;
                lastTriggerOut = null;
                return;
            } else {
                if (otherSensor == GPI_IN) {
                    lastTriggerIn = null;
                } else {
                    lastTriggerOut = null;
                }
                // Need restart
            }
        }
        // else // start A/B
        {
            if (onStartHandler != null) {
                onStartHandler.handle(null, null);
            }
        }
    }

    private static void onRFIDStopEvent(Object sender, RFIDStopEventArgs e) {
        System.out.printf("RFIDStopEvent: Reason=%s%n", e.getReason());
    }

    private static void onRFIDTagArrivedEvent(Object sender, RFIDTagArrivedEventArgs e) {
        String epcHex = BytesConverter.toHexString(e.getTag().getEPC());
        dicTag.put(epcHex, e.getTag());
        System.out.printf("RFIDTagArrivedEvent: EPC=%s, Antenna=%s, RSSI=%s%n",
                epcHex, e.getTag().getAntenna(), e.getTag().getRSSI());
    }

    private static void onDeviceGPIOEvent(Object sender, DeviceGPIOEventArgs e) {
        System.out.printf("DeviceGPIOEvent: Index=%s Level=%s%n",
                e.getGPIO().getIndex(), e.getGPIO().getLevel());

        if (e.getGPIO().getLevel() != GPI_TRIGGER_LEVEL) {
            return;
        }

        if (e.getGPIO().getIndex() == GPI_IN) {
            handleTrigger(new Date[]{lastTriggerIn}, e.getGPIO().getIndex());
            lastTriggerIn = (lastTriggerIn == null) ? new Date() : lastTriggerIn;
        } else if (e.getGPIO().getIndex() == GPI_OUT) {
            handleTrigger(new Date[]{lastTriggerOut}, e.getGPIO().getIndex());
            lastTriggerOut = (lastTriggerOut == null) ? new Date() : lastTriggerOut;
        } else {
        }
    }
}
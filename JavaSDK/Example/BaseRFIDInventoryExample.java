package com.demo;


import com.deviceapi.enums.device.EDeviceStatus;
import com.deviceapi.enums.rfid.ERFIDAntennaMask;
import com.deviceapi.enums.rfid.ERFIDParam;
import com.deviceapi.events.rfid.RFIDStopEventArgs;
import com.deviceapi.events.rfid.RFIDTagArrivedEventArgs;
import com.deviceapi.instance.DeviceInstance;
import com.deviceapi.models.params.RFIDAntennaModel;
import com.deviceapi.utils.BytesConverter;

import java.util.Scanner;

class BaseRFIDInventoryExample {
    static String link = "COM2:115200";
//    static String link = "192.168.1.201:8090";
    public static void main(String[] args) {
        DeviceInstance inst = new DeviceInstance();
        inst.addRFIDTagArrivedHandler(BaseRFIDInventoryExample::instRFIDTagArrivedEvent);
        inst.addRFIDStopHandler(BaseRFIDInventoryExample::instRFIDStopEvent);

        if (!inst.open(link)) {
            System.out.println("Open link " + link + " failed!");
            return;
        }
        System.out.println("Open link " + link + " success!");

        // enable antenna 1
        RFIDAntennaModel antennaModel = new RFIDAntennaModel();
        antennaModel.setAntennaMask(ERFIDAntennaMask.ANT1.getValue());

        EDeviceStatus rt = inst.setRFIDParam(ERFIDParam.RFID_ANTENNA, antennaModel);
        if (rt != EDeviceStatus.SUCCESS) {
            System.out.println("Set antenna enable failed: " + rt + "!");
            inst.close();
            return;
        }

        rt = inst.rfidInventory();
        if (rt != EDeviceStatus.SUCCESS) {
            System.out.println("Start inventory failed: " + rt + "!");
            inst.close();
            return;
        }
        System.out.println("Start inventory success");
        System.out.println("Press any key to stop inventory");
        new Scanner(System.in).nextLine();

        inst.rfidStop();
        System.out.println("Stop inventory");
        inst.close();
    }

    private static void instRFIDStopEvent(Object sender, RFIDStopEventArgs e) {
        System.out.println("RFIDStopEvent: Reason=" + e.getReason());
    }

    private static void instRFIDTagArrivedEvent(Object sender, RFIDTagArrivedEventArgs e) {
        System.out.println("RFIDTagArrivedEvent: EPC=" + BytesConverter.toHexString(e.getTag().getEPC()) + ", Antenna=" + e.getTag().getAntenna());
    }



}

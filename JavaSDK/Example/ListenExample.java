package com.demo;

import com.deviceapi.enums.device.EDeviceStatus;
import com.deviceapi.enums.rfid.ERFIDAntennaMask;
import com.deviceapi.enums.rfid.ERFIDParam;
import com.deviceapi.events.devcie.DeviceConnectedEventArgs;
import com.deviceapi.events.devcie.DeviceLogEventArgs;
import com.deviceapi.events.rfid.RFIDStopEventArgs;
import com.deviceapi.events.rfid.RFIDTagArrivedEventArgs;
import com.deviceapi.instance.DeviceInstance;
import com.deviceapi.models.params.RFIDAntennaModel;
import com.deviceapi.utils.BytesConverter;

import java.util.Scanner;

public class ListenExample {
    static String listenParam = "192.168.1.1:8080";
    static DeviceInstance inst = null;

    public static void main(String[] args) {

        DeviceInstance.addListenLogHandler(ListenExample::instDeviceLogEvent);
        DeviceInstance.addListenConnectedHandler(ListenExample::deviceInstanceListenDeviceConnectedEvent);
        DeviceInstance.startListen(listenParam);

        System.out.println("Start listen  " + listenParam + " success!");
        System.out.println("Press q for stop listen!");
        Scanner scanner = new Scanner(System.in);
        while (inst == null) {
            try {
                if (System.in.available() > 0) {
                    int key = System.in.read();
                    if (key == 'Q' || key == 'q') {
                        System.out.println("You  stop the listen!");
                        DeviceInstance.stopListen();
                        return;
                    }
                }
                Thread.sleep(100);
            } catch (Exception e) {

            }
        }

        System.out.println("Listen link " + inst.getLink() + " success!");

        inst.addRFIDTagArrivedHandler(ListenExample::instRFIDTagArrivedEvent);
        inst.addRFIDStopHandler(ListenExample::instRFIDStopEvent);

        // enable antenna 1 and antenna 2
        RFIDAntennaModel antennaModel = new RFIDAntennaModel();
        antennaModel.setAntennaMask(ERFIDAntennaMask.ANT1.getValue() | ERFIDAntennaMask.ANT2.getValue());

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
        scanner.nextLine();

        inst.rfidStop();
        System.out.println("Stop inventory");

        DeviceInstance.stopListen();
        inst.close();

    }

    private static void instDeviceLogEvent(Object o, DeviceLogEventArgs e) {
        System.out.println("Logger: " + e.getMessage());
    }

    public static void instRFIDStopEvent(Object sender, RFIDStopEventArgs e) {
        System.out.println("RFIDStopEvent: Reason=" + e.getReason());
    }

    private static void instRFIDTagArrivedEvent(Object sender, RFIDTagArrivedEventArgs e) {
        System.out.println("EPCTagArrivedEvent: EPC=" + BytesConverter.toHexString(e.getTag().getEPC()) + ", Antenna=" + e.getTag().getAntenna() + ", RSSI=" + e.getTag().getRSSI());
    }

    private static void deviceInstanceListenDeviceConnectedEvent(Object sender, DeviceConnectedEventArgs e) {
        inst = DeviceInstance.buildListenInstance(e.getLink());
    }
}

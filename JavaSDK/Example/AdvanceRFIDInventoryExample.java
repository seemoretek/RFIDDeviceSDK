package com.demo;


import com.deviceapi.enums.device.EDeviceStatus;
import com.deviceapi.enums.rfid.ERFIDAntennaMask;
import com.deviceapi.enums.rfid.ERFIDParam;
import com.deviceapi.enums.uhf.EUHFParam;
import com.deviceapi.events.rfid.RFIDStopEventArgs;
import com.deviceapi.events.rfid.RFIDTagArrivedEventArgs;
import com.deviceapi.instance.DeviceInstance;
import com.deviceapi.models.params.RFIDAntennaModel;
import com.deviceapi.models.params.RFIDPowerModel;
import com.deviceapi.models.params.UHFEPCBaseBandModel;
import com.deviceapi.models.rfid.RFIDInventoryModel;
import com.deviceapi.utils.BytesConverter;

import java.util.Scanner;

class AdvanceRFIDInventoryExample {
//    static String link = "COM2:115200";
    static String link = "192.168.1.201:8090";

    public static void main(String[] args) {
        DeviceInstance inst = new DeviceInstance();
        if (!inst.open(link)) {
            System.out.println("Open link " + link + " failed!");
            return;
        }
        System.out.println("Open link " + link + " success!");

        inst.addRFIDTagArrivedHandler(AdvanceRFIDInventoryExample::instRFIDTagArrivedEvent);
        inst.addRFIDStopHandler(AdvanceRFIDInventoryExample::instRFIDStopEvent);

        // enable antenna 1 and antenna 2
        RFIDAntennaModel antennaModel = new RFIDAntennaModel();
        antennaModel.setAntennaMask((ERFIDAntennaMask.ANT1.getValue() | ERFIDAntennaMask.ANT2.getValue()));

        EDeviceStatus rt = inst.setRFIDParam(ERFIDParam.RFID_ANTENNA, antennaModel);
        if (rt != EDeviceStatus.SUCCESS) {
            System.out.println("Set antenna enable failed: " + rt + "!");
            inst.close();
            return;
        }

        // set antenna 1 and antenna 2  read power to 30dBm;
        RFIDPowerModel power1 = new RFIDPowerModel();
        RFIDPowerModel power2 = new RFIDPowerModel();

        rt = inst.getRFIDParam(ERFIDParam.RFID_ANT1_POWER, power1);
        if (rt != EDeviceStatus.SUCCESS) {
            System.out.println("Get antenna1 power failed: " + rt + "!");
            inst.close();
            return;
        }
        rt = inst.getRFIDParam(ERFIDParam.RFID_ANT2_POWER, power2);
        if (rt != EDeviceStatus.SUCCESS) {
            System.out.println("Get antenna2 power failed: " + rt + "!");
            inst.close();
            return;
        }

        power1.setRdPower((short) 3000); // antenna 1
        power2.setRdPower((short) 2500); // antenna 2

        rt = inst.setRFIDParam(ERFIDParam.RFID_ANT1_POWER, power1);
        if (rt != EDeviceStatus.SUCCESS) {
            System.out.println("Set antenna1 power failed: " + rt + "!");
            inst.close();
            return;
        }
        rt = inst.setRFIDParam(ERFIDParam.RFID_ANT2_POWER, power2);
        if (rt != EDeviceStatus.SUCCESS) {
            System.out.println("Set antenna2 power failed: " + rt + "!");
            inst.close();
            return;
        }

        System.out.println("Set antenna power success: " + rt + "!");

        RFIDInventoryModel mode = new RFIDInventoryModel();
        mode.setInventoryTime(10000); //ms, inventory time, 0->infinite
        mode.setRequestTID(true);
        mode.setRequestRSSI(true);
        mode.setRequestFrequcency(true);
        mode.setRequestPhase(true);

        mode.setEnableFastID(true);
        mode.setEnableTagFocus(true);

        if (mode.isEnableFastID() || mode.isEnableTagFocus()) {
            // Impinj Feature need Session 1 Target A
            UHFEPCBaseBandModel baseband = new UHFEPCBaseBandModel();

            rt = inst.getUHFParam(EUHFParam.UHF_EPC_BASEBAND, baseband);
            if (rt != EDeviceStatus.SUCCESS) {
                System.out.println("get baseband failed: " + rt + "!");
                inst.close();
                return;
            }
            baseband.setSession((byte) 1);
            baseband.setTarget((byte) 0);

            rt = inst.setUHFParam(EUHFParam.UHF_EPC_BASEBAND, baseband);
            if (rt != EDeviceStatus.SUCCESS) {
                System.out.println("Set baseband failed: " + rt + "!");
                inst.close();
                return;
            }
        }

        rt = inst.rfidInventory(mode);
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
        System.out.println("RFIDTagArrivedEvent: EPC=" + BytesConverter.toHexString(e.getTag().getEPC()) + ", TID=" + BytesConverter.toHexString(e.getTag().getTID()) + ", Antenna=" + e.getTag().getAntenna() + ", RSSI=" + e.getTag().getRSSI() + ", Frequency=" + String.format("%.3fMHz", e.getTag().getFrequency()) + ", Phase=" + String.format("%.1f°", e.getTag().getPhase()));
    }
}

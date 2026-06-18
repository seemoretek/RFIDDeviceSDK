package com.demo;

import com.deviceapi.enums.device.EDeviceStatus;
import com.deviceapi.enums.rfid.ERFIDAntennaMask;
import com.deviceapi.enums.rfid.ERFIDParam;
import com.deviceapi.enums.uhf.EUHFBank;
import com.deviceapi.enums.uhf.EUHFParam;
import com.deviceapi.enums.uhf.EUHFSelectMask;
import com.deviceapi.events.devcie.DeviceLogEventArgs;
import com.deviceapi.events.rfid.RFIDStopEventArgs;
import com.deviceapi.instance.DeviceInstance;
import com.deviceapi.models.params.RFIDAntennaModel;
import com.deviceapi.models.params.UHFSelectModel;
import com.deviceapi.models.rfid.RFIDReadTagModel;
import com.deviceapi.models.rfid.RFIDTagModel;
import com.deviceapi.utils.BytesConverter;

import java.util.ArrayList;
import java.util.Scanner;

class RFIDReadTagExample {
    //static String link = "COM2:115200";
    static String link = "192.168.1.201:8090";

    public static void main(String[] args) {
        DeviceInstance inst = new DeviceInstance();
        if (!inst.open(link)) {
            System.out.println("Open link " + link + " failed!");
            return;
        }
        System.out.println("Open link " + link + " success!");

        inst.addRFIDStopHandler(RFIDReadTagExample::instRFIDStopEvent);
        inst.addDeviceLogHandler(RFIDReadTagExample::instDeviceLogEvent);

        // enable antenna 1 and antenna 2
        RFIDAntennaModel antennaModel = new RFIDAntennaModel();
        antennaModel.setAntennaMask((ERFIDAntennaMask.ANT1.getValue() | ERFIDAntennaMask.ANT2.getValue()));

        EDeviceStatus rt = inst.setRFIDParam(ERFIDParam.RFID_ANTENNA, antennaModel);
        if (rt != EDeviceStatus.SUCCESS) {
            System.out.println("Set antenna enable failed: " + rt + "!");
            return;
        }

        System.out.println("Do you want select tag before read? Y/N");
        Scanner scanner = new Scanner(System.in);
        String input = scanner.nextLine();

        boolean select_en = false;
        if (input.equalsIgnoreCase("Y")) {
            select_en = true;
            System.out.println("Begin inventory tag");
            ArrayList<RFIDTagModel> tagList = new ArrayList<>();
            rt = inst.rfidInventory(2000, tagList);
            if (rt != EDeviceStatus.SUCCESS) {
                System.out.println("Start inventory failed: " + rt + "!");
                inst.close();
                return;
            }

            for (RFIDTagModel tag : tagList) {
                System.out.println("Tag: EPC=" + BytesConverter.toHexString(tag.getEPC()) + ", Antenna=" + tag.getAntenna());
            }

            if (tagList.isEmpty()) {
                System.out.println("No tag found");
                inst.close();
                return;
            }

            RFIDTagModel tag = tagList.get(0);
            System.out.println("Select Tag: EPC=" + BytesConverter.toHexString(tag.getEPC()) + ", Antenna=" + tag.getAntenna());

            UHFSelectModel selectModel = new UHFSelectModel();
            selectModel.setBank(EUHFBank.ISO18K6C_EPC);
            selectModel.setBitStart((short) 0x20);
            selectModel.setBitLen((short) (tag.getEPC().length * 8));
            selectModel.setMatchValue(tag.getEPC());

            rt = inst.setUHFParam(EUHFParam.TAG_SELECT1, selectModel);
            if (rt != EDeviceStatus.SUCCESS) {
                System.out.println("Set select param failed: " + rt + "!");
                inst.close();
                return;
            }
        }

        RFIDReadTagModel readModel = new RFIDReadTagModel();
        readModel.setSelectEnableMask(EUHFSelectMask.SELECT1.getValue());
        readModel.setEnableTagSelect(select_en);

        readModel.setBank((short) EUHFBank.ISO18K6C_USER.getValue());
        readModel.setWordAddress((short) 0);
        readModel.setWordLength((short) 4);

        rt = inst.rfidReadTag(readModel);
        if (rt != EDeviceStatus.SUCCESS) {
            System.out.println("Start read failed: " + rt + "!");
            inst.close();
            return;
        }

        System.out.println("Read finish,  User=" + BytesConverter.toHexString(readModel.getReadBackData()));

        inst.close();
    }

    private static void instDeviceLogEvent(Object sender, DeviceLogEventArgs e) {
        System.out.println("Logger: " + e.getMessage());
    }

    private static void instRFIDStopEvent(Object sender, RFIDStopEventArgs e) {
        System.out.println("RFIDStopEvent: Reason=" + e.getReason());
    }
}

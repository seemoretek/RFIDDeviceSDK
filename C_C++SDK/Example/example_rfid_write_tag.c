/**
 * @file example_rfid_write_tag.c
 * @brief C Example：RFID Tag Write
 *
 * Compile:
 *   gcc -o example_rfid_write_tag example_rfid_write_tag.c -ldevice_api
 */

#include "device_api.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>


uint8_t tag_selected_tid[16] = {0};

/* RFID Tag Callback */
void on_rfid_tag(device_handle_t dev, const device_rfid_tag_t* tag, void* userdata) {
    (void)dev;
    (void)userdata;

    char epc_hex[128];
    for (int i = 0; i < tag->epc_len; i++) {
        sprintf(epc_hex + i * 2, "%02X", tag->epc[i]);
    }
    epc_hex[tag->epc_len * 2] = '\0';

    char tid_hex[128];
    for (int i = 0; i < tag->tid_len; i++) {
        sprintf(tid_hex + i * 2, "%02X", tag->tid[i]);
    }
    tid_hex[tag->tid_len * 2] = '\0';

    char data_hex[512];
    for (int i = 0; i < tag->data_len; i++) {
        sprintf(data_hex + i * 2, "%02X", tag->data[i]);
    }
    data_hex[tag->data_len * 2] = '\0';

    printf("Tag: EPC=%s, RSSI=%.2f dBm, Ant=%d, TID=%s, Data=%s\n",
           epc_hex, tag->rssi * 0.01f, (int)tag->antenna, tid_hex, data_hex);

    // select first tag
    if (tag_selected_tid[0] == 0) {
        memcpy(tag_selected_tid, tag->tid, tag->tid_len);
        printf("Selected TID: %s\n", tid_hex);
    }
}

/* RFID Inventory Stop Callback */
void on_rfid_stop(device_handle_t dev, uint8_t reason, void* userdata) {
    (void)dev;
    (void)userdata;
    printf("RFID inventory stopped. Reason: 0x%02X\n", reason);
}

/* Disconnect callback */
void on_disconnect(device_handle_t dev, int will_reconnect, void* userdata) {
    (void)dev;
    (void)userdata;
    printf("Disconnected! Will reconnect: %s\n", will_reconnect ? "yes" : "no");
}

/* Reconnect callback */
void on_reconnected(device_handle_t dev, void* userdata) {
    (void)dev;
    (void)userdata;
    printf("Reconnected!\n");
}

/* Log callback */
void on_log(device_handle_t dev, const char* msg, void* userdata) {
    (void)dev;
    (void)userdata;
    printf("[LOG] %s", msg);
}

int main(int argc, char* argv[]) {
    const char* port = (argc > 1) ? argv[1] : "192.168.1.201:8090";

    /* Set callbacks */
    device_callbacks_t cbs;
    memset(&cbs, 0, sizeof(cbs));
    cbs.on_rfid_tag    = on_rfid_tag;
    cbs.on_rfid_stop   = on_rfid_stop;
    cbs.on_disconnect  = on_disconnect;
    cbs.on_reconnected = on_reconnected;
    // cbs.on_log         = on_log;

    /* Create device instance */
    device_handle_t dev = device_create(&cbs);
    if (!dev) {
        fprintf(stderr, "Failed to create device\n");
        return 1;
    }

    /* Open device */
    printf("Opening %s ...\n", port);
    device_status_t status = device_open(dev, port, 1);
    if (status != DEVICE_OK) {
        fprintf(stderr, "Failed to open device: 0x%02X\n", (int)status);
        device_destroy(dev);
        return 1;
    }
    printf("Device opened successfully!\n");

    /* Set RFID antenna */
    device_param_value_t antenna = {0};
    printf("Setting RFID antenna...\n");
    
    param_reset(&antenna);
    param_append_uint(&antenna, ANTENNA_MASK_1); // enable antenna 1
    param_append_ushort(&antenna, 0); // antenna dwell time
    status = device_save_rfid_param(dev, RFID_ANTENNA, &antenna);
    if (status != DEVICE_OK) {
        fprintf(stderr, "Failed to set rfid antenna: 0x%02X\n", (int)status);
        device_destroy(dev);
        return 1;
    }
    printf("Antenna set successfully!\n");

    /* Select tag before write */
    printf("Do you want select tag before write? Y/N");
    char resp[16];
    scanf("%s", resp);
    bool select_en = false;

    if (resp[0] == 'Y' || resp[0] == 'y') {
        select_en = true;
        /* Inventory tags */
        printf("Starting RFID inventory 2000 ms for select tag...\n");
        rfid_inventory_t inventory;
        memset(&inventory, 0, sizeof(inventory));
        
        inventory.mask = RFID_TAGOPS_MASK_REQ_RSSI;
        
        // Set timeout 2s
        inventory.mask |= RFID_TAGOPS_MASK_ENABLE_TIMEOUT;
        inventory.timeout = 2000;

        // Read TID
        inventory.mask |= RFID_TAGOPS_MASK_REQ_TID;

        // Read data from user bank
        inventory.mask |= RFID_TAGOPS_MASK_ENABLE_READ;
        inventory.read.bank = UHF_BANK_USER;
        inventory.read.wpoint = 0;
        inventory.read.wlength = 4;

        // Merge tags after 2s delay
        inventory.mask |= RFID_TAGOPS_MASK_ENABLE_MERGE;
        inventory.merge_time = 2000;

        status = device_rfid_inventory(dev,&inventory);
        if (status != DEVICE_OK) {
            fprintf(stderr, "Failed to start inventory: 0x%02X\n", (int)status);
            device_destroy(dev);
            return 1;
        }
        printf("Inventory started successfully!\n");

        sleep(3);

        /* Stop inventory */
        printf("Stopping inventory...\n");
        device_rfid_stop(dev);
        printf("Inventory stopped successfully!\n");

        if (tag_selected_tid[0] == 0) {
            printf("No tag selected!\n");
            device_close(dev);
            device_destroy(dev);
            return 0;
        }

        /* Set UHF select */
        printf("Setting UHF select...\n");
        device_param_value_t select;
        param_reset(&select);
        param_append_byte(&select, 0); // is or 
        param_append_ushort(&select, UHF_BANK_TID); // bank
        param_append_ushort(&select, 0); // bit start
        param_append_ushort(&select, 96); // bit length
        param_append_array(&select, tag_selected_tid, 12);
        status = device_save_uhf_param(dev, UHF_TAG_SELECT1, &select);
        if (status != DEVICE_OK) {
            fprintf(stderr, "Failed to set uhf select: 0x%02X\n", (int)status);
            device_destroy(dev);
            return 1;
        }
        printf("UHF select set successfully!\n");
    } 

    /* Write tag */
    printf("Writing tag...\n");
    rfid_write_t write = {0};
    write.mask =  0;
    write.mask |= RFID_TAGOPS_MASK_ENABLE_TIMEOUT;
    write.timeout = 2000;

    if (select_en) {
        write.mask |= RFID_TAGOPS_MASK_ENABLE_SELECT;
        write.select_en = SELECT_MASK_1;
    }

    write.mask |= RFID_TAGOPS_MASK_ENABLE_PASSWD;
    write.passwd = 0x00010011;

    write.bank = UHF_BANK_USER;
    write.wpoint = 0;
    write.wlength = 4;

    srand(time(NULL));
    printf("Writing tag data: ");
    for (int i = 0; i < write.wlength * 2; i++) {
        write.data[i] = rand() & 0xFF;
        printf("%02X ", write.data[i]);
    }
    printf("\n");
    
    status = device_rfid_write_tag(dev, &write);
    if (status != DEVICE_OK) {
        fprintf(stderr, "Failed to write tag: 0x%02X\n", (int)status);
        device_destroy(dev);
        return 1;
    }
    printf("Tag written successfully!\n");
    
    /* Close connection */
    device_close(dev);
    device_destroy(dev);

    printf("Done.\n");
    return 0;
}

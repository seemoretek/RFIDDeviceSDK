/**
 * @file example_advance_rfid_inventory.c
 * @brief C Example：Advanced RFID Inventory
 *
 * Compile:
 *   gcc -o example_advance_rfid_inventory example_advance_rfid_inventory.c -ldevice_api
 */

#include "device_api.h"
#include <stdio.h>
#include <string.h>

/* RFID Tag Callback */
void on_rfid_tag(device_handle_t dev, const device_rfid_tag_t* tag, void* userdata) {
    (void)dev;
    (void)userdata;

    char epc_hex[128];
    for (int i = 0; i < tag->epc_len; i++) {
        sprintf(epc_hex + i * 2, "%02X", tag->epc[i]);
    }
    epc_hex[tag->epc_len * 2] = '\0';

    printf("Tag: EPC=%s, RSSI=%.2f dBm, Ant=%d, Count=%u Freq=%.3f MHz Phase=%.2f deg\n",
           epc_hex, tag->rssi * 0.01f, (int)tag->antenna, tag->count, tag->frequency_khz / 1000.0f, tag->phase * 0.1f );

    char tid_hex[128];
    for (int i = 0; i < tag->tid_len; i++) {
        sprintf(tid_hex + i * 2, "%02X", tag->tid[i]);
    }
    tid_hex[tag->tid_len * 2] = '\0';
    printf("TID: %s\n", tid_hex);

    char data_hex[128];
    for (int i = 0; i < tag->data_len; i++) {
        sprintf(data_hex + i * 2, "%02X", tag->data[i]);
    }
    data_hex[tag->data_len * 2] = '\0';
    printf("Data: %s\n", data_hex);
}

/* RFID Stop Callback */
void on_rfid_stop(device_handle_t dev, uint8_t reason, void* userdata) {
    (void)dev;
    (void)userdata;
    printf("RFID inventory stopped. Reason: 0x%02X\n", reason);
}

/* Disconnect Callback */
void on_disconnect(device_handle_t dev, int will_reconnect, void* userdata) {
    (void)dev;
    (void)userdata;
    printf("Disconnected! Will reconnect: %s\n", will_reconnect ? "yes" : "no");
}

/* Reconnected Callback */
void on_reconnected(device_handle_t dev, void* userdata) {
    (void)dev;
    (void)userdata;
    printf("Reconnected!\n");
}

/* Log Callback */
void on_log(device_handle_t dev, const char* msg, void* userdata) {
    (void)dev;
    (void)userdata;
    printf("[LOG] %s", msg);
}

int main(int argc, char* argv[]) {
    const char* port = (argc > 1) ? argv[1] : "/dev/ttyUSB0:115200";

    /* Set Callbacks */
    device_callbacks_t cbs;
    memset(&cbs, 0, sizeof(cbs));
    cbs.on_rfid_tag    = on_rfid_tag;
    cbs.on_rfid_stop   = on_rfid_stop;
    cbs.on_disconnect  = on_disconnect;
    cbs.on_reconnected = on_reconnected;
    // cbs.on_log         = on_log;

    /* Create Device Instance */
    device_handle_t dev = device_create(&cbs);
    if (!dev) {
        fprintf(stderr, "Failed to create device\n");
        return 1;
    }

    /* Open Device */
    printf("Opening %s ...\n", port);
    device_status_t status = device_open(dev, port, 1);
    if (status != DEVICE_OK) {
        fprintf(stderr, "Failed to open device: 0x%02X\n", (int)status);
        device_destroy(dev);
        return 1;
    }
    printf("Device opened successfully!\n");

    /* Set RFID antenna */
    printf("Setting RFID antenna...\n");
    device_param_value_t antenna;
    param_reset(&antenna);
    param_append_uint(&antenna, ANTENNA_MASK_1|ANTENNA_MASK_4); // Work antenna
    param_append_ushort(&antenna, 0); // Antenna dwell time, if 0, then default dwell time
    
    status = device_save_rfid_param(dev, RFID_ANTENNA, &antenna);
    if (status != DEVICE_OK) {
        fprintf(stderr, "Failed to set rfid antenna: 0x%02X\n", (int)status);
        device_destroy(dev);
        return 1;
    }
    printf("Antenna set successfully.\n");
    
    /* Set RFID antenna power */
    printf("Setting RFID antenna power...\n");
    device_param_value_t antenna_power;
    param_reset(&antenna_power);
    param_append_short(&antenna_power, 2000); // Antenna read power, 20dBm
    param_append_short(&antenna_power, 3000); // Antenna write power, 30dBm
    
    status = device_save_rfid_param(dev, RFID_ANT1_POWER, &antenna_power); 
    if (status != DEVICE_OK) {
        fprintf(stderr, "Failed to set rfid ant1 power power: 0x%02X\n", (int)status);
        device_destroy(dev);
        return 1;
    }

    param_reset(&antenna_power);
    param_append_short(&antenna_power, 2500); // Antenna read power, 25dBm
    param_append_short(&antenna_power, 3300); // Antenna write power, 33dBm
    
    status = device_save_rfid_param(dev, RFID_ANT4_POWER, &antenna_power); 
    if (status != DEVICE_OK) {
        fprintf(stderr, "Failed to set rfid ant4 power power: 0x%02X\n", (int)status);
        device_destroy(dev);
        return 1;
    }
    printf("Antenna power set successfully.\n");

    /* Inventory parameter */
    printf("Starting RFID advance inventory...\n");
    rfid_inventory_t inventory;
    memset(&inventory, 0, sizeof(inventory));
    inventory.mask = RFID_TAGOPS_MASK_REQ_RSSI | RFID_TAGOPS_MASK_REQ_COUNT | RFID_TAGOPS_MASK_REQ_FREQ | RFID_TAGOPS_MASK_REQ_PHASE ;

    // Set timeout to 5 seconds
    inventory.mask |= RFID_TAGOPS_MASK_ENABLE_TIMEOUT;
    inventory.timeout = 5000;

    // Read TID
    inventory.mask |= RFID_TAGOPS_MASK_REQ_TID;

    // Read data bank
    // inventory.mask |= RFID_TAGOPS_MASK_ENABLE_READ;
    // inventory.read.bank = UHF_BANK_USER;
    // inventory.read.wpoint = 0;
    // inventory.read.wlength = 1;

    // rssi filter
    // inventory.mask |= RFID_TAGOPS_MASK_ENABLE_RSSI_THRESHOLD;
    // inventory.rssi_threshold = -5000;

    // Tag merge
    inventory.mask |= RFID_TAGOPS_MASK_ENABLE_MERGE;
    inventory.merge_time = 1000;

    // Start inventory
    status = device_rfid_inventory(dev,&inventory);
    if (status != DEVICE_OK) {
        fprintf(stderr, "Failed to start inventory: 0x%02X\n", (int)status);
    }
    printf("Inventory started.\n");

    /* Wait for user input to stop inventory */
    printf("Press Enter to stop...\n");
    getchar();

    /* Stop inventory */
    device_rfid_stop(dev);

    /* Close connection */
    device_close(dev);
    device_destroy(dev);

    printf("Done.\n");
    return 0;
}

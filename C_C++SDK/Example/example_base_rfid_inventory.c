/**
 * @file example_base_rfid_inventory.c
 * @brief C Example：Basic RFID Inventory
 *
 * Compile:
 *   gcc -o example_base_inventory example_base_inventory.c -ldevice_api
 */

#include "device_api.h"
#include <stdio.h>
#include <stdlib.h>
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

    printf("Tag: EPC=%s, RSSI=%.2f dBm, Ant=%d\n",
           epc_hex, tag->rssi * 0.01f, (int)tag->antenna);

}

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
    const char* port = (argc > 1) ? argv[1] : "192.168.1.201";

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
    printf("Setting RFID antenna...\n");
    device_param_value_t antenna;
    param_reset(&antenna);
    param_append_uint(&antenna, ANTENNA_MASK_1|ANTENNA_MASK_4); // Working antenna mask
    param_append_ushort(&antenna, 0); // Antenna dwell time
    
    status = device_save_rfid_param(dev, RFID_ANTENNA, &antenna);
    if (status != DEVICE_OK) {
        fprintf(stderr, "Failed to set rfid antenna: 0x%02X\n", (int)status);
        device_destroy(dev);
        return 1;
    }
    printf("Antenna set successfully.\n");

    /* Start RFID inventory */
    printf("Starting RFID inventory...\n");
    status = device_rfid_inventory(dev, NULL);
    if (status != DEVICE_OK) {
        fprintf(stderr, "Failed to start inventory: 0x%02X\n", (int)status);
        device_destroy(dev);
        return 1;
    }
    printf("Inventory started.\n");

    /* Wait for user input to stop inventory */
    printf("Press Enter to stop...\n");
    getchar();

    /* Stop inventory */
    device_rfid_stop(dev);

    /* Close device connection */
    device_close(dev);
    device_destroy(dev);

    printf("Done.\n");
    return 0;
}

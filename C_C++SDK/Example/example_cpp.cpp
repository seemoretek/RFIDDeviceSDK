/**
 * @file example_cpp.cpp
 * @brief C++ Example: Advanced RFID Inventory with RAII device management
 *
 * Compile:
 *   g++ -std=c++17 -o example_cpp example_cpp.cpp -ldevice_api
 */

#include "../cpp/device_instance.hpp"
#include <cstdio>
#include <cstring>

using SeeMoreIOT::DeviceInstance;
using SeeMoreIOT::DeviceStatus;

/* ===== Callbacks ===== */

void on_rfid_tag(device_handle_t dev, const device_rfid_tag_t* tag, void* userdata) {
    (void)dev;
    (void)userdata;

    char epc_hex[128];
    for (int i = 0; i < tag->epc_len; i++) {
        sprintf(epc_hex + i * 2, "%02X", tag->epc[i]);
    }
    epc_hex[tag->epc_len * 2] = '\0';

    printf("Tag: EPC=%s, RSSI=%.2f dBm, Ant=%d, Count=%u Freq=%.3f MHz Phase=%.2f deg\n",
           epc_hex, tag->rssi * 0.01f, (int)tag->antenna,
           tag->count, tag->frequency_khz / 1000.0f, tag->phase * 0.1f);

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

void on_rfid_stop(device_handle_t dev, uint8_t reason, void* userdata) {
    (void)dev;
    (void)userdata;
    printf("RFID inventory stopped. Reason: 0x%02X\n", reason);
}

void on_disconnect(device_handle_t dev, int will_reconnect, void* userdata) {
    (void)dev;
    (void)userdata;
    printf("Disconnected! Will reconnect: %s\n", will_reconnect ? "yes" : "no");
}

void on_reconnected(device_handle_t dev, void* userdata) {
    (void)dev;
    (void)userdata;
    printf("Reconnected!\n");
}

void on_log(device_handle_t dev, const char* msg, void* userdata) {
    (void)dev;
    (void)userdata;
    printf("[LOG] %s", msg);
}

int main(int argc, char* argv[]) {
    const char* port = (argc > 1) ? argv[1] : "/dev/ttyUSB0:115200";

    /* Set callbacks */
    device_callbacks_t cbs;
    memset(&cbs, 0, sizeof(cbs));
    cbs.on_rfid_tag    = on_rfid_tag;
    cbs.on_rfid_stop   = on_rfid_stop;
    cbs.on_disconnect  = on_disconnect;
    cbs.on_reconnected = on_reconnected;
    // cbs.on_log         = on_log;

    /* RAII: create device instance (auto close + destroy on destruction) */
    DeviceInstance dev(cbs);

    /* Open connection */
    printf("Opening %s ...\n", port);
    DeviceStatus status = dev.open(port, 1);
    if (!status.ok()) {
        fprintf(stderr, "Failed to open device: 0x%02X\n", (int)status.value());
        return 1;
    }
    printf("Device opened successfully!\n");

    /* Set RFID antenna */
    printf("Setting RFID antenna...\n");
    device_param_value_t antenna;
    param_reset(&antenna);
    param_append_uint(&antenna, ANTENNA_MASK_1 | ANTENNA_MASK_4);  // Working antennas
    param_append_ushort(&antenna, 0);  // Dwell time, 0 = default

    status = dev.saveRFIDParam(RFID_ANTENNA, &antenna);
    if (!status.ok()) {
        fprintf(stderr, "Failed to set rfid antenna: 0x%02X\n", (int)status.value());
        return 1;
    }
    printf("Antenna set successfully.\n");

    /* Set RFID antenna power */
    printf("Setting RFID antenna power...\n");
    device_param_value_t ant_power;
    param_reset(&ant_power);
    param_append_short(&ant_power, 2000);  // Read power: 20dBm
    param_append_short(&ant_power, 3000);  // Write power: 30dBm

    status = dev.saveRFIDParam(RFID_ANT1_POWER, &ant_power);
    if (!status.ok()) {
        fprintf(stderr, "Failed to set rfid ant1 power: 0x%02X\n", (int)status.value());
        return 1;
    }

    param_reset(&ant_power);
    param_append_short(&ant_power, 2500);  // Read power: 25dBm
    param_append_short(&ant_power, 3300);  // Write power: 33dBm

    status = dev.saveRFIDParam(RFID_ANT4_POWER, &ant_power);
    if (!status.ok()) {
        fprintf(stderr, "Failed to set rfid ant4 power: 0x%02X\n", (int)status.value());
        return 1;
    }
    printf("Antenna power set successfully.\n");

    /* Start advanced RFID inventory */
    printf("Starting RFID advanced inventory...\n");
    rfid_inventory_t inventory;
    memset(&inventory, 0, sizeof(inventory));
    inventory.mask = RFID_TAGOPS_MASK_REQ_RSSI
                   | RFID_TAGOPS_MASK_REQ_COUNT
                   | RFID_TAGOPS_MASK_REQ_FREQ
                   | RFID_TAGOPS_MASK_REQ_PHASE;

    /* Set timeout to 5 seconds */
    inventory.mask |= RFID_TAGOPS_MASK_ENABLE_TIMEOUT;
    inventory.timeout = 5000;

    /* Request TID */
    inventory.mask |= RFID_TAGOPS_MASK_REQ_TID;

    /* Optional: read user bank */
    // inventory.mask |= RFID_TAGOPS_MASK_ENABLE_READ;
    // inventory.read.bank = UHF_BANK_USER;
    // inventory.read.wpoint = 0;
    // inventory.read.wlength = 1;

    /* Optional: RSSI filter */
    // inventory.mask |= RFID_TAGOPS_MASK_ENABLE_RSSI_THRESHOLD;
    // inventory.rssi_threshold = -5000;

    /* Tag deduplication merge */
    inventory.mask |= RFID_TAGOPS_MASK_ENABLE_MERGE;
    inventory.merge_time = 1000;

    /* Start inventory */
    status = dev.rfidInventory(&inventory);
    if (!status.ok()) {
        fprintf(stderr, "Failed to start inventory: 0x%02X\n", (int)status.value());
    }
    printf("Inventory started.\n");

    /* Wait for user input to stop */
    printf("Press Enter to stop...\n");
    getchar();

    /* Stop inventory */
    dev.rfidStop();

    /* Device auto-closes on destruction (RAII) */
    printf("\nDone.\n");
    return 0;
}

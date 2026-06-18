/**
 * @file example_shell.c
 * @brief C Example：Interactive Shell Test System
 *
 * Compile:
 *   gcc -o example_shell example_shell.c -ldevice_api
 */

#include "device_api.h"
#include <stdint.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>

/* ========================================================================== */
/*  Interactive Shell Command Handling                                          */
/* ========================================================================== */

typedef int (*cmd_func_t)(device_handle_t dev, int argc, char** args);

typedef struct {
    const char* name;
    const char* help;
    cmd_func_t func;
} shell_command_t;

#define MAX_CMD_LEN 256
#define MAX_ARGS 10

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

void on_ota_progress(device_handle_t dev, float progress, void* userdata) {
    (void)dev;
    (void)userdata;
    printf("OTA Progress: %0.2f%%\n", progress);
}

void on_gpio_event(device_handle_t dev, device_gpio_index_t gpio, int state, void* userdata) {
    (void)dev;
    (void)userdata;
    printf("GPIO Event: %d, GPIO ID: %d\n", state, gpio);
}

int device_param_test( device_handle_t dev)
{
    device_status_t status;
    device_param_value_t param;

    printf("\nDevice Parameters:\n");

    // system version
    status = device_get_device_param(dev, DEVICE_SYSTEM_VERSION, &param);
    if (status == DEVICE_OK) {
        char desc[256];
        param_take_string(&param, 4, desc, sizeof(desc));
        printf("System Version: %d.%d.%d.%d %s\n", 
                                        param_take_byte(&param, 0),
                                        param_take_byte(&param, 1), 
                                        param_take_byte(&param, 2), 
                                        param_take_byte(&param, 3), desc);   
        
    }

    // device version
    status = device_get_device_param(dev, DEVICE_VERSION, &param);
    if (status == DEVICE_OK) {
        char desc[256];
        param_take_string(&param, 4, desc, sizeof(desc));
        printf("Device Version: %d.%d.%d.%d %s\n", 
                                        param_take_byte(&param, 0),
                                        param_take_byte(&param, 1), 
                                        param_take_byte(&param, 2), 
                                        param_take_byte(&param, 3), desc);   
        
    } else {
        printf("Failed to get device version: 0x%02X\n", (int)status);
        return 1;
    }

    // device SN
    status = device_get_device_param(dev, DEVICE_SN, &param);
    if (status == DEVICE_OK) {
        char sn[36];
        param_take_string(&param, 0, sn, sizeof(sn));
        printf("Device SN: %s\n", sn);
    } else {
        printf("Failed to get device SN: 0x%02X\n", (int)status);
        return 1;
    }

    // device Name
    status = device_get_device_param(dev, DEVICE_NAME, &param);
    if (status == DEVICE_OK) {
        char name[36];
        param_take_string(&param, 0, name, sizeof(name));
        printf("Device Name: %s\n", name);
    } else {
        printf("Failed to get device Name: 0x%02X\n", (int)status);
        return 1;
    }

    // Assert ID
    status = device_get_device_param(dev, DEVICE_ASSERT_ID, &param);
    if (status == DEVICE_OK) {
        char sn[36];
        param_take_string(&param, 0, sn, sizeof(sn));
        printf("Assert ID: %s\n", sn);
    } else {
        printf("Failed to get Assert ID: 0x%02X\n", (int)status);
        return 1;
    }

    // eth mac
    status = device_get_device_param(dev, DEVICE_ETH_MAC, &param);
    if (status == DEVICE_OK) {
        uint8_t mac[18];
        param_take_bytes(&param, 0, 6, mac, sizeof(mac));

        char mac_str[18];
        for (int i = 0; i < 6; i++) {
            sprintf(mac_str + i * 3, "%02X:", mac[i]);
        }
        mac_str[17] = '\0';
        
        printf("ETH MAC: %s\n", mac_str);
    } else {
        printf("Failed to get ETH MAC: 0x%02X\n", (int)status);
        return 1;
    }

    // Device Date time
    status = device_get_device_param(dev, DEVICE_DATETIME, &param);
    if (status == DEVICE_OK) {
        uint32_t utc_s = param_take_uint(&param, 0);
        // uint32_t utc_ns = param_take_uint(&param, 4);

        time_t utc_timestamp = utc_s;
        struct tm local_time;
        char buffer[80];

        if (localtime_r(&utc_timestamp, &local_time) != NULL) {
            strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &local_time);
        } else {
            printf("Time conversion failed.\n");
        }

        printf("Device Date Time: %s\n", buffer);
    } else {
        printf("Failed to get Device Date Time: 0x%02X\n", (int)status);
        return 1;
    }

    // devcie power on info
    status = device_get_device_param(dev, DEVICE_PWRON_INFO, &param);
    if (status == DEVICE_OK) {
        uint32_t power_on_time = param_take_uint(&param, 0);
        uint32_t power_on_reason = param_take_uint(&param, 4);
        
        printf("Device Power On Time: %d Seconds\n", power_on_time);
        printf("Device Power On Reason: %d\n", power_on_reason);
    } else {
        printf("Failed to get Device Power On Time: 0x%02X\n", (int)status);
        return 1;
    }

    // devcie temperature
    status = device_get_device_param(dev, DEVICE_TEMPERATURE, &param);
    if (status == DEVICE_OK) {
        int16_t temperature = param_take_short(&param, 0);
        printf("Device Temperature: %.2f Celsius\n", temperature * 0.01f);
    } else {
        printf("Failed to get Device Temperature: 0x%02X\n", (int)status);
    }

    // RS232 attributes
    status = device_get_device_param(dev, DEVICE_RS232_ATTR, &param);
    if (status == DEVICE_OK) {
        uint32_t bps = param_take_uint(&param, 0);
        uint8_t data_bits = param_take_byte(&param, 4);
        uint8_t stop_bits = param_take_byte(&param, 5);
        uint8_t parity = param_take_byte(&param, 6);
        char parity_char[] = {'N', 'O', 'E'};

        printf("RS232 Attributes: %d %d %d %d %c\n", bps, data_bits, stop_bits, parity, parity_char[parity]);
    } else {
        printf("Failed to get RS232 Attributes: 0x%02X\n", (int)status);
    }

    // RS485 attributes
    status = device_get_device_param(dev, DEVICE_RS485_ATTR, &param);
    if (status == DEVICE_OK) {
        uint32_t bps = param_take_uint(&param, 0);
        uint8_t data_bits = param_take_byte(&param, 4);
        uint8_t stop_bits = param_take_byte(&param, 5);
        uint8_t parity = param_take_byte(&param, 6);
        char parity_char[] = {'N', 'O', 'E'};


        printf("RS485 Attributes: %d %d %d %d %c\n", bps, data_bits, stop_bits, parity, parity_char[parity]);
    } else {
        printf("Failed to get RS485 Attributes: 0x%02X\n", (int)status);
    }

    // ethernet attributes
    status = device_get_device_param(dev, DEVICE_ETH_NETWORK, &param);
    if (status == DEVICE_OK) {
        bool dhcp = param_take_byte(&param, 0) != 0;
        
        uint32_t ip = param_take_uint(&param, 1);
        char ip_str[16];
        sprintf(ip_str, "%d.%d.%d.%d", ip >> 24, (ip >> 16) & 0xFF, (ip >> 8) & 0xFF, ip & 0xFF);
        uint32_t mask = param_take_uint(&param, 5);
        char mask_str[16];
        sprintf(mask_str, "%d.%d.%d.%d", mask >> 24, (mask >> 16) & 0xFF, (mask >> 8) & 0xFF, mask & 0xFF);
        uint32_t gateway = param_take_uint(&param, 9);
        char gateway_str[16];
        sprintf(gateway_str, "%d.%d.%d.%d", gateway >> 24, (gateway >> 16) & 0xFF, (gateway >> 8) & 0xFF, gateway & 0xFF);
        
        uint32_t dns = param_take_uint(&param, 13);
        char dns_str[16];
        sprintf(dns_str, "%d.%d.%d.%d", dns >> 24, (dns >> 16) & 0xFF, (dns >> 8) & 0xFF, dns & 0xFF);
        uint8_t mode = param_take_byte(&param, 17);
        const char *mode_str = mode == 0 ? "TCP Server" : "TCP Client";

        uint16_t local_port = param_take_short(&param, 18);
        int pos = 20;
        char remote_ip_str[64];
        param_take_string(&param, pos, remote_ip_str, sizeof(remote_ip_str));
        pos += strlen(remote_ip_str) + 1;
        uint16_t remote_port = param_take_short(&param, pos);
       

        printf("ETH Attributes: DHCP=%d, \n\tIP=%s \n\tMask=%s \n\tGateway=%s \n\tDNS=%s \n\tMode=%s \n\tLocalPort=%d \n\tRemote=%s:%d\n", 
                dhcp, ip_str, mask_str, gateway_str, dns_str, mode_str, local_port, remote_ip_str, remote_port);
    } else {
        printf("Failed to get ETH Attributes: 0x%02X\n", (int)status);
    }

    return 0;
}


int device_rfid_param_test( device_handle_t dev)
{
    device_status_t status;
    device_param_value_t param;
    
    printf("\nRFID Parameters:\n");
    
    // RFID version
    status = device_get_rfid_param(dev, RFID_VERSION, &param);
    if (status == DEVICE_OK) {
        char desc[256];
        param_take_string(&param, 4, desc, sizeof(desc));
        printf("RFID Version: %d.%d.%d.%d %s\n", 
                                        param_take_byte(&param, 0),
                                        param_take_byte(&param, 1), 
                                        param_take_byte(&param, 2), 
                                        param_take_byte(&param, 3), desc);   
        
    } else {
        printf("Failed to get RFID version: 0x%02X\n", (int)status);
        return 1;
    }
    
    // RFID SN
    status = device_get_rfid_param(dev, RFID_SN, &param);
    if (status == DEVICE_OK) {
        char sn[36];
        param_take_string(&param, 0, sn, sizeof(sn));
        printf("RFID SN: %s\n", sn);
    } else {
        printf("Failed to get RFID SN: 0x%02X\n", (int)status);
        return 1;
    }

    // RFID Property
    status = device_get_rfid_param(dev, RFID_PROPERTY, &param);
    if (status == DEVICE_OK) {
        uint32_t protocol = param_take_uint(&param, 0);
        uint8_t antenna = param_take_byte(&param, 4);
        int16_t power_max = param_take_short(&param, 5);
        int16_t power_min = param_take_short(&param, 7);
        int16_t power_default = param_take_short(&param, 9);
        int16_t power_step = param_take_short(&param, 11);

        printf("RFID Property: \n\t Protocol=%08X \n\t Antennas=%d \n\t PowerMax=%d dBm \n\t PowerMin=%d dBm \n\t PowerStep=%d dBm \n\t PowerDefault=%d dBm\n", 
                        protocol, antenna, power_max/100, power_min/100, power_step/100, power_default/100);

    } else {
        printf("Failed to get RFID Property: 0x%02X\n", (int)status);
        return 1;
    }

    // rfid temperature
    status = device_get_device_param(dev, DEVICE_TEMPERATURE, &param);
    if (status == DEVICE_OK) {
        int16_t temperature = param_take_short(&param, 0);
        printf("RFID Temperature: %.2f Celsius\n", temperature * 0.01f);
    } else {
        printf("Failed to get RFID Temperature: 0x%02X\n", (int)status);
    }
    
    // rfid eas
    status = device_get_rfid_param(dev, RFID_EAS, &param);
    if (status == DEVICE_OK) {
        printf("RFID EAS: \n");
        printf(" \tEnable: %s\n", param_take_bool(&param, 0)? "Enabled" : "Disabled");
        printf(" \tBank: %d\n", param_take_short(&param, 1));
        printf(" \tStartBit: %d\n", param_take_short(&param, 3));

        uint8_t value_bytes[256];
        char value_str[512];
        int len;
        int pos;

        pos = 5;
        len = param_take_array(&param, pos, value_bytes, sizeof(value_bytes));
        pos += len + 1;
        for (int i = 0; i < len; i++) {
            sprintf(value_str + i * 2, "%02X", value_bytes[i]);
        }

        value_str[len * 2] = '\0';
        printf(" \tMask: %s\n", value_str);

         len = param_take_array(&param, pos, value_bytes, sizeof(value_bytes));
        pos += len + 1;
        for (int i = 0; i < len; i++) {
            sprintf(value_str + i * 2, "%02X", value_bytes[i]);
        }

        value_str[len * 2] = '\0';
        printf(" \tValue: %s\n", value_str);

         len = param_take_array(&param, pos, value_bytes, sizeof(value_bytes));
        pos += len + 1;
        for (int i = 0; i < len; i++) {
            sprintf(value_str + i * 2, "%02X", value_bytes[i]);
        }

        value_str[len * 2] = '\0';
        printf(" \tMatch Cmd: %s\n", value_str); 

         len = param_take_array(&param, pos, value_bytes, sizeof(value_bytes));
        pos += len + 1;
        for (int i = 0; i < len; i++) {
            sprintf(value_str + i * 2, "%02X", value_bytes[i]);
        }

        value_str[len * 2] = '\0';
        printf(" \tMismatch Cmd: %s\n", value_str);

    } else {
        printf("Failed to get RFID EAS: 0x%02X\n", (int)status);
    }

    // rfid wiegand output
    status = device_get_rfid_param(dev, RFID_WIEGAND_OUTPUT, &param);
    if (status == DEVICE_OK) {
        printf("RFID Wiegand Output: \n");
        printf(" \tEnable: %s\n", param_take_bool(&param, 0)? "Enabled" : "Disabled");
        printf(" \tBank: %d\n", param_take_short(&param, 1));
        printf(" \tStartBit: %d\n", param_take_short(&param, 3));
        printf(" \tBitLength: %d\n", param_take_byte(&param, 5));
    } else {
        printf("Failed to get RFID Wiegand Output: 0x%02X\n", (int)status);
    }

    // rfid Special output
    status = device_get_rfid_param(dev, RFID_SPECIAL_OUTPUT, &param);
    if (status == DEVICE_OK) {
        printf("RFID Special Output: \n");
        printf(" \tEnable: %s\n", param_take_bool(&param, 0)? "Enabled" : "Disabled");
        printf(" \tBank: %d\n", param_take_short(&param, 1));
        printf(" \tStartBit: %d\n", param_take_short(&param, 3));
        printf(" \tBitLength: %d\n", param_take_short(&param, 5));
        printf(" \tFormat: %d\n", param_take_byte(&param, 7));
        printf(" \tCheckType: %d\n", param_take_byte(&param, 8));
        printf(" \tAttach: %08X\n", param_take_uint(&param, 9));

        uint8_t value_bytes[256];
        char value_str[512];
        int len;
        int pos;

        pos = 13;
        len = param_take_array(&param, pos, value_bytes, sizeof(value_bytes));
        pos += len + 1;
        for (int i = 0; i < len; i++) {
            sprintf(value_str + i * 2, "%02X", value_bytes[i]);
        }

        value_str[len * 2] = '\0';
        printf(" \tSOF: %s\n", value_str);

        len = param_take_array(&param, pos, value_bytes, sizeof(value_bytes));
        pos += len + 1;
        for (int i = 0; i < len; i++) {
            sprintf(value_str + i * 2, "%02X", value_bytes[i]);
        }

        value_str[len * 2] = '\0';
        printf(" \tEOF: %s\n", value_str);

        len = param_take_array(&param, pos, value_bytes, sizeof(value_bytes));
        pos += len + 1;
        for (int i = 0; i < len; i++) {
            sprintf(value_str + i * 2, "%02X", value_bytes[i]);
        }

        value_str[len * 2] = '\0';
        printf(" \tSEP: %s\n", value_str);
    } else {
        printf("Failed to get RFID Special Output: 0x%02X\n", (int)status);
    }

    // rfid antenna state
    status = device_get_rfid_param(dev, RFID_ANTENNA_STATE, &param);
    if (status == DEVICE_OK) {
        printf("RFID Antenna State: %08X\n", param_take_uint(&param, 0));
    } else {
        printf("Failed to get RFID Antenna State: 0x%02X\n", (int)status);
    }

    return 0;
}


int device_uhf_param_test( device_handle_t dev)
{
    device_status_t status;
    device_param_value_t param;
    
    printf("\nUHF Parameters:\n");
    
    // UHF Property
    status = device_get_uhf_param(dev, UHF_PROPERTY, &param);
    if (status == DEVICE_OK) {
        uint8_t support_regions[32] = {0};
        uint8_t support_modes[32] = {0};

        param_take_bytes(&param, 0, 32,support_regions, sizeof(support_regions));
        param_take_bytes(&param, 32, 32,support_modes, sizeof(support_modes));

        printf("UHF Property:\n");
        char hex_str[160];
        for (size_t i = 0; i < sizeof(support_regions); i++) {
            sprintf(hex_str + i * 2, "%02X", support_regions[i]);
        }
        hex_str[sizeof(support_regions) * 2] = '\0';

        printf(" \t Support Regions: %s\n", hex_str);

        for (size_t i = 0; i < sizeof(support_modes); i++) {
            sprintf(hex_str + i * 2, "%02X", support_modes[i]);
        }
        hex_str[sizeof(support_modes) * 2] = '\0';

        printf(" \t Support Modes: %s\n", hex_str);

    } else {
        printf("Failed to get UHF Property: 0x%02X\n", (int)status);
        return 1;
    }

     // UHF Frequency
    status = device_get_uhf_param(dev, UHF_FREQUENCY, &param);
    if (status == DEVICE_OK) {
       printf("UHF Frequency:\n");
       printf(" \tRegion: %d\n", param_take_byte(&param, 0));
       printf(" \tHopping: %d\n", param_take_byte(&param, 1));

        uint8_t value_bytes[256];
        int len;
        int pos = 2;

        printf(" \tFreq Points: \n");
        len = param_take_array(&param, pos, value_bytes, sizeof(value_bytes));
        pos += len + 1;
        for (int i = 0; i < len; i++) {
            printf("%d ", value_bytes[i]);
        }

        printf("\n");

        printf(" \tInterval: %d\n", param_take_ushort(&param, pos));
        pos += 2;
        printf(" \tStart: %dKHz\n", param_take_uint(&param, pos));
        printf(" \tStep: %dKHz\n", param_take_ushort(&param, pos + 4));
        printf(" \tNumFreq: %d\n", param_take_byte(&param, pos + 6));

        pos += 7;
        printf(" \tStart2: %dKHz\n", param_take_uint(&param, pos));
        printf(" \tStep2: %dKHz\n", param_take_ushort(&param, pos + 4));
        printf(" \tNumFreq2: %d\n", param_take_byte(&param, pos + 6));

    } else {
        printf("Failed to get UHF Frequency: 0x%02X\n", (int)status);
        return 1;
    }

    // uhf epc baseband
    status = device_get_uhf_param(dev, UHF_EPC_BASEBAND, &param);
    if (status == DEVICE_OK) {
        printf("UHF EPC Baseband: \n");
        printf(" \tMode: %d\n", param_take_byte(&param, 0));
        printf(" \tSession: %d\n", param_take_byte(&param, 1));
        printf(" \tTarget: %d\n", param_take_byte(&param, 2));
        printf(" \tInit Q: %d\n", param_take_byte(&param, 3));
        printf(" \tMin Q: %d\n", param_take_byte(&param, 4));
        printf(" \tMax Q: %d\n", param_take_byte(&param, 5));

    } else {
        printf("Failed to get UHF EPC Baseband: 0x%02X\n", (int)status);
    }

    // uhf epc algo
    status = device_get_uhf_param(dev, UHF_EPC_ALGO, &param);
    if (status == DEVICE_OK) {
        printf("UHF EPC Algo: \n");
        printf(" \tType: %d\n", param_take_byte(&param, 0));
        printf(" \tParam1: %d\n", param_take_byte(&param, 1));
        printf(" \tParam2: %d\n", param_take_ushort(&param, 2));
        printf(" \tParam3: %d\n", param_take_uint(&param, 4));
    } else {
        printf("Failed to get UHF EPC Algo: 0x%02X\n", (int)status);
    }

    return 0;
}

// Parse command: split input string into arguments
static int parse_command(char* input, char** args) {
    int argc = 0;
    char* token = strtok(input, " \t\n");
    
    while (token != NULL && argc < MAX_ARGS) {
        args[argc++] = token;
        token = strtok(NULL, " \t\n");
    }
    
    return argc;
}

// help command
static int cmd_help(device_handle_t dev, int argc, char** args);

// exit/quit command
static int cmd_exit(device_handle_t dev, int argc, char** args) {
    (void)dev;
    (void)argc;
    (void)args;
    return -1;
}

// restart command
static int cmd_restart(device_handle_t dev, int argc, char** args) {
    (void)argc;
    (void)args;
    printf("Restarting device...\n");
    device_status_t status = device_restart(dev);
    if (status == DEVICE_OK) {
        printf("Device restart command sent successfully.\n");
        return 0;
    } else {
        printf("Failed to restart device: 0x%02X\n", status);
        return 1;
    }
}

// restore command
static int cmd_restore(device_handle_t dev, int argc, char** args) {
    (void)argc;
    (void)args;
    printf("Restoring factory settings...\n");
    device_status_t status = device_restore(dev);
    if (status == DEVICE_OK) {
        printf("Factory restore command sent successfully.\n");
        return 0;
    } else {
        printf("Failed to restore factory settings: 0x%02X\n", status);
        return 1;
    }
}

// ota command (local file)
static int cmd_ota(device_handle_t dev, int argc, char** args) {
    if (argc < 2) {
        printf("Error: Missing filepath argument\n");
        printf("Usage: ota <filepath>\n");
        return 1;
    }
    
    const char* filepath = args[1];
    printf("Reading firmware file: %s\n", filepath);
    
    FILE* fp = fopen(filepath, "rb");
    if (!fp) {
        printf("Error: Cannot open file %s\n", filepath);
        return 1;
    }
    
    fseek(fp, 0, SEEK_END);
    long file_size = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    
    if (file_size <= 0) {
        printf("Error: Invalid file size\n");
        fclose(fp);
        return 1;
    }
    
    uint8_t* firmware = (uint8_t*)malloc(file_size);
    if (!firmware) {
        printf("Error: Cannot allocate memory for firmware\n");
        fclose(fp);
        return 1;
    }
    
    size_t read_size = fread(firmware, 1, file_size, fp);
    fclose(fp);
    
    if (read_size != (size_t)file_size) {
        printf("Error: Failed to read entire file\n");
        free(firmware);
        return 1;
    }
    
    printf("Firmware size: %ld bytes\n", file_size);
    printf("Starting OTA upgrade...\n");
    
    device_status_t status = device_ota_local(dev, firmware, (uint32_t)file_size);
    free(firmware);
    
    if (status == DEVICE_OK) {
        printf("OTA upgrade started successfully.\n");
        return 0;
    } else {
        printf("Failed to start OTA upgrade: 0x%02X\n", status);
        return 1;
    }
}

// otaurl command (remote URL)
static int cmd_otaurl(device_handle_t dev, int argc, char** args) {
    if (argc < 2) {
        printf("Error: Missing URL argument\n");
        printf("Usage: otaurl <url>\n");
        return 1;
    }
    
    const char* url = args[1];
    printf("Starting remote OTA upgrade from: %s\n", url);
    device_status_t status = device_ota_remote(dev, OTA_TYPE_APPLICATION, url);
    
    if (status == DEVICE_OK) {
        printf("Remote OTA upgrade started successfully.\n");
        return 0;
    } else {
        printf("Failed to start remote OTA upgrade: 0x%02X\n", status);
        return 1;
    }
}

// gpio command
static int cmd_gpio(device_handle_t dev, int argc, char** args) {
    if (argc < 2) {
        printf("Error: Missing gpio subcommand\n");
        printf("Usage: gpio <getvalue|setvalue|getmode|setmode> ...\n");
        return 1;
    }
    
    const char* subcmd = args[1];
    
    if (strcmp(subcmd, "getvalue") == 0) {
        int gpio_num = (argc > 2) ? atoi(args[2]) : 0;
        
        uint32_t state, mask;
        device_status_t status = device_get_gpio_state(dev, &state, &mask);
        
        if (status != DEVICE_OK) {
            printf("Failed to get GPIO state: 0x%02X\n", status);
            return 1;
        }
        
        if (gpio_num > 0) {
            if (gpio_num < 1 || gpio_num > 8) {
                printf("Error: GPIO number must be 1-8\n");
                return 1;
            }
            
            int gpio_idx = gpio_num - 1;
            if (mask & (1 << gpio_idx)) {
                int value = (state & (1 << gpio_idx)) ? 1 : 0;
                printf("GPIO%d = %d\n", gpio_num, value);
            } else {
                printf("GPIO%d is not available\n", gpio_num);
            }
        } else {
            printf("GPIO States:\n");
            for (int i = 0; i < 8; i++) {
                if (mask & (1 << i)) {
                    int value = (state & (1 << i)) ? 1 : 0;
                    printf("  GPIO%d = %d\n", i + 1, value);
                } else {
                    printf("  GPIO%d = N/A\n", i + 1);
                }
            }
        }
        return 0;
    }
    
    if (strcmp(subcmd, "setvalue") == 0) {
        if (argc < 4) {
            printf("Error: Missing arguments\n");
            printf("Usage: gpio setvalue <gpio> <0|1>\n");
            return 1;
        }
        
        int gpio_num = atoi(args[2]);
        int value = atoi(args[3]);
        
        if (gpio_num < 1 || gpio_num > 8) {
            printf("Error: GPIO number must be 1-8\n");
            return 1;
        }
        
        if (value < 0 || value > 1) {
            printf("Error: GPIO value must be 0 or 1\n");
            return 1;
        }
        
        int gpio_idx = gpio_num - 1;
        uint16_t mask = (1 << gpio_idx);
        uint16_t state = value ? mask : 0;
        
        printf("Setting GPIO%d to %d...\n", gpio_num, value);
        device_status_t status = device_set_gpio_state(dev, mask, state);
        
        if (status == DEVICE_OK) {
            printf("GPIO%d set to %d successfully.\n", gpio_num, value);
            return 0;
        } else {
            printf("Failed to set GPIO%d: 0x%02X\n", gpio_num, status);
            return 1;
        }
    }
    
    if (strcmp(subcmd, "getmode") == 0) {
        int gpio_num = (argc > 2) ? atoi(args[2]) : 0;
        
        device_gpio_mode_t modes[16];
        uint32_t mask;
        device_status_t status = device_get_gpio_mode(dev, modes, &mask);
        
        if (status != DEVICE_OK) {
            printf("Failed to get GPIO mode: 0x%02X\n", status);
            return 1;
        }
        
        if (gpio_num > 0) {
            if (gpio_num < 1 || gpio_num > 8) {
                printf("Error: GPIO number must be 1-8\n");
                return 1;
            }
            
            int gpio_idx = gpio_num - 1;
            if (mask & (1 << gpio_idx)) {
                const char* mode_str = (modes[gpio_idx] == DEVICE_GPIO_INPUT) ? "INPUT" : "OUTPUT";
                printf("GPIO%d mode = %s\n", gpio_num, mode_str);
            } else {
                printf("GPIO%d is not available\n", gpio_num);
            }
        } else {
            printf("GPIO Modes:\n");
            for (int i = 0; i < 8; i++) {
                if (mask & (1 << i)) {
                    const char* mode_str = (modes[i] == DEVICE_GPIO_INPUT) ? "INPUT" : "OUTPUT";
                    printf("  GPIO%d = %s\n", i + 1, mode_str);
                } else {
                    printf("  GPIO%d = N/A\n", i + 1);
                }
            }
        }
        return 0;
    }
    
    if (strcmp(subcmd, "setmode") == 0) {
        if (argc < 4) {
            printf("Error: Missing arguments\n");
            printf("Usage: gpio setmode <gpio> <in|out>\n");
            return 1;
        }
        
        int gpio_num = atoi(args[2]);
        const char* mode_str = args[3];
        
        if (gpio_num < 1 || gpio_num > 8) {
            printf("Error: GPIO number must be 1-8\n");
            return 1;
        }
        
        device_gpio_mode_t mode;
        if (strcmp(mode_str, "in") == 0 || strcmp(mode_str, "input") == 0) {
            mode = DEVICE_GPIO_INPUT;
        } else if (strcmp(mode_str, "out") == 0 || strcmp(mode_str, "output") == 0) {
            mode = DEVICE_GPIO_OUTPUT;
        } else {
            printf("Error: Invalid mode. Use 'in' or 'out'\n");
            return 1;
        }
        
        int gpio_idx = gpio_num - 1;
        uint16_t mask = (1 << gpio_idx);
        device_gpio_mode_t modes[16];
        memset(modes, 0, sizeof(modes));
        modes[gpio_idx] = mode;
        
        printf("Setting GPIO%d mode to %s...\n", gpio_num, mode_str);
        device_status_t status = device_set_gpio_mode(dev, mask, modes);
        
        if (status == DEVICE_OK) {
            printf("GPIO%d mode set to %s successfully.\n", gpio_num, mode_str);
            return 0;
        } else {
            printf("Failed to set GPIO%d mode: 0x%02X\n", gpio_num, status);
            return 1;
        }
    }
    
    printf("Error: Unknown gpio subcommand: %s\n", subcmd);
    return 1;
}

// beep command
static int cmd_beep(device_handle_t dev, int argc, char** args) {
    uint32_t duration = (argc > 1) ? (uint32_t)atoi(args[1]) : 100;
    printf("Beeping for %u ms...\n", duration);
    device_status_t status = device_beep(dev, duration);
    
    if (status == DEVICE_OK) {
        printf("Beep completed.\n");
        return 0;
    } else {
        printf("Failed to beep: 0x%02X\n", status);
        return 1;
    }
}

// param command
static int cmd_param(device_handle_t dev, int argc, char** args) {
    (void)argc;
    (void)args;
    return device_param_test(dev);
}

// rfid command
static int cmd_rfid(device_handle_t dev, int argc, char** args) {
    (void)argc;
    (void)args;
    return device_rfid_param_test(dev);
}

// uhf command
static int cmd_uhf(device_handle_t dev, int argc, char** args) {
    (void)argc;
    (void)args;
    return device_uhf_param_test(dev);
}

// check command
static int cmd_check(device_handle_t dev, int argc, char** args) {
    (void)argc;
    (void)args;
    printf("Checking device connection...\n");
    device_status_t status = device_check_connect(dev);
    
    if (status == DEVICE_OK) {
        printf("Device is connected and responding.\n");
        return 0;
    } else {
        printf("Device connection check failed: 0x%02X\n", status);
        return 1;
    }
}

static const shell_command_t shell_commands[] = {
    {"help",        "Show this help message",                                      cmd_help},
    {"exit",        "Exit the program",                                            cmd_exit},
    {"quit",        "Exit the program",                                            cmd_exit},
    {"restart",     "Restart the device",                                          cmd_restart},
    {"restore",     "Restore factory settings",                                    cmd_restore},
    {"ota",         "Perform OTA upgrade with local file: ota <filepath>",         cmd_ota},
    {"otaurl",      "Perform OTA upgrade with remote URL: otaurl <url>",           cmd_otaurl},
    {"gpio",        "GPIO operations: getvalue/setvalue/getmode/setmode",          cmd_gpio},
    {"beep",        "Beep for specified duration (default: 100ms): beep [ms]",     cmd_beep},
    {"param",       "Show device parameters",                                       cmd_param},
    {"rfid",        "Show RFID parameters",                                         cmd_rfid},
    {"uhf",         "Show UHF parameters",                                          cmd_uhf},
    {"check",       "Check device connection",                                      cmd_check},
    {NULL,          NULL,                                                          NULL}
};

static int cmd_help(device_handle_t dev, int argc, char** args) {
    (void)dev;
    (void)argc;
    (void)args;
    
    printf("\n");
    printf("Available Commands:\n");
    printf("----------------------------------------------------\n");
    
    const shell_command_t* cmd = shell_commands;
    while (cmd->name != NULL) {
        printf("  %-15s - %s\n", cmd->name, cmd->help);
        cmd++;
    }
    
    printf("----------------------------------------------------\n");
    printf("\n");
    return 0;
}

static int process_command(device_handle_t dev, int argc, char** args) {
    if (argc == 0) {
        return 0;
    }
    
    const shell_command_t* cmd = shell_commands;
    while (cmd->name != NULL) {
        if (strcmp(cmd->name, args[0]) == 0) {
            return cmd->func(dev, argc, args);
        }
        cmd++;
    }
    
    printf("Error: Unknown command: %s\n", args[0]);
    printf("Type 'help' for available commands.\n");
    return 1;
}

// Interactive Shell main loop
static void interactive_shell(device_handle_t dev) {
    char input[MAX_CMD_LEN];
    char* args[MAX_ARGS];
    
    printf("\n");
    printf("========================================\n");
    printf("  Interactive Device Test Shell\n");
    printf("========================================\n");
    printf("Type 'help' for available commands.\n");
    printf("Type 'exit' or 'quit' to exit.\n");
    printf("\n");
    
    while (1) {
        printf("device> ");
        fflush(stdout);
        
        if (fgets(input, sizeof(input), stdin) == NULL) {
            break;
        }
        
        input[strcspn(input, "\n")] = 0;
        
        if (strlen(input) == 0) {
            continue;
        }
        
        int argc = parse_command(input, args);
        int result = process_command(dev, argc, args);
        
        if (result == -1) {
            printf("Exiting...\n");
            break;
        }
    }
}



int main(int argc, char* argv[]) {
    const char* port = (argc > 1) ? argv[1] : "192.168.1.201";

    /* Set callbacks */
    device_callbacks_t cbs;
    memset(&cbs, 0, sizeof(cbs));
    cbs.on_disconnect  = on_disconnect;
    cbs.on_reconnected = on_reconnected;
    cbs.on_ota_progress = on_ota_progress;
    cbs.on_gpio_event   = on_gpio_event;
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
    
    char version[128];
    device_api_version(version, sizeof(version));
    printf("Device API version: %s\n", version);

    // Enter interactive shell
    interactive_shell(dev);

    /* Close device connection */
    device_close(dev);
    device_destroy(dev);

    printf("Done.\n");
    return 0;
}

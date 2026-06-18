/**
 * @file example_param.c
 * @brief C Example：Basic Parameter Test
 *
 * Compile:
 *   gcc -o example_param example_param.c -ldevice_api
 */

#include "device_api.h"
#include <stdint.h>
#include <stdio.h>
#include <time.h>
#include <string.h>


/* Log callback */
void on_log(device_handle_t dev, const char* msg, void* userdata) {
    (void)dev;
    (void)userdata;
    printf("[LOG] %s", msg);
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

int main(int argc, char* argv[]) {
    const char* port = (argc > 1) ? argv[1] : "192.168.1.201";

    /* Set callbacks */
    device_callbacks_t cbs;
    memset(&cbs, 0, sizeof(cbs));
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
    
    // test device param
    int rt = device_param_test(dev);
    if (rt != 0) {
        printf("Failed to test device param: 0x%02X\n", (int)rt);
        device_destroy(dev);
        return 1;
    }

    // test rfid param
    rt = device_rfid_param_test(dev);
    if (rt != 0) {
        printf("Failed to test rfid param: 0x%02X\n", (int)rt);
        device_destroy(dev);
        return 1;
    }

    // test uhf param
    rt = device_uhf_param_test(dev);
    if (rt != 0) {
        printf("Failed to test uhf param: 0x%02X\n", (int)rt);
        device_destroy(dev);
        return 1;
    }

    

    /* Close device connection */
    device_close(dev);
    device_destroy(dev);

    printf("Done.\n");
    return 0;
}

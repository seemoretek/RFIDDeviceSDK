/**
 * @file device_api.h
 * @brief Device API C API - Public header file (only exposed to external)
 *
 * All enums, structures, and function declarations are defined in this file.
 * This file is self-contained and only depends on <stdint.h> / <stdbool.h> / <stddef.h>.
 *
 * @version 1.0.0
 * @date 2025-07-14
 */

#ifndef DEVICE_API_H
#define DEVICE_API_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ========================================================================== */
/*  Export Macros                                                             */
/* ========================================================================== */

#if defined(DEVICE_SHARED)
    #ifdef _WIN32
        #define DEVICE_API __declspec(dllexport)
    #else
        #define DEVICE_API __attribute__((visibility("default")))
    #endif
#elif defined(DEVICE_STATIC)
    #define DEVICE_API
#else
    #define DEVICE_API
#endif

/* ========================================================================== */
/*  Status Codes                                                              */
/* ========================================================================== */

typedef enum {
    DEVICE_OK                    = 0x00,
    DEVICE_ERR_CMD_INVALID_TYPE  = 0x01,
    DEVICE_ERR_CMD_INVALID_LEN   = 0x02,
    DEVICE_ERR_CMD_INVALID_PARAM = 0x03,
    DEVICE_ERR_CMD_EXEC          = 0x04,
    DEVICE_ERR_NO_RESPONSE       = 0x05,

    DEVICE_ERR_GPIO_OVER_RANGE   = 0x11,
    DEVICE_ERR_GPIO_INVALID_MODE = 0x12,
    DEVICE_ERR_GPIO_OTHER_ERROR  = 0x13,

    DEVICE_ERR_PARAM_INVALID_TYPE = 0x61,
    DEVICE_ERR_PARAM_INVALID_VAL = 0x62,
    DEVICE_ERR_PARAM_INVALID_LENGTH = 0x63,
    DEVICE_ERR_PARAM_GET_FAILED = 0x64,
    DEVICE_ERR_PARAM_SET_FAILED = 0x65,
    DEVICE_ERR_PARAM_SAVE_FAILED = 0x66,
    DEVICE_ERR_PARAM_OTHER_ERROR = 0x67,

    DEVICE_ERR_TAG_OTHER_ERROR = 0x80,
    DEVICE_ERR_TAG_NOT_SUPPORT = 0x81,
    DEVICE_ERR_TAG_INSUFFICIENT_PRIVILEGES = 0x82,
    DEVICE_ERR_TAG_MEMORY_OVERRUN = 0x83,
    DEVICE_ERR_TAG_MEMORY_LOCKED = 0x84,
    DEVICE_ERR_TAG_CRYPTOGRAPHIC_SUITE_ERROR = 0x85,
    DEVICE_ERR_TAG_COMMAND_NOT_ENCAPSULATED = 0x86,
    DEVICE_ERR_TAG_TAG_RESPONSEBUFFER_OVERFLOW = 0x87,
    DEVICE_ERR_TAG_SECURITY_TIMEOUT = 0x88,
    DEVICE_ERR_TAG_INSUFFICIENT_POWER = 0x8B,
    DEVICE_ERR_TAG_NON_SPECIFIC_ERROR = 0x8C,
    DEVICE_ERR_TAG_LOST          = 0x8E,

    DEVICE_ERR_RFID_INVALID_REGION = 0x90,
    DEVICE_ERR_RFID_INVALID_VALUE = 0x91,
    DEVICE_ERR_RFID_EXEC         = 0x9A,
    DEVICE_ERR_RFID_NO_ACK = 0x9B,
    DEVICE_ERR_RFID_INTERNAL_ERROR = 0x9E,
    DEVICE_ERR_RFID_OTHER_ERROR = 0x9F,

    DEVICE_ERR_BUSY              = 0xFB,
    DEVICE_ERR_INTERNAL          = 0xFE,
    DEVICE_ERR_INVALID_LINK      = 0xFF,
} device_status_t;

/* ========================================================================== */
/*  Parameter Types                                                           */
/* ========================================================================== */

typedef enum {
    DEVICE_PARAM_TYPE_DEVICE = 0x00,
    DEVICE_PARAM_TYPE_RFID   = 0x01,
    DEVICE_PARAM_TYPE_UHF    = 0x02,
    DEVICE_PARAM_TYPE_APP    = 0x10,
} device_param_type_t;

/* ========================================================================== */
/*  GPIO Related Enums                                                        */
/* ========================================================================== */

typedef enum {
    DEVICE_GPIO1  = 0,
    DEVICE_GPIO2  = 1,
    DEVICE_GPIO3  = 2,
    DEVICE_GPIO4  = 3,
    DEVICE_GPIO5  = 4,
    DEVICE_GPIO6  = 5,
    DEVICE_GPIO7  = 6,
    DEVICE_GPIO8  = 7,
} device_gpio_index_t;

typedef enum {
    DEVICE_GPIO_OUTPUT = 0,
    DEVICE_GPIO_INPUT  = 1,
} device_gpio_mode_t;

typedef enum {
    ANTENNA_MASK_1 = 0x0001,
    ANTENNA_MASK_2 = 0x0002,
    ANTENNA_MASK_3 = 0x0004,
    ANTENNA_MASK_4 = 0x0008,
    ANTENNA_MASK_5 = 0x0010,
    ANTENNA_MASK_6 = 0x0020,
    ANTENNA_MASK_7 = 0x0040,
    ANTENNA_MASK_8 = 0x0080,
    ANTENNA_MASK_9 = 0x0100,
    ANTENNA_MASK_10 = 0x0200,
    ANTENNA_MASK_11 = 0x0400,
    ANTENNA_MASK_12 = 0x0800,
    ANTENNA_MASK_13 = 0x1000,
    ANTENNA_MASK_14 = 0x2000,
    ANTENNA_MASK_15 = 0x4000,
    ANTENNA_MASK_16 = 0x8000,
} antenna_mask_t;

typedef enum  {
    ANTENNA_NONE = 0,
    ANTENNA_1 = 1,
    ANTENNA_2 = 2,
    ANTENNA_3 = 3,
    ANTENNA_4 = 4,
    ANTENNA_5 = 5,
    ANTENNA_6 = 6,
    ANTENNA_7 = 7,
    ANTENNA_8 = 8,
    ANTENNA_9 = 9,
    ANTENNA_10 = 10,
    ANTENNA_11 = 11,
    ANTENNA_12 = 12,
    ANTENNA_13 = 13,
    ANTENNA_14 = 14,
    ANTENNA_15 = 15,
    ANTENNA_16 = 16,
}antenna_index_t;


typedef enum {
    SELECT_MASK_1 = 0x0001,
    SELECT_MASK_2 = 0x0002,
    SELECT_MASK_3 = 0x0004,
    SELECT_MASK_4 = 0x0008,
    SELECT_MASK_5 = 0x0010,
    SELECT_MASK_6 = 0x0020,
    SELECT_MASK_7 = 0x0040,
    SELECT_MASK_8 = 0x0080,
    SELECT_MASK_9 = 0x0100,
    SELECT_MASK_10 = 0x0200,
    SELECT_MASK_11 = 0x0400,
    SELECT_MASK_12 = 0x0800,
    SELECT_MASK_13 = 0x1000,
    SELECT_MASK_14 = 0x2000,
    SELECT_MASK_15 = 0x4000,
    SELECT_MASK_16 = 0x8000,
} select_mask_t;

/* ========================================================================== */
/*  OTA Types                                                                 */
/* ========================================================================== */

typedef enum {
	OTA_TYPE_SYSTEM = 0x00010000, // Mainboard system
	OTA_TYPE_APPLICATION = 0x00011000, // Mainboard application
	OTA_TYPE_ETH = 0x00012000, // Mainboard Ethernet
	OTA_TYPE_WIFI = 0x00013000, // Mainboard WiFi
	OTA_TYPE_BLE = 0x00014000, // Mainboard Bluetooth
	OTA_TYPE_RF = 0x00020000, // RF module
	OTA_TYPE_RFIC = 0x00021000, // RFIC
} device_ota_type_t;

/* ========================================================================== */
/*  UHF SWR Test Result                                                       */
/* ========================================================================== */

typedef struct {
    uint32_t frequency_khz;
    float    swr;
} device_uhf_swr_result_t;

/* ========================================================================== */
/*  RFID Tag Model                                                            */
/* ========================================================================== */

#define DEVICE_RFID_TAG_EPC_MAX  62
#define DEVICE_RFID_TAG_TID_MAX  32
#define DEVICE_RFID_TAG_DATA_MAX 255

typedef struct {
    uint8_t  type;
    uint16_t  pc;
    uint8_t  epc[DEVICE_RFID_TAG_EPC_MAX];
    uint8_t  epc_len;
    uint8_t  tid[DEVICE_RFID_TAG_TID_MAX];
    uint8_t  tid_len;
    uint8_t  data[DEVICE_RFID_TAG_DATA_MAX];
    uint8_t  data_len;
    int16_t  rssi;        /**< 0.01dB */
    uint8_t  antenna;
    uint32_t count;
    uint32_t frequency_khz;
    uint16_t phase;       /**< 0.1 degrees */
    uint32_t timestamp_sec;   /**< Timestamp in seconds */
    uint32_t timestamp_ns;    /**< Timestamp in nanoseconds */

    int16_t temperature; /**< Temperature in 0.01 degrees Celsius */
    int16_t humidity;      /**< Humidity in 0.01% */
} device_rfid_tag_t;

/* ========================================================================== */
/*  Device Handle (Opaque Pointer)                                            */
/* ========================================================================== */

typedef void* device_handle_t;

/* ========================================================================== */
/*  Callback Function Types                                                   */
/* ========================================================================== */

typedef void (*device_on_connected_cb)    (device_handle_t dev, void* userdata);
typedef void (*device_on_disconnect_cb)   (device_handle_t dev, int will_reconnect, void* userdata);
typedef void (*device_on_reconnected_cb)  (device_handle_t dev, void* userdata);
typedef void (*device_on_rfid_tag_cb)     (device_handle_t dev, const device_rfid_tag_t* tag, void* userdata);
typedef void (*device_on_rfid_stop_cb)    (device_handle_t dev, uint8_t reason, void* userdata);
typedef void (*device_on_gpio_event_cb)   (device_handle_t dev, device_gpio_index_t gpio, int state, void* userdata);
typedef void (*device_on_peripheral_cb)   (device_handle_t dev, const uint8_t* data, uint16_t len, void* userdata);
typedef void (*device_on_ota_progress_cb) (device_handle_t dev, float percent, void* userdata);
typedef void (*device_on_log_cb)          (device_handle_t dev, const char* msg, void* userdata);

/**
 * @brief Callback registration structure
 */
typedef struct {
    device_on_connected_cb    on_connected;
    device_on_disconnect_cb   on_disconnect;
    device_on_reconnected_cb  on_reconnected;
    device_on_rfid_tag_cb     on_rfid_tag;
    device_on_rfid_stop_cb    on_rfid_stop;
    device_on_gpio_event_cb   on_gpio_event;
    device_on_peripheral_cb   on_peripheral_data;
    device_on_ota_progress_cb on_ota_progress;
    device_on_log_cb          on_log;
    void*                     userdata;         /**< Passed through to all callbacks */
} device_callbacks_t;

/* ========================================================================== */
/*  Lifecycle                                                                 */
/* ========================================================================== */

/**
 * @brief Create device instance
 * @param cbs Callback function set, can be NULL
 * @return Device handle, returns NULL on failure
 */
DEVICE_API device_handle_t device_create(const device_callbacks_t* cbs);

/**
 * @brief Destroy device instance and release all resources
 * @param dev Device handle
 */
DEVICE_API void device_destroy(device_handle_t dev);

/* ========================================================================== */
/*  Connection Management                                                      */
/* ========================================================================== */

/**
 * @brief Open device connection
 * @param dev   Device handle
 * @param param Connection parameters, format:
 *              - Serial: "COM1:115200" or "/dev/ttyUSB0:115200"
 *              - TCP:  "192.168.1.201:8090"
 * @param addr  Device address, 0xFF = broadcast
 * @return DEVICE_OK on success, other values are error codes
 */
DEVICE_API device_status_t device_open(device_handle_t dev, const char* param, uint8_t addr);

/**
 * @brief Close device connection
 * @param dev Device handle
 */
DEVICE_API void device_close(device_handle_t dev);

/**
 * @brief Check if connection is open
 * @param dev Device handle
 * @return 1=connected, 0=not connected
 */
DEVICE_API int device_is_connected(device_handle_t dev);

/**
 * @brief Check connection via connection confirmation command
 * @param dev Device handle
 * @return DEVICE_OK if connection is normal, other values are error codes
 */
DEVICE_API device_status_t device_check_connect(device_handle_t dev);

/* ========================================================================== */
/*  Parameter Management                                                      */
/* ========================================================================== */

// System parameters
#define DEVICE_SYSTEM_VERSION	    	0x00
#define DEVICE_VERSION	    			0x01
#define DEVICE_SN			    		0x02
#define DEVICE_ETH_MAC			  		0x03
#define DEVICE_WIFI_MAC		    		0x04
#define DEVICE_BT_MAC			       	0x05
#define DEVICE_ETH_FWVER			    0x0A
#define DEVICE_WIFI_FWVER			   	0x0B
#define DEVICE_BT_FWVER			    	0x0C

// Device parameters
#define DEVICE_NAME			    		0x10
#define DEVICE_ASSERT_ID		    	0x11
#define DEVICE_DATETIME			    	0x12
#define DEVICE_PWRON_INFO		    	0x13
#define DEVICE_PWRON_EVENT			   	0x14 

#define DEVICE_TEMPERATURE		    	0x1E
#define DEVICE_SOC			    		0x1F

// Communication parameters
#define DEVICE_ADDR			    		0x20
#define DEVICE_RS232_ATTR				0x21
#define DEVICE_RS485_ATTR				0x22

#define DEVICE_ETH_NETWORK			    0x24
#define DEVICE_WIFI_NETWORK			   	0x25
#define DEVICE_WIFI_CONFIG				0x26
#define DEVICE_WIFI_CONNECT_INFO	    0x27
#define DEVICE_BT_ATTR			        0x28

#define DEVICE_DEFAULT_LINK			    0x2F

// Buzzer and indicators
#define DEVICE_BUZZER_ATTR			    0x51
#define DEVICE_LED_COMM_ATTR		   	0x52
#define DEVICE_LED_TAG_ATTR			  	0x53

// GPIO interrupt events
#define DEVICE_GPIO1_EVENT			    0x60
#define DEVICE_GPIO2_EVENT			    0x61
#define DEVICE_GPIO3_EVENT			    0x62
#define DEVICE_GPIO4_EVENT			    0x63
#define DEVICE_GPIO5_EVENT			   	0x64
#define DEVICE_GPIO6_EVENT			   	0x65
#define DEVICE_GPIO7_EVENT			   	0x66
#define DEVICE_GPIO8_EVENT			   	0x67

// RFID info
#define RFID_VERSION					0x01
#define RFID_SN							0x02
#define RFID_PROPERTY					0x0F

// RFID parameters
#define RFID_PROTOCOL					0x10
#define RFID_TEMPERATURE				0x1E
#define RFID_IDLE						0x20

// EAS
#define RFID_EAS						0x50
#define RFID_WHITELIST					0x5F

// Special output
#define RFID_WIEGAND_OUTPUT				0x60
#define RFID_SPECIAL_OUTPUT				0x61
#define RFID_KEYBOARD_OUTPUT			0x62

#define RFID_ANTENNA_STATE				0x80
#define RFID_ANTENNA					0x81
#define RFID_ANTENNA_POWER				0x82

#define RFID_ANT1_POWER					0x90
#define RFID_ANT2_POWER					0x91
#define RFID_ANT3_POWER					0x92
#define RFID_ANT4_POWER					0x93
#define RFID_ANT5_POWER					0x94
#define RFID_ANT6_POWER					0x95
#define RFID_ANT7_POWER					0x96
#define RFID_ANT8_POWER					0x97
#define RFID_ANT9_POWER					0x98
#define RFID_ANT10_POWER				0x99
#define RFID_ANT11_POWER				0x9A
#define RFID_ANT12_POWER				0x9B
#define RFID_ANT13_POWER				0x9C
#define RFID_ANT14_POWER				0x9D
#define RFID_ANT15_POWER				0x9E
#define RFID_ANT16_POWER				0x9F
#define RFID_ANT17_POWER				0xA0
#define RFID_ANT18_POWER				0xA1
#define RFID_ANT19_POWER				0xA2
#define RFID_ANT20_POWER				0xA3
#define RFID_ANT21_POWER				0xA4
#define RFID_ANT22_POWER				0xA5
#define RFID_ANT23_POWER				0xA6
#define RFID_ANT24_POWER				0xA7
#define RFID_ANT25_POWER				0xA8
#define RFID_ANT26_POWER				0xA9
#define RFID_ANT27_POWER				0xAA
#define RFID_ANT28_POWER				0xAB
#define RFID_ANT29_POWER				0xAC
#define RFID_ANT30_POWER				0xAD
#define RFID_ANT31_POWER				0xAE
#define RFID_ANT32_POWER				0xAF

#define RFID_PRESET_KEY1				0xF0
#define RFID_PRESET_KEY2				0xF1
#define RFID_PRESET_KEY3				0xF2
#define RFID_PRESET_KEY4				0xF3
#define RFID_PRESET_KEY5				0xF4
#define RFID_PRESET_KEY6				0xF5
#define RFID_PRESET_KEY7				0xF6
#define RFID_PRESET_KEY8				0xF7


#define UHF_PROPERTY					0x0F

#define UHF_FREQUENCY					0x10


#define UHF_EPC_BASEBAND				0x60
#define UHF_EPC_ALGO					0x61

#define UHF_TAG_SELECT1				   	0x80
#define UHF_TAG_SELECT2				   	0x81
#define UHF_TAG_SELECT3				   	0x82
#define UHF_TAG_SELECT4			    	0x83
#define UHF_TAG_SELECT5			   		0x84
#define UHF_TAG_SELECT6			   		0x85
#define UHF_TAG_SELECT7			   		0x86
#define UHF_TAG_SELECT8			   		0x87
#define UHF_TAG_SELECT9			    	0x88
#define UHF_TAG_SELECT10			   	0x89
#define UHF_TAG_SELECT11			   	0x8A
#define UHF_TAG_SELECT12			   	0x8B
#define UHF_TAG_SELECT13			   	0x8C
#define UHF_TAG_SELECT14			   	0x8D
#define UHF_TAG_SELECT15			   	0x8E
#define UHF_TAG_SELECT16			   	0x8F

struct device_param_value {
    uint8_t data[255];
    uint8_t len;
};
typedef struct device_param_value device_param_value_t;

/**
 * @brief Read parameter
 * @param dev      Device handle
 * @param type     Parameter type
 * @param id       Parameter ID
 * @param param_value Output parameter value structure pointer
 * @return DEVICE_OK on success
 */
DEVICE_API device_status_t device_get_param(device_handle_t dev,
                                            device_param_type_t type, uint8_t id,
                                            device_param_value_t* param_value);

/**
 * @brief Set parameter
 * @param dev     Device handle
 * @param type    Parameter type
 * @param id      Parameter ID
 * @param param_value Input parameter value structure pointer
 * @param save        Whether to save (true=save)
 * @return DEVICE_OK on success
 */
DEVICE_API device_status_t device_set_param(device_handle_t dev,
                                            device_param_type_t type, uint8_t id,
                                            const device_param_value_t* param_value,
                                            bool save);

/* Convenience macros */
#define device_get_device_param(dev, id, param_value) \
    device_get_param(dev, DEVICE_PARAM_TYPE_DEVICE, (uint8_t)(id), param_value)
#define device_get_rfid_param(dev, id, param_value) \
    device_get_param(dev, DEVICE_PARAM_TYPE_RFID, (uint8_t)(id), param_value)
#define device_get_uhf_param(dev, id, param_value) \
    device_get_param(dev, DEVICE_PARAM_TYPE_UHF, (uint8_t)(id), param_value)
#define device_get_app_param(dev, id, param_value) \
    device_get_param(dev, DEVICE_PARAM_TYPE_APP, (uint8_t)(id), param_value)

#define device_set_device_param(dev, id, param_value, save) \
    device_set_param(dev, DEVICE_PARAM_TYPE_DEVICE, (uint8_t)(id), param_value, save)
#define device_set_rfid_param(dev, id, param_value, save) \
    device_set_param(dev, DEVICE_PARAM_TYPE_RFID, (uint8_t)(id), param_value, save)
#define device_set_uhf_param(dev, id, param_value, save) \
    device_set_param(dev, DEVICE_PARAM_TYPE_UHF, (uint8_t)(id), param_value, save)    
#define device_set_app_param(dev, id, param_value, save) \
    device_set_param(dev, DEVICE_PARAM_TYPE_APP, (uint8_t)(id), param_value, save)

#define device_save_param(dev, type, id, param_value) \
    device_set_param(dev, type, (uint8_t)(id), param_value, true)

#define device_save_device_param(dev, id, param_value) \
    device_set_device_param(dev, id, param_value, true)
#define device_save_rfid_param(dev, id, param_value) \
    device_set_rfid_param(dev, id, param_value, true)
#define device_save_uhf_param(dev, id, param_value) \
    device_set_uhf_param(dev, id, param_value, true)
#define device_save_app_param(dev, id, param_value) \
    device_set_app_param(dev, id, param_value, true)
    
void param_reset(device_param_value_t* param);
    
/* ========================================================================== */
/*  Parameter Helper Append Functions                                          */
/* ========================================================================== */

void param_append_bool(device_param_value_t* param, bool value);
void param_append_byte(device_param_value_t* param, uint8_t value);
void param_append_short(device_param_value_t* param, int16_t value);
void param_append_ushort(device_param_value_t* param, uint16_t value);
void param_append_int(device_param_value_t* param, int32_t value);
void param_append_uint(device_param_value_t* param, uint32_t value);
void param_append_bytes(device_param_value_t* param, const uint8_t* value, int len);
void param_append_array(device_param_value_t* param, const uint8_t* value, int len);
void param_append_string(device_param_value_t* param, const char* value);

/* ========================================================================== */
/*  Parameter Helper Take Functions                                            */
/* ========================================================================== */

bool     param_take_bool(const device_param_value_t* param, int pos);
uint8_t  param_take_byte(const device_param_value_t* param, int pos);
int16_t  param_take_short(const device_param_value_t* param, int pos);
uint16_t param_take_ushort(const device_param_value_t* param, int pos);
int32_t  param_take_int(const device_param_value_t* param, int pos);
uint32_t param_take_uint(const device_param_value_t* param, int pos);
int      param_take_bytes(const device_param_value_t* param, int pos, int len, uint8_t* out, int out_size);
int      param_take_array(const device_param_value_t* param, int pos, uint8_t* out, int out_size);
int      param_take_string(const device_param_value_t* param, int pos, char* out, int out_size);

/* ========================================================================== */
/*  RFID Operations                                                           */
/* ========================================================================== */
typedef enum uhf_bank_type {
	UHF_BANK_RESERVE = 0,
	UHF_BANK_EPC = 1,
	UHF_BANK_TID = 2,
	UHF_BANK_USER = 3,
} uhf_bank_type_t;

typedef enum uhf_lock_bank_type {
	UHF_LOCK_BANK_USER = 0,
	UHF_LOCK_BANK_TID = 1,
	UHF_LOCK_BANK_EPC = 2,
	UHF_LOCK_BANK_ACCESSPWD = 3,
	UHF_LOCK_BANK_KILLPWD = 4,
} uhf_lock_bank_type_t;

/*
0: Writable
1: Always writable
2: Writable with password
3: Permanently not writable
*/
typedef enum rfid_lock_type {
	RFID_LOCK_TYPE_WRITEABLE = 0,
	RFID_LOCK_TYPE_WRITE_ALWAYS = 1,
	RFID_LOCK_TYPE_WRITE_WITH_PASSWD = 2, //
	RFID_LOCK_TYPE_NOT_WRITEABLE = 3, 
} rfid_lock_type_t;

// bit0: Cache tags (do not transmit tag data)
#define RFID_TAGOPS_MASK_ENABLE_CACHE			(1 << 0)
// bit1: Request TID
#define RFID_TAGOPS_MASK_REQ_TID			(1 << 1)
// bit2: Request transmission frequency
#define RFID_TAGOPS_MASK_REQ_FREQ			(1 << 2)
// bit3: Request transmission phase
#define RFID_TAGOPS_MASK_REQ_PHASE			(1 << 3)
// bit4: Request read count
#define RFID_TAGOPS_MASK_REQ_COUNT			(1 << 4)
// bit5: Request timestamp
#define RFID_TAGOPS_MASK_REQ_TIMESTAMP		(1 << 5)
// bit6: Request rssi
#define RFID_TAGOPS_MASK_REQ_RSSI			(1 << 6)

// bit8: Enable read timeout option
#define RFID_TAGOPS_MASK_ENABLE_TIMEOUT		(1 << 8)
// bit9: Enable tag selection option
#define RFID_TAGOPS_MASK_ENABLE_SELECT			(1 << 9)
// bit10: Enable access password option
#define RFID_TAGOPS_MASK_ENABLE_PASSWD		(1 << 10)
// bit11: Enable preset password option
#define RFID_TAGOPS_MASK_ENABLE_PRESET_PASSWD		(1 << 11)

// bit16: Enable temperature tag option
#define RFID_TAGOPS_MASK_ENABLE_TEMP			(1 << 16)
// bit17: Enable humidity tag option
#define RFID_TAGOPS_MASK_ENABLE_HUMIDITY		(1 << 17)
// bit18: Enable LED tag option
#define RFID_TAGOPS_MASK_ENABLE_LED			(1 << 18)
// bit19: Enable ImpinJ tag option
#define RFID_TAGOPS_MASK_ENABLE_IMPINJ			(1 << 19)
// bit20: Enable NXP tag option
#define RFID_TAGOPS_MASK_ENABLE_NXP			(1 << 20)

// bit24: Enable read tag option
#define RFID_TAGOPS_MASK_ENABLE_READ			(1 << 24)

// bit29: Discard tags with RSSI less than specified value
#define RFID_TAGOPS_MASK_ENABLE_RSSI_THRESHOLD		(1 << 29)
// bit30: Tag deduplication (merge), unit: ms
#define RFID_TAGOPS_MASK_ENABLE_MERGE			(1 << 30)


typedef struct rfid_inventory
{
	uint32_t mask;					// Inventory function mask
	uint32_t timeout;		 		// Inventory timeout
	uint32_t select_en;		 		// Inventory tag selection enable
	uint32_t passwd;		 	// Access password
    uint32_t preset_passwd;		 	// Preset password

	union {
		struct {
			uint32_t type;	
			uint32_t args;	
		} temperature;
		struct {
			uint32_t type;	
			uint32_t args;	
		} humidity;
		struct {
			uint32_t type;	
			uint32_t args;	
		} led;
#define IMPINJ_TAG_ENABLE_TAGFOCUS		(1<<0)
#define IMPINJ_TAG_ENABLE_FASTID		(1<<1)
#define IMPINJ_TAG_ENABLE_GEN2X			(1<<8)

		uint32_t impinj;
		uint32_t nxp;
	};

	int16_t rssi_threshold;     // RSSI filter threshold, unit: 0.01dBm
	uint32_t merge_time;        // Deduplication time, unit: ms

	struct {
		uint16_t bank; 			    // rfid_bank_type
		uint16_t wpoint;
		uint16_t wlength;
	} read;
} rfid_inventory_t;

typedef struct rfid_read
{
	uint32_t mask;					// Read function mask
	uint32_t timeout;		 		// Read timeout
	uint32_t select_en;		 		// Read tag selection enable
	uint32_t passwd;		 	    // Access password
	uint32_t preset_passwd;		 	// Preset password

	uint16_t bank; 			        // rfid_bank_type
	uint16_t wpoint;
	uint16_t wlength;
	uint8_t data[DEVICE_RFID_TAG_DATA_MAX];
} rfid_read_t;

typedef struct rfid_write
{
	uint32_t mask;					// Write function mask
	uint32_t timeout;		 		// Write timeout
	uint32_t select_en;		 		// Write tag selection enable
	uint32_t passwd;		 	    // Access password
	uint32_t preset_passwd;		 	// Preset password

	uint16_t wblock;
	uint16_t bank; 			        // rfid_bank_type
	uint16_t wpoint;
	uint16_t wlength;
	uint8_t data[DEVICE_RFID_TAG_DATA_MAX];
} rfid_write_t;

typedef struct rfid_lock
{
	uint32_t mask;					// Lock function mask
	uint32_t timeout;		 		// Lock timeout
	uint32_t select_en;		 		// Lock tag selection enable
	uint32_t passwd;		 	    // Access password
    uint32_t preset_passwd;		 	// Preset password

	uint16_t bank; 			        // rfid_lock_bank_type
	uint16_t type;			        // rfid_lock_type
} rfid_lock_t;

typedef struct rfid_kill
{
	uint32_t mask;					// Kill function mask
	uint32_t timeout;		 		// Kill timeout
	uint32_t select_en;		 		// Kill tag selection enable
	uint32_t passwd;		 	    // Kill password
    uint32_t preset_passwd;		 	// Preset password
} rfid_kill_t;

/**
 * @brief Async inventory
 * @param dev              Device handle
 * @param inventory        Inventory parameters
 * @return DEVICE_OK on successful start
 */
DEVICE_API device_status_t device_rfid_inventory(device_handle_t dev, const rfid_inventory_t* inventory);


/**
 * @brief Stop inventory (retries 3 times internally)
 * @param dev Device handle
 * @return DEVICE_OK on success
 */
DEVICE_API device_status_t device_rfid_stop(device_handle_t dev);

/**
 * @brief Read RFID tag data
 * @param dev        Device handle
 * @param read       Read parameters
 * @return DEVICE_OK on success
 */
DEVICE_API device_status_t device_rfid_read_tag(device_handle_t dev,
                                               rfid_read_t* read);

/**
 * @brief Write RFID tag data
 * @param dev        Device handle
 * @param write      Write parameters
 * @return DEVICE_OK on success
 */
DEVICE_API device_status_t device_rfid_write_tag(device_handle_t dev,
                                                const rfid_write_t* write);

/**
 * @brief Lock RFID tag
 * @param dev        Device handle
 * @param lock       Lock parameters
 * @return DEVICE_OK on success
 */
DEVICE_API device_status_t device_rfid_lock_tag(device_handle_t dev,
                                                const rfid_lock_t* lock);

/**
 * @brief Kill RFID tag
 * @param dev      Device handle
 * @param kill       Kill parameters
 * @return DEVICE_OK on success
 */
DEVICE_API device_status_t device_rfid_kill_tag(device_handle_t dev,
                                                const rfid_kill_t* kill);

/* ========================================================================== */
/*  Peripheral and GPIO                                                       */
/* ========================================================================== */

/**
 * @brief Buzzer
 * @param dev          Device handle
 * @param duration_ms  Beep duration in milliseconds
 * @return DEVICE_OK on success
 */
DEVICE_API device_status_t device_beep(device_handle_t dev, uint32_t duration_ms);

/**
 * @brief Query GPIO mode
 * @param dev     Device handle
 * @param modes   GPIO mode array, each element is the mode of a GPIO, outputs each GPIO's mode
 * @param out_mask Output GPIO available mask
 * @return DEVICE_OK on success
 */
DEVICE_API device_status_t device_get_gpio_mode(device_handle_t dev,
                                                device_gpio_mode_t* modes,
                                                uint32_t* out_mask);

/**
 * @brief Set GPIO mode
 * @param dev  Device handle
 * @param mask GPIO mask, specifies which GPIO bits to set
 * @param modes GPIO mode array, each element is the mode of a GPIO, sets each GPIO's mode
 * @return DEVICE_OK on success
 */
DEVICE_API device_status_t device_set_gpio_mode(device_handle_t dev,
                                                uint32_t mask,
                                                const device_gpio_mode_t* modes);

/**
 * @brief Query GPIO level
 * @param dev       Device handle
 * @param out_state Output level state array, each element is the level state of a GPIO, outputs each GPIO's level state
 * @param out_mask  Output GPIO available mask
 * @return DEVICE_OK on success
 */
DEVICE_API device_status_t device_get_gpio_state(device_handle_t dev,
                                                 uint32_t* out_state, uint32_t* out_mask);

/**
 * @brief Set GPIO level
 * @param dev   Device handle
 * @param mask  GPIO mask, specifies which GPIO bits to set
 * @param state Level state
 * @return DEVICE_OK on success
 */
DEVICE_API device_status_t device_set_gpio_state(device_handle_t dev,
                                                 uint32_t mask, uint32_t state);

/**
 * @brief Peripheral control
 * @param dev      Device handle
 * @param type     Peripheral type
 * @param index    Peripheral index
 * @param in_data  Input data
 * @param in_len   Input data length
 * @param out_data Output data buffer
 * @param out_len  Output data length
 * @return DEVICE_OK on success
 */
DEVICE_API device_status_t device_peripheral_ctrl(device_handle_t dev,
                                                  uint8_t type, uint8_t index,
                                                  const uint8_t* in_data, uint8_t in_len,
                                                  uint8_t* out_data, uint8_t* out_len);

/* ========================================================================== */
/*  OTA Upgrade                                                               */
/* ========================================================================== */

/**
 * @brief Local firmware upgrade
 * @param dev          Device handle
 * @param firmware     Firmware data
 * @param firmware_len Firmware length
 * @return DEVICE_OK on success
 */
DEVICE_API device_status_t device_ota_local(device_handle_t dev,
                                            const uint8_t* firmware, uint32_t firmware_len);

/**
 * @brief Remote upgrade (device downloads automatically)
 * @param dev  Device handle
 * @param type OTA type
 * @param url  Firmware URL
 * @return DEVICE_OK on success
 */
DEVICE_API device_status_t device_ota_remote(device_handle_t dev,
                                             device_ota_type_t type, const char* url);

/* ========================================================================== */
/*  Device Management                                                         */
/* ========================================================================== */

/**
 * @brief Restart device
 * @param dev Device handle
 * @return DEVICE_OK on success
 */
DEVICE_API device_status_t device_restart(device_handle_t dev);

/**
 * @brief Restore factory settings (timeout 5000ms)
 * @param dev Device handle
 * @return DEVICE_OK on success
 */
DEVICE_API device_status_t device_restore(device_handle_t dev);

/* ========================================================================== */
/*  Version Information                                                       */
/* ========================================================================== */

/**
 * @brief Get SDK version string
 * @param buf      Output buffer for version string
 * @param buf_size Buffer size
 * @return Number of characters written (excluding null terminator)
 */
DEVICE_API int device_api_version(char* buf, int buf_size);

#ifdef __cplusplus
}
#endif

#endif /* DEVICE_API_H */
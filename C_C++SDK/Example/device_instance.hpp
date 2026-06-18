/**
 * @file device_instance.hpp
 * @brief DeviceInstance C++ wrapper with RAII handle management
 *
 * Usage:
 *   DeviceInstance dev;
 *   dev.setCallbacks(myCallbacks);
 *   dev.open("COM3:115200", 0xFF);
 *   dev.rfidInventory(5000, 0xFF);
 *   dev.close();
 */

#ifndef DEVICE_INSTANCE_HPP
#define DEVICE_INSTANCE_HPP

#include "device_api.h"
#include <cstdint>
#include <string>

namespace SeeMoreIOT {

/**
 * @brief Device status code wrapper
 */
class DeviceStatus {
public:
    DeviceStatus(device_status_t s) : status_(s) {}
    bool ok() const { return status_ == DEVICE_OK; }
    device_status_t value() const { return status_; }
    operator device_status_t() const { return status_; }

private:
    device_status_t status_;
};

/**
 * @brief DeviceInstance C++ wrapper
 */
class DeviceInstance {
public:
    DeviceInstance() : handle_(nullptr) {
        handle_ = device_create(nullptr);
    }

    explicit DeviceInstance(const device_callbacks_t& cbs) : handle_(nullptr) {
        handle_ = device_create(&cbs);
    }

    ~DeviceInstance() {
        if (handle_) {
            device_close(handle_);
            device_destroy(handle_);
            handle_ = nullptr;
        }
    }

    /* Disable copy */
    DeviceInstance(const DeviceInstance&) = delete;
    DeviceInstance& operator=(const DeviceInstance&) = delete;

    /* Enable move */
    DeviceInstance(DeviceInstance&& other) noexcept : handle_(other.handle_) {
        other.handle_ = nullptr;
    }

    DeviceInstance& operator=(DeviceInstance&& other) noexcept {
        if (this != &other) {
            if (handle_) {
                device_close(handle_);
                device_destroy(handle_);
            }
            handle_ = other.handle_;
            other.handle_ = nullptr;
        }
        return *this;
    }

    /* ===== Connection Management ===== */

    DeviceStatus open(const char* param, uint8_t addr = 0xFF) {
        return DeviceStatus(device_open(handle_, param, addr));
    }

    DeviceStatus open(const std::string& param, uint8_t addr = 0xFF) {
        return DeviceStatus(device_open(handle_, param.c_str(), addr));
    }

    void close() {
        device_close(handle_);
    }

    bool isConnected() const {
        return device_is_connected(handle_) != 0;
    }

    DeviceStatus checkConnect() {
        return DeviceStatus(device_check_connect(handle_));
    }

    /* ===== Callbacks ===== */

    void setCallbacks(const device_callbacks_t& cbs) {
        /* Note: C API callbacks are registered during create, need rebuild here */
        /* Simplified implementation: pass during construction */
        (void)cbs;
    }

    /* ===== Parameter Management ===== */

    DeviceStatus getParam(device_param_type_t type, uint8_t id,
                           device_param_value_t* param_value) {
        return DeviceStatus(device_get_param(handle_, type, id, param_value));
    }

    DeviceStatus setParam(device_param_type_t type, uint8_t id,
                           const device_param_value_t* param_value, bool save = true) {
        return DeviceStatus(device_set_param(handle_, type, id, param_value, save));
    }

    /* Convenience: save parameter (always persist) */
    DeviceStatus saveDeviceParam(uint8_t id, const device_param_value_t* param_value) {
        return DeviceStatus(device_save_device_param(handle_, id, param_value));
    }
    DeviceStatus saveRFIDParam(uint8_t id, const device_param_value_t* param_value) {
        return DeviceStatus(device_save_rfid_param(handle_, id, param_value));
    }
    DeviceStatus saveUHFParam(uint8_t id, const device_param_value_t* param_value) {
        return DeviceStatus(device_save_uhf_param(handle_, id, param_value));
    }
    DeviceStatus saveAppParam(uint8_t id, const device_param_value_t* param_value) {
        return DeviceStatus(device_save_app_param(handle_, id, param_value));
    }

    /* ===== RFID ===== */

    DeviceStatus rfidInventory(const rfid_inventory_t* inventory = nullptr) {
        return DeviceStatus(device_rfid_inventory(handle_, inventory));
    }


    DeviceStatus rfidStop() {
        return DeviceStatus(device_rfid_stop(handle_));
    }

    DeviceStatus rfidReadTag(rfid_read_t* read) {
        return DeviceStatus(
            device_rfid_read_tag(handle_, read));
    }

    DeviceStatus rfidWriteTag(const rfid_write_t* write) {
        return DeviceStatus(
            device_rfid_write_tag(handle_, write));
    }

    DeviceStatus rfidLockTag(const rfid_lock_t* lock) {
        return DeviceStatus(
            device_rfid_lock_tag(handle_, lock));
    }

    DeviceStatus rfidKillTag(const rfid_kill_t* kill) {
        return DeviceStatus(
            device_rfid_kill_tag(handle_, kill));
    }

    /* ===== Peripheral ===== */

    DeviceStatus beep(uint32_t durationMs = 50) {
        return DeviceStatus(device_beep(handle_, durationMs));
    }

    /* ===== GPIO ===== */

    DeviceStatus getGPIOMode(device_gpio_mode_t* modes, uint32_t* mask) {
        return DeviceStatus(device_get_gpio_mode(handle_, modes, mask));
    }

    DeviceStatus setGPIOMode(uint16_t mask, const device_gpio_mode_t* modes) {
        return DeviceStatus(device_set_gpio_mode(handle_, mask, modes));
    }

    DeviceStatus getGPIOState(uint32_t* state, uint32_t* mask) {
        return DeviceStatus(device_get_gpio_state(handle_, state, mask));
    }

    DeviceStatus setGPIOState(uint32_t mask, uint32_t state) {
        return DeviceStatus(device_set_gpio_state(handle_, mask, state));
    }

    /* ===== OTA ===== */

    DeviceStatus otaLocal(const uint8_t* firmware, uint32_t len) {
        return DeviceStatus(device_ota_local(handle_, firmware, len));
    }

    DeviceStatus otaRemote(device_ota_type_t type, const char* url) {
        return DeviceStatus(device_ota_remote(handle_, type, url));
    }

    /* ===== Device Management ===== */

    DeviceStatus restart() {
        return DeviceStatus(device_restart(handle_));
    }

    DeviceStatus restore() {
        return DeviceStatus(device_restore(handle_));
    }

    /* ===== Version ===== */

    std::string getVersion() {
        char buf[32];
        device_api_version(buf, sizeof(buf));
        return std::string(buf);
    }

    /* ===== Handle Access ===== */

    device_handle_t handle() const { return handle_; }

private:
    device_handle_t handle_;
};

} /* namespace SeeMoreIOT */

#endif /* DEVICE_INSTANCE_HPP */
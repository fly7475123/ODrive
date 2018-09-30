#ifndef __INTERFACE_CAN_HPP
#define __INTERFACE_CAN_HPP

#include <cmsis_os.h>
#include <stm32f4xx_hal.h>
#include "fibre/protocol.hpp"

#define CAN_CLK_HZ (42000000)
#define CAN_CLK_MHZ (42)

typedef struct {
    uint32_t id = 0x000;  // 11-bit max is 0x7ff, 29-bit max is 0x1FFFFFFF
    bool isExt = false;
    uint8_t len = 8;
    uint8_t buf[8] = {0, 0, 0, 0, 0, 0, 0, 0};
} CAN_message_t;

// Anonymous enum for defining the most common CAN baud rates
   
    enum {
        CAN_BAUD_125K = 125000,
        CAN_BAUD_250K = 250000,
        CAN_BAUD_500K = 500000,
        CAN_BAUD_1000K = 1000000,
        CAN_BAUD_1M = 1000000
    };

class ODriveCAN {
   public:
    struct Config_t {
        uint8_t node_id = 0;
        uint32_t baud = CAN_BAUD_250K;
    };

    ODriveCAN(CAN_HandleTypeDef *handle, ODriveCAN::Config_t &config);

    bool start_can_server();
    void can_server_thread();
    
    uint32_t write(CAN_message_t &txmsg);
    int read(CAN_message_t &rxmsg);

    osThreadId thread_id_;
    volatile bool thread_id_valid_ = false;

    auto make_protocol_definitions() {
        return make_protocol_member_list(
            make_protocol_object("config",
                                 make_protocol_ro_property("node_id", &config_.node_id),
                                 make_protocol_ro_property("baud_rate", &config_.baud)),
            make_protocol_function("set_node_id", *this, &ODriveCAN::set_node_id, "nodeID"),
            make_protocol_function("set_baud_rate", *this, &ODriveCAN::set_baud_rate, "baudRate"));
    }

   private:
    CAN_HandleTypeDef *handle_ = nullptr;
    ODriveCAN::Config_t &config_;

    void set_node_id(uint8_t nodeID);
    void set_baud_rate(uint32_t baudRate);
};

#endif  // __INTERFACE_CAN_HPP

/*
 * SPDX-FileCopyrightText: 2021-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp_modem_config.h"
#include "cxx_include/esp_modem_api.hpp"
#include <cxx_include/esp_modem_dce_factory.hpp>
#include "socket_commands.inc"
#include "sock_commands.hpp"

#pragma  once

namespace sock_dce {

class DCE : public ::esp_modem::GenericModule {
    using esp_modem::GenericModule::GenericModule;
public:

#define ESP_MODEM_DECLARE_DCE_COMMAND(name, return_type, num, ...) \
esp_modem::return_type name(__VA_ARGS__);

    DECLARE_SOCK_COMMANDS(declare name(Commandable *p, ...);)

#undef ESP_MODEM_DECLARE_DCE_COMMAND

    bool init_network();
    bool start(std::string host, int port);

    void init(int port);

    bool perform();

private:
    esp_modem::SignalGroup signal;

    void close_sock();

    void forwarding(uint8_t *data, size_t len);

    void check_async_replies(std::string_view &response) const;

    void send_cmd(std::string_view command)
    {
        dte->write((uint8_t *) command.begin(), command.size());
    }

    enum class status {
        IDLE,
        CONNECTING,
        CONNECTION_FAILED,
        SENDING,
        SENDING_1,
        SENDING_FAILED,
        RECEIVING,
        RECEIVING_FAILED
    };
    status state{status::IDLE};
    static constexpr uint8_t IDLE = 1;
    static constexpr uint8_t size = 100;
    std::array<uint8_t, size> buffer;
    size_t data_to_send = 0;
    int sock {-1};
    int listen_sock {-1};
    int data_ready_fd {-1};
};

std::unique_ptr<DCE> create(const esp_modem::dce_config *config, std::shared_ptr<esp_modem::DTE> dte);

}

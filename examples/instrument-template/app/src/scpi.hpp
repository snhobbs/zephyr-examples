#pragma once

#include "scpi/scpi.h"
#include <array>
#include <span>

const std::size_t kIdnLength = 4;
struct ScpiInterfaceContainer {
  /*
   * Container for an SCPI instance.
   * This is exclusively intended to contain the
   * */
public:
  std::array<const char *, kIdnLength> idn_fields_;
  std::span<const scpi_command_t> scpi_commands_;
  std::span<uint8_t> scpi_input_buffer_;
  std::span<scpi_error_t> scpi_error_queue_data_;
  scpi_interface_t scpi_interface_;
  scpi_t scpi_context_{};

  ScpiInterfaceContainer(const std::array<const char *, kIdnLength> idn,
                         const std::span<const scpi_command_t> command_table,
                         std::span<uint8_t> scpi_input_buffer,
                         std::span<scpi_error_t> scpi_error_queue_data,
                         const scpi_interface_t &scpi_interface)
      : idn_fields_{idn}, scpi_commands_{command_table},
        scpi_input_buffer_{scpi_input_buffer},
        scpi_error_queue_data_{scpi_error_queue_data},
        scpi_interface_{scpi_interface} {
    init();
  }

  void init() {
    /*
     * Note: scpi_units_def defined off the standard in scpi/units.c
     * */
    SCPI_Init(
        &scpi_context_,
        reinterpret_cast<const scpi_command_t *const>(scpi_commands_.data()),
        &scpi_interface_, scpi_units_def, idn_fields_.at(0), idn_fields_.at(1),
        idn_fields_.at(2), idn_fields_.at(3),
        reinterpret_cast<char *>(scpi_input_buffer_.data()),
        scpi_input_buffer_.size(),
        reinterpret_cast<scpi_error_t *>(scpi_error_queue_data_.data()),
        scpi_error_queue_data_.size());
  }

  scpi_bool_t  input(const char *str, const std::size_t size) {
    return SCPI_Input(&scpi_context_, str, size);
  }
};

extern const std::span<const scpi_command_t> scpi_commands_table;

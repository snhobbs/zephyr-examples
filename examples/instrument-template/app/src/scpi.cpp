/*
 * Copyright (c) 2025 ElectroOptical Innovations. All Rights Reserved.
 * scpi.cpp
 * Structure
 * + Forward declarations
 * + Table
 * + Functions
 * */

#include "scpi.hpp"
#include <array>
#include <cstdio>
#include <iostream>
#include <limits>
#include <scpi/scpi.h>
#include <span>

static scpi_result_t SCPI_Help(scpi_t *context);

// shell_put_line(g_serial_number.data());
// shell_put_line(compile_date);
// reset();

// ---- Helper Macros ----
#define SCPI_DEF_SETTER(name, setter)                                          \
  static void setter(uint32_t) {}                                              \
  static scpi_result_t SCPI_Set_##name(scpi_t *context) {                      \
    uint32_t value;                                                            \
    if (!SCPI_ParamUInt32(context, &value, true)) {                            \
      return SCPI_RES_ERR;                                                     \
    }                                                                          \
    setter(value);                                                             \
    return SCPI_RES_OK;                                                        \
  }

#define SCPI_DEF_GETTER(name, getter)                                          \
  static uint32_t getter() { return 0; }                                       \
  static scpi_result_t SCPI_Get_##name(scpi_t *context) {                      \
    SCPI_ResultUInt32(context, getter());                                      \
    return SCPI_RES_OK;                                                        \
  }

#define SCPI_DEF_BOOL_SETTER(name, setter)                                     \
  static void setter(bool) {}                                                  \
  static scpi_result_t SCPI_Set_##name(scpi_t *context) {                      \
    bool value;                                                                \
    if (!SCPI_ParamBool(context, &value, true)) {                              \
      return SCPI_RES_ERR;                                                     \
    }                                                                          \
    setter(value);                                                             \
    return SCPI_RES_OK;                                                        \
  }

#define SCPI_DEF_BOOL_GETTER(name, getter)                                     \
  static bool getter() { return false; }                                       \
  static scpi_result_t SCPI_Get_##name(scpi_t *context) {                      \
    SCPI_ResultBool(context, getter());                                        \
    return SCPI_RES_OK;                                                        \
  }

// ---- Concrete bindings ----
SCPI_DEF_SETTER(Vbtx, set_txdac)
SCPI_DEF_GETTER(Vbtx, get_txdac)

SCPI_DEF_SETTER(Vbrx, set_rxdac)
SCPI_DEF_GETTER(Vbrx, get_rxdac)

SCPI_DEF_SETTER(Slbias, set_slbias)
SCPI_DEF_GETTER(Slbias, get_slbias)

SCPI_DEF_SETTER(Sbbias, set_sbbias)
SCPI_DEF_GETTER(Sbbias, get_sbbias)

SCPI_DEF_SETTER(Da, set_da)
SCPI_DEF_GETTER(Da, get_da)

SCPI_DEF_SETTER(Db, set_db)
SCPI_DEF_GETTER(Db, get_db)

SCPI_DEF_BOOL_SETTER(vtdr_enable, set_vtdr_enable);
SCPI_DEF_BOOL_GETTER(vtdr_enable, get_vtdr_enable);

SCPI_DEF_BOOL_SETTER(v3p6r_enable, set_v3p6r_enable);
SCPI_DEF_BOOL_GETTER(v3p6r_enable, get_v3p6r_enable);

SCPI_DEF_SETTER(npoints, set_npoints);
SCPI_DEF_GETTER(npoints, get_npoints);

SCPI_DEF_SETTER(naverages, set_naverages);
SCPI_DEF_GETTER(naverages, get_naverages);

SCPI_DEF_SETTER(istart, set_istart);
SCPI_DEF_GETTER(istart, get_istart);

SCPI_DEF_SETTER(npulses, set_npulses);
SCPI_DEF_GETTER(npulses, get_npulses);

SCPI_DEF_SETTER(a, set_a);
SCPI_DEF_GETTER(a, get_a);

SCPI_DEF_SETTER(rc, set_rc);
SCPI_DEF_GETTER(rc, get_rc);

SCPI_DEF_SETTER(b, set_b);
SCPI_DEF_GETTER(b, get_b);

SCPI_DEF_SETTER(m, set_m);
SCPI_DEF_GETTER(m, get_m);

SCPI_DEF_SETTER(spacing, set_spacing);
SCPI_DEF_GETTER(spacing, get_spacing);

SCPI_DEF_BOOL_GETTER(is_calibrated, get_is_calibrated);
SCPI_DEF_BOOL_SETTER(echo, set_echo);

SCPI_DEF_GETTER(v3p6t, get_v3p6t);
SCPI_DEF_GETTER(imon, get_imon);
SCPI_DEF_GETTER(v5p0, get_v5p0);
SCPI_DEF_GETTER(v3p6r, get_v3p6r);
SCPI_DEF_GETTER(vtemp, get_vtemp);
SCPI_DEF_GETTER(vp, get_vp);
SCPI_DEF_GETTER(vsample, get_vsample);
SCPI_DEF_GETTER(visp, get_visp);

static void set_stop(const bool stop) {}
static void set_quiet(const bool stop) {}

static scpi_result_t SCPI_Set_stop(scpi_t *context) {
  bool stop_value;
  if (!SCPI_ParamBool(context, &stop_value, true)) {
    return SCPI_RES_ERR;
  }
  set_stop(stop_value);
  bool quiet_value;
  if (SCPI_ParamBool(context, &quiet_value, false)) {
    set_quiet(quiet_value);
  }

  return SCPI_RES_OK;
}

const char *compile_date_{__DATE__};

static scpi_result_t SCPI_Get_compile_date(scpi_t *context) {
  SCPI_ResultText(context, compile_date_);
  return SCPI_RES_OK;
}

static void set_dfu() {}
static scpi_result_t SCPI_Set_dfu(scpi_t *context) {
  const constexpr std::array<scpi_choice_def_t, 2> dfu_code_{
      scpi_choice_def_t{"BEEF", 0}, SCPI_CHOICE_LIST_END};

  int32_t index = std::numeric_limits<int32_t>::max();

  const bool choice_found =
      SCPI_ParamChoice(context, dfu_code_.data(), &index, false);

  if (!choice_found) {
    return SCPI_RES_ERR;
  }
  set_dfu();
  return SCPI_RES_OK;
}

static const constexpr std::array<scpi_choice_def_t, 7>
    programmable_calibration_slots_{scpi_choice_def_t{"A1", 1},
                                    {"A2", 2},
                                    {"B1", 4},
                                    {"B2", 5},
                                    {"C1", 7},
                                    {"C2", 8},
                                    SCPI_CHOICE_LIST_END};

static const constexpr std::array<scpi_choice_def_t, 10> calibration_slots_{
    scpi_choice_def_t{"A0", 0},
    {"A1", 1},
    {"A2", 2},
    {"B0", 3},
    {"B1", 4},
    {"B2", 5},
    {"C0", 6},
    {"C1", 7},
    {"C2", 8},
    SCPI_CHOICE_LIST_END};

static scpi_result_t SCPI_Set_store_calibration(scpi_t *context) {
  int32_t value = 0;

  if (!SCPI_ParamChoice(context, programmable_calibration_slots_.data(), &value,
                        true)) {
    return SCPI_RES_ERR;
  }
  return SCPI_RES_OK;
}

static scpi_result_t SCPI_Set_recall_calibration(scpi_t *context) {
  int32_t value = 0;

  if (!SCPI_ParamChoice(context, calibration_slots_.data(), &value, true)) {
    return SCPI_RES_ERR;
  }
  return SCPI_RES_OK;
}

static scpi_result_t SCPI_Set_timing_params(scpi_t *context) {
  std::array<float, 4> values{};

  for (auto &v : values) {
    if (!SCPI_ParamFloat(context, &v, true)) {
      return SCPI_RES_ERR;
    }
  }
  set_a(values[0]);
  set_rc(values[1]);
  set_b(values[2]);
  set_m(values[3]);
  printf("%f,%f,%f,%f\n", values[0], values[1], values[2], values[3]);
  return SCPI_RES_OK;
}

static scpi_result_t SCPI_Get_timing_params(scpi_t *context) {
  std::array<float, 4> values{get_a(), get_rc(), get_b(), get_m()};

  SCPI_ResultArrayFloat(context, values.data(), values.size(),
                        scpi_array_format_t::SCPI_FORMAT_ASCII);
  return SCPI_RES_OK;
}

struct TimingParameters {
  float a = 1;
  float rc = 1;
  float b = 1;
  float m = 1;
};

struct MeasurementParameters {
  uint32_t spacing_ps = 10;
  float rc = 100;
  uint32_t npoints = 1000;
  uint32_t i_start = 100;
  TimingParameters calibration{1, 2, 3, 4};
};

static scpi_result_t SCPI_Get_rxdac_points(scpi_t *context) {
  MeasurementParameters params{};
  const auto cal = params.calibration;
  const float rc_step = cal.rc / static_cast<float>(params.spacing_ps);
  for (std::size_t i = 0; i < params.npoints; i++) {
    const float dac = 1.2345f;
    // calc_dac_point<float>(i + params.i_start, cal.a, rc_step, cal.b, cal.m);
    SCPI_ResultFloat(context, dac);
  }
  return SCPI_RES_OK;
}

#define DEF_GET_COMMAND(arg, name) {arg, SCPI_Get_##name, 0}
#define DEF_SET_COMMAND(arg, name) {arg, SCPI_Set_##name, 0}

static const scpi_command_t scpi_commands_table_[] = {
    /* IEEE Mandated Commands (SCPI std V1999.0 4.1.1) */
    scpi_command_t{"*ESE", SCPI_CoreEse, 0},
    {"*ESE?", SCPI_CoreEseQ, 0},
    {"*ESR?", SCPI_CoreEsrQ, 0},
    {"*IDN?", SCPI_CoreIdnQ, 0},
    {"*OPC", SCPI_CoreOpc, 0},
    {"*OPC?", SCPI_CoreOpcQ, 0},
    {"*RST", SCPI_CoreRst, 0},
    {"*SRE", SCPI_CoreSre, 0},
    {"*SRE?", SCPI_CoreSreQ, 0},
    {"*STB?", SCPI_CoreStbQ, 0},
    {"*TST?", SCPI_CoreTstQ, 0},
    {"*WAI", SCPI_CoreWai, 0},
    DEF_SET_COMMAND("*SAV", store_calibration),
    DEF_SET_COMMAND("*RCL", recall_calibration),
    DEF_SET_COMMAND("Echo", echo),
    DEF_SET_COMMAND("STOP", stop),
    {"HELP", SCPI_Help, 0},

    DEF_GET_COMMAND("DATE?", compile_date),
    DEF_SET_COMMAND("DFU", dfu),
    DEF_GET_COMMAND("VTDR?", v3p6t),
    DEF_GET_COMMAND("IMON?", imon),
    DEF_GET_COMMAND("V5?", v5p0),
    DEF_GET_COMMAND("VREF?", v3p6r),
    DEF_GET_COMMAND("TEMP?", vtemp),
    DEF_GET_COMMAND("VIN?", vp),
    DEF_GET_COMMAND("VSAMP?", vsample),
    DEF_GET_COMMAND("VISP?", visp),

    DEF_GET_COMMAND("MEASure:ADC:V5?", v5p0),
    DEF_GET_COMMAND("MEASure:ADC:VREF?", v3p6r),
    DEF_GET_COMMAND("MEASure:ADC:TEMP?", vtemp),
    DEF_GET_COMMAND("MEASure:ADC:VIN?", vp),
    DEF_GET_COMMAND("MEASure:ADC:VSAMP?", vsample),
    DEF_GET_COMMAND("MEASure:ADC:VISP?", visp),

    DEF_GET_COMMAND("MEASure:VOLTs:V5?", v5p0),
    DEF_GET_COMMAND("MEASure:VOLTs:VREF?", v3p6r),
    DEF_GET_COMMAND("MEASure:VOLTs:TEMP?", vtemp),
    DEF_GET_COMMAND("MEASure:VOLTs:VIN?", vp),
    DEF_GET_COMMAND("MEASure:VOLTs:VSAMP?", vsample),
    DEF_GET_COMMAND("MEASure:VOLTs:VISP?", visp),

    DEF_GET_COMMAND("MEASure:TEMPerature[:TEMP]?", vtemp),
    DEF_GET_COMMAND("MEASure:CURRent:IMON?", imon),

    DEF_SET_COMMAND("VTX", Vbtx),
    DEF_GET_COMMAND("VTX?", Vbtx),
    DEF_SET_COMMAND("VRX", Vbrx),
    DEF_GET_COMMAND("VRX?", Vbrx),
    DEF_SET_COMMAND("SLbias", Slbias),
    DEF_GET_COMMAND("SLbias?", Slbias),
    DEF_SET_COMMAND("SBbias", Sbbias),
    DEF_GET_COMMAND("SBbias?", Sbbias),
    DEF_SET_COMMAND("DA", Da),
    DEF_GET_COMMAND("DA?", Da),
    DEF_SET_COMMAND("DB", Db),
    DEF_GET_COMMAND("DB?", Db),
    DEF_SET_COMMAND("POWer[:ENable]", vtdr_enable),
    DEF_GET_COMMAND("POWer[:ENable]?", vtdr_enable),
    DEF_SET_COMMAND("VREF:ENable", v3p6r_enable),
    DEF_GET_COMMAND("VREF:ENable?", v3p6r_enable),
    DEF_SET_COMMAND("AVG", naverages),
    DEF_GET_COMMAND("AVG?", naverages),
    DEF_SET_COMMAND("PULSES", npulses),
    DEF_GET_COMMAND("PULSES?", npulses),

    DEF_SET_COMMAND("ISTART", istart),
    DEF_GET_COMMAND("ISTART?", istart),
    DEF_SET_COMMAND("RESolution", spacing),
    DEF_GET_COMMAND("RESolution?", spacing),
    DEF_SET_COMMAND("POINTS", npoints),
    DEF_GET_COMMAND("POINTS?", npoints),
    DEF_GET_COMMAND("CALibrated?", is_calibrated),
    DEF_GET_COMMAND("TIMing:CALibrated?", is_calibrated),
    DEF_SET_COMMAND("TIMing:ISTART", istart),
    DEF_GET_COMMAND("TIMing:ISTART?", istart),
    DEF_SET_COMMAND("TIMing:RESolution", spacing),
    DEF_GET_COMMAND("TIMing:RESolution?", spacing),
    DEF_SET_COMMAND("TIMing:POINTS", npoints),
    DEF_GET_COMMAND("TIMing:POINTS?", npoints),
    DEF_SET_COMMAND("TIMing:Amplitude", a),
    DEF_GET_COMMAND("TIMing:Amplitude?", a),
    DEF_SET_COMMAND("TIMing", timing_params),
    DEF_GET_COMMAND("TIMing?", timing_params),
    DEF_SET_COMMAND("TIMing:RC", rc),
    DEF_GET_COMMAND("TIMing:RC?", rc),
    DEF_SET_COMMAND("TIMing:B", b),
    DEF_GET_COMMAND("TIMing:B?", b),
    DEF_SET_COMMAND("TIMing:M", m),
    DEF_GET_COMMAND("TIMing:M?", m),
    DEF_GET_COMMAND("TIMing:RXdac?", rxdac_points),
    DEF_GET_COMMAND("RXdac?", rxdac_points),
    {nullptr, nullptr, 0}};

const std::span<const scpi_command_t> scpi_commands_table{
    scpi_commands_table_,
    sizeof(scpi_commands_table_) / sizeof(scpi_commands_table_[0])};

static scpi_result_t SCPI_Help(scpi_t *context) {
  // const char intro[] = "Available Functions:";
  // context->interface->write(context, intro, strlen(intro));
  for (const auto &cmd : scpi_commands_table) {
    if (cmd.pattern) {
      SCPI_ResultText(context, cmd.pattern);
    }
  }

  context->interface->write(context, "\r\n", 2);
  return SCPI_RES_OK;
}






#include <zephyr/console/console.h>
#include <zephyr/app_version.h>


static std::array<uint8_t, 256> scpi_input_buffer{};
static std::array<scpi_error_t, 8> scpi_error_buffer{};
static size_t SCPI_Write(scpi_t *context, const char *data, size_t len) {
  /*
   * uart or tcp output
   */
  // std::array<char, 1024> buff{};
  // snprintf(buff.data(), buff.size(), "%s", data);
  console_write(NULL, data, len);
  // printk("%s", data);
  // printk("\r\n%s", data);
  return SCPI_RES_OK;
}

static int SCPI_Error(scpi_t *context, int_fast16_t err) {
  std::array<char, 32> buff{};
  snprintf(buff.data(), buff.size(), "SCPI Error: %d\r\n", err);
  console_write(NULL, buff.data(), buff.size());
  return SCPI_RES_OK;
}

static scpi_result_t SCPI_Control(scpi_t *context, scpi_ctrl_name_t ctrl,
                                  scpi_reg_val_t val) {

  std::array<char, 32> buff{};
  snprintf(buff.data(), buff.size(), "SCPI Control: %d\r\n", val);
  console_write(NULL, buff.data(), buff.size());
  return SCPI_RES_OK;
}

static scpi_result_t SCPI_Reset(scpi_t *context) { return SCPI_RES_OK; }
static scpi_result_t SCPI_Flush(scpi_t *context) { return SCPI_RES_OK; }

K_THREAD_STACK_DEFINE(scpi_stack, 512);
struct k_thread scpi_thread_data;

#define BOARD_IDENTITY CONFIG_BOARD CONFIG_BOARD_REVISION
static ScpiInterfaceContainer scpi_interface(
    {"Hobbs ElectroOptics", BOARD_IDENTITY, APP_VERSION_EXTENDED_STRING, ""},
    {scpi_commands_table.data(), scpi_commands_table.size()},
    {scpi_input_buffer.data(), scpi_input_buffer.size()},
    {scpi_error_buffer.data(), scpi_error_buffer.size()},
    {SCPI_Error, SCPI_Write, SCPI_Control, SCPI_Flush, SCPI_Reset});

static void scpi_input_thread(void *arg1, void *arg2, void *arg3) {
  ARG_UNUSED(arg1);
  ARG_UNUSED(arg2);
  ARG_UNUSED(arg3);

  while (true) {
    const auto value = console_getchar();
    if (value >= 0) {
	  const char ch = static_cast<char>(value);
	  scpi_interface.input(&ch, 1);
	} else {
      k_sleep(K_MSEC(1));
	}
  }
}

void start_scpi_thread() {
  k_thread_create(&scpi_thread_data, scpi_stack,
                  K_THREAD_STACK_SIZEOF(scpi_stack), scpi_input_thread, NULL,
                  NULL, NULL, K_LOWEST_APPLICATION_THREAD_PRIO, 0, K_NO_WAIT);
}


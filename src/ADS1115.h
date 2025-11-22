/**
 * SPDX-FileCopyrightText: 2025 Maximiliano Ramirez <maximiliano.ramirezbravo@gmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <Arduino.h>
#include <Wire.h>

namespace ADS1115 {

/// @brief I2C address combinations according to ADDR pin
enum class I2CAddress : uint8_t { Gnd = 0x48, Vdd, Sda, Scl };

/// @brief Input multiplexer configuration
enum class Mux : uint8_t {
  P0_N1,
  P0_N3,
  P1_N3,
  P2_N3,
  P0_GND,
  P1_GND,
  P2_GND,
  P3_GND,
};

/// @brief Programmable gain amplifier configuration
enum class Pga : uint8_t {
  FSR_6_144V,
  FSR_4_096V,
  FSR_2_048V,
  FSR_1_024V,
  FSR_0_512V,
  FSR_0_256V,
};

/// @brief ADC conversion mode
enum class AdcMode : uint8_t { Continuous, SingleShot };

/// @brief Data rate configuration
enum class DataRate : uint8_t {
  SPS_8,
  SPS_16,
  SPS_32,
  SPS_64,
  SPS_128,
  SPS_250,
  SPS_475,
  SPS_860,
};

/// @brief Comparator mode configuration
enum class CompMode : uint8_t { Traditional, Window };

/// @brief Comparator polarity configuration
enum class CompPolarity : uint8_t { ActiveLow, ActiveHigh };

/// @brief Comparator latching configuration
enum class CompLatching : uint8_t { NonLatching, Latching };

/// @brief Comparator queue configuration
enum class CompQueue : uint8_t { AssertAfter1, AssertAfter2, AssertAfter4, Disable };

/// @brief Status codes
enum class Status : uint8_t {
  Ok,
  DataTooLong,
  ReceivedNackOnTxAddress,
  ReceivedNackOnTxData,
  OtherError,
  Timeout,
  FailedToRequestBytes,
  ChannelOutOfRange
};

class ADS1115_ADC {
  public:
  ADS1115_ADC(const I2CAddress address, TwoWire& i2c = Wire);

  /**
   * @brief Test if the device is connected to the I2C bus.
   * @return true Connected
   * @return false Not connected, check wiring or power
   */
  bool isConnected();

  /**
   * @brief Initialize ADS1115. This method must be called before using any other method. This will
   * set the MUX to P0_GND, PGA to FSR_2_048V, ADC mode to SingleShot, data rate to SPS_128,
   * comparator mode to Traditional, comparator polarity to ActiveLow, comparator latching to
   * NonLatching and comparator queue to Disable. Calling this method again will reset the device
   * to the default configuration.
   * @return Status Status code
   */
  Status init();

  /**
   * @brief Set the input multiplexer configuration. To take effect, call uploadConfig() after this.
   * @param mux Input multiplexer configuration
   */
  void setMux(const Mux mux);

  /**
   * @brief Get the current input multiplexer configuration from the device.
   * @param mux Input multiplexer configuration
   * @return Status Status code
   */
  Status getMux(Mux& mux);

  /**
   * @brief Set the programmable gain amplifier configuration. To take effect, call uploadConfig()
   * after this.
   * @param pga Programmable gain amplifier configuration
   */
  void setPga(const Pga pga);

  /**
   * @brief Get the current programmable gain amplifier configuration from the device.
   * @param pga Programmable gain amplifier configuration
   * @return Status Status code
   */
  Status getPga(Pga& pga);

  /**
   * @brief Set the ADC conversion mode. To take effect, call uploadConfig() after this.
   * @param mode ADC conversion mode
   */
  void setAdcMode(const AdcMode mode);

  /**
   * @brief Get the current ADC conversion mode from the device.
   * @param mode ADC conversion mode
   * @return Status Status code
   */
  Status getAdcMode(AdcMode& mode);

  /**
   * @brief Set the data rate configuration. To take effect, call uploadConfig() after this.
   * @param rate Data rate configuration
   */
  void setDataRate(const DataRate rate);

  /**
   * @brief Get the current data rate configuration from the device.
   * @param rate Data rate configuration
   * @return Status Status code
   */
  Status getDataRate(DataRate& rate);

  /**
   * @brief Set the comparator mode configuration. To take effect, call uploadConfig() after this.
   * @param mode Comparator mode configuration
   */
  void setComparatorMode(const CompMode mode);

  /**
   * @brief Get the current comparator mode configuration from the device.
   * @param mode Comparator mode configuration
   * @return Status Status code
   */
  Status getComparatorMode(CompMode& mode);

  /**
   * @brief Set the comparator polarity configuration. To take effect, call uploadConfig() after
   * this.
   * @param polarity Comparator polarity configuration
   */
  void setComparatorPolarity(const CompPolarity polarity);

  /**
   * @brief Get the current comparator polarity configuration from the device.
   * @param polarity Comparator polarity configuration
   * @return Status Status code
   */
  Status getComparatorPolarity(CompPolarity& polarity);

  /**
   * @brief Set the comparator latching configuration. To take effect, call uploadConfig() after
   * this.
   * @param latching Comparator latching configuration
   */
  void setComparatorLatching(const CompLatching latching);

  /**
   * @brief Get the current comparator latching configuration from the device.
   * @param latching Comparator latching configuration
   * @return Status Status code
   */
  Status getComparatorLatching(CompLatching& latching);

  /**
   * @brief Set the comparator queue configuration. To take effect, call uploadConfig() after this.
   * @param queue Comparator queue configuration
   */
  void setComparatorQueue(const CompQueue queue);

  /**
   * @brief Get the current comparator queue configuration from the device.
   * @param queue Comparator queue configuration
   * @return Status Status code
   */
  Status getComparatorQueue(CompQueue& queue);

  /**
   * @brief Upload the configuration to the ADS1115. You must call this method after setting one or
   * more configurations.
   * @return Status Status code
   */
  Status uploadConfig();

  /**
   * @brief Set the low threshold for the comparator. You don't need to call uploadConfig() after
   * this.
   * @param threshold Low threshold value
   * @return Status Status code
   */
  Status setLowThreshold(const int16_t threshold);

  /**
   * @brief Get the low threshold for the comparator from the device.
   * @param threshold Low threshold value
   * @return Status Status code
   */
  Status getLowThreshold(int16_t& threshold);

  /**
   * @brief Set the high threshold for the comparator. You don't need to call uploadConfig() after
   * this.
   * @param threshold High threshold value
   * @return Status Status code
   */
  Status setHighThreshold(const int16_t threshold);

  /**
   * @brief Get the high threshold for the comparator from the device.
   * @param threshold High threshold value
   * @return Status Status code
   */
  Status getHighThreshold(int16_t& threshold);

  /**
   * @brief Enable the conversion ready pin (ALERT/RDY). This will enable the pin to go low when a
   * new conversion is ready. The pin will go high when a conversion is in progress. This is useful
   * to trigger an interrupt when a conversion is ready.
   * @note To enable the ALERT/RDY pin, the comparator queue must be different from Disable.
   * @return Status Status code
   */
  Status enableConversionReadyPin();

  /**
   * @brief Start a single shot conversion with the current MUX configuration. This will start a new
   * conversion and the data can be read using readConversion() method once a conversion is ready.
   * @return Status Status code
   */
  Status startSingleShotConversion();

  /**
   * @brief Start a single shot conversion on channel 0-3 with respect to GND. This will start a new
   * conversion and the data can be read using readConversion() method once a conversion is ready.
   * @param channel Channel number (0-3)
   * @return Status Status code
   */
  Status startSingleShotConversion(uint8_t channel);

  /**
   * @brief Start a continuous conversion with the current MUX configuration. This will start a new
   * conversion and the data can be read using readConversion() method once a conversion is ready.
   * @return Status Status code
   */
  Status startContinuousConversion();

  /**
   * @brief Start a continuous conversion on channel 0-3 with respect to GND. This will start a new
   * conversion and the data can be read using readConversion() method once a conversion is ready.
   * @param channel Channel number (0-3)
   * @return Status Status code
   */
  Status startContinuousConversion(uint8_t channel);

  /**
   * @brief Check if a conversion is ready to be read.
   * @param ready Conversion ready
   * @return Status Status code
   */
  Status isConversionReady(bool& ready);

  /**
   * @brief Read the conversion value. This method must be called after a conversion is ready.
   * @param value Conversion value
   * @return Status Status code
   */
  Status readConversion(int16_t& value);

  /**
   * @brief Read the conversion value and convert it to voltage, based on the current PGA
   * configuration. This method must be called after a conversion is ready.
   * @param voltage Conversion voltage
   * @return Status Status code
   */
  Status readConversionVoltage(float& voltage);

  /**
   * @brief Convert a raw value to voltage based on the current PGA configuration.
   * @param value Raw value
   * @return float Voltage
   */
  float convertToVoltage(const int16_t value);

  private:
  enum class Register : uint8_t {
    Conversion,
    Config,
    LoThresh,
    HiThresh,
  };

  enum class ConfigShift : uint8_t {
    Os       = 15,
    Mux      = 12,
    Pga      = 9,
    Mode     = 8,
    Dr       = 5,
    CompMode = 4,
    CompPol  = 3,
    CompLat  = 2,
    CompQue  = 0,
  };

  enum class ConfigMask : uint16_t {
    Os       = 0x8000,
    Mux      = 0x7000,
    Pga      = 0x0E00,
    Mode     = 0x0100,
    Dr       = 0x00E0,
    CompMode = 0x0010,
    CompPol  = 0x0008,
    CompLat  = 0x0004,
    CompQue  = 0x0003
  };

  void updateConfig(const ConfigShift shift, const ConfigMask mask, const uint16_t value);
  Status writeRegister(const Register reg, const uint16_t value);
  Status readRegister(const Register reg, uint16_t& value);
  uint8_t readBits(const uint16_t value, const ConfigMask mask, const ConfigShift shift);

  I2CAddress _address;
  TwoWire& _i2c;
  uint16_t _config;
  Pga _pga;

  static constexpr float _fsr_0_256 = 0.256 / 32767.0;
  static constexpr float _fsr_0_512 = 0.512 / 32767.0;
  static constexpr float _fsr_1_024 = 1.024 / 32767.0;
  static constexpr float _fsr_2_048 = 2.048 / 32767.0;
  static constexpr float _fsr_4_096 = 4.096 / 32767.0;
  static constexpr float _fsr_6_144 = 6.144 / 32767.0;
};

} // namespace ADS1115
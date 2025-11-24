/**
 * SPDX-FileCopyrightText: 2025 Maximiliano Ramirez <maximiliano.ramirezbravo@gmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include "ADS1115.h"

namespace ADS1115 {
ADS1115_ADC::ADS1115_ADC(const I2CAddress address, TwoWire& i2c)
    : _address(address)
    , _i2c(i2c)
    , _config(0)
    , _pga(Pga::FSR_2_048V) {}

bool ADS1115_ADC::isConnected() {
  _i2c.beginTransmission(static_cast<uint8_t>(_address));
  return _i2c.endTransmission() == 0;
}

Status ADS1115_ADC::init() {
  setMux(Mux::P0_GND);
  setPga(Pga::FSR_2_048V);
  setAdcMode(AdcMode::SingleShot);
  setDataRate(DataRate::SPS_128);
  setComparatorMode(CompMode::Traditional);
  setComparatorPolarity(CompPolarity::ActiveLow);
  setComparatorLatching(CompLatching::NonLatching);
  setComparatorQueue(CompQueue::Disable);

  Status status = uploadConfig();
  if (status != Status::Ok) return status;

  status = setLowThreshold(0x8000);
  if (status != Status::Ok) return status;

  status = setHighThreshold(0x7FFF);
  if (status != Status::Ok) return status;

  _pga = Pga::FSR_2_048V;
  return Status::Ok;
}

void ADS1115_ADC::setMux(const Mux mux) {
  updateConfig(ConfigShift::Mux, ConfigMask::Mux, static_cast<uint16_t>(mux));
}

Status ADS1115_ADC::getMux(Mux& mux) {
  uint16_t current_config;
  Status status = readRegister(Register::Config, current_config);
  if (status != Status::Ok) return status;

  mux = static_cast<Mux>(readBits(current_config, ConfigMask::Mux, ConfigShift::Mux));
  return Status::Ok;
}

void ADS1115_ADC::setPga(const Pga pga) {
  updateConfig(ConfigShift::Pga, ConfigMask::Pga, static_cast<uint16_t>(pga));
}

Status ADS1115_ADC::getPga(Pga& pga) {
  uint16_t current_config;
  Status status = readRegister(Register::Config, current_config);
  if (status != Status::Ok) return status;

  pga  = static_cast<Pga>(readBits(current_config, ConfigMask::Pga, ConfigShift::Pga));
  _pga = pga;

  return Status::Ok;
}

void ADS1115_ADC::setAdcMode(const AdcMode mode) {
  updateConfig(ConfigShift::Mode, ConfigMask::Mode, static_cast<uint16_t>(mode));
}

Status ADS1115_ADC::getAdcMode(AdcMode& mode) {
  uint16_t current_config;
  Status status = readRegister(Register::Config, current_config);
  if (status != Status::Ok) return status;

  mode = static_cast<AdcMode>(readBits(current_config, ConfigMask::Mode, ConfigShift::Mode));
  return Status::Ok;
}

void ADS1115_ADC::setDataRate(const DataRate rate) {
  updateConfig(ConfigShift::Dr, ConfigMask::Dr, static_cast<uint16_t>(rate));
}

Status ADS1115_ADC::getDataRate(DataRate& rate) {
  uint16_t current_config;
  Status status = readRegister(Register::Config, current_config);
  if (status != Status::Ok) return status;

  rate = static_cast<DataRate>(readBits(current_config, ConfigMask::Dr, ConfigShift::Dr));
  return Status::Ok;
}

void ADS1115_ADC::setComparatorMode(const CompMode mode) {
  updateConfig(ConfigShift::CompMode, ConfigMask::CompMode, static_cast<uint16_t>(mode));
}

Status ADS1115_ADC::getComparatorMode(CompMode& mode) {
  uint16_t current_config;
  Status status = readRegister(Register::Config, current_config);
  if (status != Status::Ok) return status;

  mode =
    static_cast<CompMode>(readBits(current_config, ConfigMask::CompMode, ConfigShift::CompMode));
  return Status::Ok;
}

void ADS1115_ADC::setComparatorPolarity(const CompPolarity polarity) {
  updateConfig(ConfigShift::CompPol, ConfigMask::CompPol, static_cast<uint16_t>(polarity));
}

Status ADS1115_ADC::getComparatorPolarity(CompPolarity& polarity) {
  uint16_t current_config;
  Status status = readRegister(Register::Config, current_config);
  if (status != Status::Ok) return status;

  polarity =
    static_cast<CompPolarity>(readBits(current_config, ConfigMask::CompPol, ConfigShift::CompPol));
  return Status::Ok;
}

void ADS1115_ADC::setComparatorLatching(const CompLatching latching) {
  updateConfig(ConfigShift::CompLat, ConfigMask::CompLat, static_cast<uint16_t>(latching));
}

Status ADS1115_ADC::getComparatorLatching(CompLatching& latching) {
  uint16_t current_config;
  Status status = readRegister(Register::Config, current_config);
  if (status != Status::Ok) return status;

  latching =
    static_cast<CompLatching>(readBits(current_config, ConfigMask::CompLat, ConfigShift::CompLat));
  return Status::Ok;
}

void ADS1115_ADC::setComparatorQueue(const CompQueue queue) {
  updateConfig(ConfigShift::CompQue, ConfigMask::CompQue, static_cast<uint16_t>(queue));
}

Status ADS1115_ADC::getComparatorQueue(CompQueue& queue) {
  uint16_t current_config;
  Status status = readRegister(Register::Config, current_config);
  if (status != Status::Ok) return status;

  queue =
    static_cast<CompQueue>(readBits(current_config, ConfigMask::CompQue, ConfigShift::CompQue));
  return Status::Ok;
}

Status ADS1115_ADC::uploadConfig() {
  Status status = writeRegister(Register::Config, _config);
  if (status != Status::Ok) return status;

  _pga = static_cast<Pga>(readBits(_config, ConfigMask::Pga, ConfigShift::Pga));
  return Status::Ok;
}

Status ADS1115_ADC::setLowThreshold(const int16_t threshold) {
  return writeRegister(Register::LoThresh, static_cast<uint16_t>(threshold));
}

Status ADS1115_ADC::getLowThreshold(int16_t& threshold) {
  uint16_t temp_value;
  Status status = readRegister(Register::LoThresh, temp_value);
  if (status != Status::Ok) return status;

  threshold = static_cast<int16_t>(temp_value);
  return Status::Ok;
}

Status ADS1115_ADC::setHighThreshold(const int16_t threshold) {
  return writeRegister(Register::HiThresh, static_cast<uint16_t>(threshold));
}

Status ADS1115_ADC::getHighThreshold(int16_t& threshold) {
  uint16_t temp_value;
  Status status = readRegister(Register::HiThresh, temp_value);
  if (status != Status::Ok) return status;

  threshold = static_cast<int16_t>(temp_value);
  return Status::Ok;
}

Status ADS1115_ADC::enableConversionReadyPin() {
  Status status = setLowThreshold(0);
  if (status != Status::Ok) return status;

  return setHighThreshold(0x8000);
}

Status ADS1115_ADC::startSingleShotConversion() {
  updateConfig(ConfigShift::Os, ConfigMask::Os, 1);
  return uploadConfig();
}

Status ADS1115_ADC::startSingleShotConversion(uint8_t channel) {
  if (channel > 3) return Status::ChannelOutOfRange;

  channel = static_cast<uint8_t>(Mux::P0_GND) + channel;

  setMux(static_cast<Mux>(channel));
  return startSingleShotConversion();
}

Status ADS1115_ADC::startContinuousConversion() {
  setAdcMode(AdcMode::Continuous);
  return uploadConfig();
}

Status ADS1115_ADC::startContinuousConversion(uint8_t channel) {
  if (channel > 3) return Status::ChannelOutOfRange;

  channel = static_cast<uint8_t>(Mux::P0_GND) + channel;

  setMux(static_cast<Mux>(channel));
  return startContinuousConversion();
}

Status ADS1115_ADC::isConversionReady(bool& ready) {
  uint16_t config;
  Status status = readRegister(Register::Config, config);
  if (status != Status::Ok) return status;

  ready = (config & static_cast<uint16_t>(ConfigMask::Os)) == 0;
  return Status::Ok;
}

Status ADS1115_ADC::readConversion(int16_t& value) {
  uint16_t temp_value;
  Status status = readRegister(Register::Conversion, temp_value);
  if (status != Status::Ok) return status;

  value = static_cast<int16_t>(temp_value);
  return Status::Ok;
}

Status ADS1115_ADC::readConversionVoltage(float& voltage) {
  int16_t value;
  Status status = readConversion(value);
  if (status != Status::Ok) return status;

  voltage = convertToVoltage(value);
  return Status::Ok;
}

float ADS1115_ADC::convertToVoltage(const int16_t value) {
  float voltage = 0.0f;

  switch (_pga) {
    case Pga::FSR_0_256V: voltage = value * _fsr_0_256; break;
    case Pga::FSR_0_512V: voltage = value * _fsr_0_512; break;
    case Pga::FSR_1_024V: voltage = value * _fsr_1_024; break;
    case Pga::FSR_2_048V: voltage = value * _fsr_2_048; break;
    case Pga::FSR_4_096V: voltage = value * _fsr_4_096; break;
    case Pga::FSR_6_144V: voltage = value * _fsr_6_144; break;
  }

  return voltage;
}

void ADS1115_ADC::updateConfig(const ConfigShift shift, const ConfigMask mask,
                               const uint16_t value) {
  _config &= ~static_cast<uint16_t>(mask);
  _config |= (value << static_cast<uint8_t>(shift));
}

Status ADS1115_ADC::writeRegister(const Register reg, const uint16_t value) {
  _i2c.beginTransmission(static_cast<uint8_t>(_address));
  _i2c.write(static_cast<uint8_t>(reg));
  _i2c.write(static_cast<uint8_t>(value >> 8));
  _i2c.write(static_cast<uint8_t>(value & 0xFF));
  return static_cast<Status>(_i2c.endTransmission());
}

Status ADS1115_ADC::readRegister(const Register reg, uint16_t& value) {
  _i2c.beginTransmission(static_cast<uint8_t>(_address));
  _i2c.write(static_cast<uint8_t>(reg));

  Status status = static_cast<Status>(_i2c.endTransmission());
  if (status != Status::Ok) return status;

  const uint8_t bytes = 2;

  if (_i2c.requestFrom(static_cast<uint8_t>(_address), bytes) != bytes)
    return Status::FailedToRequestBytes;

  value = _i2c.read() << 8;
  value |= _i2c.read();
  return Status::Ok;
}

uint8_t ADS1115_ADC::readBits(const uint16_t value, const ConfigMask mask,
                              const ConfigShift shift) {
  return (value & static_cast<uint16_t>(mask)) >> static_cast<uint8_t>(shift);
}
} // namespace ADS1115

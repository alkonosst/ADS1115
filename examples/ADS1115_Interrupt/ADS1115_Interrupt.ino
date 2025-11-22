/**
 * SPDX-FileCopyrightText: 2025 Maximiliano Ramirez <maximiliano.ramirezbravo@gmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include "ADS1115.h"

using namespace ADS1115;
ADS1115_ADC adc(I2CAddress::Gnd);

const uint8_t ADC_INT   = 4;
volatile bool adc_ready = false;

void IRAM_ATTR alertCallback();

void setup() {
  Serial.begin(115200);
  Serial.println("Starting...");

  pinMode(ADC_INT, INPUT);

  // Initialize I2C
  Wire.begin();

  // Check if the ADC is connected
  if (!adc.isConnected()) {
    Serial.println("ADS1115 not connected");
    while (true)
      ;
  }

  // Initialize the ADC
  Status status = adc.init();

  if (status != Status::Ok) {
    Serial.printf("ADS1115 initialization failed, error: %u", status);
    while (true)
      ;
  }

  // Set the data rate to maximum
  // Set the comparator queue to assert after 4 conversions (enable the ALERT/RDY pin)
  adc.setDataRate(DataRate::SPS_860);
  adc.setComparatorQueue(CompQueue::AssertAfter4);
  status = adc.uploadConfig();

  if (status != Status::Ok) {
    Serial.printf("ADS1115 uploadConfig failed, error: %u", status);
    while (true)
      ;
  }

  // Enable the ALERT/RDY pin to trigger an interrupt when a conversion is ready
  status = adc.enableConversionReadyPin();

  if (status != Status::Ok) {
    Serial.printf("ADS1115 enableConversionReadyPin failed, error: %u", status);
    while (true)
      ;
  }

  // Attach the interrupt to the ALERT/RDY pin
  attachInterrupt(ADC_INT, alertCallback, FALLING);
}

void loop() {
  static bool converting    = false;
  static uint8_t channel    = 0;
  static uint32_t last_time = 0;

  // Start a conversion every 50ms
  if (millis() - last_time < 50) return;

  if (!converting) {
    Status status = adc.startSingleShotConversion(channel);

    if (status != Status::Ok) {
      Serial.printf("ADS1115 startSingleShotConversion failed, error: %u", status);
      last_time = millis();
      return;
    }

    converting = true;
  }

  // Read the conversion value once it's ready (flag changed in the interrupt)
  if (adc_ready) {
    adc_ready = false;

    const uint8_t channels = 4;
    static int16_t value[channels];
    static float voltage[channels];

    Status status = adc.readConversion(value[channel]);

    if (status != Status::Ok) {
      Serial.printf("ADS1115 readConversion failed, error: %u", status);
      converting = false;
      last_time  = millis();
      return;
    }

    voltage[channel] = adc.convertToVoltage(value[channel]);

    Serial.printf("Channel %u: %5u - %.2fV\n", channel, value[channel], voltage[channel]);

    channel    = (channel + 1) % channels;
    converting = false;
    last_time  = millis();
  }
}

void IRAM_ATTR alertCallback() { adc_ready = true; }
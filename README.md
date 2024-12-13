<h1 align="center">
  <a><img src=".img/logo.png" alt="Logo" width="300"></a>
  <br>
  ADS1115
</h1>

<p align="center">
  <b>Read the popular 4 x 16-bit ADC via I²C!</b>
</p>

---

# Table of contents <!-- omit in toc -->

- [Description](#description)
- [Usage](#usage)
  - [Adding library to Arduino IDE](#adding-library-to-arduino-ide)
  - [Adding library to platformio.ini (PlatformIO)](#adding-library-to-platformioini-platformio)
  - [Using the library](#using-the-library)
    - [Including the library](#including-the-library)
    - [What is inside the library](#what-is-inside-the-library)
    - [Creating an instance of the ADC and initializing it](#creating-an-instance-of-the-adc-and-initializing-it)
    - [Changing the configuration](#changing-the-configuration)
    - [Using the ALERT/RDY pin to detect conversions](#using-the-alertrdy-pin-to-detect-conversions)
    - [Starting a conversion](#starting-a-conversion)
    - [Reading the conversion result](#reading-the-conversion-result)
- [TODOs](#todos)

# Description

The ADS1115 is a **high-precision**, **low-power**, **16-bit** analog-to-digital converter (ADC)
with **four channels**, compatible with **I²C** communication. It features a low-drift voltage
reference, an internal oscillator, a programmable gain amplifier, and a digital comparator. These
characteristics, combined with its wide operating supply range, make the ADS1115 ideal for sensor
measurement applications where power and space are limited.

**Features:**

- Designed for Arduino environment.
- I²C functions return a status code to check if the operation was successful.
- Simple to use APIs. If you use a auto-complete IDE, you will see all the available functions and
  their documentation.
- All the features of the ADS1115 are implemented.

# Usage

## Adding library to Arduino IDE

Search for the library in the Library Manager and install it.

## Adding library to platformio.ini (PlatformIO)

```ini
; Most recent changes
lib_deps =
  https://github.com/alkonosst/ADS1115.git

; Release vx.y.z (using an exact version is recommended)
; Example: v1.2.3
lib_deps =
  https://github.com/alkonosst/ADS1115.git#vx.y.z
```

## Using the library

### Including the library

```cpp
#include "ADS1115.h"
```

### What is inside the library

All the functionalities of the library are inside the `ADS1115` namespace. So you can access the classes and functions like this:

```cpp
ADS1115::ADS1115_ADC adc(ADS1115::I2CAddress::Gnd);
```

If you prefer, you can rename the namespace to something shorter:

```cpp
namespace ads = ADS1115;
ads::ADS1115_ADC adc(ads::I2CAddress::Gnd);
```

Or maybe you want to use the `using` directive:

```cpp
using namespace ADS1115;
ADS1115_ADC adc(I2CAddress::Gnd);
```

The library uses the `ADS1115::Status` enum to return the status of the functions. The possible
values. Almost every function returns a status code, so you can check if the operation was
successful.

```cpp
using namespace ADS1115;
ADS1115_ADC adc(I2CAddress::Gnd);

void setup() {
  Status status = adc.init();

  if (status != Status::Ok) {
    // Handle the error
  }
}
```

### Creating an instance of the ADC and initializing it

The constructor needs the `I²C address` (_enum based on the ADDR pin_) and the `Wire` object (default is **Wire**).

```cpp
using namespace ADS1115;

// Using default Wire object
ADS1115_ADC adc(I2CAddress::Gnd);

// Using a custom Wire object
TwoWire myWire(1);
ADS1115_ADC adc1(I2CAddress::Gnd, myWire);

void setup() {
  Serial.begin(115200);

  // Initialize I2C communication
  Wire.begin();

  // Check if the ADC is connected
  if (!adc.isConnected()) {
    // Handle the error
  }

  Status status = adc.init();

  // Initialize the ADC
  if (status != Status::Ok) {
    // Print the error
    Serial.printf("ADC init error: %u", status);

    // Handle the error
  }
}
```

### Changing the configuration

You can change the `ADS1115` configuration register using the corresponding `set` methods. The configuration register is a 16-bit value that contains the following fields:

- `OS`: Operational status or single-shot conversion start
- `MUX`: Input multiplexer configuration
- `PGA`: Programmable gain amplifier configuration
- `MODE`: Device operating mode
- `DR`: Data rate
- `COMP_MODE`: Comparator mode
- `COMP_POL`: Comparator polarity
- `COMP_LAT`: Latching comparator
- `COMP_QUE`: Comparator queue and disable

After changing the configuration, you need to upload to the device using the `uploadConfig` method:

```cpp
using namespace ADS1115;
ADS1115_ADC adc(I2CAddress::Gnd);

void setup() {
  // Initialization code
  // ...

  // Change the configuration as needed
  adc.setMux(Mux::P2_GND);
  adc.setPga(Pga::FSR_4_096V);
  adc.setDataRate(DataRate::SPS_860);

  // Upload the configuration to the device
  Status status = adc.uploadConfig();

  if (status != Status::Ok) {
    // Handle the error
  }
}
```

All the `set` methods require to call the `uploadConfig` method to apply the changes to the
configuration register, except when changing the **comparator thresholds**, because they are stored
in their own registers:

```cpp
using namespace ADS1115;
ADS1115_ADC adc(I2CAddress::Gnd);

void setup() {
  // Initialization code
  // ...

  // Change lower threshold
  Status status = adc.setLowThreshold(0);

  if (status != Status::Ok) {
    // Handle the error
  }

  // Change upper threshold
  status = adc.setHighThreshold(1000);

  if (status != Status::Ok) {
    // Handle the error
  }
}
```

### Using the ALERT/RDY pin to detect conversions

The ADS1115 has an ALERT/RDY pin that can be used to detect when a conversion is ready. You can
enable this pin to trigger an interrupt on the microcontroller. The comparator queue must be set to
any value other than `Disable` to use the ALERT/RDY pin:

```cpp
using namespace ADS1115;
ADS1115_ADC adc(I2CAddress::Gnd);

const uint8_t ALERT_PIN = 0;
volatile bool conversion_ready = false;

void IRAM_ATTR conversionReady() {
  // Conversion ready
  conversion_ready = true;
}

void setup() {
  // Initialization code
  // ...

  // Change the comparator queue
  adc.setComparatorQueue(CompQueue::AssertAfter4);
  Status status = adc.uploadConfig();

  if (status != Status::Ok) {
    // Handle the error
  }

  // Enable the ALERT/RDY pin
  status = adc.enableConversionReadyPin();

  if (status != Status::Ok) {
    // Handle the error
  }

  // Attach the interrupt
  attachInterrupt(ALERT_PIN, conversionReady, FALLING);
}

void loop() {
  if (conversion_ready) {
    // Read the conversion result
    // ...

    // Clear the flag
    conversion_ready = false;
  }
}
```

### Starting a conversion

The ADS1115 has two conversion modes: **single-shot** and **continuous**. In single-shot mode, the
ADC performs a single conversion and then enters a low-power state. In continuous mode, the ADC
performs conversions continuously at the programmed data rate. You can start a conversion using the following methods:

- `startSingleShotConversion`: Starts a single-shot conversion

```cpp
using namespace ADS1115;
ADS1115_ADC adc(I2CAddress::Gnd);

void setup() {
  // Initialization code
  // ...

  // Start a single-shot conversion using the current Mux configuration
  Status status = adc.startSingleShotConversion();

  // Alternatively, start a single-shot conversion,
  // using one of the four channels with respect to GND
  // status = adc.startSingleShotConversion(0); // 0, 1, 2, 3


  if (status != Status::Ok) {
    // Handle the error
  }

  // Wait for the conversion to finish and read the result
  // ...
}
```

- `startContinuousConversion`: Starts continuous conversions

```cpp
using namespace ADS1115;
ADS1115_ADC adc(I2CAddress::Gnd);

void setup() {
  // Initialization code
  // ...

  // Start continuous conversions using the current Mux configuration
  Status status = adc.startContinuousConversion();

  // Alternatively, start continuous conversions,
  // using one of the four channels with respect to GND
  // status = adc.startContinuousConversion(0); // 0, 1, 2, 3

  if (status != Status::Ok) {
    // Handle the error
  }

  // Read the conversion result
  // ...
}
```

### Reading the conversion result

You can read the conversion result using the `readConversion` method. The result is a 16-bit signed
integer. Or you can use the `readConversionVoltage` method to read the conversion result and convert
it to voltage using the current PGA configuration:

```cpp
using namespace ADS1115;
ADS1115_ADC adc(I2CAddress::Gnd);

void setup() {
  // Initialization code
  // ...

  // Start a single-shot conversion or continuous conversions
  // ...

  // Read the conversion result
  int16_t result;
  Status status = adc.readConversion(result);

  // Alternatively, read the conversion result and convert it to voltage
  // float voltage;
  // status = adc.readConversionVoltage(voltage);

  if (status != Status::Ok) {
    // Handle the error
  }
}
```

# TODOs

- [ ] Add more examples

# Zephyr Test Project for XIAO nRF54L15

This project demonstrates various functionalities of the XIAO nRF54L15 development board using Zephyr RTOS, including system management, sensor interfaces, and communication interfaces through shell commands.

## Build and Run

1. Configure your Zephyr RTOS development environment.
2. Navigate to the project directory.
3. Build the project:
```shell
west build -p -b xiao_nrf54l15_cpuapp
```
4. Flash the board:
```shell
west flash
```

## Available Commands

### Microphone Commands
- `mic capture [<duration>]` - Capture audio data from microphone.
  ```shell
  mic capture
  ```
  Returns:
  - Audio data stream starting with "S" flag and ending with "E" flag

### Button Commands
- `button check` - Check user button status changes.
  ```shell
  button check
  ```
  Output:
  - "usr button pressed" when pressed
  - "usr button released" when released
  - "No input received" after 5 seconds timeout
  - Error message if device error occurs

### ADC Commands
- `adc_read get` - Read ADC channel value.
  ```shell
  adc_read get
  ```
  Returns:
  - Success: Shows raw ADC reading and converted voltage value
  - Error: Returns -EINVAL on initialization failure or appropriate error code on read failure

### NFC Commands
- `nfc play` - Start NFC display/emulation.
  ```shell
  nfc play
  ```
  
### System sleep Commands
- `sys off` - Enter system sleep mode.
  ```shell
  sys off
  ```
  Notes:
  - Disables RF switch and configures user button as wake-up source
  - Closes serial console
  - System can be woken up by pressing user button
  - Shows specific error reason in case of failure

### I2C Commands
- `i2c scan <device>` - Scan for devices on I2C bus.
  ```shell
  i2c scan i2c@104000
  ```
- `i2c read <device> <address> <register> [<length>]` - Read data from I2C device.
  ```shell
  i2c read i2c@104000 0x6A 0x0F
  ```
- `i2c write <device> <address> <register> [<data>]` - Write data to I2C device.
  ```shell
  i2c write i2c@104000 0x6A 0x10 0x00
  ```

### SPI Commands
- `spi conf <device> <frequency> [<settings>]` - Configure SPI device parameters.
  ```shell
  spi conf spi1 1000000 ol
  ```
  Setting options:
  - `o` - SPI_MODE_CPOL (Clock Polarity)
  - `h` - SPI_MODE_CPHA (Clock Phase)
  - `l` - SPI_TRANSFER_LSB (Least Significant Bit First)
  - `T` - SPI_FRAME_FORMAT_TI (TI Frame Format)

- `spi transceive <TX byte 1> [<TX byte 2> ...]` - Transceive data to and from an SPI device.
  ```shell
  spi transceive 0x55 0xAA
  ```

### GPIO Commands

- `gpio conf <device> <pin> <configuration>` - Configure GPIO pin
  ```shell
  gpio conf gpio0 0 ou1  # Configure as output, pull-up, initial level 1
  ```
  Configuration options:
  - `i|o` - input/output mode
  - `u|d` - pull up/pull down (optional)
  - `h|l` - active high/active low (optional, defaults to active high)
  - `0|1` - initialize to logic level (optional, defaults to 0)

- `gpio get <device> <pin>` - Get GPIO pin value
  ```shell
  gpio get gpio0 0
  ```

- `gpio set <device> <pin> <level>` - Set GPIO pin value
  ```shell
  gpio set gpio0 0 1
  ```

- `gpio toggle <device> <pin>` - Toggle GPIO pin state
  ```shell
  gpio toggle gpio0 0
  ```

- `gpio devices` - List all GPIO devices
  ```shell
  gpio devices
  ```

- `gpio blink <device> <pin>` - Blink GPIO pin
  ```shell
  gpio blink gpio0 0
  ```

- `gpio info [device]` - Show GPIO device information
  ```shell
  gpio info gpio0
  ```

5. Open a serial terminal (baudrate 115200) for testing.

## Hardware Requirements

- XIAO nRF54L15 Development Board
- Optional: NFC antenna (for NFC functionality)
- Optional: Battery (for power management features)
- Optional: I2C/SPI peripherals (for communication testing)
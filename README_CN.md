# XIAO nRF54L15 Zephyr测试项目

本项目演示了使用Zephyr RTOS在XIAO nRF54L15开发板上的各种功能。主要包括系统管理、传感器接口、通信接口等功能的shell命令测试。


## 构建和运行

1. 配置Zephyr RTOS开发环境。
2. 进入项目目录。
3. 构建项目：
```shell
west build -p -b xiao_nrf54l15_cpuapp
```
4. 烧录开发板：
```shell
west flash
```

## 可用命令

### 系统命令
- `sys off` - 进入系统关机模式。可以通过按用户按钮唤醒系统。
  ```shell
  sys off
  ```

### IMU传感器命令
- `imu get` - 获取LSM6DSO IMU传感器数据（加速度计和陀螺仪）。
  ```shell
  imu get
  ```
  返回值:
  - 成功: 显示加速度和陀螺仪数据，格式为 "X.YYYYYY, X.YYYYYY, X.YYYYYY"
  - 错误: 设备未就绪(-ENODEV)或通信错误时返回相应错误码

#### 麦克风操作示例
- `mic capture [<时长>]` - 采集麦克风音频数据。
  ```shell
    mic capture
    ``` 
    返回值:
    - 以"S"为开始标志，随后是音频数据，再以"E"结束标志

### 按钮命令
- `button check` - 检测用户按钮状态变化。
  ```shell
  button check
  ```
  输出:
  - 按下时显示 "usr button pressed"
  - 释放时显示 "usr button released"
  - 超时(5秒无输入)时显示 "No input received"
  - 设备错误时返回对应错误信息

### ADC命令
- `adc_read get` - 读取ADC通道值。
  ```shell
  adc_read get
  ```
  返回值:
  - 成功: 显示ADC原始读数和转换后的电压值
  - 错误: 初始化失败(-EINVAL)或读取错误时返回相应错误码

### NFC命令
- `nfc play` - 启动NFC显示/模拟。
  ```shell
  nfc play
  ```

### 休眠命令
- `sys off` - 进入系统休眠模式。
  ```shell
  sys off
  ```
  说明:
  - 执行此命令会禁用RF开关并配置用户按钮为唤醒源
  - 关闭串口控制台
  - 可通过按下用户按钮唤醒系统
  - 错误情况下会显示具体的错误原因

### I2C命令
- `i2c scan <设备>` - 扫描I2C总线上的设备。
  ```shell
  i2c scan i2c@104000
  ```
- `i2c read <设备> <地址> <寄存器> [<数据长度>]` - 从I2C设备读取数据。
  ```shell
  i2c read i2c@104000 0x6A 0x0F
  ```
- `i2c write <设备> <地址> <寄存器> [<数据>]` - 向I2C设备写入数据。
  ```shell
  i2c write i2c@104000 0x6A 0x10 0x00
  ```

### SPI命令
- `spi conf <设备> <频率> [<设置>]` - 配置SPI设备参数。
  ```shell
  spi conf spi1 1000000 ol
  ```
  设置选项:
  - `o` - SPI_MODE_CPOL（时钟极性）
  - `h` - SPI_MODE_CPHA（时钟相位）
  - `l` - SPI_TRANSFER_LSB（最低位优先）
  - `T` - SPI_FRAME_FORMAT_TI（TI帧格式）

- `spi transceive <发送字节1> [<发送字节2> ...]` - 在SPI设备上收发数据。
  ```shell
  spi transceive 0x55 0xAA
  ```

### GPIO命令

- `gpio conf <设备> <引脚> <配置>` - 配置GPIO引脚
  ```shell
  gpio conf gpio0 0 ou1  # 配置为输出，上拉，初始电平为1
  ```
  配置选项:
  - `i|o` - 输入/输出模式
  - `u|d` - 上拉/下拉（可选）
  - `h|l` - 高电平有效/低电平有效（可选，默认高电平有效）
  - `0|1` - 初始化电平（可选，默认为0）

- `gpio get <设备> <引脚>` - 读取GPIO引脚状态
  ```shell
  gpio get gpio0 0
  ```

- `gpio set <设备> <引脚> <电平>` - 设置GPIO引脚输出
  ```shell
  gpio set gpio0 0 1
  ```

- `gpio toggle <设备> <引脚>` - 切换GPIO引脚状态
  ```shell
  gpio toggle gpio0 0
  ```

- `gpio devices` - 列出所有GPIO设备
  ```shell
  gpio devices
  ```

- `gpio blink <设备> <引脚>` - 使GPIO引脚闪烁
  ```shell
  gpio blink gpio0 0
  ```

- `gpio info [设备]` - 显示GPIO设备信息
  ```shell
  gpio info gpio0
  ```

5. 打开串口终端(波特率115200)进行测试。

## 硬件要求

- XIAO nRF54L15开发板
- 可选：NFC天线（用于NFC功能）
- 可选：电池（用于电源管理功能）
- 可选：I2C/SPI外设（用于通信测试）





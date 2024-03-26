/*
BILIBILI:技术宅物语
 */

//EEPROM设置
#define EEPROM_WIFINAME_ADDR 100 //信息保存地址
#define EEPROM_WIFIPASSWORD_ADDR 125
#define EEPROM_BLUETOOTHNAME_ADDR 150
#define EEPROM_ROBOTPASSWORD_ADDR 175
#define EEPROM_LOCALIP_ADDR 200
#define EEPROM_RESET_ADDR 225 //连接WIFI后重启次数，1byte

// PWM通道设置
#define Camera_Ch 0
#define Headlamp_Ch 1 //PWM通道
#define MotorL_A_Ch 2 //PWM通道
#define MotorR_A_Ch 3 //PWM通道
#define MotorL_B_Ch 4 //PWM通道
#define MotorR_B_Ch 5 //PWM通道
#define Servo_Ch 6 //PWM通道

#define MOTOR_L 1
#define MOTOR_R 2

// 其他
#define UDP_RBUFF_SIZE 512 //udp接收缓冲区大小

//机器人型号选择！！！！！！
//#define JSZ_MINI_ROBOT //掌上mini侦察机器人选这个
#define JSZ_MEDIUM_ROBOT //中号教学机器人选这个


/////
#ifdef JSZ_MEDIUM_ROBOT
// IO配置
#define batteryVoltage_Pin -1 //电池电压检测
#define Servo_Pin 42 //舵机
#define Camera_xclk_Pin 14
#define Headlamp_Pin 46 //照明灯
#define MotorR_A_Pin 41 //马达驱动
#define MotorR_B_Pin 40 //马达驱动
#define MotorL_A_Pin 2 //马达驱动
#define MotorL_B_Pin 1 //马达驱动

// 摄像头
#define PWDN_GPIO_NUM     -1
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM     Camera_xclk_Pin
#define SIOD_GPIO_NUM     39
#define SIOC_GPIO_NUM     38

#define Y9_GPIO_NUM       21
#define Y8_GPIO_NUM       13
#define Y7_GPIO_NUM       12
#define Y6_GPIO_NUM       10
#define Y5_GPIO_NUM       8
#define Y4_GPIO_NUM       17
#define Y3_GPIO_NUM       18
#define Y2_GPIO_NUM       9
#define VSYNC_GPIO_NUM    48
#define HREF_GPIO_NUM     47
#define PCLK_GPIO_NUM     11
#endif

#ifdef JSZ_MINI_ROBOT
// IO配置
#define batteryVoltage_Pin -1 //电池电压检测
#define Servo_Pin 10 //舵机
#define Camera_xclk_Pin 36
#define Headlamp_Pin 19 //照明灯
#define MotorR_A_Pin 18 //马达驱动
#define MotorR_B_Pin 8 //马达驱动
#define MotorL_A_Pin 6 //马达驱动
#define MotorL_B_Pin 5 //马达驱动

// 摄像头
#define PWDN_GPIO_NUM     -1
#define RESET_GPIO_NUM    47
#define XCLK_GPIO_NUM     Camera_xclk_Pin
#define SIOD_GPIO_NUM     14
#define SIOC_GPIO_NUM     21

#define Y9_GPIO_NUM       35
#define Y8_GPIO_NUM       37
#define Y7_GPIO_NUM       38
#define Y6_GPIO_NUM       40
#define Y5_GPIO_NUM       42
#define Y4_GPIO_NUM       1
#define Y3_GPIO_NUM       2
#define Y2_GPIO_NUM       41
#define VSYNC_GPIO_NUM    48
#define HREF_GPIO_NUM     45
#define PCLK_GPIO_NUM     39
#endif

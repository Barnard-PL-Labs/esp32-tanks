/*
BILIBILI:技术宅物语
*/

extern SemaphoreHandle_t  camera_state; //图像获取完成信号量
extern SemaphoreHandle_t  sendJpg_state; //图像发送完成信号量

extern char localAddress[20];

extern int capture_handler(void);
extern void Communication( void * parameter);
void headlampInit(void);
void headlampSet(int bri);
void motorInit(void);
void motorSet(uint8_t motor, int spe);
void servoInit(void);
void servoSet(float angle);

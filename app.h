#ifndef __APP_H__
#define __APP_H__

typedef struct {
  u8  rf_len1;
  u16 vid;
  u32 pid;
  u8  control_key;
  u8  rf_seq_no;
  u8  start_code;// a1;

  u8 d1;
  u8 d2;
  u8 j1;
  u8 ad5;
  u8 ad6;
  u8 ad7;
  u8 j2;
  u8 unused;
}__attribute__((packed))rf_package_t;

#define M_KEY_ZHAOMING           KEY14
#define M_KEY_FENGWEN            KEY10
#define M_KEY_CHONGSHUI          KEY17
#define M_KEY_ZUOWEN             KEY2
#define M_KEY_SHUIWEN            KEY6
#define M_KEY_ANMO               KEY4
#define M_KEY_NVXINGQINGXI       KEY8
#define M_KEY_TINGZHI            KEY15
#define M_KEY_TUNBUQINGXI        KEY0
#define M_KEY_NUANFENG           KEY12

#define M_KEY_PENZUI_UP           KEY13
#define M_KEY_QINGXIQIANGDU_DOWN  KEY9
#define M_KEY_PENZUIQINGJIE       KEY16
#define M_KEY_QINGXIQIANGDU_UP    KEY1
#define M_KEY_PENZUI_DOWN         KEY5
#define M_KEY_YONGHU_1            KEY11
#define M_KEY_PAOMO               KEY7
#define M_KEY_YONGHU_2            KEY3

#define LED_FENGWEN_INDICATE_RED      HAL_LED_1
#define LED_FENGWEN_INDICATE_BLUE     HAL_LED_2

#define LED_SHUIWEN_INDICATE_RED      HAL_LED_3
#define LED_SHUIWEN_INDICATE_BLUE     HAL_LED_4

#define LED_ZUOWEN_INDICATE_RED       HAL_LED_5
#define LED_ZUOWEN_INDICATE_BLUE      HAL_LED_6

#define COMBIN_KEY_SETUP_TIME  (1000*16*1000)
#define COMBIN_TIME            (3000*16*1000)//ms2tick(3000)
#define COMBIN_KEY_LAST_TIME   (3000*16*1000)

#define SHORT_TIME  (3000*16*1000)//ms2tick(3000)
#define LONG_TIME   (4000*16*1000)//ms2tick(4000)
#define STUCK_TIME  (50000*16*1000)//ms2tick(50000)

#define KEY_PROCESS_TIME        5000//US
#define LED_UPDATE_PROCESS_TIME 10000//US

#define LED_ON_TIME     15000 //MS
#define SLEEP_WAIT_TIME 15000//MS

#define ID_Flash_Addr	0x020000 //address store id
#define VAL_ADDRESS   0x021000 //address store global var ,zuowen fenwen etc

#define USER1_VAL_ADDRESS 0x022000
#define USER2_VAL_ADDRESS 0x023000

#define LED_BRIGHT_LEVEL 50

#define PWM_PERIOD  20000  //US
#define PWM_ON_DUTY 60     //US

#define THRESHOLD 2290

#define PAIR_KEY_VALUE	0x55
#define MAX_EVENT       30//the number is the register_key_event call times
extern void app_init();
extern void clr_display_flag();
extern void display_led();
extern bool is_set_display_flag();

#endif

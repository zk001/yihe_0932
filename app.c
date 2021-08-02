#include "../../drivers.h"
#include "../../genfsk_ll/genfsk_ll.h"
#include "key.h"
#include "app.h"
#include "led.h"
#include "key.h"
#include "board.h"
#include "mac_id.h"
#include "rf.h"
#include "n_timer.h"
#include "power_saving.h"
#include "main.h"
#include "wakeup.h"
#include "low_power.h"

typedef enum {
  SHUIWEN_LED_INDICATE = 1,
  FENGWEN_LED_INDICATE,
  ZUOWEN_LED_INDICATE
}led_level_indi_t;

typedef struct {
  u32 red_led;
  u32 blue_led;
}led_color_t;

typedef enum {
  CHONGSHUI = 0,
  TINGZHI,
  TUNXI,
  FUXI,
  NUANFENGHONGGAN,
  LENREANMO,
  YEDENG,
  PENZUIQINGXI,
  FANGAI,
  FANQUANG,
  PENZUIWEIZHI_UP,
  PENZUIWEIZHI_DOWN,
  ERTONGQINGXI,
  QINGXIQIANGDU_PLUS,
  QINGXIQIANGDU_DEC,
  SHUIWENTIAOJIE,
  ZUOWENTIAOJIE,
  FENGWENTIAOJIE,
  WEIBO_KAIGUAN,
  WIFI,
  SHEZHI,
  YONGHU_1_XIANSHI,//22
  YONGHU_1_JIYI,
  YONGHU_2_XIANSHI,
  YONGHU_2_JIYI,
  PAOMO_SHORT,
  PAOMO_LONG,//27
  DUIMA
}function_t;

static const u8 key_ad[] = {
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0x11,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,//YONGHU_1_XIANSHI
  0,//YONGHU_1_JIYI
  0,//YONGHU_2_XIANSHI
  0,//YONGHU_2_JIYI
  0x16,//PAOMO_SHORT
  0x17//PAOMO_LONG
};

static const u8 key_d[] = {
  0x15,
  0x04,
  0x0c,
  0x0d,
  0x0a,
  0x11,
  0x16,
  0x05,
  0x17,
  0x1a,
  0x0e,
  0x0f,
  0x0b,//ertongqingxi
  0x08,
  0x09,
  0x06,
  0x07,
  0x1c,
  0x1b,
  0x03,
  0x1f,
  0x13,//YONGHU_1_XIANSHI
  0x12,//YONGHU_1_JIYI
  0x02,//YONGHU_2_XIANSHI
  0x01,//YONGHU_2_JIYI
  0x0b,//PAOMO_SHORT
  0x0b//PAOMO_LONG
};
static const u8 shuiwen_d[11] = {
  0x00,
  0x01,
  0x02,
  0x03,
  0x00,
  0x01,
  0x02,
  0x03,
  0x00,
  0x01,
  0x02
};
static const u8 zuowen_d[11] = {
  0x00,
  0x01,
  0x02,
  0x03,
  0x00,
  0x01,
  0x02,
  0x03,
  0x00,
  0x01,
  0x02
};
static const u8 fenwen_d[11] = {
  0x00,
  0x01,
  0x02,
  0x03,
  0x00,
  0x01,
  0x02,
  0x03,
  0x00,
  0x01,
  0x02
};
static const u8 penzui_d[12] = {
  0x00,
  0x01,
  0x02,
  0x03,
  0x00,
  0x01,
  0x02,
  0x03,
  0x00,
  0x01,
  0x02,
  0x03,
};
static const u8 shuiya_d[12] = {
  0x00,
  0x01,
  0x02,
  0x03,
  0x00,
  0x01,
  0x02,
  0x03,
  0x00,
  0x01,
  0x02,
  0x03,
};

static const u8 shuiwen_ad[11] = {
  0x00,
  0x00,
  0x00,
  0x00,
  0x01,
  0x01,
  0x01,
  0x01,
  0x02,
  0x02,
  0x02,
};
static const u8 zuowen_ad[11] = {
  0x00,
  0x00,
  0x00,
  0x00,
  0x01,
  0x01,
  0x01,
  0x01,
  0x02,
  0x02,
  0x02,
};
static const u8 fenwen_ad[11] = {
  0x00,
  0x00,
  0x00,
  0x00,
  0x01,
  0x01,
  0x01,
  0x01,
  0x02,
  0x02,
  0x02,
};
static const u8 penzui_ad[12] = {
  0x00,
  0x00,
  0x00,
  0x00,
  0x01,
  0x01,
  0x01,
  0x01,
  0x02,
  0x02,
  0x02,
  0x02,
};
static const u8 shuiya_ad[12] = {
  0x00,
  0x00,
  0x00,
  0x00,
  0x01,
  0x01,
  0x01,
  0x01,
  0x02,
  0x02,
  0x02,
  0x02,
};
static const u8 taocichongshui_ad[4] = {
  0x00,
  0x01,
  0x02,
  0x03,
};
static const u8 weibo_ad[4] = {
  0x00,
  0x01,
  0x02,
  0x03,
};

_attribute_data_retention_ static u32 uid;
_attribute_data_retention_ static u32 new_id;

typedef struct {
  u8 shuiya;
  u8 penzui;
  u8 fenwen;
  u8 shuiwen;
  u8 zuowen;
  u8 unused;
  u8 taoci;
  u8 weibo;
}user_value_t __attribute__ ((aligned (4)));

_attribute_data_retention_ static user_value_t cur_user_value;
_attribute_data_retention_ static user_value_t user_1_value;
_attribute_data_retention_ static user_value_t user_2_value;

typedef enum{
  SHUIYA_INDEX = 1,
  DAOGANG_INDEX,
  FENGWEN_INDEX,
  SHUIWEN_INDEX,
  ZUOWEN_INDEX
}user_en_t;

void get_cur_user_value_from_flash()
{
  flash_read_page(VAL_ADDRESS, sizeof(user_value_t), (unsigned char *)&cur_user_value);
}

void get_yonghu_1_value_from_flash()
{
  flash_read_page(USER1_VAL_ADDRESS, sizeof(user_value_t), (unsigned char *)&user_1_value);
}

void get_yonghu_2_value_from_flash()
{
  flash_read_page(USER2_VAL_ADDRESS, sizeof(user_value_t), (unsigned char *)&user_2_value);
}

u8 get_cur_user_value(user_en_t u_val)
{
  u8 r_val;

  switch(u_val){
    case SHUIYA_INDEX:  r_val = cur_user_value.shuiya;break;
    case DAOGANG_INDEX: r_val = cur_user_value.penzui;break;
    case FENGWEN_INDEX: r_val = cur_user_value.fenwen;break;
    case SHUIWEN_INDEX: r_val = cur_user_value.shuiwen;break;
    case ZUOWEN_INDEX:  r_val = cur_user_value.zuowen;break;
    default: r_val = 0;break;
  }

  return r_val;
}

void set_user_value(u32 address, user_en_t u_val, user_value_t *user_value, u8 val)
{
  switch(u_val){
    case SHUIYA_INDEX:  user_value->shuiya  = val;break;
    case DAOGANG_INDEX: user_value->penzui  = val;break;
    case FENGWEN_INDEX: user_value->fenwen  = val;break;
    case SHUIWEN_INDEX: user_value->shuiwen = val;break;
    case ZUOWEN_INDEX:  user_value->zuowen  = val;break;
    default:break;
  }

  flash_erase_sector(address);

  flash_write_page(address, sizeof(user_value_t), (unsigned char *)user_value);
}

void set_default_user_value(u32 address, user_value_t *user_value)
{
  flash_read_page(address, sizeof(user_value_t), (unsigned char *)user_value);

  if(user_value->shuiya == 0xff &&\
      user_value->penzui == 0xff &&\
      user_value->fenwen == 0xff &&\
      user_value->shuiwen == 0xff &&\
      user_value->zuowen == 0xff){

    user_value->shuiya    = 2;
    user_value->penzui    = 2;
    user_value->fenwen    = 2;
    user_value->shuiwen   = 2;
    user_value->zuowen    = 2;

    flash_erase_sector(address);

    flash_write_page(address, sizeof(user_value_t), (unsigned char *)user_value);
  }
}

void set_shuiya_weizhi_default_value()
{
  cur_user_value.shuiya    = 2;
  cur_user_value.penzui    = 2;
  set_user_value(VAL_ADDRESS, SHUIYA_INDEX,  &cur_user_value, cur_user_value.shuiya);
  set_user_value(VAL_ADDRESS, DAOGANG_INDEX, &cur_user_value, cur_user_value.penzui);
}

void fix_pack_with_user_value(rf_package_t *rf_pack, function_t fn)
{
  rf_pack->rf_len1     = 0;
  rf_pack->vid         = 0x5453;

  if(fn == DUIMA){
    gen_random_id(&new_id);

    rf_pack->pid           = new_id;
    rf_pack->control_key   = PAIR_KEY_VALUE;

    rf_pack->rf_seq_no     = 1;

    rf_pack->start_code    = 1;
    rf_pack->d1            = 1;
    rf_pack->d2            = 1;
    rf_pack->j1            = 1;
    rf_pack->ad5           = 1;
    rf_pack->ad6           = 1;
    rf_pack->ad7           = 1;
    rf_pack->j2            = 1;
  }else{
    rf_pack->pid         = uid;
    rf_pack->control_key = 0;

    rf_pack->rf_seq_no   = 1;

    rf_pack->start_code  = 0x3a;

    rf_pack->d1     = 0x80 | (cur_user_value.shuiya << 5) | key_d[fn];
    rf_pack->d2     = (fenwen_d[cur_user_value.fenwen] << 6) | (penzui_d[cur_user_value.penzui] << 4) | (zuowen_d[cur_user_value.zuowen]  << 2) | shuiwen_d[cur_user_value.shuiwen];
    rf_pack->j1     = (rf_pack->start_code + rf_pack->d1 + rf_pack->d2) & 0xff;
    //    rf_pack->ad5    = (shuiya_ad[cur_user_value.shuiya] << 4) | (taocichongshui_ad[cur_user_value.taoci] << 2) | (weibo_ad[cur_user_value.weibo]);
    //    rf_pack->ad6    = (fenwen_ad[cur_user_value.fenwen] << 6 ) | (penzui_ad[cur_user_value.penzui] << 4) | (zuowen_ad[cur_user_value.zuowen] << 2) | shuiwen_ad[cur_user_value.shuiwen];
    if(fn == PAOMO_SHORT || fn == PAOMO_LONG){
      rf_pack->ad7    = key_ad[fn];
      rf_pack->j2     = (rf_pack->ad5 ^ rf_pack->ad6 ^ rf_pack->ad7) & 0xff;
    }else{
      rf_pack->ad5 = 0;
      rf_pack->ad6 = 0;
      rf_pack->ad7 = 0;
      rf_pack->j2  = 0;
    }
  }
}

void set_led_level_on(u32 leds)
{
  u32 ledoff;

  if(low_power){
    leds &= ~LED_SHUIWEN_INDICATE_BLUE;
    ledoff = leds ^ (HAL_LED_ALL & ~LED_SHUIWEN_INDICATE_BLUE);
    HalLedSet (ledoff, HAL_LED_MODE_OFF);
  }else{
    ledoff = leds ^ HAL_LED_ALL;
    HalLedSet (ledoff, HAL_LED_MODE_OFF);
  }
  HalLedSet (leds, HAL_LED_MODE_ON);//ON AND OFF SYNC,MUST ON FIRST
  HalLedBlink (leds, 1, 100, MS2TICK(LED_ON_TIME));
}

void choose(led_level_indi_t which, led_color_t *led)
{
  switch(which){
    case SHUIWEN_LED_INDICATE:
      led->red_led  = LED_SHUIWEN_INDICATE_RED;
      led->blue_led = LED_SHUIWEN_INDICATE_BLUE;break;
    case FENGWEN_LED_INDICATE:
      led->red_led  = LED_FENGWEN_INDICATE_RED;
      led->blue_led = LED_FENGWEN_INDICATE_BLUE ;break;
    case ZUOWEN_LED_INDICATE:
      led->red_led  = LED_ZUOWEN_INDICATE_RED;
      led->blue_led = LED_ZUOWEN_INDICATE_BLUE;break;
    default:break;
  }
}

u32 set_led_level(led_level_indi_t which, u8 user_value)
{
  u32 leds = 0;
  led_color_t color_led = {0, 0};

  choose(which, &color_led);

  switch(user_value){
    case 0: leds = 0; break;
    case 1: leds = color_led.blue_led;break;
    case 2: leds = color_led.blue_led | color_led.red_led;break;
    case 3: leds = color_led.red_led;break;
    default:break;
  }

  return leds;
}

void display_led()
{
  u8 shuiwen;
  u8 zuowen;
  u8 fengwen;
  u32 leds = 0;

  shuiwen = get_cur_user_value(SHUIWEN_INDEX);
  zuowen  = get_cur_user_value(ZUOWEN_INDEX);
  fengwen = get_cur_user_value(FENGWEN_INDEX);

  leds |= set_led_level(SHUIWEN_LED_INDICATE, shuiwen);
  leds |= set_led_level(ZUOWEN_LED_INDICATE,  zuowen);
  leds |= set_led_level(FENGWEN_LED_INDICATE, fengwen);

  set_led_level_on(leds);
}

bool flag;

void set_display_flag()
{
  flag = 1;
}

bool is_set_display_flag()
{
  return flag;
}

void clr_display_flag()
{
  flag = 0;
}

void reload_led()
{
  if(is_set_display_flag())
    display_led();
}

void app_low_bat_check()
{
  if(is_enable_low_bat_check()){
    disable_low_bat_check();
    trigger_low_bat_check();
  }
}

//"key action" "which key" "cmd-code"
void short_m_key_zhaoming_yedeng()
{
  rf_package_t rf_pack;

  fix_pack_with_user_value(&rf_pack, YEDENG);

  app_low_bat_check();

  reload_led();

  send_rf_data_yihe(&rf_pack, sizeof(rf_pack));
}

void short_m_key_tingzhi_tingzhi()
{
  rf_package_t rf_pack;

  HalLedSet(HAL_LED_ALL, HAL_LED_MODE_OFF);

  fix_pack_with_user_value(&rf_pack, TINGZHI);

  send_rf_data_yihe(&rf_pack, sizeof(rf_pack));
}

void short_m_key_tunbuqingxi_tunxi()
{
  rf_package_t rf_pack;

  fix_pack_with_user_value(&rf_pack, TUNXI);

  app_low_bat_check();

  display_led();

  set_display_flag();

  send_rf_data_yihe(&rf_pack, sizeof(rf_pack));
}

void short_m_key_nvxingqingxi_fuxi()
{
  rf_package_t rf_pack;

  fix_pack_with_user_value(&rf_pack, FUXI);

  app_low_bat_check();

  display_led();

  set_display_flag();

  send_rf_data_yihe(&rf_pack, sizeof(rf_pack));
}

void short_m_key_nuanfenghonggan_nuanfenghonggan()
{
  rf_package_t rf_pack;

  fix_pack_with_user_value(&rf_pack, NUANFENGHONGGAN);

  app_low_bat_check();

  display_led();

  set_display_flag();

  send_rf_data_yihe(&rf_pack, sizeof(rf_pack));
}

void short_m_key_anmo_anmo()
{
  rf_package_t rf_pack;

  fix_pack_with_user_value(&rf_pack, LENREANMO);

  app_low_bat_check();

  reload_led();

  send_rf_data_yihe(&rf_pack, sizeof(rf_pack));
}

void short_m_key_penzuiqingjie_penzuiqingjie()
{
  rf_package_t rf_pack;

  fix_pack_with_user_value(&rf_pack, PENZUIQINGXI);

  app_low_bat_check();

  reload_led();

  send_rf_data_yihe(&rf_pack, sizeof(rf_pack));
}

bool is_enter_set_status()
{
  static bool tiaojie_status = 0;

  if(pre_key == M_KEY_TUNBUQINGXI || pre_key == M_KEY_NVXINGQINGXI ||\
      pre_key == M_KEY_YONGHU_1 || pre_key == M_KEY_YONGHU_2){
    tiaojie_status = 1;
  }else if(pre_key != M_KEY_TUNBUQINGXI && pre_key != M_KEY_NVXINGQINGXI &&\
      pre_key != M_KEY_QINGXIQIANGDU_UP && pre_key != M_KEY_QINGXIQIANGDU_DOWN &&\
      pre_key != M_KEY_PENZUI_UP && pre_key != M_KEY_PENZUI_DOWN && pre_key != M_KEY_YONGHU_1 &&\
      pre_key != M_KEY_YONGHU_2){
    tiaojie_status = 0;
  }

  return tiaojie_status;
}

void short_m_key_penzui_up_penzui_up()
{
  rf_package_t rf_pack;
  u8 daogan;

  if(is_enter_set_status()){
    daogan = get_cur_user_value(DAOGANG_INDEX);
    daogan++;

    if(daogan == 4)
      daogan = 3;

    set_user_value(VAL_ADDRESS, DAOGANG_INDEX, &cur_user_value, daogan);

  }

  fix_pack_with_user_value(&rf_pack, PENZUIWEIZHI_UP);

  app_low_bat_check();

  reload_led();

  send_rf_data_yihe(&rf_pack, sizeof(rf_pack));
}

void short_m_key_penzui_down_penzui_down()
{
  rf_package_t rf_pack;
  u8 daogan;

  if(is_enter_set_status()){
    daogan = get_cur_user_value(DAOGANG_INDEX);
    daogan--;

    if(daogan == 0)
      daogan = 1;

    set_user_value(VAL_ADDRESS, DAOGANG_INDEX, &cur_user_value, daogan);
  }

  fix_pack_with_user_value(&rf_pack, PENZUIWEIZHI_DOWN);

  app_low_bat_check();

  reload_led();

  send_rf_data_yihe(&rf_pack, sizeof(rf_pack));
}

void long_m_key_yonghu1_yonghujiyi()
{
  rf_package_t rf_pack;

  user_1_value.shuiya  = cur_user_value.shuiya;
  user_1_value.penzui  = cur_user_value.penzui;
  user_1_value.fenwen  = cur_user_value.fenwen;
  user_1_value.shuiwen = cur_user_value.shuiwen;
  user_1_value.zuowen  = cur_user_value.zuowen;
  user_1_value.unused  = cur_user_value.unused;
  user_1_value.taoci   = cur_user_value.taoci;
  user_1_value.weibo   = cur_user_value.weibo;

  flash_erase_sector(USER1_VAL_ADDRESS);

  flash_write_page(USER1_VAL_ADDRESS, sizeof(user_1_value), (unsigned char *)&user_1_value);

  fix_pack_with_user_value(&rf_pack, YONGHU_1_JIYI);

  send_rf_data_yihe(&rf_pack, sizeof(rf_pack));

  reload_led();
}

void short_m_key_yonghu1_yonghuxianshi()
{
  rf_package_t rf_pack;

  get_yonghu_1_value_from_flash();

  cur_user_value.shuiya  = user_1_value.shuiya;
  cur_user_value.penzui  = user_1_value.penzui;
  cur_user_value.fenwen  = user_1_value.fenwen;
  cur_user_value.shuiwen = user_1_value.shuiwen;
  cur_user_value.zuowen  = user_1_value.zuowen;
  cur_user_value.unused  = user_1_value.unused;
  cur_user_value.taoci   = user_1_value.taoci;
  cur_user_value.weibo   = user_1_value.weibo;

  fix_pack_with_user_value(&rf_pack, YONGHU_1_XIANSHI);

  send_rf_data_yihe(&rf_pack, sizeof(rf_pack));

  display_led();

  set_display_flag();
}

void short_m_key_yonghu2_yonghuxianshi()
{
  rf_package_t rf_pack;

  get_yonghu_2_value_from_flash();

  cur_user_value.shuiya  = user_2_value.shuiya;
  cur_user_value.penzui  = user_2_value.penzui;
  cur_user_value.fenwen  = user_2_value.fenwen;
  cur_user_value.shuiwen = user_2_value.shuiwen;
  cur_user_value.zuowen  = user_2_value.zuowen;
  cur_user_value.unused  = user_2_value.unused;
  cur_user_value.taoci   = user_2_value.taoci;
  cur_user_value.weibo   = user_2_value.weibo;

  fix_pack_with_user_value(&rf_pack, YONGHU_2_XIANSHI);

  send_rf_data_yihe(&rf_pack, sizeof(rf_pack));

  display_led();

  set_display_flag();
}

void long_m_key_yonghu2_yonghujiyi()
{
  rf_package_t rf_pack;

  user_2_value.shuiya  = cur_user_value.shuiya;
  user_2_value.penzui  = cur_user_value.penzui;
  user_2_value.fenwen  = cur_user_value.fenwen;
  user_2_value.shuiwen = cur_user_value.shuiwen;
  user_2_value.zuowen  = cur_user_value.zuowen;
  user_2_value.unused  = cur_user_value.unused;
  user_2_value.taoci   = cur_user_value.taoci;
  user_2_value.weibo   = cur_user_value.weibo;

  flash_erase_sector(USER2_VAL_ADDRESS);

  flash_write_page(USER2_VAL_ADDRESS, sizeof(user_2_value), (unsigned char *)&user_2_value);

  fix_pack_with_user_value(&rf_pack, YONGHU_2_JIYI);

  send_rf_data_yihe(&rf_pack, sizeof(rf_pack));

  reload_led();
}

void short_m_key_paomo_paomo_duan()
{
  rf_package_t rf_pack;

  fix_pack_with_user_value(&rf_pack, PAOMO_SHORT);

  send_rf_data_yihe(&rf_pack, sizeof(rf_pack));

  reload_led();
}

void long_m_key_paomo_paomo_chang()
{
  rf_package_t rf_pack;

  fix_pack_with_user_value(&rf_pack, PAOMO_LONG);

  send_rf_data_yihe(&rf_pack, sizeof(rf_pack));

  reload_led();
}

void short_m_key_qingxiqiangdu_up_qingxiqiangdu_up()
{
  rf_package_t rf_pack;
  u8 shuiya;

  if(is_enter_set_status()){
    shuiya = get_cur_user_value(SHUIYA_INDEX);
    shuiya++;

    if(shuiya == 4)
      shuiya = 3;

    set_user_value(VAL_ADDRESS, SHUIYA_INDEX, &cur_user_value, shuiya);
  }

  fix_pack_with_user_value(&rf_pack, QINGXIQIANGDU_PLUS);

  app_low_bat_check();

  reload_led();

  send_rf_data_yihe(&rf_pack, sizeof(rf_pack));
}

void short_m_key_qingxiqiangdu_down_qingxiqiangdu_down()
{
  rf_package_t rf_pack;
  u8 shuiya;

  if(is_enter_set_status()){
    shuiya = get_cur_user_value(SHUIYA_INDEX);
    shuiya--;

    if(shuiya == 0)
      shuiya = 1;

    set_user_value(VAL_ADDRESS, SHUIYA_INDEX, &cur_user_value, shuiya);
  }

  fix_pack_with_user_value(&rf_pack, QINGXIQIANGDU_DEC);

  app_low_bat_check();

  reload_led();

  send_rf_data_yihe(&rf_pack, sizeof(rf_pack));
}

void short_m_key_shuiwen_shuiwen()
{
  rf_package_t rf_pack;

  u8 shuiwen;

  shuiwen = get_cur_user_value(SHUIWEN_INDEX);

  shuiwen++;

  if(shuiwen == 4)
    shuiwen = 0;

  set_user_value(VAL_ADDRESS, SHUIWEN_INDEX, &cur_user_value, shuiwen);

  app_low_bat_check();

  display_led();

  set_display_flag();

  fix_pack_with_user_value(&rf_pack, SHUIWENTIAOJIE);

  send_rf_data_yihe(&rf_pack, sizeof(rf_pack));
}

void short_m_key_zuowen_zuowen()
{
  rf_package_t rf_pack;

  u8 zuowen;

  zuowen = get_cur_user_value(ZUOWEN_INDEX);

  zuowen++;

  if(zuowen == 4)
    zuowen = 0;

  set_user_value(VAL_ADDRESS, ZUOWEN_INDEX, &cur_user_value, zuowen);

  app_low_bat_check();

  display_led();

  set_display_flag();

  fix_pack_with_user_value(&rf_pack, ZUOWENTIAOJIE);

  send_rf_data_yihe(&rf_pack, sizeof(rf_pack));
}

void short_m_key_fengwen_fengwen()
{
  rf_package_t rf_pack;

  u8 fenwen;

  fenwen = get_cur_user_value(FENGWEN_INDEX);

  fenwen++;

  if(fenwen == 4)
    fenwen = 0;

  set_user_value(VAL_ADDRESS, FENGWEN_INDEX, &cur_user_value, fenwen);

  app_low_bat_check();

  display_led();

  set_display_flag();

  fix_pack_with_user_value(&rf_pack, FENGWENTIAOJIE);

  send_rf_data_yihe(&rf_pack, sizeof(rf_pack));
}

void short_m_key_chongshui_chongshui()
{
  rf_package_t rf_pack;

  fix_pack_with_user_value(&rf_pack, CHONGSHUI);

  app_low_bat_check();

  send_rf_data_yihe(&rf_pack, sizeof(rf_pack));

  reload_led();
}

void long_m_key_tingzhi_yonghuduima()
{
  rf_package_t rf_pack;
  u8 rx_buf[32] = {0};
  u32 send_period;

  HalLedSet(HAL_LED_ALL, HAL_LED_MODE_OFF);

  HalLedBlink (LED_SHUIWEN_INDICATE_RED | LED_ZUOWEN_INDICATE_RED | LED_FENGWEN_INDICATE_RED, 30, 50, MS2TICK(1000));

  fix_pack_with_user_value(&rf_pack, DUIMA);

  send_period = clock_time();

  while(1){
    if(!HalLedUpdate(NULL)){
      rf_8359_set_tx();
      clr_app_read_key_flag();
      reload_sys_time();
      return;
    }

    if(n_clock_time_exceed(send_period, 1000000)){//1S
      rf_8359_set_tx();
      send_rf_data_yihe(&rf_pack, sizeof(rf_pack));
      rf_8359_set_rx();
      send_period = clock_time();
    }

    if(receive_rf_data(rx_buf))
    {
      if(rx_buf[7] == 0x88){//received peer code
        rf_8359_set_tx();
        write_id(&new_id, 4);
        uid = new_id;
        clr_app_read_key_flag();
        HalLedBlink (LED_SHUIWEN_INDICATE_RED | LED_ZUOWEN_INDICATE_RED | LED_FENGWEN_INDICATE_RED, 1, 100, MS2TICK(5000));
        reload_sys_time();
        return;
      }
    }

    if(app_read_single_key(M_KEY_TINGZHI)){
      rf_8359_set_tx();
      HalLedSet (LED_SHUIWEN_INDICATE_RED | LED_ZUOWEN_INDICATE_RED | LED_FENGWEN_INDICATE_RED, HAL_LED_MODE_OFF);
      reload_sys_time();
      return;
    }
  }
}

void app_init()
{
  if(!is_wakeup_from_sleep()){

    set_default_user_value(VAL_ADDRESS, &cur_user_value);
    set_default_user_value(USER1_VAL_ADDRESS, &user_1_value);
    set_default_user_value(USER2_VAL_ADDRESS, &user_2_value);

    read_id(&uid, 4);
  }

  get_cur_user_value_from_flash();

  set_shuiya_weizhi_default_value();//reload default shuiya penzui value

  register_key_event(M_KEY_ZHAOMING,      0, 0, 0, SHORT_KEY_IMMEDIATELY, short_m_key_zhaoming_yedeng);
  register_key_event(M_KEY_FENGWEN,       0, 0, 0, SHORT_KEY_IMMEDIATELY, short_m_key_fengwen_fengwen);
  register_key_event(M_KEY_CHONGSHUI,     0, 0, 0, SHORT_KEY_IMMEDIATELY, short_m_key_chongshui_chongshui);
  register_key_event(M_KEY_ZUOWEN,        0, 0, 0, SHORT_KEY_IMMEDIATELY, short_m_key_zuowen_zuowen);
  register_key_event(M_KEY_SHUIWEN,       0, 0, 0, SHORT_KEY_IMMEDIATELY, short_m_key_shuiwen_shuiwen);

  register_key_event(M_KEY_ANMO,         0, 0, 0, SHORT_KEY_IMMEDIATELY, short_m_key_anmo_anmo);
  register_key_event(M_KEY_NVXINGQINGXI, 0, 0, 0, SHORT_KEY_IMMEDIATELY, short_m_key_nvxingqingxi_fuxi);

  register_key_event(M_KEY_TINGZHI,      0, 0, 0, SHORT_KEY_IMMEDIATELY, enable_low_bat_check);
  register_key_event(M_KEY_TINGZHI,      0, MS2TICK(3000), 0, SHORT_KEY, short_m_key_tingzhi_tingzhi);
  register_key_event(M_KEY_TINGZHI,      0, MS2TICK(5000), 0, LONG_KEY,  long_m_key_tingzhi_yonghuduima);
  register_key_event(M_KEY_TUNBUQINGXI,  0, 0, 0, SHORT_KEY_IMMEDIATELY, short_m_key_tunbuqingxi_tunxi);
  register_key_event(M_KEY_NUANFENG,     0, 0, 0, SHORT_KEY_IMMEDIATELY, short_m_key_nuanfenghonggan_nuanfenghonggan);

  register_key_event(M_KEY_PENZUI_UP,         0, 0, 0, SHORT_KEY_IMMEDIATELY, short_m_key_penzui_up_penzui_up);
  register_key_event(M_KEY_QINGXIQIANGDU_DOWN,0, 0, 0, SHORT_KEY_IMMEDIATELY, short_m_key_qingxiqiangdu_down_qingxiqiangdu_down);
  register_key_event(M_KEY_PENZUIQINGJIE,     0, 0, 0, SHORT_KEY_IMMEDIATELY, short_m_key_penzuiqingjie_penzuiqingjie);
  register_key_event(M_KEY_QINGXIQIANGDU_UP,  0, 0, 0, SHORT_KEY_IMMEDIATELY, short_m_key_qingxiqiangdu_up_qingxiqiangdu_up);
  register_key_event(M_KEY_PENZUI_DOWN,       0, 0, 0, SHORT_KEY_IMMEDIATELY, short_m_key_penzui_down_penzui_down);

  register_key_event(M_KEY_YONGHU_1, 0, 0, 0, SHORT_KEY_IMMEDIATELY, app_low_bat_check);
  register_key_event(M_KEY_YONGHU_1, 0, MS2TICK(3000), 0, SHORT_KEY, short_m_key_yonghu1_yonghuxianshi);
  register_key_event(M_KEY_YONGHU_1, 0, MS2TICK(5000), 0, LONG_KEY,  long_m_key_yonghu1_yonghujiyi);

  register_key_event(M_KEY_PAOMO,    0, 0, 0, SHORT_KEY_IMMEDIATELY, app_low_bat_check);
  register_key_event(M_KEY_PAOMO,    0, MS2TICK(3000), 0, SHORT_KEY, short_m_key_paomo_paomo_duan);
  register_key_event(M_KEY_PAOMO,    0, MS2TICK(5000), 0, LONG_KEY,  long_m_key_paomo_paomo_chang);

  register_key_event(M_KEY_YONGHU_2, 0, 0, 0, SHORT_KEY_IMMEDIATELY, app_low_bat_check);
  register_key_event(M_KEY_YONGHU_2, 0, MS2TICK(3000), 0, SHORT_KEY, short_m_key_yonghu2_yonghuxianshi);
  register_key_event(M_KEY_YONGHU_2, 0, MS2TICK(5000), 0, LONG_KEY,  long_m_key_yonghu2_yonghujiyi);
}

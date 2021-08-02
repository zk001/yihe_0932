#include "../../drivers.h"
#include "../../genfsk_ll/genfsk_ll.h"
#include "key.h"
#include "rf.h"
#include "app.h"
#include "led.h"
#include "dc.h"
#include "n_timer.h"
#include "../../common/mmem.h"
#include "../../common/mempool.h"
#include "board.h"
#include "mac_id.h"
#include "gpio_key.h"
#include "gpio_led.h"
#include "power_saving.h"
#include "low_power.h"
#include "interrupt.h"
#include "timer_clock.h"
#include "wakeup.h"

u32 low_bat_time_ref;
bool low_power;
bool low_bat_check;
bool low_bat_check_start;
MEMPOOL_DECLARE(KEY_EVENT_POOL, KEY_EVENT_POOL_MEM, sizeof(mem_block_t) + sizeof(event_handler_t), MAX_EVENT);
_attribute_data_retention_ key_map_t key_arry[MAX_GPIO_KEYS] = {
  {ROW0, COL0, IS_WAKE_UP},
  {ROW0, COL1, IS_WAKE_UP},
  {ROW0, COL2, IS_WAKE_UP},
  {ROW0, COL3, IS_WAKE_UP},
  {ROW1, COL0, IS_WAKE_UP},
  {ROW1, COL1, IS_WAKE_UP},
  {ROW1, COL2, IS_WAKE_UP},
  {ROW1, COL3, IS_WAKE_UP},
  {ROW2, COL0, IS_WAKE_UP},
  {ROW2, COL1, IS_WAKE_UP},
  {ROW2, COL2, IS_WAKE_UP},
  {ROW2, COL3, IS_WAKE_UP},
  {ROW3, COL0, IS_WAKE_UP},
  {ROW3, COL1, IS_WAKE_UP},
  {ROW3, COL2, IS_WAKE_UP},
  {ROW4, COL0, IS_WAKE_UP},
  {ROW4, COL1, IS_WAKE_UP},
  {ROW4, COL2, IS_WAKE_UP},
};

const u32 led_arry[MAX_GPIO_LEDS] = {
  LED1, LED2, LED3, LED4, LED5, LED6
};

const hal_led_t led_enum_arry[MAX_LEDS] = {
  {HAL_LED_1,  gpio_led_on_off, NULL},
  {HAL_LED_2,  gpio_led_on_off, NULL},
  {HAL_LED_3,  gpio_led_on_off, NULL},
  {HAL_LED_4,  gpio_led_on_off, NULL},
  {HAL_LED_5,  gpio_led_on_off, NULL},
  {HAL_LED_6,  gpio_led_on_off, NULL},
};

const key_type_t key_enum_arry[MAX_KEYS] = {
  {KEY0,  MECHANICAL_KEY, gpio_key_init, gpio_key_low_scan, gpio_stuck_key_low_scan},
  {KEY1,  MECHANICAL_KEY, gpio_key_init, gpio_key_low_scan, gpio_stuck_key_low_scan},
  {KEY2,  MECHANICAL_KEY, gpio_key_init, gpio_key_low_scan, gpio_stuck_key_low_scan},
  {KEY3,  MECHANICAL_KEY, gpio_key_init, gpio_key_low_scan, gpio_stuck_key_low_scan},
  {KEY4,  MECHANICAL_KEY, gpio_key_init, gpio_key_low_scan, gpio_stuck_key_low_scan},
  {KEY5,  MECHANICAL_KEY, gpio_key_init, gpio_key_low_scan, gpio_stuck_key_low_scan},
  {KEY6,  MECHANICAL_KEY, gpio_key_init, gpio_key_low_scan, gpio_stuck_key_low_scan},
  {KEY7,  MECHANICAL_KEY, gpio_key_init, gpio_key_low_scan, gpio_stuck_key_low_scan},
  {KEY8,  MECHANICAL_KEY, gpio_key_init, gpio_key_low_scan, gpio_stuck_key_low_scan},
  {KEY9,  MECHANICAL_KEY, gpio_key_init, gpio_key_low_scan, gpio_stuck_key_low_scan},
  {KEY10, MECHANICAL_KEY, gpio_key_init, gpio_key_low_scan, gpio_stuck_key_low_scan},
  {KEY11, MECHANICAL_KEY, gpio_key_init, gpio_key_low_scan, gpio_stuck_key_low_scan},
  {KEY12, MECHANICAL_KEY, gpio_key_init, gpio_key_low_scan, gpio_stuck_key_low_scan},
  {KEY13, MECHANICAL_KEY, gpio_key_init, gpio_key_low_scan, gpio_stuck_key_low_scan},
  {KEY14, MECHANICAL_KEY, gpio_key_init, gpio_key_low_scan, gpio_stuck_key_low_scan},
  {KEY15, MECHANICAL_KEY, gpio_key_init, gpio_key_low_scan, gpio_stuck_key_low_scan},
  {KEY16, MECHANICAL_KEY, gpio_key_init, gpio_key_low_scan, gpio_stuck_key_low_scan},
  {KEY17, MECHANICAL_KEY, gpio_key_init, gpio_key_low_scan, gpio_stuck_key_low_scan},
};

bool is_trigger_low_bat_check()
{
  return low_bat_check_start;
}

void trigger_low_bat_check()
{
  low_bat_check_start = 1;
}

void stop_low_bat_check()
{
  low_bat_check_start = 0;
}

bool is_enable_low_bat_check()
{
  return low_bat_check;
}

void enable_low_bat_check()
{
  low_bat_check = 1;
}

void disable_low_bat_check()
{
  low_bat_check = 0;
}

void high_level_low_pow_check()
{
  if(is_trigger_low_bat_check()){
    stop_low_bat_check();
    if(is_low_power(THRESHOLD)){
      low_power = 1;
      HalLedBlink(LED_SHUIWEN_INDICATE_BLUE, 10, 50, MS2TICK(1000));
      low_bat_time_ref = clock_time();
    }
  }

  if(low_power){
    if(n_clock_time_exceed(low_bat_time_ref, 10000000))//10S
      low_power = 0;
  }
}

int main(void)
{
  blc_pm_select_internal_32k_crystal();

  cpu_wakeup_init();

  clock_init(SYS_CLK_24M_Crystal);

  gpio_init(1);

  dc_power_on();

  if(!is_wakeup_from_sleep()){
    gpio_key_alloc(key_arry, MAX_GPIO_KEYS);
    register_key(key_enum_arry, MAX_KEYS);

    gpio_led_alloc(led_arry, MAX_GPIO_LEDS);
    register_led(led_enum_arry, MAX_LEDS);
  }

  //init m_key and t_key
  if(!is_wakeup_from_sleep())
    key_init();
  else
    key_wakeup_init();

  key_process(NULL);

  rf_8359_set_tx();

  if(!is_wakeup_from_sleep())
    HalLedInit();

//  ev_on_timer(key_process, NULL, KEY_PROCESS_TIME);

  ev_on_timer(HalLedUpdate, clr_display_flag, LED_UPDATE_PROCESS_TIME);

  if(!is_wakeup_from_sleep())
    id_init();

  //init mempool for key event, the mempool length is equal to (siezof(mem_block_t) + siezeof(event_handler_t))*MAX_EVENT
  mempool_init(&KEY_EVENT_POOL, &KEY_EVENT_POOL_MEM[0], sizeof(mem_block_t) + sizeof(event_handler_t), MAX_EVENT);

  //user should register key event in this function
  app_init();

  if(!is_wakeup_from_sleep())
    idle_time_for_sleep(SLEEP_WAIT_TIME);

  reload_sys_time();

  enable_low_bat_check();

  while(1){
    poll_key_event();

	key_process(NULL);

//	HalLedUpdate(clr_display_flag);

    ev_process_timer();

    high_level_low_pow_check();

    if(poll_idle_time()){
      HalLedSet(HAL_LED_ALL, HAL_LED_MODE_OFF);
      gpio_key_sleep_setup();
      dc_shutdown();
      set_wakeup_flag();
      cpu_sleep_wakeup(DEEPSLEEP_MODE_RET_SRAM_LOW32K, PM_WAKEUP_PAD, 0);
    }
  }
}



#include <stdio.h>
#include "driver/twai.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define GPIO_OUTPUT_PIN_SEL (1ULL<<10)
#define TX_GPIO_NUM 2
#define RX_GPIO_NUM 4

void app_main(void)
{
  twai_general_config_t g_config = TWAI_GENERAL_CONFIG_DEFAULT(TX_GPIO_NUM, RX_GPIO_NUM, TWAI_MODE_NO_ACK);
  twai_timing_config_t t_config = TWAI_TIMING_CONFIG_1MBITS();
  twai_filter_config_t f_config = {.acceptance_code = (0),
      .acceptance_mask = ~(0),
      .single_filter = true};
  ESP_ERROR_CHECK(twai_driver_install(&g_config,&t_config,&f_config));
  ESP_ERROR_CHECK(twai_start());

  gpio_config_t io_conf = {};
  io_conf.intr_type = GPIO_INTR_DISABLE;
  io_conf.mode = GPIO_MODE_OUTPUT;
  io_conf.pin_bit_mask = GPIO_OUTPUT_PIN_SEL;
  io_conf.pull_down_en = 0;
  io_conf.pull_up_en = 0;
  gpio_config(&io_conf);

  int cnt = 0;
  while(1)
  {
    cnt++;
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    gpio_set_level(10,cnt % 2);
  }

}

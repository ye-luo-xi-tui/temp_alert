#include <stdio.h>
#include "driver/twai.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define GPIO_OUTPUT_PIN_SEL ((1ULL<<GPIO_NUM_27)|(1ULL<<GPIO_NUM_26))
#define TX_GPIO_NUM 2
#define RX_GPIO_NUM 4

static bool halted = false;
static int temp = 0;

void tempStatusUpdate(void *n) {
  twai_message_t rx_message;

  while (1) {
    ESP_ERROR_CHECK(twai_receive(&rx_message, portMAX_DELAY));
    if (rx_message.identifier == 0x100) {
      temp = rx_message.data[0];
    }
    if (temp > 50)
      halted = true;
    else {
      printf("current_temp:%d,threshold:%d\n", temp, 50);
      halted = false;
    }
  }
}

void app_main(void) {
  twai_general_config_t g_config = TWAI_GENERAL_CONFIG_DEFAULT(TX_GPIO_NUM, RX_GPIO_NUM, TWAI_MODE_NO_ACK);
  twai_timing_config_t t_config = TWAI_TIMING_CONFIG_1MBITS();
  twai_filter_config_t f_config = {.acceptance_code = (0),
      .acceptance_mask = ~(0),
      .single_filter = true};
  ESP_ERROR_CHECK(twai_driver_install(&g_config, &t_config, &f_config));
  ESP_ERROR_CHECK(twai_start());

  static TaskHandle_t fb_handle;
  xTaskCreate(tempStatusUpdate, "temp_status_update", 4096, NULL, 2 | portPRIVILEGE_BIT, &fb_handle);

  gpio_config_t io_conf = {};
  io_conf.intr_type = GPIO_INTR_DISABLE;
  io_conf.mode = GPIO_MODE_OUTPUT;
  io_conf.pin_bit_mask = GPIO_OUTPUT_PIN_SEL;
  io_conf.pull_down_en = 0;
  io_conf.pull_up_en = 0;
  gpio_config(&io_conf);

  int cnt = 0;
  while (1) {
    cnt++;
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    if (halted) {
      printf("halted!\n");
      ESP_ERROR_CHECK(gpio_set_level(GPIO_NUM_27, 0));
      ESP_ERROR_CHECK(gpio_set_level(GPIO_NUM_26, cnt % 2));
    } else {
      ESP_ERROR_CHECK(gpio_set_level(27, 1));
      ESP_ERROR_CHECK(gpio_set_level(26, 0));
    }
  }

}

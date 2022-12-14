#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/twai.h"
#include "driver/temperature_sensor.h"

#define TX_GPIO_NUM 7
#define RX_GPIO_NUM 6

void initTempSensor(temperature_sensor_handle_t* temp_sensor_handle,int min_temp,int max_temp)
{

  temperature_sensor_config_t temp_sensor_config = TEMPERATURE_SENSOR_CONFIG_DEFAULT(min_temp,max_temp);
  ESP_ERROR_CHECK(temperature_sensor_install(&temp_sensor_config,temp_sensor_handle));
  ESP_ERROR_CHECK(temperature_sensor_enable(*temp_sensor_handle));
}

void app_main(void)
{
  temperature_sensor_handle_t temp_sensor = NULL;
  initTempSensor(&temp_sensor,10,50);
  twai_general_config_t g_config = TWAI_GENERAL_CONFIG_DEFAULT(TX_GPIO_NUM, RX_GPIO_NUM, TWAI_MODE_NO_ACK);
  twai_timing_config_t t_config = TWAI_TIMING_CONFIG_1MBITS();
  twai_filter_config_t f_config = {.acceptance_code = (0),
      .acceptance_mask = ~(0),
      .single_filter = true};
  ESP_ERROR_CHECK(twai_driver_install(&g_config,&t_config,&f_config));
  ESP_ERROR_CHECK(twai_start());
  while(1)
  {
    float tsens_value;
    ESP_ERROR_CHECK(temperature_sensor_get_celsius(temp_sensor,&tsens_value));
    twai_message_t tx_msg = {.data_length_code=1,.identifier=0x100,.self=0,.extd=0};
    printf("temp:%f\n",tsens_value);
    tx_msg.data[0] = (int)tsens_value;
    ESP_ERROR_CHECK(twai_transmit(&tx_msg,portMAX_DELAY));
    vTaskDelay(pdMS_TO_TICKS(1));
  }
}

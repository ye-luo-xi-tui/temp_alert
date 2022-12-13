#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/twai.h"
#include "driver/temperature_sensor.h"

void initTempSensor(temperature_sensor_handle_t* temp_sensor_handle,double min_temp,double max_temp)
{

  temperature_sensor_config_t temp_sensor_config = TEMPERATURE_SENSOR_CONFIG_DEFAULT(min_temp,max_temp);
  ESP_ERROR_CHECK(temperature_sensor_install(&temp_sensor_config,temp_sensor_handle));
  ESP_ERROR_CHECK(temperature_sensor_enable(*temp_sensor_handle));
}

void app_main(void)
{
  temperature_sensor_handle_t temp_sensor = NULL;
  initTempSensor(&temp_sensor,10,50);
}

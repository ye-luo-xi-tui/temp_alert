#include <stdio.h>
#include "driver/twai.h"
#include "temp.h"

void app_main(void)
{
  gpioInit();

  printf("temp: %d",Ds18b20ReadTemp());

}

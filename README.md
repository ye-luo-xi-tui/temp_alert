# temp_alert
## 简介
首先声明，这是我的一个大作业，用了两块esp32。一块烧录**temp_node**，一块烧录**alert_node**。temp_node的功能是读取板子上自带的温度传感器数据，通过can总线发送出去。alert_node的功能是读取温度，判断是否过热。如果过热则报警。
## 构建与部署
本项目基于**ESP-IDF**开发。编译、烧录等详情可参阅https://docs.espressif.com/projects/esp-idf/zh_CN/stable/esp32/get-started/windows-setup.html#get-started-windows-first-steps

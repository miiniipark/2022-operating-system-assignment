cmd_/home/pi/hw5/modules.order := {   echo /home/pi/hw5/motor_driver.ko; :; } | awk '!x[$$0]++' - > /home/pi/hw5/modules.order

# PowerSupplyRaspberryPi3
MSP430 control the power supply for the Raspberry Pi3 B+. It wait until RPi to shutdown completely and cut the supply power\n
_Raspberry must also be configured: 1)Install gpio-startup-service.sh into /etc/profile.d to make sure it run when RPi is turned on\n
2)Install user-shutdown-service.sh as the link below. Noted that "systemclt daemon reload" instead of "systemd..."\n
Noted: the .service file is also available in this repository.\n
Reference :\n
https://www.golinuxcloud.com/run-script-with-systemd-before-shutdown-linux/ \n
Thank you

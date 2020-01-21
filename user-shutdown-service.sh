#!/bin/sh

echo 24 > /sys/class/gpio/export
echo out > /sys/class/gpio/gpio24/direction
echo 1 > /sys/class/gpio/gpio24/value

echo 0 > /sys/class/gpio/gpio23/value
echo 23 > /sys/class/gpio/unexport

#!/bin/sh

echo 950000 > /sys/kernel/debug/regulator/vdd_npu_s0/voltage
echo performance > /sys/devices/platform/fb000000.gpu/devfreq/fb000000.gpu/governor
echo performance > /sys/devices/system/cpu/cpufreq/policy0/scaling_governor        
echo performance > /sys/devices/system/cpu/cpufreq/policy4/scaling_governor        
echo performance > /sys/devices/system/cpu/cpufreq/policy6/scaling_governor        

/system/bin/log -t "Svep" "CPU/GPU set performance complete!"

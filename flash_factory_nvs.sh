#!/bin/sh

idx=$1
if [ "$idx" = "" ]; then
    echo "Please, provide device index"
    exit 1
fi

if [ "$2" = "old" ]; then
${IDF_PATH}/components/nvs_flash/nvs_partition_generator/nvs_partition_gen.py \
      devices/device-${idx}.csv device-${idx}.bin 0x6000
else
${IDF_PATH}/components/nvs_flash/nvs_partition_generator/nvs_partition_gen.py \
      --input devices/device-${idx}.csv --output device-${idx}.bin --size 0x6000
fi

if [ $? -ne 0 ]; then
    echo "Failed to generate NVS partition data"
    exit 1
fi
if [ "$2" = "old" ]; then
${IDF_PATH}/components/esptool_py/esptool/esptool.py --port /dev/ttyS6 write_flash 0x12000 device-${idx}.bin
else
${IDF_PATH}/components/esptool_py/esptool/esptool.py --port /dev/ttyUSB0 write_flash 0x12000 device-${idx}.bin
fi
if [ $? -ne 0 ]; then
    echo "Failed to flash NVS partition data"
    exit 1
fi
rm device-${idx}.bin

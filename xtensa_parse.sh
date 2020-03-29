#!/bin/sh


if [ "$1" = "" ]; then
    echo "Usage: xtensa_parse.sh file.out"
    exit 1
fi

xtensa-esp32-elf-nm --size-sort -C -r $1 >elfdump.txt
xtensa-esp32-elf-objdump -C -d $1 >elfdisasm.txt
xtensa-esp32-elf-size $1 > elfsize.txt
cat elfsize.txt

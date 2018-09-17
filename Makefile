#
# This is a project Makefile. It is assumed the directory this Makefile resides in is a
# project subdirectory.
#

PROJECT_NAME := nixie_clock
EXTRA_COMPONENT_DIRS := ./components
# COMPONENTS=freertos esp32 newlib esptool_py nvs_flash spi_flash log tcpip_adapter lwip main xtensa-debug-module driver bt

include $(IDF_PATH)/make/project.mk


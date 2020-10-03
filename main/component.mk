#
# "main" pseudo-component makefile.
#
# (Uses default behaviour of compiling all source files in directory, adding 'include' to include path.)

# COMPONENT_EMBED_FILES := server_root_cert.pem
# COMPONENT_EMBED_TXTFILES.
# extern const uint8_t server_root_cert_pem_start[] asm("_binary_server_root_cert_pem_start");
# extern const uint8_t server_root_cert_pem_end[]   asm("_binary_server_root_cert_pem_end");

COMPONENT_ADD_INCLUDEDIRS := .
COMPONENT_SRCDIRS := . ./states ./tubes ./platform ./bluetooth ./controllers ./hardware
COMPONENT_EMBED_TXTFILES := html/index.html \
    html/debug.html \
    html/login.html \
    html/styles.css \
    certs/cacert.pem \
    certs/prvtkey.pem \

ifeq ($(wildcard $(PROJECT_PATH)/main/certs/cacert.pem),)
$(shell mkdir -p $(PROJECT_PATH)/main/certs)
$(shell openssl req -newkey rsa:2048 -nodes -keyout $(PROJECT_PATH)/main/certs/prvtkey.pem -x509 -days 3650 -out $(PROJECT_PATH)/main/certs/cacert.pem -subj "/CN=ESP32 Nixie Clock")
endif

COMPONENT_EMBED_FILES := html/favicon.ico \
    sound/wicked_child.vgm \
    sound/vampire_killer.vgm \
    sound/cave_explorer.vgm \
    sound/ice_path.vgm  \
    sound/mario.nsf  \
    sound/mario3.nsf  \
    sound/crisis_force.nsf \
    sound/castlevania3.nsf \
    sound/contra.nsf \
    sound/tinytoon.nsf \
    sound/bucky_ohare.nsf \

#    sound/crysis_force.vgm \
#    sound/castle_beginning.vgm \

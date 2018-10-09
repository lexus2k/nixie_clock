#include "tftp_ota_server.h"

extern "C" void start_tftp(void)
{
    TftpOtaServer server;
    server.start();
}
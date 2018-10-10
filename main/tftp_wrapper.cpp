#include "tftp_ota_server.h"

TftpOtaServer server;

extern "C" void start_tftp(void)
{
    if (server.start()<0)
    {
        return;
    }
}

extern "C" void run_tftp(void)
{
    while (server.run()>=0)
    {
    }
}

extern "C" void stop_tftp(void)
{
    server.stop();
}
#pragma once


#include "http_applet_engine.h"
#include <stdint.h>
#include <vector>
#include <map>
#include <string>

typedef int (*TPReadHandler)(const char *name, char *buffer, int max_len, void *user_data);

class TextProcessor
{
public:
    TextProcessor();

    ~TextProcessor();

    void setText(const char *content, int len = -1);

    int getNextBlock(char *outBuffer, int maxSize);

    void registerHandler( const char *name, TPReadHandler handler, void *userData );

private:
    struct Handler
    {
        TPReadHandler handler;
        void *userData;
    };
    const char *m_content = nullptr;
    int m_len;
    std::string m_prefix = "[%";
    std::string m_suffix = "%]";
    std::map<std::string, TextProcessor::Handler> m_handlers;
};


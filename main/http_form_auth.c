#include "http_form_auth.h"
#include "platform/system.h"

#include <esp_log.h>
#include <esp_system.h>
#include <mbedtls/base64.h>
#include <mbedtls/md.h>

#include <ctype.h>
#include <stdio.h>

static const char *TAG="FORM_AUTH";

static uint64_t s_last_access_ts = 0;
static uint32_t s_salt = 0;

// 1 if session is valid, otherwise 0
int form_auth_server_generate_token(char *token, int max_len, const char *user, const char *password)
{
    char hash[MBEDTLS_MD_MAX_SIZE];
    const mbedtls_md_info_t *md_info4 = mbedtls_md_info_from_type(MBEDTLS_MD_SHA256);
    if ( !s_salt ) s_salt = rand();
    if (md_info4 == NULL)
    {
        ESP_LOGE(TAG, "Failed to run sha256 algo");
        return 0;
    }
    mbedtls_md_context_t ctx4;
    mbedtls_md_init(&ctx4);
    int ret4 = mbedtls_md_setup(&ctx4, md_info4, 0);
    if (ret4 != 0)
    {
        ESP_LOGE(TAG, "Failed to run sha256 algo");
        return 0;
    }
    if ( (uint64_t)(micros64() - s_last_access_ts) > 5*60000000 ) // 5 minutes timeout for web access
    {
        s_salt = rand();
    }
    s_last_access_ts = micros64();
    mbedtls_md_starts(&ctx4);
    mbedtls_md_update(&ctx4, (const unsigned char *)&s_salt, sizeof(s_salt));
    mbedtls_md_update(&ctx4, (const unsigned char *)user, strlen(user));
    mbedtls_md_update(&ctx4, (const unsigned char *)":", 1);
    mbedtls_md_update(&ctx4, (const unsigned char *)password, strlen(password));
    mbedtls_md_finish(&ctx4, (unsigned char *)hash);
    mbedtls_md_free(&ctx4);
    size_t len;
    mbedtls_base64_encode( (unsigned char *)token, max_len, &len, (const unsigned char *)hash, mbedtls_md_get_size(md_info4));
    return len;
}

uint8_t form_auth_server_validate_token(const char *token, const char *user, const char *password)
{
    char hash[MBEDTLS_MD_MAX_SIZE*2] = { 0 };
    int len = form_auth_server_generate_token( hash, sizeof(hash), user, password );
    if (len && !strncmp(token, hash, len))
    {
        s_last_access_ts = micros64();
        return 1;
    }
    s_salt = rand();
    return 0;
}


#include "http_digest.h"
#include <mbedtls/base64.h>
#include <mbedtls/md.h>
#include <mbedtls/md5.h>

#include <esp_log.h>

#include <esp_ota_ops.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>


static const char *TAG="DIGEST";

static unsigned int hex_char_to_uint(char hex)
{
    if (hex <= '9') return hex - '0';
    if (hex <='F') return hex - 'A' + 10;
    return hex - 'a' + 10;
}

static char hex_to_char( unsigned char hex )
{
    if ( hex < 10 ) return '0' + hex;
    return 'a' + hex - 10;
}

static int binary_to_hex_str(unsigned char *input, int len, char *output, int max_len)
{
    if ( max_len < len * 2 ) return 0;
    for(int i=0; i<len; i++)
    {
        output[i*2] = hex_to_char( input[i] >> 4 );
        output[i*2+1] = hex_to_char( input[i] & 0x0F );
    }
    return len * 2;
}

/**
 * Returns length of generated hash string
 */
int digest_server_generate_md5_hash(char *token, int max_len, const char *data, int data_len)
{
    unsigned char hash[16];
    mbedtls_md5_context ctx;
    mbedtls_md5_init( &ctx );
    mbedtls_md5_starts_ret( &ctx );
    mbedtls_md5_update_ret( &ctx, (const unsigned char *)data, strlen(data) );
    mbedtls_md5_finish_ret( &ctx, hash );
    mbedtls_md5_free( &ctx );
    return binary_to_hex_str( hash, 16, token, max_len );
}

static int get_token_unquoted_value(const char *str, const char *templ,
                                    const char **token, const char *delim)
{
        //ESP_LOGI( TAG, "Cookie: %s", session );
    const char *value = strstr( str, templ );
    if ( value == NULL )
    {
        return -1;
    }
    value += strlen(templ);
    const char *endp = str + strlen(str);
    while (*delim)
    {
        const char *temp = strchr( value, *delim );
        if (temp == NULL) temp = endp;
        if (temp < endp) endp = temp;
        delim++;
    }
    endp--;
    if (*value == '"') value++;
    if (*endp == '"') endp--;
    *token = value;
    return endp - value + 1;
}

int digest_server_calc_ha1(char *buffer, int max_len,
                           const char *expected_realm, int realm_len,
                           const char *expected_user, int user_len,
                           const char *expected_password, int password_len)
{
    char *ha1 = (char *)malloc( 256 );
    if ( ha1 == NULL || buffer == NULL )
    {
        return -1;
    }
    if ( realm_len < 0 ) realm_len = strlen( expected_realm );
    if ( user_len < 0 ) user_len = strlen( expected_user );
    if ( password_len < 0 ) password_len = strlen( expected_password );
//    HA1 = MD5(username_str:realm:passwd)
    ha1[0] = '\0';
    strncat( ha1, expected_user, (user_len < 255 - strlen( ha1 )) ? user_len: (255 - strlen( ha1 )) );
    strncat( ha1, ":", 255 - strlen( ha1 ) );
    strncat( ha1, expected_realm, (realm_len < 255 - strlen( ha1 )) ? realm_len: (255 - strlen( ha1 )) );
    strncat( ha1, ":", 255 - strlen( ha1 ) );
    strncat( ha1, expected_password, (password_len < 255 - strlen( ha1 )) ? password_len: (255 - strlen( ha1 )) );
    ESP_LOGD( TAG, "HA1=\"%s\"", ha1 );
    int ha1_len = digest_server_generate_md5_hash( ha1, 256, ha1, strlen(ha1) );
    ESP_LOGD( TAG, "HA1=\"%.*s\"", ha1_len, ha1 );
    if ( max_len + 1 > ha1_len )
    {
        buffer[0] = '\0';
        strncat( buffer, ha1, ha1_len );
    }
    else
    {
        ha1_len = -1;
    }
    free( ha1 );
    return ha1_len;
}

bool digest_server_validate_session_ha1( const char *authorization, const char *method, const char *expected_ha1 )
{
    ESP_LOGD( TAG, "AUTH: %s", authorization );
    const char *qop_str;
    int qop_len = get_token_unquoted_value( authorization, "qop=", &qop_str, "," );
    const char *nc_str;
    int nc_len = get_token_unquoted_value( authorization, "nc=", &nc_str, "," );
    const char *nonce_str;
    int nonce_len = get_token_unquoted_value( authorization, "nonce=", &nonce_str, "," );
    const char *cnonce_str;
    int cnonce_len = get_token_unquoted_value( authorization, "cnonce=", &cnonce_str, "," );
    const char *opaque_str;
    int opaque_len = get_token_unquoted_value( authorization, "opaque=", &opaque_str, "," );
    const char *response_str;
    int response_len = get_token_unquoted_value( authorization, "response=", &response_str, "," );
    const char *uri_str;
    int uri_len = get_token_unquoted_value( authorization, "uri=", &uri_str, "," );
    const char *algorithm_str;
    int algorithm_len = get_token_unquoted_value( authorization, "algorithm=", &algorithm_str, "," );

    if ( algorithm_len > 0 )
    {
        if ( strncmp("MD5", algorithm_str, algorithm_len) )
        {
            ESP_LOGE( TAG, "Algorithm not supported: %.*s", algorithm_len, algorithm_str );
            return false;
        }
    }

    char *ha2 = (char *)malloc( 256 );
    char *resp = (char *)malloc( 256 );
//    HA2 = MD5(GET/POST:uri)
    strcpy( ha2, method );
    strcat( ha2, ":" );
    strncat( ha2, uri_str, uri_len );
    ESP_LOGD( TAG, "HA2=\"%s\"", ha2 );
    int ha2_len = digest_server_generate_md5_hash( ha2, 100, ha2, strlen(ha2) );
    ESP_LOGD( TAG, "HA2=\"%.*s\"", ha2_len, ha2 );
//    RESP = MD5(HA1:nonce:nc:cnonce:qop:HA2)
    resp[0] = '\0';
    strncat( resp, expected_ha1, 255 - strlen(resp) );
    strcat( resp, ":" );
    strncat( resp, nonce_str, nonce_len );
    if ( qop_len > 0 )
    {
        strcat( resp, ":" );
        strncat( resp, nc_str, nc_len );
        strcat( resp, ":" );
        strncat( resp, cnonce_str, cnonce_len );
        strcat( resp, ":" );
        strncat( resp, qop_str, qop_len );
    }
    strcat( resp, ":" );
    strncat( resp, ha2, ha2_len );
    ESP_LOGD( TAG, "RESP=\"%s\"", resp );
    int resp_len = digest_server_generate_md5_hash( resp, 100, resp, strlen(resp) );
    free( ha2 );
    if ( resp_len != response_len || strncmp(resp, response_str, resp_len) )
    {
        ESP_LOGE( TAG, "RESP: calc=%.*s, resp_str=%.*s", resp_len, resp, response_len, response_str );
        free( resp );
        return false;
    }
    free( resp );
    return true;
}

bool digest_server_validate_session_creds( const char *authorization, const char *method,
                                    const char *expected_realm,
                                    const char *expected_user,
                                    const char *expected_password )
{
    const char *username_str;
    int username_len = get_token_unquoted_value( authorization, "username=", &username_str, "," );
    const char *realm_str;
    int realm_len = get_token_unquoted_value( authorization, "realm=", &realm_str, "," );
    if ( username_len < 0 || realm_len < 0 )
    {
        ESP_LOGE( TAG, "Auth request doesn't contain username/realm" );
        return false;
    }
    if ( strncmp(expected_user, username_str, username_len ) )
    {
        ESP_LOGE( TAG, "Wrong user" );
        return false;
    }
    if ( strncmp(expected_realm, realm_str, realm_len ) )
    {
        ESP_LOGE( TAG, "Wrong realm" );
        return false;
    }
    char *ha1 = (char *)malloc( 256 );
    if ( ha1 == NULL )
    {
        return false;
    }
    int ha1_len = digest_server_calc_ha1( ha1, 256, expected_realm, -1,
                                                    expected_user, -1,
                                                    expected_password, -1 );
    if ( ha1_len < 0 )
    {
        free( ha1 );
        return false;
    }
    bool result = digest_server_validate_session_ha1( authorization, method, ha1 );
    free( ha1 );
    return result;
}

int digest_server_get_user( char *buffer, int max_len, const char *authorization )
{
    const char *username_str;
    int username_len = get_token_unquoted_value( authorization, "username=", &username_str, "," );
    if ( username_len > 0 )
    {
        buffer[0] = '\0';
        strncat( buffer, username_str, max_len < username_len ? max_len : username_len );
    }
    return username_len;
}

void digest_server_www_auth_request(char *buffer, int max_len, const char *realm)
{
    buffer[0] = '\0';
    strncat( buffer, "Digest realm=\"", max_len - strlen(buffer) - 1 );
    strncat( buffer, realm, max_len - strlen(buffer) - 1 );
    strncat( buffer, "\",qop=\"auth\",nonce=\"dcd98b7102dd2f0e8b11d0f600bfb0c093\",opaque=\"5ccc069c403ebaf9f0171e9517f40e41\"", max_len - strlen(buffer) - 1 );
}

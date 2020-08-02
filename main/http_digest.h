#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>

int digest_server_generate_md5_hash(char *token, int max_len, const char *data, int data_len);
int digest_server_calc_ha1(char *buffer, int max_len,
                           const char *expected_realm, int realm_len,
                           const char *expected_user, int user_len,
                           const char *expected_password, int password_len);
bool digest_server_validate_session_ha1( const char *authorization, const char *method, const char *expected_ha1 );
bool digest_server_validate_session_creds( const char *authorization, const char *method,
                                           const char *expected_realm,
                                           const char *expected_user,
                                           const char *expected_password );
int digest_server_get_user( char *buffer, int max_len, const char *authorization );
void digest_server_www_auth_request(char *buffer, int max_len, const char *realm);


#ifdef __cplusplus
}
#endif

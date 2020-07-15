#include "../build_revision"
#include <stdlib.h>

#define FW_VERSION "1.0." BUILD_REVISION

static inline bool is_version_newer(const char *version)
{
    const char *current = FW_VERSION;
    char *p1;
    char *p2;
    // current major
    int current_major = strtoul( current, &p1, 10 );
    int new_major = strtoul( version, &p2, 10 );
    if ( current_major < new_major ) return true;
    if ( current_major > new_major ) return false;
    int current_minor = strtoul( p1+1, &p1, 10 );
    int new_minor = strtoul( p2+1, &p2, 10 );
    if ( new_minor > current_minor ) return true;
    if ( new_minor < current_minor ) return false;
    int current_build = strtoul( p1+1, NULL, 10 );
    int new_build = strtoul( p2+1, NULL, 10 );
    return new_build > current_build;
}

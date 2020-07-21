#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>

/**
 * Struct describes single parameter, processed by http engine.
 */
typedef struct
{
    const char *name; // Can be empty to process params by default if no other handler is invoked
    /**
     * Callback when write parameter is requested.
     * @param name name of the parameter, for applet params with name specified, this is alway name content
     * @param buffer pointer to the buffer, containing data to write to the parameter
     * @param len length of data in buffer
     * @param user_data user data provided during engine initialization.
     * @return callback should return negative value in case of error
     */
    int (*cb_write)(const char *name, const char *buffer, int len, void *user_data);
    /**
     * Callback when engine reads parameter.
     * @param name name of the parameter, for applet params with name specified, this is alway name content
     * @param buffer pointer to the buffer to store read date to.
     * @param max_len  maximum length of buffer in bytes.
     * @param user_data user data provided during engine initialization.
     * @return callback should return negative value in case of error
     *         if 0 is returned, engine uses strlen to detect length of useful data
     */
    int (*cb_read)(const char *name, char *buffer, int max_len, void *user_data);
} applet_param_t;

/**
 * Applet engine struct
 */
typedef struct
{
    // Internal below
    void *user_data; ///< user data provided in applet_engine_init() function
    const char *source_buffer; ///< pointer to html data being process, set by applet_engine_set_html
    const applet_param_t *params; ///< pointer to applet params, set by applet_engine_set_params
    const char *position; ///< pointer to the character being processed by engine in html source page
    const char *end; ///< pointer to last character in html source page
    bool user_condition_true; ///< internal variable, used for loop cycles
    const char *start_loop_position; ///< pointer to character in html source page pointing to start of loop entity
} applet_engine_t;

/**
 * Initializes engine.
 */
bool applet_engine_init(applet_engine_t *engine, void *user_data);

/**
 * Sets html page for processing
 */
void applet_engine_set_html(applet_engine_t *engine, const char *html, int len);

/**
 * Executes read operation for specified parameter
 */
int applet_engine_read_var(applet_engine_t *engine, const char *varname, char *buffer, int max_len);

/**
 * Executes write operation for specified parameter
 */
int applet_engine_write_var(applet_engine_t *engine, const char *varname, const char *buffer, int len);

/**
 * Sets applets parameters for processing. This must be pointer to array, which last element has zero content.
 * @important applet array is not copied by engine, and must exist all time while engine works
 */
void applet_engine_set_params(applet_engine_t *engine, const applet_param_t *params);

/**
 * Processes next portion of html data
 * @return negative value if parse error happened. In this case subsequent calls will cause the same error.
 *         0 if nothing to process any more
 *         position value - number of bytes written to output buffer.
 */
int applet_engine_process(applet_engine_t *engine, char *buffer, int buffer_size);

/**
 * Stops applet engine
 */
void applet_engine_close(applet_engine_t *engine);

#ifdef __cplusplus
}

#define APPLET_INLINE_R(x) [](const char *name, char *value, int max_len, void *user_data)->int { \
                            x; }

#define APPLET_INLINE_W(x) [](const char *name, const char *value, int len, void *user_data)->int { \
                            x; }

#endif

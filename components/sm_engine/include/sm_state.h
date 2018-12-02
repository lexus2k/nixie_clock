#pragma once

#include <stdint.h>

class SmEngine2;

typedef struct
{
    uint8_t event;
    uintptr_t arg;
} SEventData;

class SmState
{
    friend class SmEngine2;
public:
    SmState(const char *name);

    virtual bool begin();
    virtual void end();

    virtual void enter();
    virtual void run();
    virtual void exit();
    virtual bool on_event(SEventData event);

protected:
    void switch_state(uint8_t new_state);
    const char *get_name() { return m_name; }

    template <typename T>
    T &get_engine() { return *static_cast<T*>(m_engine); }

    virtual uint8_t get_id() = 0;

private:
    const char * m_name=nullptr;
    SmState *m_next = nullptr;
    SmEngine2 *m_engine = nullptr;
    void set_engine( SmEngine2 &engine );
};

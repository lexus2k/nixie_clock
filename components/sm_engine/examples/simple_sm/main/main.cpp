#include "sm_engine2.h"
#include "sm_state.h"
#include <stdio.h>

enum
{
    MY_STATE_ID,
};

class MyState: public SmState
{
public:
    MyState(): SmState("state") {}

    void enter()
    {
        printf("Enter\n");
    }

    void run()
    {
        printf("Run state\n");
    }

    void exit()
    {
        printf("Exit\n");
    }

protected:
    uint8_t get_id()
    {
        return MY_STATE_ID;
    }
};

SmEngine2 sm;
MyState my_state;



extern "C" void app_main()
{
    sm.add_state( my_state );
    sm.begin();
    sm.switch_state( MY_STATE_ID );
    sm.run( 100 );
    sm.end();
}


#include "sm_engine.h"
#include <stdio.h>

enum
{
    MY_STATE_ID,
};

class MyState: public SmState
{
public:
    MyState(): SmState("state") { }

    bool begin() override { printf("Begin\n"); return true; }

    void enter() override { printf("Enter\n"); }

    void update() override { printf("Run state\n"); }

    void exit() override { printf("Exit\n"); }

    void end() override { printf("end\n"); }
};

SmEngine sm;

extern "C" void app_main()
{
    // at this step state machine automatically creates class of MyState type
    // and assigns specified id. Remember, all auto allocated objects, will be
    // destroyed, when state machine is destroyed.
    sm.add_state<MyState>( MY_STATE_ID );
    // This will call begin method for all registered states
    sm.begin();
    // This will call MyState::enter() method call
    sm.switch_state( MY_STATE_ID );
    // run() method will run infinite loop until state machine is stopped
    // in this infinite loop, it will call update() method of MyState every 100 milliseconds
    sm.loop( 100 );
    // This will call end methods for all registered states
    sm.end();
}


#pragma once

#include "sme_config.h"

#if SM_ENGINE_USE_STL

#include <stack>
namespace sme {

template <typename T>
using stack = std::stack<T>;

}
#else

namespace sme {

static constexpr int MAX_STACK_EL = 4;

template <typename T>
class stack
{
public:
    stack() {}

    void push( T e ) { if ( m_ptr < MAX_STACK_EL - 1) m_elem[++m_ptr] = e; }

    void pop() { if ( m_ptr >=0 ) m_ptr--; }

    T &top() { return m_ptr < 0 ? m_elem[0] : m_elem[m_ptr]; }

    bool empty() { return m_ptr < 0; }
private:
    T m_elem[MAX_STACK_EL];
    int m_ptr = -1;
};

}
#endif

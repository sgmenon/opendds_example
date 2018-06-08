#ifndef __SHARED_INPORTINTERFACE_
#define __SHARED_INPORTINTERFACE_

namespace PortInterfaces {
    template <class T>
    class InPortInterface {
    public:
        InPortInterface() {}
        virtual ~InPortInterface() {}
        virtual void messageReceive(T&) = 0; // Async recv   
    };
}
#endif /*__SHARED_INPORTINTERFACE_*/
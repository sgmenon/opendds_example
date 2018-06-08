
#ifndef __DDSEXAMPLE_PUBLISHER_
#define __DDSEXAMPLE_PUBLISHER_

#include "StockQuoterC.h"
#include "ExchangeEventInPort.h"
#include "QuoteInPort.h"
#include <type_traits>

namespace SlMdl {

    bool is_exchange_closed();

    class Subscriber {
        // public data and function members
    public:
        // model initialize function
        void initialize();

        // model step function
        void step();

        // model terminate function
        void terminate();

        //Simulink functions
        void on_exchange_event_recv(StockQuoter::ExchangeEvent&);
        void on_quote_event_recv(StockQuoter::Quote&);
        
        // Constructor
        Subscriber();

    private:
        StockQuoter::ExchangeEventInPort ex_port;
        StockQuoter::QuoteInPort q_port;
    };
}
#endif /*__DDSEXAMPLE_PUBLISHER_*/

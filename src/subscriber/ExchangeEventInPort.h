
#ifndef __EXCHANGEEVENT_IN_PORT
#define __EXCHANGEEVENT_IN_PORT

#include <dds/DdsDcpsSubscriptionC.h>

#include "InPortInterface.hpp"
#include "ddsContext.hpp"
#include <functional>

namespace SlMdl {
	class Subscriber;
}

namespace StockQuoter {
    struct ExchangeEvent;

    class ExchangeEventInPort
        : public PortInterfaces::InPortInterface<ExchangeEvent>
    {
    public:
        ExchangeEventInPort();

        virtual ~ExchangeEventInPort();

        void init(const SlMdl::Subscriber&);
        void term();

        virtual void messageReceive(ExchangeEvent&) override;  

    private:
        std::shared_ptr<MWOpenDDS::context> ctx;
        DDS::Topic_var topic;
        DDS::DataReader_var datareader;
        DDS::DataReaderListener_var qos_listener;
        std::function<void(ExchangeEvent&)> callback;
    };
}

#endif /* __EXCHANGEEVENT_IN_PORT  */

#ifndef __DDSEXAMPLE_DDSCONTEXT_
#define __DDSEXAMPLE_DDSCONTEXT_

#include "dds/DdsDcpsPublicationC.h"
#include "dds/DdsDcpsSubscriptionC.h"
#include "dds/DdsDcpsDomainC.h"

#include <memory>

namespace MWOpenDDS {
    enum class RequestType {
        GET,
        SET,
        CLEAR
    };
    struct context {
        ~context();
        static std::shared_ptr<context> getInstance(RequestType requestType = RequestType::GET, int argc = 0, char *argv[] = nullptr);

        //DDS Specific Properties
        DDS::DomainParticipantFactory_var dpf;
        DDS::DomainParticipant_var participant;
        DDS::Publisher_var pub;
        DDS::Subscriber_var sub;
    private:
        context(int argc, char *argv[]);
        context(const context&) = delete;
        void operator=(const context&) = delete;
    };
}
#endif /*__DDSEXAMPLE_DDSCONTEXT_*/
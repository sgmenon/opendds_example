#include "ddsContext.hpp"

#include <dds/DdsDcpsInfrastructureC.h>

#include <dds/DCPS/Service_Participant.h>
#include <dds/DCPS/Marked_Default_Qos.h>
#include <dds/DCPS/StaticIncludes.h>

#include <iostream>
#include <assert.h>

namespace {
    // constants for Stock Quoter domain Id, types, and topic
    const DDS::DomainId_t QUOTER_DOMAIN_ID = 1066;
}
namespace MWOpenDDS {
    std::shared_ptr<context> context::getInstance(RequestType requestType, int argc, char *argv[]) {
        static std::shared_ptr<context> instance;
        if (requestType == RequestType::SET) {
            assert(argv != nullptr);
            instance.reset(new context(argc, argv));
        }
        else if (requestType == RequestType::CLEAR) {
            instance.reset();
        }
        return instance;
    }
    context::context(int argc, char *argv[])
        : dpf{ DDS::DomainParticipantFactory::_nil() }
        , participant{ DDS::DomainParticipant::_nil() }
    {
        // Initialize, and create a DomainParticipant
        dpf = TheParticipantFactoryWithArgs(argc, argv);

        participant = dpf->create_participant(
            QUOTER_DOMAIN_ID,
            PARTICIPANT_QOS_DEFAULT,
            DDS::DomainParticipantListener::_nil(),
            OpenDDS::DCPS::DEFAULT_STATUS_MASK);
        if (CORBA::is_nil(participant.in()))
        {
            std::cerr << "create_participant failed." << std::endl;
            ACE_OS::exit(1);
        }

        pub = participant->create_publisher(
            PUBLISHER_QOS_DEFAULT,
            DDS::PublisherListener::_nil(),
            OpenDDS::DCPS::DEFAULT_STATUS_MASK);
        if (CORBA::is_nil(pub.in())) {
            std::cerr << "create_subscriber failed." << std::endl;
            ACE_OS::exit(1);
        }

        sub = participant->create_subscriber(SUBSCRIBER_QOS_DEFAULT,
                DDS::SubscriberListener::_nil(),
                ::OpenDDS::DCPS::DEFAULT_STATUS_MASK);
        if (CORBA::is_nil(sub.in())) {
            std::cerr << "create_subscriber failed." << std::endl;
            ACE_OS::exit(1);
        }
    }
    context::~context() {
        // Cleanup
        try {
            if (!CORBA::is_nil(participant.in())) {
                participant->delete_contained_entities();
            }
            if (!CORBA::is_nil(dpf.in())) {
                dpf->delete_participant(participant.in());
            }
        }
        catch (CORBA::Exception& e) {
            std::cerr << "Exception caught in cleanup."
                << std::endl
                << e << std::endl;
            ACE_OS::exit(1);
        }
        TheServiceParticipant->shutdown();
    }
}

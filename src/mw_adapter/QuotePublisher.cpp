#include "QuoteOutPort.hpp"

#include <dds/DdsDcpsInfrastructureC.h>

#include <dds/DCPS/Service_Participant.h>
#include <dds/DCPS/Marked_Default_Qos.h>
#include <dds/DCPS/PublisherImpl.h>
#include <dds/DCPS/StaticIncludes.h>

#include "StockQuoterTypeSupportImpl.h"

namespace {
    const char* QUOTER_QUOTE_TYPE = "Quote Type";
    const char* QUOTER_QUOTE_TOPIC = "Stock Quotes";
}

namespace StockQuoter {
    void QuoteOutPort::init()
    {
        try {
            auto ctx = MWOpenDDS::context::getInstance();
            auto &participant = ctx->participant;
            auto & pub = ctx->pub;

            // Register the Quote type
            QuoteTypeSupport_var quote_servant
                = new QuoteTypeSupportImpl();

            if (DDS::RETCODE_OK !=
                quote_servant->register_type(participant.in(),
                    QUOTER_QUOTE_TYPE))
            {
                std::cerr << "register_type for " << QUOTER_QUOTE_TYPE
                    << " failed." << std::endl;
                ACE_OS::exit(1);
            }

            // Create a topic for the Quote type...
            topic = participant->create_topic(QUOTER_QUOTE_TOPIC,
                    QUOTER_QUOTE_TYPE,
                    TOPIC_QOS_DEFAULT,
                    DDS::TopicListener::_nil(),
                    OpenDDS::DCPS::DEFAULT_STATUS_MASK);

            if (CORBA::is_nil(topic.in()))
            {
                std::cerr << "create_topic for "
                    << QUOTER_QUOTE_TOPIC
                    << " failed." << std::endl;
                ACE_OS::exit(1);
            }

            // Create a DataWriter for the Quote topic
            DDS::DataWriter_var quote_base_dw =
                pub->create_datawriter(topic.in(),
                    DATAWRITER_QOS_DEFAULT,
                    DDS::DataWriterListener::_nil(),
                    OpenDDS::DCPS::DEFAULT_STATUS_MASK);

            if (CORBA::is_nil(quote_base_dw.in()))
            {
                std::cerr << "create_datawriter for "
                    << QUOTER_QUOTE_TOPIC
                    << " failed." << std::endl;
                ACE_OS::exit(1);
            }

            datawriter = QuoteDataWriter::_narrow(quote_base_dw.in());
            if (CORBA::is_nil(datawriter.in()))
            {
                std::cerr << "QuoteDataWriter could not be narrowed"
                    << std::endl;
                ACE_OS::exit(1);
            }

            // Register the Exchange Event and the two
            // Quoted securities (SPY and MDY) with the
            // appropriate data writer
            /*
            We have two Quote instances, for tickers symbols "SPY"
            (S&P Depository Receipts, i.e. the S&P 500) and "MDY"
            (S&P MidCap Depository Receipts, i.e. the S&P MidCap 400).S
            */

            Quote spy;
            spy.ticker = CORBA::string_dup("SPY");
            handles["SPY"] = datawriter->register_instance(spy);

            Quote mdy;
            mdy.ticker = CORBA::string_dup("MDY");
            handles["MDY"] = datawriter->register_instance(mdy);

        }
        catch (CORBA::Exception& e) {
            std::cerr << "Exception caught in main.cpp:" << std::endl
                << e << std::endl;
            ACE_OS::exit(1);
        }
    }

    void QuoteOutPort::term() {
        // Cleanup
        try {
            ctx->pub->delete_datawriter(datawriter);
            ctx->participant->delete_topic(topic.in());
        }
        catch (CORBA::Exception& e) {
            std::cerr << "Exception caught in QuoteOutPort cleanup."
                << std::endl
                << e << std::endl;
            ACE_OS::exit(1);
        }
        MWOpenDDS::context::getInstance(MWOpenDDS::RequestType::CLEAR);
    }

    void QuoteOutPort::send(const Quote& data)
    {
        try {
            auto inst = handles.find(data.ticker);
            if (inst == handles.end()) {
                auto newhandle = datawriter->register_instance(data);
                inst = handles.insert(handles.end(), std::make_pair(data.ticker, newhandle));
            }
            DDS::ReturnCode_t ret =
                datawriter->write(data, inst->second);

            if (ret != DDS::RETCODE_OK)
            {
                ACE_ERROR((
                    LM_ERROR,
                    ACE_TEXT("(%P|%t)ERROR: send returned %d.\n"),
                    ret));
            }
        }
        catch (CORBA::Exception& e) {
            std::cerr << "Exception caught while sending message:" << std::endl
                << e << std::endl;
            ACE_OS::exit(1);
        }
    }
}

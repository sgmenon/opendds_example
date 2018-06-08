// -*- C++ -*-
// *******************************************************************
//
// (c) Copyright 2006, Object Computing, Inc.
// All Rights Reserved.
//
// *******************************************************************

#include "ExchangeEventDataReaderListenerImpl.h"
#include "StockQuoterTypeSupportC.h"
#include "StockQuoterTypeSupportImpl.h"

#include <dds/DCPS/Service_Participant.h>
#include <dds/DCPS/Marked_Default_Qos.h>
#include <dds/DCPS/PublisherImpl.h>
#include <dds/DCPS/StaticIncludes.h>

#include <ace/streams.h>

namespace StockQuoter {
    ExchangeEventDataReaderListenerImpl::ExchangeEventDataReaderListenerImpl(Port& inst)
        :_inst{ inst }
    {
    }

    ExchangeEventDataReaderListenerImpl::~ExchangeEventDataReaderListenerImpl()
    {
    }

    void ExchangeEventDataReaderListenerImpl::on_data_available(DDS::DataReader_ptr reader)
    {
        try {
            StockQuoter::ExchangeEventDataReader_var exchange_evt_dr
                = StockQuoter::ExchangeEventDataReader::_narrow(reader);
            if (CORBA::is_nil(exchange_evt_dr.in())) {
                cerr << "ExchangeEventDataReaderListenerImpl::on_data_available: _narrow failed." << endl;
                ACE_OS::exit(1);
            }

            int count = 0;
            while (true) {
                StockQuoter::ExchangeEvent exchange_evt;
                DDS::SampleInfo si;
                DDS::ReturnCode_t status = exchange_evt_dr->take_next_sample(exchange_evt, si);

                if (status == DDS::RETCODE_OK) {
                    ++count;
                    _inst.messageReceive(exchange_evt);
                    cout << "SampleInfo.sample_rank        = " << si.sample_rank << endl;
                    cout << "SampleInfo.instance_handle    = " << hex << si.instance_handle << endl;
                    cout << "SampleInfo.publication_handle = " << hex << si.publication_handle << endl;
                    break;
                }
                else if (status == DDS::RETCODE_NO_DATA) {
                    cerr << "INFO: reading complete after " << count << " samples." << endl;
                    break;
                }
                else {
                    cerr << "ERROR: read ExchangeEvent: Error: " << status << endl;
                }
            }

        }
        catch (CORBA::Exception& e) {
            cerr << "Exception caught in read:" << endl << e << endl;
            ACE_OS::exit(1);
        }
    }

    void ExchangeEventDataReaderListenerImpl::on_requested_deadline_missed(
        DDS::DataReader_ptr,
        const DDS::RequestedDeadlineMissedStatus &)
    {
        cerr << "ExchangeEventDataReaderListenerImpl::on_requested_deadline_missed" << endl;
    }

    void ExchangeEventDataReaderListenerImpl::on_requested_incompatible_qos(
        DDS::DataReader_ptr,
        const DDS::RequestedIncompatibleQosStatus &)
    {
        cerr << "ExchangeEventDataReaderListenerImpl::on_requested_incompatible_qos" << endl;
    }

    void ExchangeEventDataReaderListenerImpl::on_liveliness_changed(
        DDS::DataReader_ptr,
        const DDS::LivelinessChangedStatus &)
    {
        cerr << "ExchangeEventDataReaderListenerImpl::on_liveliness_changed" << endl;
    }

    void ExchangeEventDataReaderListenerImpl::on_subscription_matched(
        DDS::DataReader_ptr,
        const DDS::SubscriptionMatchedStatus &)
    {
        cerr << "ExchangeEventDataReaderListenerImpl::on_subscription_matched" << endl;
    }

    void ExchangeEventDataReaderListenerImpl::on_sample_rejected(
        DDS::DataReader_ptr,
        const DDS::SampleRejectedStatus&)
    {
        cerr << "ExchangeEventDataReaderListenerImpl::on_sample_rejected" << endl;
    }

    void ExchangeEventDataReaderListenerImpl::on_sample_lost(
        DDS::DataReader_ptr,
        const DDS::SampleLostStatus&)
    {
        cerr << "ExchangeEventDataReaderListenerImpl::on_sample_lost" << endl;
    }
}

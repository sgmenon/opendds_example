#include "publisher.hpp"
#include <ace/Log_Msg.h>
#include <orbsvcs/Time_Utilities.h>

#include <iostream>
#include <thread>
#include <chrono>

namespace {
    const char* STOCK_EXCHANGE_NAME = "Sid's Stock Exchange";
    TimeBase::TimeT get_timestamp()
    {
        TimeBase::TimeT retval;
        ACE_hrtime_t t = ACE_OS::gethrtime();
        ORBSVCS_Time::hrtime_to_TimeT(retval, t);
        return retval;
    }
}

namespace SlMdl {

    Publisher::Publisher()
        : i{ 0 }
    {}

    void Publisher::initialize()
    {
        ex_port.init();
        q_port.init();
    }
    void Publisher::step()
    {
        if (i == 0)
        {
            StockQuoter::ExchangeEvent opened;
            opened.exchange = STOCK_EXCHANGE_NAME;
            opened.event = StockQuoter::TRADING_OPENED;
            opened.timestamp = get_timestamp();

            std::cout << "Publishing TRADING_OPENED" << std::endl;
            ex_port.send(opened);
        }
        else {
            //
            // SPY
            //

            StockQuoter::Quote spy_quote;
            spy_quote.exchange = STOCK_EXCHANGE_NAME;
            spy_quote.ticker = CORBA::string_dup("SPY");
            spy_quote.full_name =
                CORBA::string_dup("S&P Depository Receipts");
            spy_quote.value = 1200.0 + 10.0*i;
            spy_quote.timestamp = get_timestamp();

            std::cout << "Publishing SPY Quote: "
                << spy_quote.value << std::endl;

            q_port.send(spy_quote);

            //
            // MDY
            //

            StockQuoter::Quote mdy_quote;
            mdy_quote.exchange = STOCK_EXCHANGE_NAME;
            mdy_quote.ticker = CORBA::string_dup("MDY");
            mdy_quote.full_name =
                CORBA::string_dup("S&P MidCap Depository Receipts");
            mdy_quote.value = 1400.0 + 10.0*i;
            mdy_quote.timestamp = get_timestamp();

            std::cout << "Publishing MDY Quote: "
                << mdy_quote.value << std::endl;

            q_port.send(mdy_quote);
        }
        ++i;
    }
    void Publisher::terminate()
    {
        //Last, we publish a TRADING_CLOSED event on the "Stock Exchange Event" topic to indicate that the stock exchange is closed for the day.
        StockQuoter::ExchangeEvent closed;
        closed.exchange = STOCK_EXCHANGE_NAME;
        closed.event = StockQuoter::TRADING_CLOSED;
        closed.timestamp = get_timestamp();

        std::cout << "Publishing TRADING_CLOSED" << std::endl;

        ex_port.send(closed);
        std::this_thread::sleep_for(std::chrono::seconds(1));
        std::cout << "Exiting..." << std::endl;
        ex_port.term();
        q_port.term();
    }
}
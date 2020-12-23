//
// Created by Глеб Яровой on 12/20/20.
//
#include <iostream>
#include "testing.h"

#define ID_SIZE 24
#define MESSAGE_SIZE 64
#define DATA_SIZE 32
#define LRC_SIZE 8


DeviceReceiver::DeviceReceiver      ( std::initializer_list<uint64_t> data )
        : d_Data ( data )
{
}
//-------------------------------------------------------------------------------------------------
bool               DeviceReceiver::Recv                               ( uint64_t        & message )
{
    if ( ! d_Data . size () )
        return false;
//  this_thread::sleep_for ( std::chrono::milliseconds ( rand () % 100 ) );
    message = d_Data . front ();
    d_Data . pop_front ();
    return true;
}

bool               DeviceMock::makeMessage                            ( uint32_t num_valid, uint32_t num_bug ){
    uint64_t all_message = num_bug+num_valid;
    m_Data.reserve(all_message);

    for(auto m = 0; m < all_message; m++) {

        ///create random data + device id
        for(size_t i=0; i < 56; i++)
            bitset[i] = (rand()%2 == 1) ? '1' : '0';

        uint64_t m_result = 0;
        uint16_t tmp = 0;

        ///create LRC for random bitset and store in temporary m_result variable
        for ( size_t i = 0; i < LRC_SIZE; i++ ) {
            tmp = 0;
            for ( size_t j = 0; j < 8; j++ ) {
                tmp += ( bitset[i + ( 8 * j )] == '1' ) ? 1 : 0;
            }
            m_result = m_result << 1;
            m_result += ( tmp % 2 == 0 ) ? 0 : 1;
        }

        ///create bugs
        if(num_bug > 0){
            size_t random_var = rand() % 56;
            for(size_t i = random_var; i < random_var+5; i++){
                if(i > 55)
                    bitset[i-56] = (bitset[i-56] == '1') ? '0' : '1';
                else
                    bitset[i] = (bitset[i] == '1') ? '0' : '1';
            }
            num_bug--;
        }

        /// make full message
        for ( size_t i = 0; i < 56; i++ ) {
            m_result = m_result << 1;
            m_result += ( bitset[i] == '1' ) ? 1 : 0;
        }

        ///store message
        m_Data.push_back(m_result);
    }
}


void               DeviceMock::MessageSender                          ( std::function<void(uint64_t)> target, uint64_t num_valid, uint64_t num_bug)
{
    makeMessage(num_valid, num_bug);

    for ( auto x : m_Data )
    {
        target ( x );
//    this_thread::sleep_for ( std::chrono::milliseconds ( rand () % 100 ) );
    }
}

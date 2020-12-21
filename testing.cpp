//
// Created by Глеб Яровой on 12/20/20.
//

#include "testing.h"

/*DeviceMessage *    DeviceMock::MakeMessage  ( size_t amount, size_t  bugs, size_t fragments ){


    return NULL;
}*/

DeviceReceiver::DeviceReceiver      ( std::initializer_list<uint64_t> data )
        : m_Data ( data )
{
}
//-------------------------------------------------------------------------------------------------
bool               DeviceReceiver::Recv                  ( uint64_t        & fragment )
{
    if ( ! m_Data . size () )
        return false;
//  this_thread::sleep_for ( std::chrono::milliseconds ( rand () % 100 ) );
    fragment = m_Data . front ();
    m_Data . pop_front ();
    return true;
}

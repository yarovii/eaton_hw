//
// Created by Глеб Яровой on 12/20/20.
//

#ifndef EATON_HW_TESTING_H
#define EATON_HW_TESTING_H

#include <cstdint>
#include <memory>
#include <deque>

uint32_t    CalculateCRC32  (   const uint8_t * data, size_t  bits );

class DeviceReceiver {
    std::deque<uint64_t>     m_Data;
public:
    DeviceReceiver                                 ( std::initializer_list<uint64_t> data );
    bool             Recv                          ( uint64_t        & fragment );
};

/*
struct DeviceMessage
{
    uint32_t                         m_ID;
    uint32_t                         m_CRC32;
    std::deque<uint64_t>             m_Data;
};

class DeviceMock {
    DeviceMessage * tail_device;

public:
    uint32_t    CreateMeasurement  (    const uint8_t * data,
                                         size_t  bits );

    DeviceMessage * MakeMessage  ( size_t amount, size_t  bugs, size_t fragments );

    bool    MakeFragments ( const uint8_t * data, size_t  bits );
};
*/



#endif //EATON_HW_TESTING_H

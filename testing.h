//
// Created by Глеб Яровой on 12/20/20.
//

#ifndef EATON_HW_TESTING_H
#define EATON_HW_TESTING_H

#include <cstdint>
#include <memory>
#include <deque>
#include <vector>

uint32_t    CalculateCRC32  (   const uint8_t * data, size_t  bits );

class DeviceReceiver {
    std::deque<uint64_t>     d_Data;
public:
    DeviceReceiver                                 ( std::initializer_list<uint64_t> data );
    bool             Recv                          ( uint64_t        & fragment );
};


class DeviceMock {

    char bitset[64];

    bool createLRC(void);

    std::vector<uint64_t>     m_Data;
    bool    makeMessage ( uint32_t num_valid, uint32_t num_bug );
public:
    void               MessageSender                          ( std::function<void(uint64_t)> target,
                                                                 uint64_t num_valid, uint64_t num_bug);
};



#endif //EATON_HW_TESTING_H

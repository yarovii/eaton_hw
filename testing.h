//
// Created by Глеб Яровой on 12/20/20.
//

#ifndef EATON_HW_TESTING_H
#define EATON_HW_TESTING_H

#include <cstdint>
#include <memory>
#include <deque>
#include <vector>

//---------------------------------------------------------------------------------------------
/**
 * basic Receiver for testing purpose
 */
class DeviceReceiver {
    //---------------------------------------------------------------------------------------------
    /**
     * the variable stores all income messages from device
     */
    std::deque<uint64_t>      d_Data;
public:
    //---------------------------------------------------------------------------------------------
    /**
     * simple constructor to store messages to d_Data deque
     * @param[in] m_CRC32        received lrc from message
     * @param[in] data           received fragments
     */
                        DeviceReceiver                         ( std::initializer_list<uint64_t> data );
    //---------------------------------------------------------------------------------------------
    /**
     * the method send data to receiver function. Parody  to receiver
     * @param[in] message        message to send
     */
    bool                Recv                                   ( uint64_t        & message );
};

//---------------------------------------------------------------------------------------------
/**
 * simple parody of device, for testing purpose only
 */
class DeviceMock {

    char                      bitset[56];
    std::vector<uint64_t>     m_Data;
    //---------------------------------------------------------------------------------------------
    /**
     * the method creates messages and store them in m_Data vector. Method can create valid or invalid messages
     * @param[in] num_valid        number of valid messages to produce
     * @param[in] num_bug          number of invalid messages to produce
     */
    void                makeMessage                            ( uint32_t num_valid, uint32_t num_bug );
public:
    //---------------------------------------------------------------------------------------------
    /**
     * the method send created messages to target function. In our example is asynchronous method MonitorDevices::AddFragment
     * @param[in] target           function to send data
     * @param[in] num_valid        number of valid messages to produce
     * @param[in] num_bug          number of invalid messages to produce
     */
    void                MessageSender                          ( std::function<void(uint64_t)> target,
                                                                 uint64_t num_valid, uint64_t num_bug);
};



#endif //EATON_HW_TESTING_H

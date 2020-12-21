//
// Created by Глеб Яровой on 12/19/20.
//
#include "testing.h"
#include <deque>
#ifndef EATON_HW_MONITOR_H
#define EATON_HW_MONITOR_H



class MonitorDevices {
    std::mutex mx_receiver;
    //---------------------------------------------------------------------------------------------
    /**
     * the variable stores all DeviceReceiver to get data synchronously.
     */
    std::deque<DeviceReceiver> receiver;

    std::deque<uint64_t> fragment;
    //---------------------------------------------------------------------------------------------
    /**
     * the method check if message is valid. Method CheckCRC32 get crc32 from fragments by calling
     * method DeviceMock::CalculateCRC32. Then checks equality of our crc32 with received one
     * @param[in] m_CRC32        received CRC32 from message
     * @param[in] data           received fragments
     */
    bool                     CheckCRC32                    (   uint32_t & m_CRC32, const uint8_t * data );
    //---------------------------------------------------------------------------------------------
    /**
     * the method receives fragments from device receiver and stores message fragments.
     * Can throw exception if receives no data.
     * @param[in] index        index in deque(one thread will work with one receiver from std::deque<DeviceReceiver> receiver)
     */
    void                     ReceiveFragments              ( uint8_t index );
public:
    //---------------------------------------------------------------------------------------------
    /**
     * the method registers a new receiver(just one). The method itself does not do anything else (in particular,
     * it does not start any thread). The receiver thread will be created after the computation
     * actually starts (method Start).
     * @param[in] rec        receiver to set
     */
    void                     AddDeviceReceiver             ( DeviceReceiver          rec );
    //---------------------------------------------------------------------------------------------
    /**
     * the method registers a new list of receivers. The method itself does not do anything else (in particular,
     * it does not start any thread). The receiver thread will be created after the computation
     * actually starts (method Start).
     * @param[in] list        all receivers to set
     */
    void                     AddSomeDeviceReceivers             ( std::initializer_list<DeviceReceiver>           list );
    //---------------------------------------------------------------------------------------------
    /**
     * the method is called asynchronously from thread(s) in the testing environment.
     * The asynchronous input is another way to get message fragment from device. Method AddFragment is called
     * only between the invocations of MonitorDevices::Start and MonitorDevices::Stop.
     * @param[in] fragment        fragment of message
     */
    void                     AddFragment                   ( uint64_t          fragment );
    //---------------------------------------------------------------------------------------------
    /**
     * the method check if message fragments read correctly. Otherwise, throw exception and delete device data.
     * Method MonitorDevices::CheckMessage is called only after all fragments read( if fragments counter == 0)
     */
    void                     CheckMessage                  ( void );
    //---------------------------------------------------------------------------------------------
    /**
     * the method starts all required threads and starts to process the incoming message fragments. It does
     * not wait for the completion of the threads.
     * @param[in] thrCount        number of worker threads
     */
    void                     Start                         ( uint8_t          thrCount );
    //---------------------------------------------------------------------------------------------
    /**
     * the method is called from the main thread to stop the computation. the method waits until all fragments
     * are read from the receivers (AddFragment is not be executed once the testing environment calls Stop).
     * Incomplete messages throw exceptions and delete device.
     */
    void                     Stop                          ( void );

};


#endif //EATON_HW_MONITOR_H

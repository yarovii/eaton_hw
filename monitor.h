//
// Created by Глеб Яровой on 12/19/20.
//
#include "testing.h"
#include <deque>
#include <vector>
#include <optional>
#include <condition_variable>
#ifndef EATON_HW_MONITOR_H
#define EATON_HW_MONITOR_H

class MonitorDevices {
    std::condition_variable     cv_worker;
    std::mutex                  mx_receiver;
    std::mutex                  mx_fragment;
    std::mutex                  mx_worker;
    std::mutex                  mx_console_message;
    std::atomic<uint32_t>       count_valid_message;
    std::atomic<uint32_t>       count_invalid_message;

    std::vector<std::thread *>  w_threads;
    bool stop;
    //---------------------------------------------------------------------------------------------
    /**
     * the variable stores all DeviceReceiver to get data synchronously.
     */
    std::deque<DeviceReceiver>  receiver;
    //---------------------------------------------------------------------------------------------
    /**
     * the variable stores all received messages.
     */
    std::deque<uint64_t>        messages;
    //---------------------------------------------------------------------------------------------
    /**
     * the method check if message is valid. Method CheckLRC encodes lrc from message. Then checks equality of data lrc with received one
     * @param[in] m_CRC32        received lrc from message
     * @param[in] data           received fragments
     */
    bool                     checkLRC                         (   uint16_t & lrc, char *& bitset );
    //---------------------------------------------------------------------------------------------
    /**
     * the method receives fragments from device receiver and stores message fragments.
     * @param[in] index        index in deque(one thread will work with one receiver from std::deque<DeviceReceiver> receiver)
     */
    void                     ReceiveFragments                 ( uint8_t index );
    //---------------------------------------------------------------------------------------------
    /**
     * the method decompose message. So, it takes bitset of message, then pull out id, data, lrc.
     * @param[in] bitset           reference to bitset of message
     * @param[in] id               reference to id variable
     * @param[in] data             reference to data variable
     * @param[in] lrc              reference to lrc variable
     */
    void                     decomposeMessage( char *&bitset, uint32_t &id, uint32_t &data, uint16_t &lrc );
    //---------------------------------------------------------------------------------------------
    /**
     * the method turns message of type uint64_t to binary char array( bitset )
     * @param[in] bitset           reference to empty bitset of message
     * @param[in] message          message variable
     */
    void                     toBinary                         ( char *& bitset, uint64_t message);
public:
    //---------------------------------------------------------------------------------------------
    /**
     * the method registers a new receiver(just one). The method itself does not do anything else (in particular,
     * it does not start any thread). The receiver thread will be created after the computation
     * actually starts (method Start).
     * @param[in] rec        receiver to set
     */
                             MonitorDevices                   ( void );
    //---------------------------------------------------------------------------------------------
    /**
     * the method returns counter value of valid messages
     */
    uint32_t                 getValidMessageNum               ( void );
    //---------------------------------------------------------------------------------------------
    /**
     * the method returns counter value of invalid messages
     */
    uint32_t                 getInvalidMessageNum             ( void );
    //---------------------------------------------------------------------------------------------
    /**
     * the method registers a new receiver(just one). The method itself does not do anything else (in particular,
     * it does not start any thread). The receiver thread will be created after the computation
     * actually starts (method Start).
     * @param[in] rec        receiver to set
     */
    void                     AddDeviceReceiver                ( DeviceReceiver          rec );
    //---------------------------------------------------------------------------------------------
    /**
     * the method registers a new list of receivers. The method itself does not do anything else (in particular,
     * it does not start any thread). The receiver thread will be created after the computation
     * actually starts (method Start).
     * @param[in] list        all receivers to set
     */
    void                     AddSomeDeviceReceivers           ( std::initializer_list<DeviceReceiver>           list );
    //---------------------------------------------------------------------------------------------
    /**
     * the method is called asynchronously from thread(s) in the testing environment.
     * The asynchronous input is another way to get message fragment from device. Method AddFragment is called
     * only between the invocations of MonitorDevices::Start and MonitorDevices::Stop.
     * @param[in] fragment        fragment of message
     */
    void                     AddFragment                      ( uint64_t          frag );
    //---------------------------------------------------------------------------------------------
    /**
     * the method check if message fragments read correctly. Otherwise, throw exception and delete device data.
     * Method MonitorDevices::CheckMessage is called only after all fragments read( if fragments counter == 0)
     */
    void                     ProcessMessage                   ( void );
    //---------------------------------------------------------------------------------------------
    /**
     * the method starts all required threads and starts to process the incoming messages. It does
     * not wait for the completion of the threads.
     * @param[in] thrCount        number of worker threads
     */
    void                     Start                            ( uint8_t          thrCount );
    //---------------------------------------------------------------------------------------------
    /**
     * the method is called from the main thread to stop the computation. the method waits until all fragments
     * are read from the receivers (AddFragment is not be executed once the testing environment calls Stop).
     * Incomplete messages throw exceptions and delete device.
     */
    void                     Stop                             ( void );

};


#endif //EATON_HW_MONITOR_H

//
// Created by Глеб Яровой on 12/19/20.
//

#include <iostream>
#include <thread>
#include <mutex>
#include <memory>
#include <atomic>
#include <optional>
#include <chrono>
#include <sstream>
#include <functional>
#include "monitor.h"

#define ID_SIZE 24
#define MESSAGE_SIZE 64
#define DATA_SIZE 32
#define LRC_SIZE 8

MonitorDevices::MonitorDevices(){
    count_valid_message=0;
    count_invalid_message=0;
    stop = false;
//    tail.;
}

void                     MonitorDevices::AddDeviceReceiver             ( DeviceReceiver          rec ){
    receiver.push_back(rec);
}

void                     MonitorDevices::AddSomeDeviceReceivers        ( std::initializer_list<DeviceReceiver>           list ){
    for( auto rec : list)
        receiver.emplace_back(rec);
}

void                     MonitorDevices::ReceiveFragments              ( uint8_t index ){
    uint64_t frag = 0;
    bool rc = false;
    auto rec = receiver.at(index);

    while ( 1 ) {
        rc = rec.Recv(frag);

        if ( !rc ) break;

        mx_receiver.lock();
        fragment.emplace_back(frag);
        mx_receiver.unlock();
    }
}

void                     MonitorDevices::AddFragment                   ( uint64_t          frag ) {
    if(!stop){
        mx_receiver.lock();
        fragment.emplace_back(frag);
        mx_receiver.unlock();
        cv_worker.notify_all();
    }
}

void                     MonitorDevices::ProcessMessage                  ( void ){
    char *bitset = new char[64];
    uint32_t id = 0;
    uint32_t data = 0;
    uint16_t lrc = 0;
//    std::thread::id this_id = std::this_thread::get_id();

    while(true) {
        std::unique_lock<std::mutex> locker(mx_worker);
        cv_worker.wait(locker, [this](){
            return !fragment.empty() || stop; });

//        std::cout << "WWWWWWWWWWW    " << this_id << std::endl;

        locker.unlock();
        if(stop && fragment.empty()) {
            cv_worker.notify_all();
            return;
        }

//        std::this_thread::sleep_for ( std::chrono::milliseconds ( 100 ) );
        id = 0;
        data = 0;
        lrc = 0;
        mx_fragment.lock();
        if(fragment.size() == 0) {
            mx_fragment.unlock();
            break;
        }
        uint64_t frag = fragment.front();
        fragment.pop_front();
        mx_fragment.unlock();

        toBinary(bitset, frag);
        decomposeFragment(bitset, id, data, lrc);

        if ( !checkLRC(lrc, bitset)) {
            mx_console_message.lock();
            printf("#Damaged data, device id: %i\n", id);
            mx_console_message.unlock();
            count_invalid_message++;
            continue;
        }

        count_valid_message++;

        /* mx_device_message.lock();
         auto str = findDevice(id);
         mx_device_message.unlock();

         if(str.has_value()) {
             mx_device_message.lock();
             str.value()->m_Data.push_back(data);
             mx_device_message.unlock();
         }
         else{
     //        createStructAndStore();
         }*/
        std::stringstream stream;
        stream << std::hex << frag;
        std::string frag_to_hex( stream.str() );
        mx_console_message.lock();
        std::cout << "frag: " << frag_to_hex << "  data: " << data << "  id: " << id << "  lrc: " << lrc << "\n";
        mx_console_message.unlock();
    }
    delete[]bitset;
}

/*std::optional<DeviceMessage*> MonitorDevices::findDevice( uint32_t &id ) {
    auto tmp = tail;

    while(tmp.has_value()){
        if(tmp.value()->m_ID == id)
            return tmp;
        tmp = tmp.value()->m_Next;
    }
    return {};
}*/

bool                     MonitorDevices::checkLRC                       ( uint16_t & lrc, char *& bitset ){
    uint16_t result_lrc=0;
    uint16_t tmp=0;
//    std::cout << "\n" << (MESSAGE_SIZE-LRC_SIZE)/8 << "  jdjdd\n";/
    for(size_t i=LRC_SIZE; i < LRC_SIZE+8; i++){
        tmp=0;
        for(size_t j=0; j < 8; j++) {
            tmp += (bitset[i+(8*j)] == '1') ? 1 : 0;
        }
        result_lrc = result_lrc << 1;
        result_lrc += (tmp % 2 == 0) ? 0 : 1;
    }

    return lrc == result_lrc;
}

void                     MonitorDevices::toBinary                       ( char *& bitset, uint64_t number){

    for(size_t i=0; i<MESSAGE_SIZE; ++i)
    {
        if((number & 1) != 0)
            bitset[MESSAGE_SIZE-1-i] = '1';
        else
            bitset[MESSAGE_SIZE-1-i] = '0';

        number >>= 1;
    }
}

void                     MonitorDevices::decomposeFragment              ( char *& bitset, uint32_t &id, uint32_t &data, uint16_t &lrc ){

    for(size_t i=0; i < MESSAGE_SIZE; i++){
        if(i > MESSAGE_SIZE-ID_SIZE-1){
            id = id<<1;
            if (bitset[i]=='1') id += 1;
        }
        else if(i > LRC_SIZE-1){
            data = data << 1;
            if ( bitset[i] == '1' ) data += 1;
        }
        else {
            lrc = lrc<<1;
            if (bitset[i]=='1') lrc += 1;
        }
    }
}

void                     MonitorDevices::Start                          ( uint8_t         thrCount ){
    std::thread *th;

    for(size_t i=0; i < receiver.size(); i++) {
        th = new std::thread(&MonitorDevices::ReceiveFragments, this, i);
        th->join();
    }

    w_threads.reserve(thrCount);
    for(size_t i=0; i < thrCount; i++){
        w_threads.emplace_back(new std::thread(&MonitorDevices::ProcessMessage,this));
//        th =new std::thread(&MonitorDevices::ProcessMessage,this);
//        th->join();

    }

}

uint32_t                 MonitorDevices::getValidMessageNum               ( void ){
    return count_valid_message;
}

uint32_t                 MonitorDevices::getInvalidMessageNum               ( void ){
    return count_invalid_message;
}

void                     MonitorDevices::Stop                          ( void ){
//    std::this_thread::sleep_for ( std::chrono::milliseconds ( 4000 ) );
    stop = true;
    cv_worker.notify_all();
    for(auto th : w_threads)
        th->join();
}


int main(){
    std::cout << "NNNNN\n";

    MonitorDevices monitor;
    DeviceMock device;

    DeviceReceiver deviceReceiver = DeviceReceiver(std::initializer_list<uint64_t> { 0x02230000000c, 0x071e124dabef, 0x02360037680e, 0x071d2f8fe0a1, 0x055500150755 } );

    monitor.AddDeviceReceiver(deviceReceiver);
    monitor.AddSomeDeviceReceivers(std::initializer_list<DeviceReceiver> {DeviceReceiver(std::initializer_list<uint64_t> { 0x02230000000c, 0x071e124dabef, 0xffffffffffffffff } ),
                                                                          DeviceReceiver(std::initializer_list<uint64_t> { 0x02360037680e, 0x071d2f8fe0a1, 0x055500150755 } )});
    monitor.Start(2);
//    std::this_thread::sleep_for ( std::chrono::milliseconds ( 4000 ) );
    device.MessageSender(std::bind ( &MonitorDevices::AddFragment, &monitor,std::placeholders::_1 ), 10, 2);
    std::this_thread::sleep_for ( std::chrono::milliseconds ( 8000 ) );
    monitor.Stop();
    std::cout << "valid: " << monitor.getValidMessageNum() << "  invalid: "
              << monitor.getInvalidMessageNum() << "  all: " << monitor.getInvalidMessageNum() + monitor.getValidMessageNum() << std::endl;

    return 0;
}
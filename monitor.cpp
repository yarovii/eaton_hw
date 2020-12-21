//
// Created by Глеб Яровой on 12/19/20.
//

#include <iostream>
#include <thread>
#include <mutex>
#include <memory>
#include <atomic>
#include <optional>
#include "monitor.h"

#define ID_SIZE 24
#define MESSAGE_SIZE 64
#define DATA_SIZE 32
#define LRC_SIZE 8

MonitorDevices::MonitorDevices(){
    tail = NULL;
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
        count_all_message++;
    }
}

void                     MonitorDevices::ProcessMessage                  ( void ){
    uint32_t id = 0;
    uint32_t data = 0;
    uint16_t lrc = 0;

    mx_fragment.lock();
    uint64_t frag = fragment.front();
    fragment.pop_front();
    mx_fragment.unlock();

//    frag = 0xffffffffffffffff;
    decomposeFragment(frag, id, data, lrc);

    checkMessage();

    mx_device_message.lock();
    auto str = findDevice(id);
    mx_device_message.unlock();

    if(str.has_value()) {
        mx_device_message.lock();
        str.value()->m_Data.push_back(data);
        mx_device_message.unlock();
    }
    else{
        createStructAndStore();
    }

    std::cout << "frag: " << frag << "  data: " << data << "   id: "<< id << "   lrc: " << lrc << "\n";

}

std::optional<DeviceMessage*> MonitorDevices::findDevice( uint32_t &id ) {
    auto tmp = tail;

    while(tmp.has_value()){
        if(tmp.value()->m_ID == id)
            return tmp;
        tmp = tmp.value()->m_Next;
    }
    return {};
}

void                     MonitorDevices::decomposeFragment              (uint64_t fragment, uint32_t &id, uint32_t &data, uint16_t &lrc ){
    char bitset[64];
    uint64_t number = fragment;

    for(size_t i=0; i<MESSAGE_SIZE; ++i)
    {
        if((number & 1) != 0)
            bitset[MESSAGE_SIZE-1-i] = '1';
        else
            bitset[MESSAGE_SIZE-1-i] = '0';

        number >>= 1;
    }

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

void                     MonitorDevices::Start                         ( uint8_t         thrCount ){
    std::thread *th;

    for(size_t i=0; i < receiver.size(); i++) {
        th = new std::thread(&MonitorDevices::ReceiveFragments, this, i);
        th->join();
    }

    for(size_t i=0; i < thrCount; i++){
        th =new std::thread(&MonitorDevices::ProcessMessage,this);
        th->join();

    }

}

void                     MonitorDevices::Stop                          ( void ){

}


int main(){
    std::cout << "NNNNN\n";

    MonitorDevices monitor;

    DeviceReceiver deviceReceiver = DeviceReceiver(std::initializer_list<uint64_t> { 0x02230000000c, 0x071e124dabef, 0x02360037680e, 0x071d2f8fe0a1, 0x055500150755 } );

//    deviceReceiver.
    monitor.AddDeviceReceiver(deviceReceiver);
    monitor.AddSomeDeviceReceivers(std::initializer_list<DeviceReceiver> {DeviceReceiver(std::initializer_list<uint64_t> { 0x02230000000c, 0x071e124dabef } ),
                                                                          DeviceReceiver(std::initializer_list<uint64_t> { 0x02360037680e, 0x071d2f8fe0a1, 0x055500150755 } )});


    monitor.Start(2);



    return 0;
}
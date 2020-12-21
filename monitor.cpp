//
// Created by Глеб Яровой on 12/19/20.
//

#include <iostream>
#include <thread>
#include <mutex>
#include <atomic>
#include "monitor.h"

void                     MonitorDevices::AddDeviceReceiver             ( DeviceReceiver          rec ){
    receiver.push_back(rec);
}

void                     MonitorDevices::AddSomeDeviceReceivers        ( std::initializer_list<DeviceReceiver>           list ){
    for( auto rec : list)
        receiver.push_back(rec);
}

void                     MonitorDevices::ReceiveFragments              ( uint8_t index ){
    uint64_t frag = 0;
    bool rc = false;
    auto rec = receiver.at(index);

    while ( 1 ) {
        rc = rec.Recv(frag);

        if ( !rc ) break;

        mx_receiver.lock();
        fragment.push_back(frag);
        mx_receiver.unlock();
    }
}

void                     MonitorDevices::Start                         ( uint8_t         thrCount ){
    std::thread *th;

    for(size_t i=0; i < receiver.size(); i++) {
        th = new std::thread(&MonitorDevices::ReceiveFragments, this, i);
        th->join();
    }


}

void                     MonitorDevices::Stop                          ( void ){
    
}


int main(){
    std::cout << "NNNNN\n";

    MonitorDevices monitor;

    DeviceReceiver deviceReceiver = DeviceReceiver(std::initializer_list<uint64_t> { 0x02230000000c, 0x071e124dabef, 0x02360037680e, 0x071d2f8fe0a1, 0x055500150755 } );


    monitor.AddDeviceReceiver(deviceReceiver);
    monitor.AddSomeDeviceReceivers(std::initializer_list<DeviceReceiver> {DeviceReceiver(std::initializer_list<uint64_t> { 0x02230000000c, 0x071e124dabef } ),
                                                                          DeviceReceiver(std::initializer_list<uint64_t> { 0x02360037680e, 0x071d2f8fe0a1, 0x055500150755 } )});


    monitor.Start(2);



    return 0;
}
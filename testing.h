//
// Created by Глеб Яровой on 12/20/20.
//

#ifndef EATON_HW_TESTING_H
#define EATON_HW_TESTING_H


class testing {

};

struct DeviceMessage
{
    uint32_t                         m_ID;
    std::initializer_list<uint64_t>  m_Fragments;
};

class DeviceMock {

public:
    uint32_t    CalculateCRC32  (   const uint8_t * data,
                                    size_t  bits );

    uint32_t    CreateMeasurements  (    const uint8_t * data,
                                         size_t  bits );

    bool    MakeFragment ( const uint8_t * data, size_t  bits );
    bool    MakeMessage  ( const uint8_t * data, size_t  bits );
};



#endif //EATON_HW_TESTING_H

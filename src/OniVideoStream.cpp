#include "OniVideoStream.hpp"
#include <QLoggingCategory>
#include "OniDevice.hpp"
#include <qlogging.h>
#include <stdexcept>

#include <iostream>

OniVideoStream::~OniVideoStream()
{
    m_stream.destroy();
}

openni::VideoStream *OniVideoStream::get() noexcept
{
    return &m_stream;
}

bool OniVideoStream::create(const OniDevice &device, openni::SensorType mode)
{
    openni::Status status = m_stream.create(device.get(), mode);
    std::string sensorType = "";
    switch (mode) {
    case openni::SENSOR_IR:
        sensorType = "SENSOR_IR";
        break;
    case openni::SENSOR_COLOR:
        sensorType = "SENSOR_COLOR";
        break;
    case openni::SENSOR_DEPTH:
        sensorType = "SENSOR_DEPTH";
        break;
    }
    if (status != openni::STATUS_OK) {
        std::cout << "Could not create " << sensorType << "stream "
                  << openni::OpenNI::getExtendedError() << '\n';
        return false;
    }
    status = m_stream.start();
    if (status != openni::STATUS_OK) {
        std::cout << "Could not read from " << sensorType << "stream "
                  << openni::OpenNI::getExtendedError() << '\n';
        return false;
    }
    return true;
}

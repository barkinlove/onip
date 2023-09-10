#pragma once
#include <cstdint>
#include <openni2/OpenNI.h>

class OniDevice;

class OniVideoStream final
{
public:
    OniVideoStream() = default;
    ~OniVideoStream();

    bool create(OniDevice &device, openni::SensorType mode);
    openni::VideoStream *get() noexcept;
    const openni::VideoStream &ref() const noexcept;

private:
    openni::VideoStream m_stream;
};

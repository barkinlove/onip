#include "OniDevice.hpp"

bool OniDevice::open(std::string_view filename)
{
    m_device.open(filename.data());
    return m_device.isValid();
}

OniDevice::~OniDevice()
{
    m_device.close();
}

const openni::Device &OniDevice::get() const noexcept
{
    return m_device;
}

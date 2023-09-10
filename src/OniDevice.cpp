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

openni::Device &OniDevice::get() noexcept
{
    return m_device;
}

void OniDevice::close()
{
    m_device.close();
}

bool OniDevice::isValid() const noexcept
{
    return m_device.isValid();
}

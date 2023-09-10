#pragma once
#include <openni2/OpenNI.h>
#include <string_view>

class OniDevice final
{
public:
    ~OniDevice();
    bool open(std::string_view filename);
    const openni::Device &get() const noexcept;

private:
    openni::Device m_device;
};

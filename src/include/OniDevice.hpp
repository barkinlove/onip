#pragma once
#include <openni2/OpenNI.h>
#include <string_view>

// oni wrappers were a bad idea...

class OniDevice final
{
public:
    ~OniDevice();
    bool open(std::string_view filename);
    void close();
    bool isValid() const noexcept;
    openni::Device &get() noexcept;

private:
    openni::Device m_device;
};

#include <stdio.h>

#include<AXDevice.h>

static AXDevice * _dev = nil;


extern "C" __attribute__((weak)) int __io_putchar(int ch)
{
    return _dev ? _dev->write(&ch, 1, 100) : 1;
}
void retarget(AXDevice *     dev)
{
    _dev = dev;
}

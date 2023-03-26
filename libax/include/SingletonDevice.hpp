/*
 *     SocialLedge.com - Copyright (C) 2013
 *
 *     This file is part of free software framework for embedded processors.
 *     You can use it and/or distribute it as long as this copyright header
 *     remains unmodified.  The code is free for personal use and requires
 *     permission to use in a commercial product.
 *
 *      THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 *      OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 *      MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 *      I SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR
 *      CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 *
 *     You can reach the author of this software at :
 *          p r e e t . w i k i @ g m a i l . c o m
 */

/**
 * @file SingletonDevice.hpp
 * @brief Provides Singleton C++ Class Template
 * @ingroup Utilities
 *
 * Version: 06192012    Initial
 */

#ifndef SingletonDevice_HPP_
#define SingletonDevice_HPP_



/**
 * There are 3 kinds of Singleton Instances to choose from:
 * - STATIC_INSTANCE
 *      Static instance is created inside the getInstance() which returns this static instance's address
 * - STATIC_POINTER_INSTANCE
 *      Static pointer is created which is initialized upon declaring it, and getInstance() returns this pointer.
 * - STATIC_PRIVATE_POINTER_INSTANCE
 *      Private Pointer is initialized to NULL as a member, and then if it is NULL, the class instance is created.
 */
#define STATIC_INSTANCE                     1
#define STATIC_POINTER_INSTANCE             2
#define STATIC_PRIVATE_POINTER_INSTANCE     3


#ifndef SINGLETON_INIT_METHOD
#define SINGLETON_INIT_METHOD               STATIC_PRIVATE_POINTER_INSTANCE
#endif

#if (SINGLETON_INIT_METHOD == STATIC_PRIVATE_POINTER_INSTANCE)
    #ifndef SINGLETONDEV_UNIQUES_NUMBER
        #define SINGLETONDEV_UNIQUES_NUMBER    3
    #endif
#endif


/**
 * This Singleton template can be inherited and the parent class can make its constructor
 * private with this class as a friend to inherit this Singleton pattern.
 *
 * @ingroup Utilities
 */
template <typename classType, typename deviceType>
class SingletonDevice
{
protected:
    int             thisDeviceId;
    int             defaultDeviceId;
    deviceType *    device;

public:
    static classType&   getInstance(int unique = -1);

    void                setDevice(deviceType * dev, int isPreffered = 0) { device = dev; if (isPreffered) setDefault(); }

    deviceType*         getDevice() { return device; }

    void                setDefault() { getInstance(0).defaultDeviceId = thisDeviceId; }

    const char *        getName() { return device->getName(); };


protected:
    static SingletonDevice* mpSingletonInstance[SINGLETONDEV_UNIQUES_NUMBER];

    /**
     * Protected Constructor for this Singleton Class so only the class
     * inheriting this template class can make this part of itself
     */
    SingletonDevice() { defaultDeviceId = 0; device = (deviceType*)0; }


private:
    /* No Private Members */
};

    template <typename classType, typename deviceType>
    SingletonDevice<classType, deviceType>* SingletonDevice<classType,deviceType>::mpSingletonInstance [ SINGLETONDEV_UNIQUES_NUMBER ] = { 0 };

    template <typename classType, typename deviceType>
    classType& SingletonDevice<classType,deviceType>::getInstance(int unique)
    {
        if ((-1 == unique) || (unique >= SINGLETONDEV_UNIQUES_NUMBER))
        {
            unique = (0 == mpSingletonInstance[0]) ? 0 : mpSingletonInstance[0]->defaultDeviceId;
        }

        if (unique < SINGLETONDEV_UNIQUES_NUMBER)
        {
            if (0 == mpSingletonInstance[unique])
            {
                mpSingletonInstance[unique]                 = new classType();
                mpSingletonInstance[unique]->thisDeviceId   = unique;
            }
        }

        return *(classType*)mpSingletonInstance[unique];
    }


#endif /* SingletonDevice_HPP_ */

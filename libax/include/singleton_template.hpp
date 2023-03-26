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
 * @file singletonTemplate.hpp
 * @brief Provides Singleton C++ Class Template
 * @ingroup Utilities
 *
 * Version: 06192012    Initial
 */

#ifndef SINGLETONTEMPLATE_HPP_
#define SINGLETONTEMPLATE_HPP_

#undef SINGLETON_UNIQUES_NUMBER
#define SINGLETON_UNIQUES_NUMBER    10



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
    #ifndef SINGLETON_UNIQUES_NUMBER
        #define SINGLETON_UNIQUES_NUMBER    1
    #endif
#endif


/**
 * This Singleton template can be inherited and the parent class can make its constructor
 * private with this class as a friend to inherit this Singleton pattern.
 *
 * @ingroup Utilities
 */
template <typename classType>
class SingletonTemplate
{
public:
    /// Public member to get instance of this SINGLETON class.
    #if (SINGLETON_INIT_METHOD == STATIC_PRIVATE_POINTER_INSTANCE)
        int index;
        static classType& getInstance(int unique = 0);

        static void destroyInstance(int unique = 0);

        unsigned int getSingletoneIndex()
        {
            return index;
        }

        void destroy()
        {
            destroyInstance(index);
        }

#else
        static classType& getInstance();
    #endif

virtual  ~SingletonTemplate() { }

protected:
    #if (SINGLETON_INIT_METHOD == STATIC_PRIVATE_POINTER_INSTANCE)
        static SingletonTemplate* mpSingletonInstance[SINGLETON_UNIQUES_NUMBER];
    #endif

    /**
     * Protected Constructor for this Singleton Class so only the class
     * inheriting this template class can make this part of itself
     */
    SingletonTemplate() { index = 0; }

private:
    /* No Private Members */
};







#if (SINGLETON_INIT_METHOD == STATIC_POINTER_INSTANCE)
    template <typename classType>
    classType& SingletonTemplate<classType>::getInstance()
    {
        static classType* mpSingletonInstance[] = new classType();
        return *mpSingletonInstance;
    }
#elif (SINGLETON_INIT_METHOD == STATIC_INSTANCE)
    template <typename classType>
    classType& SingletonTemplate<classType>::getInstance()
    {
        static classType singletonInstance;
        return singletonInstance;
    }
#elif (SINGLETON_INIT_METHOD == STATIC_PRIVATE_POINTER_INSTANCE)
    template <typename classType>
    SingletonTemplate<classType>* SingletonTemplate<classType>::mpSingletonInstance [ SINGLETON_UNIQUES_NUMBER ] = { 0 };

    template <typename classType>
    classType& SingletonTemplate<classType>::getInstance(int unique)
    {
        if (0 == mpSingletonInstance[unique])
        {
            mpSingletonInstance[unique] = new classType();
            mpSingletonInstance[unique]->index = unique;
        }

        return *(classType*)mpSingletonInstance[unique];
    }

    template <typename classType>
    void SingletonTemplate<classType>::destroyInstance(int unique)
    {
        if (0 != mpSingletonInstance[unique])
        {
//            delete &mpSingletonInstance[unique];
            mpSingletonInstance[unique] = 0;
        }
    }

#else
    #error SINGLETON_INIT_METHOD contains invalid value
#endif

#undef SINGLETON_UNIQUES_NUMBER

#endif /* SINGLETONTEMPLATE_HPP_ */

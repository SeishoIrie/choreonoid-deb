
#ifndef CNOID_UTIL_TIME_MEASURE_H_INCLUDED
#define CNOID_UTIL_TIME_MEASURE_H_INCLUDED

#ifdef WIN32
#include <windows.h>

namespace cnoid {
    inline void msleep(int msec){
        ::Sleep(msec);
    }
    inline void usleep(int usec){
        ::Sleep(usec / 1000);
    }
}

#else

#include <unistd.h>

namespace cnoid {
    inline void msleep(int msec){
        ::usleep(msec * 1000);
    }
    inline void usleep(int usec){
        ::usleep(usec);
    }
}

#endif

#endif


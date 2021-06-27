#ifndef COMMON1_H
#define COMMON1_H

#include <mutex>

#define DEPTH_IMAGE 0
#define COLOR_IMAGE 1


extern std::mutex g_ThreadMutex;

extern bool g_ThreadFlag;


#endif // COMMON1_H

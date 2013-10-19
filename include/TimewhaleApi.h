/*
 * Timewhale API
 * The TIMEWHALEAPI macro is used to mark any classes which need to be exported
 * through the dll interface. If you do not mark the class then the game devs
 * will not beable to access the methods.
 *
 * Example
 *
 * class TIMEWHALEAPI AwesomeClassName {
 * };
 *
 * Take take when deciding which classes need to be exported. Ideally less is better.
 * Also take note of what classes are being included in classes that will be exported.
 * If you are not directly consuming a class type in the header file, then consider
 * only including that header file in the .cpp instead.
 */

#pragma once
#ifndef __TIMEWHALE_API
#define __TIMEWHALE_API

#ifdef _WIN32_
    #ifdef TIMEWHALE_EXPORTS
        #define TIMEWHALEAPI __declspec(dllexport)
        #define TIMEWHALE_TEMPLATE
    #else
        #define TIMEWHALEAPI __declspec(dllimport)
        #define TIMEWHALE_TEMPLATE extern
    #endif
#else
    #define TIMEWHALEAPI
#endif

#endif

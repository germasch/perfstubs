// Copyright (c) 2019 University of Oregon
// Distributed under the BSD Software License
// (See accompanying file LICENSE.txt)

#pragma once

#include <stdint.h>
#include <stdlib.h>
#include "perfstubs_api/Config.h"
#include "perfstubs_api/Tool.h"

/* These macros help generate unique variable names */
#define CONCAT_(x,y) x##y
#define CONCAT(x,y) CONCAT_(x,y)

/* ------------------------------------------------------------------ */
/* Define the C++ API and PerfStubs glue class first */
/* ------------------------------------------------------------------ */

#ifdef __GNUC__
#define __PERFSTUBS_FUNCTION__ __PRETTY_FUNCTION__
#else
#define __PERFSTUBS_FUNCTION__ __func__
#endif

#define PERFSTUBS_UNKNOWN 0
#define PERFSTUBS_SUCCESS 1
#define PERFSTUBS_FAILURE 2
#define PERFSTUBS_FINALIZED 3

extern int perfstubs_initialized;

#ifdef __cplusplus

#if defined(PERFSTUBS_USE_TIMERS)

/*
 * We allow the namespace to be changed, so that different libraries
 * can include their own implementation and not have a namespace collision.
 * For example, library A and executable B could both include the 
 * perfstubs_api code in their source tree, and change the namespace
 * respectively, instead of linking in the perfstubs library.
 */

#if defined(PERFSTUBS_NAMESPACE)
#define PERFSTUBS_INTERNAL_NAMESPACE PERFSTUBS_NAMESPACE
#else
#define PERFSTUBS_INTERNAL_NAMESPACE perfstubs_profiler
#endif

#include <memory>
#include <sstream>
#include <string>

namespace external
{

namespace PERFSTUBS_INTERNAL_NAMESPACE
{

class Timer
{
public:
    static void RegisterThread(void);
    static void DumpData(void);
    // data measurement api
    static void* Create(const char *timer_name);
    static void* Create(const std::string &timer_name);
    static void Start(const void * timer);
    static void Stop(const void * timer);
    static void SetParameter(const char * parameter_name,
                             int64_t parameter_value);
    static void DynamicPhaseStart(const char *phase_prefix,
                                  int iteration_index);
    static void DynamicPhaseStart(const std::string &phase_prefix,
                                  int iteration_index);
    static void DynamicPhaseStop(const char *phase_prefix, int iteration_index);
    static void DynamicPhaseStop(const std::string &phase_prefix,
                                 int iteration_index);
    static void* CreateCounter(const char *name);
    static void SampleCounter(const void *counter, const double value);
    static void MetaData(const char *name, const char *value);

    // data query api
    // data query function declarations
    static void GetTimerData(perftool_timer_data_t *timer_data, int tool_index);
    static void GetCounterData(perftool_counter_data_t *counter_data, int tool_index);
    static void GetMetaData(perftool_metadata_t *metadata, int tool_index);
    static void FreeTimerData(perftool_timer_data_t *timer_data, int tool_index);
    static void FreeCounterData(perftool_counter_data_t *counter_data, int tool_index);
    static void FreeMetaData(perftool_metadata_t *metadata, int tool_index);

    // The only way to get an instance of this class
    static Timer &Get(void);
    // destructor
    ~Timer(void);
    bool m_Initialized;
    static thread_local bool m_ThreadSeen;
    static std::string MakeTimerName(const char * file,
        const char * func, int line);

private:
    static void _RegisterThread(void);
    // Private constructor to prevent construction
    Timer(void);
    // Prevent copies
    Timer(const Timer &old);
    const Timer &operator=(const Timer &old);
};

class ScopedTimer
{
private:
    const void * m_timer;

public:
    ScopedTimer(const void * timer) : m_timer(timer)
    {
        if (perfstubs_initialized == PERFSTUBS_SUCCESS) Timer::Start(m_timer);
    }
    ~ScopedTimer()
    {
        if (perfstubs_initialized == PERFSTUBS_SUCCESS) Timer::Stop(m_timer);
    }
};

} // namespace PERFSTUBS_INTERNAL_NAMESPACE

} // namespace external

namespace PSNS = external::PERFSTUBS_INTERNAL_NAMESPACE;

#define PERFSTUBS_INIT() PSNS::Timer::Get();

#define PERFSTUBS_DUMP_DATA() PSNS::Timer::DumpData();

#define PERFSTUBS_REGISTER_THREAD() PSNS::Timer::RegisterThread();

#define PERFSTUBS_TIMER_START(_timer, _timer_name) \
    static void * _timer = PSNS::Timer::Create(_timer_name); \
    if (perfstubs_initialized == PERFSTUBS_SUCCESS) PSNS::Timer::Start(_timer);

#define PERFSTUBS_TIMER_STOP(_timer) \
    if (perfstubs_initialized == PERFSTUBS_SUCCESS) PSNS::Timer::Stop(_timer);

#define PERFSTUBS_SET_PARAMETER(_parameter, _value) \
    if (perfstubs_initialized == PERFSTUBS_SUCCESS) \
    PSNS::Timer::SetParameter(_parameter, _value);

#define PERFSTUBS_DYNAMIC_PHASE_START(_phase_prefix, _iteration_index) \
    if (perfstubs_initialized == PERFSTUBS_SUCCESS) \
    PSNS::Timer::DynamicPhaseStart( _phase_prefix, _iteration_index);

#define PERFSTUBS_DYNAMIC_PHASE_STOP(_phase_prefix, _iteration_index) \
    if (perfstubs_initialized == PERFSTUBS_SUCCESS) \
    PSNS::Timer::DynamicPhaseStop( _phase_prefix, _iteration_index);

#define PERFSTUBS_SAMPLE_COUNTER(_name, _value) \
    static void * CONCAT(__counter,__LINE__) = \
        PSNS::Timer::CreateCounter(_name); \
    if (perfstubs_initialized == PERFSTUBS_SUCCESS) \
    PSNS::Timer::SampleCounter(CONCAT(__counter,__LINE__), _value);

#define PERFSTUBS_METADATA(_name, _value) PSNS::Timer::MetaData(_name, _value);

#define PERFSTUBS_SCOPED_TIMER(__name) \
    static void * CONCAT(__var,__LINE__) = PSNS::Timer::Create(__name); \
    PSNS::ScopedTimer CONCAT(__var2,__LINE__)(CONCAT(__var,__LINE__));

#define PERFSTUBS_SCOPED_TIMER_FUNC() \
    static void * CONCAT(__var,__LINE__) = \
        PSNS::Timer::Create(PSNS::Timer::MakeTimerName(__FILE__, \
        __PERFSTUBS_FUNCTION__, __LINE__)); \
    PSNS::ScopedTimer CONCAT(__var2,__LINE__)(CONCAT(__var,__LINE__));

#else // defined(PERFSTUBS_USE_TIMERS)

#define PERFSTUBS_INIT()
#define PERFSTUBS_DUMP_DATA()
#define PERFSTUBS_REGISTER_THREAD()
#define PERFSTUBS_TIMER_START(_timer, _timer_name)
#define PERFSTUBS_TIMER_STOP(_timer)
#define PERFSTUBS_SET_PARAMETER(_parameter, _value)
#define PERFSTUBS_DYNAMIC_PHASE_START(_phase_prefix, _iteration_index)
#define PERFSTUBS_DYNAMIC_PHASE_STOP(_phase_prefix, _iteration_index)
#define PERFSTUBS_TIMER_START_FUNC(_timer)
#define PERFSTUBS_TIMER_STOP_FUNC(_timer)
#define PERFSTUBS_SAMPLE_COUNTER(_name, _value)
#define PERFSTUBS_METADATA(_name, _value)
#define PERFSTUBS_SCOPED_TIMER(__name)
#define PERFSTUBS_SCOPED_TIMER_FUNC()

#endif // defined(PERFSTUBS_USE_TIMERS)

#else // ifdef __cplusplus

#include <string.h>
#include <stdio.h>
#include <stdint.h>

/* ------------------------------------------------------------------ */
/* Now define the C API */
/* ------------------------------------------------------------------ */

#if defined(PERFSTUBS_USE_TIMERS)

/* regular C API */

void psInit(void);
void psRegisterThread(void);
void psDumpData(void);
void* psTimerCreate(const char *timerName);
void psTimerStart(const void *timer);
void psTimerStop(const void *timer);
void psSetParameter(const char *parameter_name, int64_t parameter_value);
void psDynamicPhaseStart(const char *phasePrefix, int iterationIndex);
void psDynamicPhaseStop(const char *phasePrefix, int iterationIndex);
void* psCreateCounter(const char *name);
void psSampleCounter(const void *counter, const double value);
void psMetaData(const char *name, const char *value);

/* data query API */

void psGetTimerData(perftool_timer_data_t *timer_data, int tool_index);
void psGetCounterData(perftool_counter_data_t *counter_data, int tool_index);
void psGetMetaData(perftool_metadata_t *metadata, int tool_index);
void psFreeTimerData(perftool_timer_data_t *timer_data, int tool_index);
void psFreeCounterData(perftool_counter_data_t *counter_data, int tool_index);
void psFreeMetaData(perftool_metadata_t *metadata, int tool_index);

char * psMakeTimerName(const char * file, const char * func, int line);

/* Macro API for option of entirely disabling at compile time
 * To use this API, set the Macro PERFSTUBS_USE_TIMERS on the command
 * line or in a config.h file, however your project does it
 */

#define PERFSTUBS_INIT() psInit();

#define PERFSTUBS_DUMP_DATA() psDumpData();

#define PERFSTUBS_REGISTER_THREAD() psRegisterThread();

#define PERFSTUBS_TIMER_START(_timer, _timer_name) \
    static void * _timer = NULL; \
    if (perfstubs_initialized == PERFSTUBS_SUCCESS) { \
        if (_timer == NULL) { \
            _timer = psTimerCreate(_timer_name); \
        } \
        psTimerStart(_timer); \
    };

#define PERFSTUBS_TIMER_STOP(_timer) \
    if (perfstubs_initialized == PERFSTUBS_SUCCESS) psTimerStop(_timer); \

#define PERFSTUBS_SET_PARAMETER(_parameter, _value) \
    if (perfstubs_initialized == PERFSTUBS_SUCCESS) psSetParameter(_parameter, _value);

#define PERFSTUBS_DYNAMIC_PHASE_START(_phase_prefix, _iteration_index) \
    if (perfstubs_initialized == PERFSTUBS_SUCCESS) \
    psDynamicPhaseStart(_phase_prefix, _iteration_index);

#define PERFSTUBS_DYNAMIC_PHASE_STOP(_phase_prefix, _iteration_index) \
    if (perfstubs_initialized == PERFSTUBS_SUCCESS) \
    psDynamicPhaseStop(_phase_prefix, _iteration_index);

#define PERFSTUBS_TIMER_START_FUNC(_timer) \
    static void * _timer = NULL; \
    if (perfstubs_initialized == PERFSTUBS_SUCCESS) { \
        if (_timer == NULL) { \
            char * tmpstr = psMakeTimerName(__FILE__, \
            __PERFSTUBS_FUNCTION__, __LINE__); \
            _timer = psTimerCreate(tmpstr); \
            free(tmpstr); \
        } \
        psTimerStart(_timer); \
    };

#define PERFSTUBS_TIMER_STOP_FUNC(_timer) \
    if (perfstubs_initialized == PERFSTUBS_SUCCESS) psTimerStop(_timer);

#define PERFSTUBS_SAMPLE_COUNTER(_name, _value) \
    static void * CONCAT(__var,__LINE__) =  NULL; \
    if (perfstubs_initialized == PERFSTUBS_SUCCESS) { \
        if (CONCAT(__var,__LINE__) == NULL) { \
            CONCAT(__var,__LINE__) = psCreateCounter(_name); \
        } \
        psSampleCounter(CONCAT(__var,__LINE__), _value); \
    };

#define PERFSTUBS_METADATA(_name, _value) \
    if (perfstubs_initialized == PERFSTUBS_SUCCESS) psMetaData(_name, _value);

#else // defined(PERFSTUBS_USE_TIMERS)

#define PERFSTUBS_INIT()
#define PERFSTUBS_DUMP_DATA()
#define PERFSTUBS_REGISTER_THREAD()
#define PERFSTUBS_TIMER_START(_timer, _timer_name)
#define PERFSTUBS_TIMER_STOP(_timer_name)
#define PERFSTUBS_SET_PARAMETER(_parameter, _value)
#define PERFSTUBS_DYNAMIC_PHASE_START(_phase_prefix, _iteration_index)
#define PERFSTUBS_DYNAMIC_PHASE_STOP(_phase_prefix, _iteration_index)
#define PERFSTUBS_TIMER_START_FUNC(_timer)
#define PERFSTUBS_TIMER_STOP_FUNC(_timer)
#define PERFSTUBS_SAMPLE_COUNTER(_name, _value)
#define PERFSTUBS_METADATA(_name, _value)

#endif // defined(PERFSTUBS_USE_TIMERS)

#endif // ifdef __cplusplus

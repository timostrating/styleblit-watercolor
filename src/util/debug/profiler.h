/**
 * this file is based on the code of TheCherno 
 * https://gist.github.com/TheCherno/31f135eea6ee729ab5f26a6908eb3a5e
 */

#pragma once

#include <string>
#include <chrono>
#include <fstream>
#include <algorithm>
#include <iostream>
#include <thread>

struct ProfileResult
{
    std::string name;
    long long start, end;
    uint32_t threadID;
};


class Profiler
{
    std::string     _sessionName   = "None";
    std::ofstream   _outputStream;
    int             _profileCount  = 0;
    bool            _activeSession = false;
 
    Profiler() { }

public:
    static Profiler& getInstance()
    {
        static Profiler instance;
        return instance;
    }
 
    ~Profiler()
    {
        std::cout << "\tWriting Profiling info to file." << std::endl;
        endSession();
    }
 
    void beginSession(const std::string& name, const std::string& filepath = "results.json")
    {
        if (_activeSession) { endSession(); }
        _activeSession = true;
        _outputStream.open(filepath);
        writeHeader();
        _sessionName = name;
    }
 
    void endSession()
    {
        
        if (!_activeSession) { return; }
        _activeSession = false;
        writeFooter();
        _outputStream.close();
        _profileCount = 0;
    }
 
    void writeProfile(const ProfileResult& result)
    {
 
        if (_profileCount++ > 0) { _outputStream << ","; }
 
        std::string name = result.name;
        std::replace(name.begin(), name.end(), '"', '\'');
 
        _outputStream << "{";
        _outputStream << "\"cat\":\"function\",";
        _outputStream << "\"dur\":" << (result.end - result.start) << ',';
        _outputStream << "\"name\":\"" << name << "\",";
        _outputStream << "\"ph\":\"X\",";
        _outputStream << "\"pid\":0,";
        _outputStream << "\"tid\":" << result.threadID << ',';
        _outputStream << "\"ts\":" << result.start;
        _outputStream << "}";
    }
 
    void writeHeader()
    {
        _outputStream << "{\"otherData\": {},\"traceEvents\":[";
    }
 
    void writeFooter()
    {
        _outputStream << "]}";
    }
};


class ProfilerTimer
{
    ProfileResult _result;
 
    std::chrono::time_point<std::chrono::high_resolution_clock> _startTimepoint;
    bool _stopped;
 
public:
 
    ProfilerTimer(const std::string & name): _result({ name, 0, 0 }), _stopped(false)
    {
        _startTimepoint = std::chrono::high_resolution_clock::now();
    }
 
    ~ProfilerTimer()
    {
        if (!_stopped) { stop(); }
    }
 
    void stop()
    {
        auto endTimepoint = std::chrono::high_resolution_clock::now();
 
        _result.start = std::chrono::time_point_cast<std::chrono::microseconds>(_startTimepoint).time_since_epoch().count();
        _result.end   = std::chrono::time_point_cast<std::chrono::microseconds>(endTimepoint).time_since_epoch().count();
        _result.threadID = std::hash<std::thread::id>{}(std::this_thread::get_id());

        Profiler::getInstance().writeProfile(_result);
 
        _stopped = true;
    }
};


#if PROFILING == 1
	#define PROFILER_BEGIN_SESSION(name, filepath) Profiler::getInstance().beginSession(name, filepath)
	#define PROFILER_END_SESSION() Profiler::getInstance().endSession()

    #define PROFILE_SCOPE(name) ProfilerTimer timer##__LINE__(name)
	#define PROFILE_FUNCTION() PROFILE_SCOPE(__FUNCTION__)
    
    
    #if AUTOMATIC_PROFILING == 1
        #ifndef AUTOMATIC_PROFILING_FIRST 
        #define AUTOMATIC_PROFILING_FIRST
            static std::vector<std::chrono::time_point<std::chrono::high_resolution_clock>> timers;

            extern "C" {
                void __cyg_profile_func_enter(void *this_fn, void *call_site) __attribute__((no_instrument_function));
                void __cyg_profile_func_exit(void *this_fn, void *call_site) __attribute__((no_instrument_function));
            }
            void __cyg_profile_func_enter(void * func, void * caller) {
                timers.push_back(std::chrono::high_resolution_clock::now());
            }
            void __cyg_profile_func_exit(void * func, void * caller) {
                ProfileResult result;

                auto endTimepoint = std::chrono::high_resolution_clock::now();
                auto startTimepoint = timers.back();
    
                result.name  = "todo"; // We get random pointers instead of useable names
                result.start = std::chrono::time_point_cast<std::chrono::microseconds>(startTimepoint).time_since_epoch().count();
                result.end   = std::chrono::time_point_cast<std::chrono::microseconds>(endTimepoint).time_since_epoch().count();
                result.threadID = std::hash<std::thread::id>{}(std::this_thread::get_id());
                Profiler::getInstance().writeProfile(result);
            }
        #endif
    #endif
#else
	#define PROFILER_BEGIN_SESSION(name, filepath)
	#define PROFILER_END_SESSION()
	#define PROFILE_SCOPE(name)
	#define PROFILE_FUNCTION()
#endif
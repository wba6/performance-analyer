/*
 * A header file for the profiler
 *
 * Author wba6
 */

#ifndef PERFORMANCE_ANALYER_PROFILER_H
#define PERFORMANCE_ANALYER_PROFILER_H

#include <string>
#include <chrono>
#include <algorithm>
#include <fstream>
#include <thread>
#include "Timer.h"


#ifdef PROFILING_ENABLED
/*
 * A macro to create a profile scope
 *
 * @param std::string name : the name of the profile
 */
#define PROFILE_SCOPE(name) Timer timer##__LINE__(name)

/*
 * A macro to create a profile function
 */
#define PROFILE_FUNCTION() PROFILE_SCOPE(__FUNCTION__)
#else
#define PROFILE_SCOPE(name)
#define PROFILE_FUNCTION()
#endif


/*
 * A struct to hold the profile result
 *
 * @param std::string Name : the name of the profile
 * @param long long Start : the start time of the profile
 * @param long long End : the end time of the profile
 * @param uint32_t ThreadID : the thread id of the profile
 */
struct ProfileResult
{
    std::string Name;
    long long Start, End;
    uint32_t ThreadID;
};

struct ProfilerSession
{
    std::string Name;
};

class Profiler {
private:
    ProfilerSession* m_CurrentSession;
    std::ofstream m_OutputStream;
    int m_ProfileCount;
public:
    Profiler()
            : m_CurrentSession(nullptr), m_ProfileCount(0)
    {
    }

    void BeginSession(const std::string& name, const std::string& filepath = "results.json")
    {
        m_OutputStream.open(filepath);
        WriteHeader();
        m_CurrentSession = new ProfilerSession{ name };
    }

    void EndSession()
    {
        WriteFooter();
        m_OutputStream.close();
        delete m_CurrentSession;
        m_CurrentSession = nullptr;
        m_ProfileCount = 0;
    }

    void WriteProfile(const ProfileResult& result)
    {
        if (m_ProfileCount++ > 0)
            m_OutputStream << ",";

        std::string name = result.Name;
        std::replace(name.begin(), name.end(), '"', '\'');

        m_OutputStream << "{";
        m_OutputStream << R"("cat":"function",)";
        m_OutputStream << "\"dur\":" << (result.End - result.Start) << ',';
        m_OutputStream << R"("name":")" << name << "\",";
        m_OutputStream << R"("ph":"X",)";
        m_OutputStream << "\"pid\":0,";
        m_OutputStream << "\"tid\":" << result.ThreadID << ",";
        m_OutputStream << "\"ts\":" << result.Start;
        m_OutputStream << "}";

        m_OutputStream.flush();
    }

    void WriteHeader()
    {
        m_OutputStream << R"({"otherData": {},"traceEvents":[)";
        m_OutputStream.flush();
    }

    void WriteFooter()
    {
        m_OutputStream << "]}";
        m_OutputStream.flush();
    }

    static Profiler& Get()
    {
        static Profiler instance;
        return instance;
    }
};


#endif //PERFORMANCE_ANALYER_PROFILER_H

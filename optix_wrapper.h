#pragma once

// Platform defines
#if defined(_WIN32) || defined(_WIN64)
#    define OPTIX_Platform_Windows
#    if defined(_MSC_VER)
#        define OPTIX_Platform_Windows_MSVC
#    endif
#elif defined(__APPLE__)
#    define OPTIX_Platform_macOS
#endif

#if defined(OPTIX_Platform_Windows_MSVC)
#   define NOMINMAX
#   define _USE_MATH_DEFINES
#   include <Windows.h>
#   undef near
#   undef far
#   undef RGB
#endif

#include "cuda_helper.h"

#include <optix.h>
#include <optix_function_table_definition.h>
#include <optix_stubs.h>



namespace optix {
#ifdef _DEBUG
#   define OPTIX_ENABLE_ASSERT
#endif

#ifdef OPTIX_Platform_Windows_MSVC
    void devPrintf(const char* fmt, ...);
#else
#   define devPrintf(fmt, ...) printf(fmt, ##__VA_ARGS__);
#endif

#if 1
#   define optixPrintf(fmt, ...) do { devPrintf(fmt, ##__VA_ARGS__); printf(fmt, ##__VA_ARGS__); } while (0)
#else
#   define optixPrintf(fmt, ...) printf(fmt, ##__VA_ARGS__)
#endif

#ifdef OPTIX_ENABLE_ASSERT
#   define optixAssert(expr, fmt, ...) if (!(expr)) { devPrintf("%s @%s: %u:\n", #expr, __FILE__, __LINE__); devPrintf(fmt"\n", ##__VA_ARGS__); abort(); } 0
#else
#   define optixAssert(expr, fmt, ...)
#endif

#define optixAssert_ShouldNotBeCalled() optixAssert(false, "Should not be called!")
#define optixAssert_NotImplemented() optixAssert(false, "Not implemented yet!")

    using namespace CUDAHelper;



    class Context;
    class ProgramGroup;
    class GeometryInstance;
    class GeometryAccelerationStructure;
    class InstanceAccelerationStructure;

#define OPTIX_PIMPL(Name) \
public: \
    class Impl; \
private: \
    Impl* m_opaque



    class Context {
        OPTIX_PIMPL(Context);

    public:
        static Context create();
        void destroy();

        void setNumRayTypes(uint32_t numRayTypes);
        void setMaxTraceDepth(uint32_t maxTraceDepth);

        void setPipelineOptions(uint32_t numPayloadValues, uint32_t numAttributeValues, const char* launchParamsVariableName,
                                bool useMotionBlur, uint32_t traversableGraphFlags, uint32_t exceptionFlags);

        int32_t createModuleFromPTXString(const std::string &ptxString, int32_t maxRegisterCount, OptixCompileOptimizationLevel optLevel, OptixCompileDebugLevel debugLevel);

        ProgramGroup createRayGenProgram(int32_t moduleID, const char* entryFunctionName);
        ProgramGroup createExceptionProgram(int32_t moduleID, const char* entryFunctionName);
        ProgramGroup createMissProgram(int32_t moduleID, const char* entryFunctionName);
        ProgramGroup createHitProgramGroup(int32_t moduleID_CH, const char* entryFunctionNameCH,
                                           int32_t moduleID_AH, const char* entryFunctionNameAH,
                                           int32_t moduleID_IS, const char* entryFunctionNameIS);
        ProgramGroup createCallableGroup(int32_t moduleID_DC, const char* entryFunctionNameDC,
                                         int32_t moduleID_CC, const char* entryFunctionNameCC);

        GeometryInstance createGeometryInstance();

        GeometryAccelerationStructure createGeometryAccelerationStructure();
        InstanceAccelerationStructure createInstanceAccelerationStructure();
    };



    class ProgramGroup {
        OPTIX_PIMPL(ProgramGroup);

    public:
        void destroy();
    };



    class GeometryInstance {
        OPTIX_PIMPL(GeometryInstance);

    public:
        void destroy();

        void setVertexBuffer(Buffer &vertexBuffer);
        void setTriangleBuffer(Buffer &triangleBuffer);

        void setNumHitGroups(uint32_t num);
        void setGeometryFlags(uint32_t idx, OptixGeometryFlags flags);
        void setHitGroup(uint32_t idx, uint32_t rayType, const ProgramGroup &hitGroup,
                         void* sbtRecordData, size_t size);
    };



    class GeometryAccelerationStructure {
        OPTIX_PIMPL(GeometryAccelerationStructure);

    public:
        void destroy();

        void addChild(const GeometryInstance &geomInst);

        void rebuild(bool preferFastTrace, bool allowUpdate, bool enableCompaction, CUstream stream);
        void compaction(CUstream rebuildOrUpdateStream, CUstream stream);
        void removeUncompacted(CUstream compactionStream);
        void update(CUstream stream);
    };


    //class InstanceAccelerationStructure {
    //    friend class _InstanceAccelerationStructure;
    //    _InstanceAccelerationStructure* m_opaque;

    //public:
    //    void destroy();

    //    void addChild(const GeometryAccelerationStructure &gas, float instantTransform[12]);

    //    void rebuild(bool preferFastTrace, bool allowUpdate, bool enableCompaction, CUstream stream);
    //    void compaction(CUstream rebuildOrUpdateStream, CUstream stream);
    //    void removeUncompacted(CUstream compactionStream);
    //    void update(CUstream stream);
    //};
}

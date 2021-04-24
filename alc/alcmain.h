#ifndef ALC_MAIN_H
#define ALC_MAIN_H

#include <algorithm>
#include <array>
#include <atomic>
#include <bitset>
#include <chrono>
#include <cstdint>
#include <cstddef>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <utility>

#include "AL/al.h"
#include "AL/alc.h"
#include "AL/alext.h"

#include "almalloc.h"
#include "alnumeric.h"
#include "alspan.h"
#include "atomic.h"
#include "core/ambidefs.h"
#include "core/bufferline.h"
#include "core/devformat.h"
#include "core/device.h"
#include "core/filters/splitter.h"
#include "core/hrtf.h"
#include "core/mixer/defs.h"
#include "inprogext.h"
#include "intrusive_ptr.h"
#include "vector.h"

struct ALbuffer;
struct ALeffect;
struct ALfilter;

using uint = unsigned int;


struct BufferSubList {
    uint64_t FreeMask{~0_u64};
    ALbuffer *Buffers{nullptr}; /* 64 */

    BufferSubList() noexcept = default;
    BufferSubList(const BufferSubList&) = delete;
    BufferSubList(BufferSubList&& rhs) noexcept : FreeMask{rhs.FreeMask}, Buffers{rhs.Buffers}
    { rhs.FreeMask = ~0_u64; rhs.Buffers = nullptr; }
    ~BufferSubList();

    BufferSubList& operator=(const BufferSubList&) = delete;
    BufferSubList& operator=(BufferSubList&& rhs) noexcept
    { std::swap(FreeMask, rhs.FreeMask); std::swap(Buffers, rhs.Buffers); return *this; }
};

struct EffectSubList {
    uint64_t FreeMask{~0_u64};
    ALeffect *Effects{nullptr}; /* 64 */

    EffectSubList() noexcept = default;
    EffectSubList(const EffectSubList&) = delete;
    EffectSubList(EffectSubList&& rhs) noexcept : FreeMask{rhs.FreeMask}, Effects{rhs.Effects}
    { rhs.FreeMask = ~0_u64; rhs.Effects = nullptr; }
    ~EffectSubList();

    EffectSubList& operator=(const EffectSubList&) = delete;
    EffectSubList& operator=(EffectSubList&& rhs) noexcept
    { std::swap(FreeMask, rhs.FreeMask); std::swap(Effects, rhs.Effects); return *this; }
};

struct FilterSubList {
    uint64_t FreeMask{~0_u64};
    ALfilter *Filters{nullptr}; /* 64 */

    FilterSubList() noexcept = default;
    FilterSubList(const FilterSubList&) = delete;
    FilterSubList(FilterSubList&& rhs) noexcept : FreeMask{rhs.FreeMask}, Filters{rhs.Filters}
    { rhs.FreeMask = ~0_u64; rhs.Filters = nullptr; }
    ~FilterSubList();

    FilterSubList& operator=(const FilterSubList&) = delete;
    FilterSubList& operator=(FilterSubList&& rhs) noexcept
    { std::swap(FreeMask, rhs.FreeMask); std::swap(Filters, rhs.Filters); return *this; }
};


struct ALCdevice : public al::intrusive_ref<ALCdevice>, DeviceBase {
    ALCuint NumMonoSources{};
    ALCuint NumStereoSources{};

    // Maximum number of sources that can be created
    uint SourcesMax{};
    // Maximum number of slots that can be created
    uint AuxiliaryEffectSlotMax{};

    std::string mHrtfName;
    al::vector<std::string> mHrtfList;
    ALCenum mHrtfStatus{ALC_FALSE};

    ALCenum LimiterState{ALC_DONT_CARE_SOFT};

    std::atomic<ALCenum> LastError{ALC_NO_ERROR};

    // Map of Buffers for this device
    std::mutex BufferLock;
    al::vector<BufferSubList> BufferList;

    // Map of Effects for this device
    std::mutex EffectLock;
    al::vector<EffectSubList> EffectList;

    // Map of Filters for this device
    std::mutex FilterLock;
    al::vector<FilterSubList> FilterList;


    ALCdevice(DeviceType type) : DeviceBase{type} { }
    ~ALCdevice();

    void enumerateHrtfs();

    DEF_NEWDEL(ALCdevice)
};

/* Must be less than 15 characters (16 including terminating null) for
 * compatibility with pthread_setname_np limitations. */
#define MIXER_THREAD_NAME "alsoft-mixer"

#define RECORD_THREAD_NAME "alsoft-record"


/**
 * Returns the index for the given channel name (e.g. FrontCenter), or
 * INVALID_CHANNEL_INDEX if it doesn't exist.
 */
inline uint GetChannelIdxByName(const RealMixParams &real, Channel chan) noexcept
{ return real.ChannelIndex[chan]; }
#define INVALID_CHANNEL_INDEX ~0u

#endif

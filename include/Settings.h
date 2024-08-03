#pragma once

class Settings : public Singleton<Settings>
{
public:
    static void LoadSettings() noexcept;

    inline static bool debug_logging{};

    inline static bool dump_music_tracks{};

    inline static bool dump_music_types{};

    inline static bool dump_locations{};

    inline static bool dump_regions{};
};

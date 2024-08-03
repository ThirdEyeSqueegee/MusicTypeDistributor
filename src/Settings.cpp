#include "Settings.h"

#include "Parser.h"

void Settings::LoadSettings() noexcept
{
    logger::info("Loading settings");

    CSimpleIniA ini;

    ini.SetUnicode();
    ini.LoadFile(R"(.\Data\SKSE\Plugins\MusicTypeDistributor.ini)");

    debug_logging = ini.GetBoolValue("Log", "Debug");

    if (debug_logging) {
        spdlog::set_level(spdlog::level::debug);
        logger::debug("Debug logging enabled");
    }

    dump_music_tracks = ini.GetBoolValue("General", "bDumpMusicTracks");
    dump_music_types  = ini.GetBoolValue("General", "bDumpMusicTypes");
    dump_locations    = ini.GetBoolValue("General", "bDumpLocations");
    dump_regions      = ini.GetBoolValue("General", "bDumpRegions");

    logger::info("Loaded settings");
    logger::info("\tbDumpMusicTracks = {}", dump_music_tracks);
    logger::info("\tbDumpMusicTypes = {}", dump_music_types);
    logger::info("\tbDumpLocations = {}", dump_locations);
    logger::info("\tbDumpRegions = {}", dump_regions);
    logger::info("");

    ini.Reset();

    Parser::ParseINIs(ini);
}

#include "Distributor.h"
#include "Logging.h"
#include "Parser.h"
#include "Settings.h"
#include "Utility.h"

void Listener(SKSE::MessagingInterface::Message* message) noexcept
{
    if (message->type == SKSE::MessagingInterface::kDataLoaded) {
        if (!REX::W32::GetModuleHandleW(L"po3_Tweaks")) {
            logger::error("ERROR: powerofthree's Tweaks not found");
            stl::report_and_fail("ERROR [MusicTypeDistributor.dll]: powerofthree's Tweaks not found");
        }
        Settings::LoadSettings();
        Parser::PrepareDistrMap();
        Distributor::Distribute();
        if (Settings::dump_music_tracks) {
            Utility::DumpMusicTracks();
        }
        if (Settings::dump_music_types) {
            Utility::DumpMusicTypes();
        }
        if (Settings::dump_locations) {
            Utility::DumpLocations();
        }
        if (Settings::dump_regions) {
            Utility::DumpRegions();
        }
    }
}

SKSEPluginLoad(const SKSE::LoadInterface* skse)
{
    InitializeLogging();

    const auto plugin{ SKSE::PluginDeclaration::GetSingleton() };
    const auto name{ plugin->GetName() };
    const auto version{ plugin->GetVersion() };

    logger::info("{} {} is loading...", name, version);

    Init(skse);

    if (const auto messaging{ SKSE::GetMessagingInterface() }; !messaging->RegisterListener(Listener)) {
        return false;
    }

    logger::info("{} has finished loading.", name);
    logger::info("");

    return true;
}

#include "Distributor.h"

#include "Map.h"
#include "Utility.h"

void Distributor::Distribute() noexcept
{
    logger::info(">----------------------------------------------------- Distributing... -----------------------------------------------------<");
    logger::info("");

    for (const auto& [music_type, pair] : Map::distr_map) {
        const auto& [tracks, clear_list]{ pair };
        const auto music_type_name{ Utility::GetFormEditorID(music_type) };
        const auto music_type_form_id{ music_type->GetFormID() };
        if (clear_list) {
            logger::info("Clearing {} (0x{:x})", music_type_name, music_type_form_id);
            music_type->tracks.clear();
        }
        for (const auto& track : tracks) {
            logger::info("Distributing {} (0x{:x}) to {} (0x{:x})", Utility::GetFormEditorID(track), track->GetFormID(), music_type_name, music_type_form_id);
            music_type->tracks.emplace_back(track);
        }
    }

    for (const auto& [location, music_type] : Map::location_distr_map) {
        logger::info("Updating location {} (0x{:x}) music type to {} (0x{:x})", Utility::GetFormEditorID(location), location->GetFormID(), Utility::GetFormEditorID(music_type),
                     music_type->GetFormID());
        location->musicType = music_type;
    }

    for (const auto& [region, music_type] : Map::region_distr_map) {
        const auto reg_edid{ Utility::GetFormEditorID(region) };
        for (const auto& reg_data : region->dataList->regionDataList) {
            const auto manager{ RE::TESDataHandler::GetSingleton()->GetRegionDataManager() };
            if (const auto reg_data_sound{ manager->AsRegionDataSound(reg_data) }) {
                logger::info("Updating region {} (0x{:x}) music type to {} (0x{:x})", reg_edid, region->GetFormID(), Utility::GetFormEditorID(music_type), music_type->GetFormID());
                reg_data_sound->music = music_type;
            }
        }
    }

    logger::info("");
    logger::info(">-------------------------------------------------- Finished distribution --------------------------------------------------<");
    logger::info("");
}

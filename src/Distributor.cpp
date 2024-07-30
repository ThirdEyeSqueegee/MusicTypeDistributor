#include "Distributor.h"
#include "Map.h"

void Distributor::Distribute() noexcept
{
    logger::info(">----------------------------------------------------- Distributing... -----------------------------------------------------<");
    logger::info("");

    for (const auto& [music_type, pair] : Map::distr_map) {
        const auto& [tracks, clear_list]{ pair };
        const auto music_type_name{ music_type->GetFormEditorID() };
        const auto music_type_form_id{ music_type->GetFormID() };
        if (clear_list) {
            logger::info("Clearing {} (0x{:x})", music_type_name, music_type_form_id);
            music_type->tracks.clear();
        }
        for (const auto& track : tracks) {
            logger::info("Distributing track 0x{:x} to music type {} (0x{:x})", track->GetFormID(), music_type_name, music_type_form_id);
            music_type->tracks.push_back(track);
        }
    }

    logger::info("");
    logger::info(">-------------------------------------------------- Finished distribution --------------------------------------------------<");
    logger::info("");
}

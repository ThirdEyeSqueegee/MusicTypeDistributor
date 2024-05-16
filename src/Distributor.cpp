#include "Distributor.h"
#include "Map.h"

void Distributor::Distribute() noexcept
{
    logger::info(">------------------------------ Distributing... ------------------------------<");
    logger::info("");

    const auto start_time{ std::chrono::system_clock::now() };

    for (const auto& [music_type, pair] : Map::distr_map) {
        const auto& [tracks, clear_list]{ pair };
        const auto music_type_name{ music_type->GetFormEditorID() };
        const auto music_type_form_id{ music_type->GetFormID() };
        if (clear_list) {
            logger::debug("Clearing {} (0x{:x})", music_type_name, music_type_form_id);
            music_type->tracks.clear();
        }
        for (const auto& track : tracks) {
            logger::info("Distributing track {} (0x{:x}) to music type {} (0x{:x})", track->GetFormEditorID(), track->GetFormID(), music_type_name, music_type_form_id);
            music_type->tracks.push_back(track);
        }
    }

    const auto elapsed{ std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now() - start_time) };
    logger::info("");
    logger::info(">------------------------------ Finished distribution in {} us ------------------------------<", elapsed.count());
    logger::info("");
}

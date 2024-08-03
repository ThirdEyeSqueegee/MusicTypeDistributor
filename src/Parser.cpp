#include "Parser.h"

#include "Map.h"
#include "Utility.h"

void Parser::ParseINIs(CSimpleIniA& ini) noexcept
{
    const std::filesystem::path data_dir{ R"(.\Data)" };
    const auto                  pattern{ L"_MUS.ini" };

    if (!exists(data_dir)) {
        logger::error("ERROR: Failed to find Data directory");
        stl::report_and_fail(fmt::format("{}: Failed to find Data directory", SKSE::PluginDeclaration::GetSingleton()->GetName()));
    }

    logger::info(">------------------------------------------------ Parsing _MUS.ini files... ------------------------------------------------<");
    logger::info("");

    std::set<std::filesystem::path> mus_inis;
    for (std::error_code ec{}; const auto& file : std::filesystem::directory_iterator{ data_dir, ec }) {
        if (ec.value()) {
            logger::debug("ERROR CODE: {}", ec.value());
            continue;
        }

        const auto& path{ file.path() };

        if (path.extension() != ".ini") {
            continue;
        }

        const auto filename{ path.filename() };
        const auto filename_str{ filename.string() };
        const auto filename_w{ filename.wstring() };

        if (!filename_w.ends_with(pattern)) {
            continue;
        }

        if (mus_inis.contains(path)) {
            logger::warn("WARNING: Found duplicate _MUS.ini file: {}", filename_str);
            continue;
        }

        mus_inis.insert(path);
    }

    std::regex comma_space{ ", " };

    for (const auto& f : mus_inis) {
        const auto filename{ f.filename().string() };

        logger::info("Loading config file: {}", filename);

        ini.LoadFile(f.wstring().data());

        CSimpleIniA::TNamesDepend keys{};
        ini.GetAllKeys("General", keys);

        for (const auto& k : keys) {
            CSimpleIniA::TNamesDepend values{};
            ini.GetAllValues("General", k.pItem, values);
            for (const auto& v : values) {
                const auto trimmed{ std::regex_replace(std::string{ v.pItem }, comma_space, ",") };
                Map::prep_map[k.pItem].insert(trimmed);
                logger::debug("Added [{}: {}] to prep map", k.pItem, trimmed);
            }
        }

        if (ini.SectionExists("Location")) {
            CSimpleIniA::TNamesDepend loc_keys{};
            ini.GetAllKeys("Location", loc_keys);

            for (const auto& k : loc_keys) {
                CSimpleIniA::TNamesDepend values{};
                ini.GetAllValues("Location", k.pItem, values);
                for (const auto& v : values) {
                    Map::location_prep_map[k.pItem] = v.pItem;
                    logger::debug("Added [{}: {}] to location prep map", k.pItem, v.pItem);
                }
            }
        }

        if (ini.SectionExists("Region")) {
            CSimpleIniA::TNamesDepend reg_keys{};
            ini.GetAllKeys("Region", reg_keys);

            for (const auto& k : reg_keys) {
                CSimpleIniA::TNamesDepend values{};
                ini.GetAllValues("Region", k.pItem, values);
                for (const auto& v : values) {
                    Map::region_prep_map[k.pItem] = v.pItem;
                    logger::debug("Added [{}: {}] to region prep map", k.pItem, v.pItem);
                }
            }
        }

        ini.Reset();
    }

    logger::info("");
    logger::info(">--------------------------------------------- Finished parsing _MUS.ini files ---------------------------------------------<");
    logger::info("");
}

void Parser::PrepareDistrMap() noexcept
{
    logger::info(">------------------------------------------------ Preparing Distribution... ------------------------------------------------<");
    logger::info("");

    for (const auto& [k, v] : Map::prep_map) {
        auto k_copy{ k };
        bool clear_list{};
        if (k_copy.back() == '!') {
            k_copy     = k_copy.substr(0, k_copy.size() - 1);
            clear_list = true;
        }
        if (const auto music_type{ RE::TESForm::LookupByEditorID<RE::BGSMusicType>(k_copy) }) {
            logger::info("Found music type {} (0x{:x})", music_type->formEditorID, music_type->GetFormID());
            const auto tokens{ Utility::Split(v) };
            const auto form_id_vec{ Utility::BuildFormIDVec(tokens) };
            Map::distr_map[music_type] = { form_id_vec, clear_list };
        }
    }

    for (const auto& [k, v] : Map::location_prep_map) {
        if (const auto location{ RE::TESForm::LookupByEditorID<RE::BGSLocation>(k) }) {
            logger::info("Found location {} (0x{:x}) in {}", Utility::GetFormEditorID(location), location->GetFormID(), location->GetFile()->GetFilename());
            if (const auto music_type{ RE::TESForm::LookupByEditorID<RE::BGSMusicType>(v) }) {
                logger::info("\tFound music type {} (0x{:x})", music_type->formEditorID, music_type->GetFormID());
                Map::location_map[location] = music_type;
            }
        }
    }

    for (const auto& [k, v] : Map::region_prep_map) {
        if (const auto region{ RE::TESForm::LookupByEditorID<RE::TESRegion>(k) }) {
            logger::info("Found region {} (0x{:x}) in {}", Utility::GetFormEditorID(region), region->GetFormID(), region->GetFile()->GetFilename());
            if (const auto music_type{ RE::TESForm::LookupByEditorID<RE::BGSMusicType>(v) }) {
                logger::info("\tFound music type {} (0x{:x})", music_type->formEditorID, music_type->GetFormID());
                Map::region_map[region] = music_type;
            }
        }
    }

    logger::info("");
    logger::info(">-------------------------------------- Finished preparing distribution. Map size: {} --------------------------------------<", Map::distr_map.size());
    logger::info("");
}

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

    for (const auto& f : mus_inis) {
        const auto filename{ f.filename().string() };

        logger::info("Loading config file: {}", filename);

        ini.LoadFile(f.wstring().data());

        CSimpleIniA::TNamesDepend keys{};
        ini.GetAllKeys("General", keys);

        logger::debug("");
        logger::debug("{} has {} keys", filename, keys.size());

        for (const auto& k : keys) {
            CSimpleIniA::TNamesDepend values{};
            ini.GetAllValues("General", k.pItem, values);
            for (const auto& v : values) {
                Map::prep_map[k.pItem].insert(v.pItem);
                logger::debug("Added [{}: {}] to prep map", k.pItem, v.pItem);
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
        logger::debug("Processing key {} for distribution map", k);
        auto k_copy{ k };
        bool clear_list{};
        if (k_copy.back() == '!') {
            k_copy     = k_copy.substr(0, k_copy.size() - 1);
            clear_list = true;
            logger::debug("Clearing list for {}", k_copy);
        }
        logger::debug("Looking up {}", k_copy);
        if (const auto music_type{ RE::TESForm::LookupByEditorID<RE::BGSMusicType>(k_copy) }) {
            logger::info("Found music type {} (0x{:x})", music_type->formEditorID, music_type->GetFormID());
            const auto tokens{ Utility::Split(v) };
            const auto form_id_vec{ Utility::BuildFormIDVec(tokens) };
            Map::distr_map[music_type] = { form_id_vec, clear_list };
        }
    }

    logger::info(">-------------------------------------- Finished preparing distribution. Map size: {} --------------------------------------<", Map::distr_map.size());
    logger::info("");
}

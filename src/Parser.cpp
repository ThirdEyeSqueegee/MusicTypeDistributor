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

    logger::info(">------------------------------ Parsing _MUS.ini files... ------------------------------<");
    logger::info("");

    const auto start_time{ std::chrono::system_clock::now() };
    for (std::error_code ec{}; const auto& file : std::filesystem::directory_iterator{ data_dir, ec }) {
        if (ec.value()) {
            logger::debug("ERROR CODE: {}", ec.value());
            continue;
        }

        if (file.path().extension() != ".ini") {
            continue;
        }

        const auto& filepath{ file.path() };
        const auto  filename{ filepath.filename() };
        const auto  filename_cstr{ filepath.filename().c_str() };
        if (!std::wcsstr(filename_cstr, pattern)) {
            continue;
        }

        if (const auto underscore{ std::wcschr(filename_cstr, '_') }; underscore && std::wcscmp(underscore, pattern) != 0) {
            continue;
        }

        logger::info("Loading config file: {}", filename.string());

        ini.LoadFile(filepath.c_str());

        CSimpleIniA::TNamesDepend keys{};
        ini.GetAllKeys("General", keys);

        logger::debug("");
        logger::debug("{} has {} keys", filename.string(), keys.size());

        for (const auto& [key, key_count, key_order] : keys) {
            logger::debug("Key {} occurs {} times", key, key_count);
            CSimpleIniA::TNamesDepend values{};
            ini.GetAllValues("General", key, values);
            for (const auto& [value, value_count, value_order] : values) {
                Map::prep_map[key].insert(value);
                logger::debug("Added [{}: {}] to prep map", key, value);
            }
        }
        ini.Reset();
    }

    const auto elapsed{ std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now() - start_time) };
    logger::info("");
    logger::info(">------------------------------ Finished parsing _MUS.ini files in {} us ------------------------------<", elapsed.count());
    logger::info("");
}

void Parser::PrepareDistrMap() noexcept
{
    logger::info(">------------------------------ Preparing Distribution... ------------------------------<");
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
            logger::debug("Found music type {} (0x{:x})", music_type->formEditorID, music_type->GetFormID());
            const auto tokens{ Utility::Split(v) };
            const auto form_id_vec{ Utility::BuildFormIDVec(tokens) };
            Map::distr_map[music_type] = { form_id_vec, clear_list };
        }
    }

    logger::info("Finished preparing distribution. Map size: {}", Map::distr_map.size());
    logger::info("");
}

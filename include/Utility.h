#pragma once

struct FormIDAndPluginName
{
    RE::FormID  form_id{};
    std::string plugin_name{};
};

class Utility : public Singleton<Utility>
{
public:
    static auto Join(const std::unordered_set<std::string>& strings) noexcept
    {
        std::string result{};

        for (const auto& s : strings | std::ranges::views::join_with(',')) {
            result += s;
        }

        return result;
    }

    static auto Split(const std::unordered_set<std::string>& strings) noexcept
    {
        auto s{ Join(strings) };

        std::vector<std::string> result{};

        std::size_t pos{};
        while ((pos = s.find(',')) != std::string::npos) {
            result.emplace_back(s.substr(0, pos));
            s.erase(0, pos + 1);
        }

        return result;
    }

    static auto ToFormID(const std::string_view& s) noexcept { return static_cast<RE::FormID>(std::stoul(s.data(), nullptr, 16)); }

    static FormIDAndPluginName GetFormIDAndPluginName(const std::string& identifier) noexcept
    {
        if (const auto tilde{ identifier.find('~') }; tilde != std::string_view::npos) {
            return { ToFormID(identifier.substr(0, tilde)), identifier.substr(tilde + 1) };
        }
        logger::error("ERROR: Failed to get FormID and plugin name for {}", identifier);

        return { 0, "" };
    }

    static auto BuildFormIDVec(const std::vector<std::string>& tokens) noexcept
    {
        std::vector<RE::BGSMusicTrackFormWrapper*> result;

        for (const auto& token : tokens) {
            logger::debug("\tProcessing token: {}", token);
            const auto [form_id, plugin_name]{ GetFormIDAndPluginName(token) };
            if (const auto handler{ RE::TESDataHandler::GetSingleton() }) {
                if (const auto music_track{ handler->LookupForm<RE::BGSMusicTrackFormWrapper>(form_id, plugin_name) }) {
                    logger::info("\t\tFound music track 0x{:x}", music_track->GetFormID());
                    result.emplace_back(music_track);
                }
            }
        }

        return result;
    }
};

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
        logger::debug("Splitting string: {}", s);

        std::vector<std::string> result{};

        auto pos{ s.find(',') };
        while (pos != std::string::npos) {
            result.emplace_back(s.substr(0, pos));
            s   = s.substr(pos + 1);
            pos = s.find(',', pos);
        }

        if (!s.empty()) {
            result.emplace_back(s);
        }

        logger::debug("Split result: {} items", result.size());
        for (const auto& r : result) {
            logger::debug("\tSplit item: {}", r);
        }

        return result;
    }

    static auto ToUnsignedInt(const std::string_view& s) noexcept { return static_cast<unsigned>(std::stoul(s.data(), nullptr, 0)); }

    static FormIDAndPluginName GetFormIDAndPluginName(const std::string_view identifier) noexcept
    {
        if (const auto tilde{ identifier.find('~') }; tilde != std::string_view::npos) {
            return { ToUnsignedInt(identifier.substr(0, tilde)), identifier.substr(tilde + 1).data() };
        }
        logger::error("ERROR: Failed to get FormID and plugin name for {}", identifier);

        return { 0, "" };
    }

    static auto BuildFormIDVec(const std::vector<std::string>& tokens) noexcept
    {
        std::vector<RE::BGSMusicTrackFormWrapper*> result;

        for (const auto& token : tokens) {
            logger::debug("Processing token {}", token);
            const auto [form_id, plugin_name]{ GetFormIDAndPluginName(token) };
            if (const auto handler{ RE::TESDataHandler::GetSingleton() }) {
                if (const auto music_track{ handler->LookupForm<RE::BGSMusicTrackFormWrapper>(form_id, plugin_name) }) {
                    logger::debug("Found music track 0x{:x}", music_track->GetFormID());
                    result.emplace_back(music_track);
                }
            }
        }

        return result;
    }
};

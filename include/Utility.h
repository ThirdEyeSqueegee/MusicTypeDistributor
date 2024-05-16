#pragma once

struct FormIDAndPluginName
{
    RE::FormID  form_id{};
    std::string plugin_name{};
};

class Utility : public Singleton<Utility>
{
public:
    static std::vector<std::string> Split(const std::string_view str)
    {
        std::vector<std::string>    result;
        std::string_view::size_type pos{};
        std::string_view::size_type prev{};

        while ((pos = str.find_first_of(',', prev)) != std::string_view::npos) {
            if (pos > prev) {
                result.emplace_back(str.substr(prev, pos - prev));
            }
            prev = pos + 1;
        }
        if (prev < str.size()) {
            result.emplace_back(str.substr(prev));
        }

        return result;
    }

    static auto GetPos(const std::string_view s, const char c) noexcept
    {
        const auto ptr{ std::strrchr(s.data(), c) };

        return ptr ? static_cast<std::size_t>(ptr - s.data()) : ULLONG_MAX;
    }

    static auto ToUnsignedInt(const std::string_view& s) noexcept { return static_cast<unsigned>(std::strtol(s.data(), nullptr, 0)); }

    static FormIDAndPluginName GetFormIDAndPluginName(const std::string_view identifier)
    {
        if (const auto tilde{ std::strchr(identifier.data(), '~') }) {
            const auto tilde_pos{ static_cast<int>(tilde - identifier.data()) };
            return { ToUnsignedInt(identifier.substr(0, tilde_pos)), identifier.substr(tilde_pos + 1).data() };
        }
        logger::error("ERROR: Failed to get FormID and plugin name for {}", identifier);

        return { 0, "" };
    }

    static std::vector<RE::BGSMusicTrackFormWrapper*> BuildFormIDVec(const std::vector<std::string>& tokens)
    {
        std::vector<RE::BGSMusicTrackFormWrapper*> result;

        for (const auto& token : tokens) {
            logger::debug("Processing token {}", token);
            const auto [form_id, plugin_name]{ GetFormIDAndPluginName(token) };
            if (const auto handler{ RE::TESDataHandler::GetSingleton() }) {
                if (const auto music_track{ handler->LookupForm<RE::BGSMusicTrackFormWrapper>(form_id, plugin_name) }) {
                    logger::debug("Found music track {} (0x{:x})", music_track->GetFormEditorID(), music_track->GetFormID());
                    result.emplace_back(music_track);
                }
            }
        }

        return result;
    }
};

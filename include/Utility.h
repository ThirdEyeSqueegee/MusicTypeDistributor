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

    static std::string GetFormEditorID(const RE::TESForm* form) noexcept
    {
        using TGetFormEditorID = const char* (*)(std::uint32_t);

        switch (form->GetFormType()) {
        case RE::FormType::Keyword:
        case RE::FormType::LocationRefType:
        case RE::FormType::Action:
        case RE::FormType::MenuIcon:
        case RE::FormType::Global:
        case RE::FormType::HeadPart:
        case RE::FormType::Race:
        case RE::FormType::Sound:
        case RE::FormType::Script:
        case RE::FormType::Navigation:
        case RE::FormType::Cell:
        case RE::FormType::WorldSpace:
        case RE::FormType::Land:
        case RE::FormType::NavMesh:
        case RE::FormType::Dialogue:
        case RE::FormType::Quest:
        case RE::FormType::Idle:
        case RE::FormType::AnimatedObject:
        case RE::FormType::ImageAdapter:
        case RE::FormType::VoiceType:
        case RE::FormType::Ragdoll:
        case RE::FormType::DefaultObject:
        case RE::FormType::MusicType:
        case RE::FormType::StoryManagerBranchNode:
        case RE::FormType::StoryManagerQuestNode:
        case RE::FormType::StoryManagerEventNode:
        case RE::FormType::SoundRecord:
            return form->GetFormEditorID();
        default: {
            static auto tweaks{ REX::W32::GetModuleHandleW(L"po3_Tweaks") };
            static auto func{ reinterpret_cast<TGetFormEditorID>(GetProcAddress(tweaks, "GetFormEditorID")) };
            if (func) {
                return func(form->formID);
            }
            return "";
        }
        }
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
            const auto [form_id, plugin_name]{ GetFormIDAndPluginName(token) };
            if (const auto handler{ RE::TESDataHandler::GetSingleton() }) {
                if (const auto music_track{ handler->LookupForm<RE::BGSMusicTrackFormWrapper>(form_id, plugin_name) }) {
                    logger::info("\tFound music track {} (0x{:x})", GetFormEditorID(music_track), music_track->GetFormID());
                    result.emplace_back(music_track);
                }
            }
        }

        return result;
    }

    static auto DumpMusicTracks() noexcept
    {
        logger::info(">-------------------------------------------------- Dumping music tracks... ------------------------------------------------<");
        logger::info("");
        const auto& [map, lock_ref]{ RE::TESForm::GetAllForms() };
        auto lock{ lock_ref.get() };
        auto count{ 0 };
        lock.LockForRead();
        for (const auto& [form_id, form] : *map) {
            if (form->Is(RE::FormType::MusicTrack)) {
                count++;
                logger::info("\t[{}] 0x{:x}~{}", GetFormEditorID(form), form_id, form->GetFile()->GetFilename());
            }
        }
        lock.UnlockForRead();
        logger::info("");
        logger::info(">-------------------------------------------------- Dumped {} music tracks -------------------------------------------------<", count);
        logger::info("");
    }

    static auto DumpMusicTypes() noexcept
    {
        logger::info(">-------------------------------------------------- Dumping music types... -------------------------------------------------<");
        logger::info("");
        const auto& [map, lock_ref]{ RE::TESForm::GetAllForms() };
        auto lock{ lock_ref.get() };
        auto count{ 0 };
        lock.LockForRead();
        for (const auto& [form_id, form] : *map) {
            if (form->Is(RE::FormType::MusicType)) {
                count++;
                logger::info("\t[{}] 0x{:x}~{}", GetFormEditorID(form), form_id, form->GetFile()->GetFilename());
            }
        }
        lock.UnlockForRead();
        logger::info("");
        logger::info(">-------------------------------------------------- Dumped {} music types --------------------------------------------------<", count);
        logger::info("");
    }

    static auto DumpLocations() noexcept
    {
        logger::info(">--------------------------------------------------- Dumping locations... --------------------------------------------------<");
        logger::info("");
        const auto& [map, lock_ref]{ RE::TESForm::GetAllForms() };
        auto lock{ lock_ref.get() };
        auto count{ 0 };
        lock.LockForRead();
        for (const auto& [form_id, form] : *map) {
            if (form->Is(RE::FormType::Location)) {
                count++;
                logger::info("\t[{}] 0x{:x}~{}", GetFormEditorID(form), form_id, form->GetFile()->GetFilename());
            }
        }
        lock.UnlockForRead();
        logger::info("");
        logger::info(">--------------------------------------------------- Dumped {} locations ---------------------------------------------------<", count);
        logger::info("");
    }

    static auto DumpRegions() noexcept
    {
        logger::info(">---------------------------------------------------- Dumping regions... ---------------------------------------------------<");
        logger::info("");
        const auto& [map, lock_ref]{ RE::TESForm::GetAllForms() };
        auto lock{ lock_ref.get() };
        auto count{ 0 };
        lock.LockForRead();
        for (const auto& [form_id, form] : *map) {
            if (form->Is(RE::FormType::Region)) {
                count++;
                logger::info("\t[{}] 0x{:x}~{}", GetFormEditorID(form), form_id, form->GetFile()->GetFilename());
            }
        }
        lock.UnlockForRead();
        logger::info("");
        logger::info(">---------------------------------------------------- Dumped {} regions ----------------------------------------------------<", count);
        logger::info("");
    }
};

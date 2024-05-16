#pragma once

class Parser : public Singleton<Parser>
{
public:
    static void ParseINIs(CSimpleIniA& ini) noexcept;

    static void PrepareDistrMap() noexcept;
};

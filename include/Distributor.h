#pragma once

class Distributor : public Singleton<Distributor>
{
public:
    static void Distribute() noexcept;
};

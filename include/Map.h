#pragma once

#include "parallel_hashmap/phmap.h"

class Map : public Singleton<Map>
{
public:
    using TPrepMapType  = phmap::parallel_flat_hash_map<std::string, std::unordered_set<std::string>>;
    using TMapPair      = std::pair<std::vector<RE::BGSMusicTrackFormWrapper*>, bool>;
    using TDistrMapType = phmap::parallel_flat_hash_map<RE::BGSMusicType*, TMapPair>;

    inline static TPrepMapType prep_map;

    inline static TDistrMapType distr_map;
};

#pragma once

#include "parallel_hashmap/phmap.h"

class Map : public Singleton<Map>
{
public:
    using TPrepMap = phmap::parallel_flat_hash_map<std::string, std::unordered_set<std::string>>;

    using TMapPair  = std::pair<std::vector<RE::BGSMusicTrackFormWrapper*>, bool>;
    using TDistrMap = phmap::parallel_flat_hash_map<RE::BGSMusicType*, TMapPair>;

    using TLocationRegionPrepMap = phmap::parallel_flat_hash_map<std::string, std::string>;

    using TLocationMap = phmap::parallel_flat_hash_map<RE::BGSLocation*, RE::BGSMusicType*>;

    using TRegionMap = phmap::parallel_flat_hash_map<RE::TESRegion*, RE::BGSMusicType*>;

    inline static TPrepMap prep_map;

    inline static TDistrMap distr_map;

    inline static TLocationRegionPrepMap location_prep_map;

    inline static TLocationMap location_distr_map;

    inline static TLocationRegionPrepMap region_prep_map;

    inline static TRegionMap region_distr_map;
};

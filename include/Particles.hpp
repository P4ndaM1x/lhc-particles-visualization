#pragma once
#include "utils.hpp"

class Particles {
public:
    struct ParticleProperties {
        float phi, pt, q;
    };
    struct SpacepointsMinMax {
        sf::Vector3f minCoordValue, maxCoordValue;
        ParticleProperties minPropertyValue, maxPropertyValue;
    };

    using spacepoints_t = std::vector<std::pair<unsigned, sf::Vector3f>>;
    using properties_t = std::unordered_map<unsigned, ParticleProperties>;
    using directions_t = std::unordered_map<unsigned, sf::Vector3f>;

    Particles(const std::string& spacepointsFilepath, const std::string& propertiesFilepath);

    const auto& getAllSpacepoints() const { return spacepoints; }
    const auto& getAllProperties() const { return properties; }
    const auto& getAllDirections() const { return directions; }
    const auto& getStats() const { return stats; }
    void resetFilters() { filters = stats; }

    void resetFilter(const char filterName);
    const spacepoints_t getFilteredSpacepoints(const std::optional<std::vector<unsigned>> eventIDs = std::nullopt) const;

    void initDataVectors();
    void calculateDataStats();

    SpacepointsMinMax filters;

private:
    bool applyFilters(const Particles::spacepoints_t::value_type& pair) const;

    SpacepointsMinMax stats;

    spacepoints_t spacepoints {};
    properties_t properties {};
    directions_t directions {};

    const std::string spacepointsFilepath;
    const std::string propertiesFilepath;
};

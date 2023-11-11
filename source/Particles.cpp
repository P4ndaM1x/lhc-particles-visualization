#include "../pch.h"

#include "../include/Particles.hpp"

#include <ranges>

// struct Particles::ParticleProperties {
//     float phi, pt, q;
// };
//
// struct Particles::SpacepointsFilters {
//     sf::Vector3f minXYZ, maxXYZ;
//     ParticleProperties minProperties, maxProperties;
// };

Particles::Particles(const std::string& spacepointsFilepath, const std::string& propertiesFilepath)
    : spacepointsFilepath { spacepointsFilepath }
    , propertiesFilepath { propertiesFilepath }
{
    initDataVectors();
    calculateDataStats();
    resetFilters();
}

void Particles::initDataVectors()
{
    const size_t spacepointsLength = csv::get_file_info(spacepointsFilepath).n_rows;
    spacepoints.reserve(spacepointsLength);

    csv::CSVReader spacepointsReader { spacepointsFilepath };
    for (csv::CSVRow& row : spacepointsReader) {
        spacepoints.push_back(std::make_pair(
            row["event_id"].get<unsigned>(),
            sf::Vector3f {
                row["x"].get<float>(),
                row["y"].get<float>(),
                row["z"].get<float>() }));
    }

    const size_t propertiesLength = csv::get_file_info(propertiesFilepath).n_rows;
    properties.reserve(propertiesLength);

    csv::CSVReader propertiesReader { propertiesFilepath };
    for (csv::CSVRow& row : propertiesReader) {
        properties[row["event_id"].get<unsigned>()] = ParticleProperties {
            row["phi"].get<float>(), row["pt"].get<float>(), row["q"].get<float>()
        };
    }
}

void Particles::calculateDataStats()
{
    auto getColumnIndex = [](csv::CSVStat& csvStats, const std::string& columnName) {
        auto colNames = csvStats.get_col_names();
        return std::distance(colNames.begin(), std::find(colNames.begin(), colNames.end(), columnName));
    };

    csv::CSVStat spacepointsStats { spacepointsFilepath };
    auto spacepointsMin = spacepointsStats.get_mins(), spacepointsMax = spacepointsStats.get_maxes();

    stats.minCoordValue.x = spacepointsMin[getColumnIndex(spacepointsStats, "x")];
    stats.minCoordValue.y = spacepointsMin[getColumnIndex(spacepointsStats, "y")];
    stats.minCoordValue.z = spacepointsMin[getColumnIndex(spacepointsStats, "z")];

    stats.maxCoordValue.x = spacepointsMax[getColumnIndex(spacepointsStats, "x")];
    stats.maxCoordValue.y = spacepointsMax[getColumnIndex(spacepointsStats, "y")];
    stats.maxCoordValue.z = spacepointsMax[getColumnIndex(spacepointsStats, "z")];

    csv::CSVStat propertiesStats { propertiesFilepath };
    auto propertiesMin = propertiesStats.get_mins(), propertiesMax = propertiesStats.get_maxes();

    stats.minPropertyValue.phi = propertiesMin[getColumnIndex(propertiesStats, "phi")];
    stats.minPropertyValue.pt = propertiesMin[getColumnIndex(propertiesStats, "pt")];
    stats.minPropertyValue.q = propertiesMin[getColumnIndex(propertiesStats, "q")];

    stats.maxPropertyValue.phi = propertiesMax[getColumnIndex(propertiesStats, "phi")];
    stats.maxPropertyValue.pt = propertiesMax[getColumnIndex(propertiesStats, "pt")];
    stats.maxPropertyValue.q = propertiesMax[getColumnIndex(propertiesStats, "q")];
}

bool Particles::applyFilters(const Particles::spacepoints_t::value_type& pair) const
{
    const auto& coords = pair.second;
    bool isXinRange = coords.x >= filters.minCoordValue.x and coords.x <= filters.maxCoordValue.x;
    bool isYinRange = coords.y >= filters.minCoordValue.y and coords.y <= filters.maxCoordValue.y;
    bool isZinRange = coords.z >= filters.minCoordValue.z and coords.z <= filters.maxCoordValue.z;

    const auto& props = properties.at(pair.first);
    bool isPhiInRange = props.phi >= filters.minPropertyValue.phi and props.phi <= filters.maxPropertyValue.phi;
    bool isPtInRange = props.pt >= filters.minPropertyValue.pt and props.pt <= filters.maxPropertyValue.pt;
    bool isQinRange = props.q >= filters.minPropertyValue.q and props.q <= filters.maxPropertyValue.q;

    return isXinRange and isYinRange and isZinRange and isPhiInRange and isPtInRange and isQinRange;
}

const Particles::spacepoints_t Particles::getFilteredSpacepoints() const
{
    Particles::spacepoints_t filteredSpacepoints;
    filteredSpacepoints.reserve(spacepoints.size());
    std::copy_if(spacepoints.begin(), spacepoints.end(), std::back_inserter(filteredSpacepoints),
        std::bind(std::mem_fn(&Particles::applyFilters), this, std::placeholders::_1));
    return filteredSpacepoints;
}

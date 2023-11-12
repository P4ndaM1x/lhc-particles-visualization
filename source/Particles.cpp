#include "../pch.h"

#include "../include/Particles.hpp"

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
    directions.reserve(propertiesLength);

    csv::CSVReader propertiesReader { propertiesFilepath };
    for (csv::CSVRow& row : propertiesReader) {
        properties[row["event_id"].get<unsigned>()] = ParticleProperties {
            row["phi"].get<float>(), row["pt"].get<float>(), row["q"].get<float>()
        };
        directions[row["event_id"].get<unsigned>()] = sf::Vector3f {
            row["px"].get<float>(), row["py"].get<float>(), row["pz"].get<float>()
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

const Particles::spacepoints_t Particles::getFilteredSpacepoints(const std::optional<std::vector<unsigned>> eventIDs) const
{
    Particles::spacepoints_t& spacepointsPool = const_cast<Particles::spacepoints_t&>(spacepoints);

    Particles::spacepoints_t specificEventIDspacepoints;
    Particles::spacepoints_t filteredSpacepoints;

    if (eventIDs.has_value()) {
        std::copy_if(spacepoints.begin(), spacepoints.end(), std::back_inserter(specificEventIDspacepoints),
            [&eventIDs](const Particles::spacepoints_t::value_type& pair) { return std::any_of(eventIDs.value().begin(), eventIDs.value().end(), [&pair](const auto& eventID) { return pair.first == eventID; }); });
        spacepointsPool = specificEventIDspacepoints;
    }
    filteredSpacepoints.reserve(spacepointsPool.size());

    std::copy_if(spacepointsPool.begin(), spacepointsPool.end(), std::back_inserter(filteredSpacepoints),
        std::bind(std::mem_fn(&Particles::applyFilters), this, std::placeholders::_1));
    return filteredSpacepoints;
}

void Particles::resetFilter(const char filterName)
{
    switch (filterName) {
    case 'x':
        filters.minCoordValue.x = stats.minCoordValue.x;
        filters.maxCoordValue.x = stats.maxCoordValue.x;
        break;
    case 'y':
        filters.minCoordValue.y = stats.minCoordValue.y;
        filters.maxCoordValue.y = stats.maxCoordValue.y;
        break;
    case 'z':
        filters.minCoordValue.z = stats.minCoordValue.z;
        filters.maxCoordValue.z = stats.maxCoordValue.z;
        break;
    case 'h':
        filters.minPropertyValue.phi = stats.minPropertyValue.phi;
        filters.maxPropertyValue.phi = stats.maxPropertyValue.phi;
        break;
    case 't':
        filters.minPropertyValue.pt = stats.minPropertyValue.pt;
        filters.maxPropertyValue.pt = stats.maxPropertyValue.pt;
        break;
    case 'q':
        filters.minPropertyValue.q = stats.minPropertyValue.q;
        filters.maxPropertyValue.q = stats.maxPropertyValue.q;
        break;
    default:
        break;
    }
}

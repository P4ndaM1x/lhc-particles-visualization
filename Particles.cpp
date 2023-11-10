#include "Particles.hpp"
#include "pch.h"

Particles::Particles(const std::string& spacepoints_filepath, const std::string& properties_filepath)
    : spacepoints_filepath { spacepoints_filepath }
    , properties_filepath { properties_filepath }
{
    initDataVectors();
}

void Particles::initDataVectors()
{
    const size_t spacepoints_len = csv::get_file_info(spacepoints_filepath).n_rows;
    spacepoints.reserve(spacepoints_len);

    csv::CSVReader spacepoints_reader(spacepoints_filepath);
    for (csv::CSVRow& row : spacepoints_reader) {
        spacepoints.push_back(std::make_pair(
            row["event_id"].get<unsigned>(),
            sf::Vector3f {
                row["x"].get<float>(),
                row["y"].get<float>(),
                row["z"].get<float>() }));
    }

    const size_t properties_len = csv::get_file_info(properties_filepath).n_rows;
    properties.reserve(properties_len);

    csv::CSVReader properties_reader(properties_filepath);
    for (csv::CSVRow& row : properties_reader) {
        properties[row["event_id"].get<unsigned>()] = utils::ParticleProperties {
            row["phi"].get<float>(), row["pt"].get<float>(), row["q"].get<float>()
        };
    }
}
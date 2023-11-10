#pragma once

#include "utils.hpp"

class Particles {
public:
    Particles(const std::string& spacepoints_filepath, const std::string& properties_filepath);

    void initDataVectors();

    std::vector<std::pair<unsigned, sf::Vector3f>> spacepoints {};
    std::unordered_map<unsigned, utils::ParticleProperties> properties {};

private:
    const std::string spacepoints_filepath;
    const std::string properties_filepath;
};

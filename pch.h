// pch.h: This is a precompiled header file.
// Files listed below are compiled only once, improving build performance for future builds.
// This also affects IntelliSense performance, including code completion and many code browsing features.
// However, files listed here are ALL re-compiled if any one of them is updated between builds.
// Do not add files here that you will be updating frequently as this negates the performance advantage.

#ifndef PCH_H
#define PCH_H

#include <SFML\Window.hpp>
#include <SFML\Graphics.hpp>
#include <SFML\OpenGL.hpp>
#include <gl\GLU.h>
#include "imgui/imgui.h" 
#include "imgui/imgui-SFML.h"
#include "csv-parser-master/single_include/csv.hpp"

#include <iostream>
#include <algorithm>
#include <numbers>
#include <unordered_map>

constexpr float minFloatValue = std::numeric_limits<float>::min();
constexpr float maxFloatValue = std::numeric_limits<float>::max();

#endif //PCH_H

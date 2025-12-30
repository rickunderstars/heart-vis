#pragma once

#include "utils.hpp"

#include <string>
#include <vector>

inline constexpr int NULL_VALUE = -99999;

inline const std::vector<std::string> validQualities = {
	"unipolar", "bipolar", "lat", "eml", "exteml", "scar"};

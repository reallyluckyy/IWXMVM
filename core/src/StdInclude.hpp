#pragma once

#include <cstdio>
#include <cstdint>
#include <string>
#include <map>
#include <numeric>
#include <vector>
#include <functional>
#include <fstream>
#include <filesystem>

#include <d3d9.h>
#include <Windows.h>

#include "Logger.hpp"

#pragma warning(push, 0)
#include "imgui.h"
#include "backends/imgui_impl_dx9.h"
#include "backends/imgui_impl_win32.h"
#pragma warning(pop)

#include <d3dx9tex.h>

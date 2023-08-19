#pragma once

#include <algorithm>
#include <array>
#include <atomic>
#include <cstdint>
#include <cstdio>
#include <cwctype>
#include <filesystem>
#include <fstream>
#include <functional>
#include <map>
#include <memory>
#include <numeric>
#include <span>
#include <string>
#include <string_view>
#include <vector>

#include <d3d9.h>
#pragma comment(lib, "d3d9.lib")
#include <d3dx9.h>
#pragma comment(lib, "d3dx9.lib")
#include <d3dx9tex.h>

#include <Windows.h>
#include <psapi.h>

#include "Logger.hpp"

#include "Types/Vector2.hpp"
#include "Types/Vector3.hpp"

#pragma warning(push, 0)
#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui.h"
#include "backends/imgui_impl_dx9.h"
#include "backends/imgui_impl_win32.h"
#pragma warning(pop)

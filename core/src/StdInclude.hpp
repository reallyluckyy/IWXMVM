#pragma once

#define _CRT_SECURE_NO_WARNINGS

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
#include <random>
#include <span>
#include <string>
#include <string_view>
#include <vector>
#include <variant>
#include <stack>
#include <chrono>

#include <initguid.h>
#include <d3d9.h>
#pragma comment(lib, "d3d9.lib")
#include <d3dx9.h>
#pragma comment(lib, "d3dx9.lib")
#include <d3dx9tex.h>

#include <Windows.h>
#include <psapi.h>
#include <shobjidl.h>
#include <Propkey.h>

#include "Logger.hpp"

#include "glm/glm.hpp"
#include "glm/ext.hpp"
#include "glm/gtx/euler_angles.hpp"
#include "glm/gtx/scalar_multiplication.hpp"
#include "glm/gtx/vector_angle.hpp"
#include "glm/gtx/rotate_vector.hpp"
#include "glm/gtx/spline.hpp"
#include "glm/gtx/intersect.hpp"
#include "Utilities/GLMExtensions.hpp"

#pragma warning(push, 0)
#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui.h"
#include "backends/imgui_impl_dx9.h"
#include "backends/imgui_impl_win32.h"
#pragma warning(pop)

#include "magic_enum/magic_enum.hpp"

#include "FastNoiseLite.h"

#pragma once

#include <string>
#include <unordered_map>
#include <map>
#include <optional>
#include <source_location>
#include <condition_variable>
#include <memory>
#include <filesystem>
#include <thread>
#include <mutex>
#include <type_traits>
#include <typeindex>
#include <algorithm>

#include "SDL3/SDL.h"
#include "Assert.h"
#include "Log.h"

#include "SimpleMath.h"
namespace DXSM = DirectX::SimpleMath;
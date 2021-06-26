#pragma once

#include <filesystem>

namespace mozjs::hack
{

/// @brief This is a hack, don't use it unless it's REALLY necessary
/// @throw qwr::QwrException
/// @throw smp::JsException
[[nodiscard]] std::string CacheUtf8Path( const std::filesystem::path& path );

/// @brief This is a hack, don't use it unless it's REALLY necessary
[[nodiscard]] std::optional<std::filesystem::path>
GetCachedUtf8Path( const std::string_view& pathId );

} // namespace mozjs::hack
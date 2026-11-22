#pragma once

#include <cstddef>
#include <cstdint>

/**
 * Cross-plugin API for other SKSE plugins, header-only on the consumer side.
 *
 * After SKSE sends kPostLoad, look up the export and ask for an interface:
 *
 *   auto request = reinterpret_cast<CommunityShadersAPI::RequestPluginAPI>(
 *       GetProcAddress(GetModuleHandleW(L"CommunityShaders.dll"), "RequestPluginAPI"));
 *   auto* api = request ? static_cast<CommunityShadersAPI::ICommunityShadersInterface001*>(
 *       request(CommunityShadersAPI::InterfaceVersion::V1)) : nullptr;
 *
 * Every call runs on the game's main thread.
 */
namespace CommunityShadersAPI
{
	enum class InterfaceVersion : std::uint8_t
	{
		V1
	};

	struct ICommunityShadersInterface001
	{
		/**
		 * Writes the settings of a loaded feature, by short name, as a JSON object into
		 * a_buffer. Returns the size the JSON needs, its terminating NUL included, or 0 when
		 * no loaded feature has that name. Nothing is written when a_size is too small.
		 */
		virtual std::size_t GetFeatureSettings(const char* a_feature, char* a_buffer, std::size_t a_size) = 0;

		/**
		 * Merges a JSON object into the settings of a loaded feature: keys it names change,
		 * every other setting keeps its value. The change applies at once and is not saved;
		 * it reaches disk only if the settings are saved later. Returns false when no loaded
		 * feature has that name or the JSON is not an object the feature accepts.
		 */
		virtual bool SetFeatureSettings(const char* a_feature, const char* a_settingsJson) = 0;
	};

	using RequestPluginAPI = void* (*)(InterfaceVersion a_version);
}

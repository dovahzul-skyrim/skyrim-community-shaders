#include "CommunityShadersAPI.h"

#include "Feature.h"

namespace
{
	struct Interface001 final : CommunityShadersAPI::ICommunityShadersInterface001
	{
		std::size_t GetFeatureSettings(const char* a_feature, char* a_buffer, std::size_t a_size) override
		{
			auto* feature = a_feature ? Feature::FindFeatureByShortName(a_feature) : nullptr;
			if (!feature)
				return 0;
			json settings;
			feature->SaveSettings(settings);
			const std::string text = settings.dump();
			const std::size_t needed = text.size() + 1;
			if (a_buffer && a_size >= needed)
				std::memcpy(a_buffer, text.c_str(), needed);
			return needed;
		}

		bool SetFeatureSettings(const char* a_feature, const char* a_settingsJson) override
		{
			auto* feature = a_feature ? Feature::FindFeatureByShortName(a_feature) : nullptr;
			if (!feature || !a_settingsJson)
				return false;
			const json patch = json::parse(a_settingsJson, nullptr, false);
			if (patch.is_discarded() || !patch.is_object()) {
				logger::warn("[API] Ignored settings for {} that are not a JSON object", a_feature);
				return false;
			}
			json previous;
			feature->SaveSettings(previous);
			json settings = previous;
			settings.merge_patch(patch);
			try {
				feature->LoadSettings(settings);
			} catch (const std::exception& e) {
				logger::warn("[API] {} refused settings {}: {}", a_feature, patch.dump(), e.what());
				feature->LoadSettings(previous);
				return false;
			}
			logger::info("[API] {} settings changed: {}", a_feature, patch.dump());
			return true;
		}
	};
}

extern "C" __declspec(dllexport) void* RequestPluginAPI(CommunityShadersAPI::InterfaceVersion a_version)
{
	static Interface001 interface001;
	switch (a_version) {
	case CommunityShadersAPI::InterfaceVersion::V1:
		return &interface001;
	}
	return nullptr;
}

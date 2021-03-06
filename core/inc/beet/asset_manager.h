#pragma once
#include <beet/assert.h>
#include <beet/asset.h>
#include <beet/subsystem.h>
#include <beet/types.h>
#include <filesystem>
#include <map>

namespace beet {
namespace asset {
class AssetManager : public Subsystem {
   public:
    AssetManager() = default;
    AssetManager(const AssetManager& other) = delete;

    void on_awake() override;
    void on_destroy() override;

    void load_assets_manual();
    void load_assets_serialize();

    static std::optional<std::reference_wrapper<AssetManager>> get_asset_manager() { return s_assetManager; };

    template <typename T>
    inline static std::weak_ptr<T> load_asset(const std::string& path) {
        auto managerOpt = get_asset_manager();
        BEET_ASSERT_MESSAGE(managerOpt.has_value(), "ASSET MANAGER IS EMPTY")
        auto& assetManager = managerOpt->get();
        return assetManager.internal_load_asset<T>(path);
    }

   private:
    std::map<std::string, std::shared_ptr<Asset>> m_assets;

    inline static std::optional<std::reference_wrapper<AssetManager>> s_assetManager = std::nullopt;

    template <typename T>
    inline std::weak_ptr<T> internal_load_asset(const std::string& path) {
        static_assert(!std::is_base_of<T, Asset>::value, "ASSET IS NOT OF BASE CLASS ASSET");

        auto iterator = m_assets.find(path);

        if (iterator != m_assets.end()) {
            auto r = std::static_pointer_cast<T>(m_assets[path]);
            r->on_awake();
            return r;
        }

        log::debug("Asset : " + path + " is being loaded");
        std::shared_ptr<T> tempAsset = std::make_shared<T>(path);
        tempAsset.get()->on_awake();

        if (tempAsset.get()->get_asset_state() == AssetState::Failed) {
            log::warn("Asset manager failed to load {} loading fallback");
            return std::static_pointer_cast<T>(m_assets[tempAsset.get()->get_fallback_name()]);
        } else {
            log::debug("adding asset: {}", path);
            m_assets.insert({path, tempAsset});
            std::shared_ptr<T> result = std::static_pointer_cast<T>(m_assets[path]);
            return result;
        }
    }
};
}  // namespace asset
}  // namespace beet

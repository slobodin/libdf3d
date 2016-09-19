#pragma once

namespace df3d {

struct DF3D_DLL StoreProduct
{
    std::string identifier;
    std::string localizedPrice;
};

struct StoreRetrieveProductsResult
{
    bool success;
    std::vector<StoreProduct> products;
};

struct StorePurchaseResult
{
    bool success;
    std::string productId;
};

class DF3D_DLL InAppPurchase : NonCopyable
{
    struct Impl;
    unique_ptr<Impl> m_pImpl;

public:
    InAppPurchase();
    ~InAppPurchase();

    void retrieveProductInfo(const std::vector<std::string> &products,
                             std::function<void(const StoreRetrieveProductsResult &)> &&onComplete);
    void purchase(const std::string &productId,
                  std::function<void(const StorePurchaseResult &)> &&onComplete);
};

}

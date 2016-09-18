#pragma once

namespace df3d {

struct DF3D_DLL StoreProduct
{
    std::string identifier;
};

class DF3D_DLL InAppPurchaseDelegate
{
public:
    InAppPurchaseDelegate() = default;
    virtual ~InAppPurchaseDelegate() = default;

    virtual void productRequestFailed(const std::string &err) { }

    virtual void productInfoReceived(const std::vector<StoreProduct> &products) { }
    virtual void productPurchased(const std::string &productId) { }
    virtual void productPurchaseCancelled() { }
};

class DF3D_DLL InAppPurchase : NonCopyable
{
    struct Impl;
    unique_ptr<Impl> m_pImpl;

public:
    InAppPurchase();
    ~InAppPurchase();

    void setDelegate(InAppPurchaseDelegate *delegate);

    void retrieveProductInfo(const std::vector<std::string> &products);
    void purchase(const std::string &productId);
};

}

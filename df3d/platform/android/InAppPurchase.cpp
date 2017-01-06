#include <df3d/platform/InAppPurchase.h>

namespace df3d {

struct InAppPurchase::Impl
{

};

InAppPurchase::InAppPurchase()
    : m_pImpl(new InAppPurchase::Impl())
{

}

InAppPurchase::~InAppPurchase()
{

}

void InAppPurchase::retrieveProductInfo(const std::vector<std::string> &products,
                                        std::function<void(const StoreRetrieveProductsResult &)> &&onComplete)
{
    StoreRetrieveProductsResult result;
    result.success = true;

    for (const auto &productId : products)
    {
        StoreProduct p;
        p.identifier = productId;
        result.products.push_back(p);
    }

    onComplete(result);
}

void InAppPurchase::purchase(const std::string &productId,
                             std::function<void(const StorePurchaseResult &)> &&onComplete)
{
    StorePurchaseResult result;
    result.success = true;
    result.productId = productId;

    onComplete(result);
}

}

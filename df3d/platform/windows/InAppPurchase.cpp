#include <df3d/platform/InAppPurchase.h>

namespace df3d {

struct InAppPurchase::Impl
{
    InAppPurchaseDelegate *clientDelegate = nullptr;
};

InAppPurchase::InAppPurchase()
    : m_pImpl(new InAppPurchase::Impl())
{

}

InAppPurchase::~InAppPurchase()
{

}

void InAppPurchase::setDelegate(InAppPurchaseDelegate *delegate)
{
    m_pImpl->clientDelegate = delegate;
}

void InAppPurchase::retrieveProductInfo(const std::vector<std::string> &products)
{

}

void InAppPurchase::purchase(const std::string &productId)
{
    if (m_pImpl->clientDelegate)
        m_pImpl->clientDelegate->productPurchased(productId);
}

}

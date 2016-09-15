#import <df3d/df3d.h>
#import <df3d/platform/InAppPurchase.h>
#import <StoreKit/StoreKit.h>

@interface StoreDelegate : NSObject<SKProductsRequestDelegate>
{
    @public
    df3d::InAppPurchaseDelegate *clientDelegate;
}
-(id)init;
-(void)productsRequest:(SKProductsRequest *)request didReceiveResponse:(SKProductsResponse *)response;
@end

@implementation StoreDelegate

-(id)init
{
    if (self = [super init])
    {
        clientDelegate = nullptr;
    }
    return self;
}

-(void)productsRequest:(SKProductsRequest *)request didReceiveResponse:(SKProductsResponse *)response
{
    if ([response.products count] == 0)
    {
        for (NSString *invalidIdentifier in response.invalidProductIdentifiers)
        {
            DFLOG_WARN("Invalid store product identifier %s", [invalidIdentifier UTF8String]);
        }

        if (clientDelegate)
            clientDelegate->retrieveProductInfoFailed();
    }
    else
    {
        std::vector<df3d::StoreProduct> result;

        for (SKProduct *product in response.products)
        {
            df3d::StoreProduct p;

            p.identifier = std::string([product.productIdentifier UTF8String]);

            result.push_back(p);
        }

        if (clientDelegate)
            clientDelegate->receivedProductInfo(result);
    }

    [request release];
}

@end

namespace df3d {

struct InAppPurchase::Impl
{
    StoreDelegate *storeDelegate;

    Impl()
    {
        storeDelegate = [[StoreDelegate alloc] init];
    }

    ~Impl()
    {
        [storeDelegate release];
    }
};

InAppPurchase::InAppPurchase()
    : m_pImpl(make_unique<Impl>())
{

}

InAppPurchase::~InAppPurchase()
{

}

void InAppPurchase::setDelegate(InAppPurchaseDelegate *delegate)
{
    m_pImpl->storeDelegate->clientDelegate = delegate;
}

void InAppPurchase::retrieveProductInfo(const std::vector<std::string> &products)
{
    if (products.empty())
        return;

    NSMutableSet *productSet = [NSMutableSet set];

    for (const auto &productId : products)
    {
        NSString *nsstr = [NSString stringWithCString:productId.c_str() encoding:[NSString defaultCStringEncoding]];
        [productSet addObject:nsstr];
    }

    SKProductsRequest *request = [[SKProductsRequest alloc] initWithProductIdentifiers:productSet];
    request.delegate = m_pImpl->storeDelegate;
    [request start];
}

}

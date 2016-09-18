#import <df3d/df3d.h>
#import <df3d/platform/InAppPurchase.h>
#import <StoreKit/StoreKit.h>

@interface StoreDelegate : NSObject<SKProductsRequestDelegate, SKPaymentTransactionObserver>
{
    @public
    df3d::InAppPurchaseDelegate *clientDelegate;
    NSMutableDictionary *products;
}
@end

@implementation StoreDelegate

-(id)init
{
    if (self = [super init])
    {
        clientDelegate = nullptr;
        products = [[NSMutableDictionary alloc] init];
        [[SKPaymentQueue defaultQueue] addTransactionObserver:self];
    }
    return self;
}

-(void)dealloc
{
    [[SKPaymentQueue defaultQueue] removeTransactionObserver:self];
    [products release];
    [super dealloc];
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
            clientDelegate->productRequestFailed("Got 0 valid products");
    }
    else
    {
        std::vector<df3d::StoreProduct> result;
        [self->products removeAllObjects];

        for (SKProduct *product in response.products)
        {
            df3d::StoreProduct p;

            p.identifier = std::string([product.productIdentifier UTF8String]);

            result.push_back(p);

            [self->products setObject:product forKey:product.productIdentifier];
        }

        if (clientDelegate)
            clientDelegate->productInfoReceived(result);
    }

    [request release];
}

-(void)productPurchased:(NSString *) productId
{
    if (clientDelegate)
    {
        if (productId)
            clientDelegate->productPurchased([productId UTF8String]);
        else
            clientDelegate->productRequestFailed("df3d: invalid product id");
    }
}

-(void)paymentQueue:(SKPaymentQueue *)queue updatedTransactions:(NSArray<SKPaymentTransaction *> *)transactions
{
    for (SKPaymentTransaction *transaction in transactions)
    {
        switch(transaction.transactionState)
        {
            case SKPaymentTransactionStatePurchased:
                [self productPurchased:transaction.payment.productIdentifier];
                [[SKPaymentQueue defaultQueue] finishTransaction:transaction];
                break;
            case SKPaymentTransactionStateFailed:
                if (clientDelegate)
                {
                    if (transaction.error.code == SKErrorPaymentCancelled)
                    {
                        clientDelegate->productPurchaseCancelled();
                    }
                    else
                    {
                        NSString *errStr = [transaction.error localizedDescription];
                        if (errStr)
                            clientDelegate->productRequestFailed([errStr UTF8String]);
                        else
                            clientDelegate->productRequestFailed("df3d: unknown error");
                    }
                }
                [[SKPaymentQueue defaultQueue] finishTransaction:transaction];
                break;
            case SKPaymentTransactionStateRestored:
                [self productPurchased:transaction.payment.productIdentifier];
                [[SKPaymentQueue defaultQueue] finishTransaction:transaction];
                break;
            case SKPaymentTransactionStateDeferred:
            case SKPaymentTransactionStatePurchasing:
            default:
                break;
        };
    }
}

-(void)request:(SKRequest *)request didFailWithError:(NSError *)error
{
    if (clientDelegate)
    {
        std::string err = [error.localizedDescription UTF8String];
        clientDelegate->productRequestFailed(err);
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

void InAppPurchase::purchase(const std::string &productId)
{
    SKProduct *p = [m_pImpl->storeDelegate->products objectForKey:[NSString stringWithUTF8String:productId.c_str()]];
    if (p == nil)
    {
        if (m_pImpl->storeDelegate->clientDelegate)
            m_pImpl->storeDelegate->clientDelegate->productRequestFailed("No such product");

        return;
    }
    SKPayment *payment = [SKPayment paymentWithProduct:p];
    [[SKPaymentQueue defaultQueue] addPayment:payment];
}

}

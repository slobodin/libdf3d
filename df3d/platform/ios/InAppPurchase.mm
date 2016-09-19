#import <df3d/df3d.h>
#import <df3d/platform/InAppPurchase.h>
#import <StoreKit/StoreKit.h>

@interface ProductRequestDelegate : NSObject<SKProductsRequestDelegate>
{
@public
    NSMutableDictionary *products;
    std::function<void(const df3d::StoreRetrieveProductsResult &)> completitionHandler;
}
@end

@interface PurchasesDelegate : NSObject<SKPaymentTransactionObserver>
{
@public
    std::function<void(const df3d::StorePurchaseResult &)> completitionHandler;
}
@end

@implementation ProductRequestDelegate

-(id)init
{
    if (self = [super init])
    {
        products = [[NSMutableDictionary alloc] init];
    }
    return self;
}

-(void)dealloc
{
    [products release];
    [super dealloc];
}

-(void)productsRequest:(SKProductsRequest *)request didReceiveResponse:(SKProductsResponse *)response
{
    df3d::StoreRetrieveProductsResult result;

    if ([response.products count] == 0)
    {
        DFLOG_WARN("Got 0 valid iTunes Store products");

        for (NSString *invalidIdentifier in response.invalidProductIdentifiers)
        {
            DFLOG_WARN("Invalid store product identifier %s", [invalidIdentifier UTF8String]);
        }

        result.success = false;
        self->completitionHandler(result);
    }
    else
    {
        result.success = true;
        [self->products removeAllObjects];

        NSNumberFormatter *numberFormatter = [[NSNumberFormatter alloc] init];
        [numberFormatter setFormatterBehavior:NSNumberFormatterBehavior10_4];
        [numberFormatter setNumberStyle:NSNumberFormatterCurrencyStyle];

        for (SKProduct *product in response.products)
        {
            df3d::StoreProduct p;

            p.identifier = std::string([product.productIdentifier UTF8String]);
            [numberFormatter setLocale:product.priceLocale];
            p.localizedPrice = [[numberFormatter stringFromNumber:product.price] UTF8String];

            result.products.push_back(p);
            [self->products setObject:product forKey:product.productIdentifier];
        }

        [numberFormatter release];

        self->completitionHandler(result);
    }

    self->completitionHandler = {};
    [request release];
}

-(void)request:(SKRequest *)request didFailWithError:(NSError *)error
{
    DFLOG_WARN("List store products request failed with an error: %s", [error.localizedDescription UTF8String]);

    df3d::StoreRetrieveProductsResult result;
    result.success = false;
    self->completitionHandler(result);

    self->completitionHandler = {};
    [request release];
}

@end

@implementation PurchasesDelegate

-(id)init
{
    if (self = [super init])
    {
        [[SKPaymentQueue defaultQueue] addTransactionObserver:self];
    }
    return self;
}

-(void)dealloc
{
    [[SKPaymentQueue defaultQueue] removeTransactionObserver:self];
    [super dealloc];
}

-(void)productPurchased:(NSString *) productId
{
    df3d::StorePurchaseResult result;
    if (productId)
    {
        result.success = true;
        result.productId = [productId UTF8String];
    }
    else
        result.success = false;

    self->completitionHandler(result);
    self->completitionHandler = {};
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
            {
                NSString *errStr = [transaction.error localizedDescription];
                if (errStr)
                    DFLOG_WARN("In-app purchase failed: %s", [errStr UTF8String]);

                df3d::StorePurchaseResult result;
                result.success = false;
                self->completitionHandler(result);
                self->completitionHandler = {};
                [[SKPaymentQueue defaultQueue] finishTransaction:transaction];
            }
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
    DFLOG_WARN("Failed to perform in-app purchase %s", [error.localizedDescription UTF8String]);

    df3d::StorePurchaseResult result;
    result.success = false;

    completitionHandler(result);
    completitionHandler = {};

    [request release];
}

@end

namespace df3d {

struct InAppPurchase::Impl
{
    ProductRequestDelegate *productRequestsDelegate;
    PurchasesDelegate *purchasesDelegate;

    Impl()
    {
        productRequestsDelegate = [[ProductRequestDelegate alloc] init];
        purchasesDelegate = [[PurchasesDelegate alloc] init];
    }

    ~Impl()
    {
        [productRequestsDelegate release];
        [purchasesDelegate release];
    }
};

InAppPurchase::InAppPurchase()
    : m_pImpl(make_unique<Impl>())
{

}

InAppPurchase::~InAppPurchase()
{

}


void InAppPurchase::retrieveProductInfo(const std::vector<std::string> &products,
                                        std::function<void(const StoreRetrieveProductsResult &)> &&onComplete)
{
    DF3D_ASSERT(!products.empty());

    NSMutableSet *productSet = [NSMutableSet set];

    for (const auto &productId : products)
    {
        NSString *nsstr = [NSString stringWithCString:productId.c_str() encoding:[NSString defaultCStringEncoding]];
        [productSet addObject:nsstr];
    }

    SKProductsRequest *request = [[SKProductsRequest alloc] initWithProductIdentifiers:productSet];
    request.delegate = m_pImpl->productRequestsDelegate;
    m_pImpl->productRequestsDelegate->completitionHandler = std::move(onComplete);
    [request start];
}

void InAppPurchase::purchase(const std::string &productId,
                             std::function<void(const StorePurchaseResult &)> &&onComplete)
{
    SKProduct *p = [m_pImpl->productRequestsDelegate->products objectForKey:[NSString stringWithUTF8String:productId.c_str()]];
    if (p == nil)
    {
        df3d::StorePurchaseResult result;
        result.success = false;
        result.productId = productId;
        onComplete(result);
    }
    else
    {
        m_pImpl->purchasesDelegate->completitionHandler = std::move(onComplete);
        SKPayment *payment = [SKPayment paymentWithProduct:p];
        [[SKPaymentQueue defaultQueue] addPayment:payment];
    }
}

}

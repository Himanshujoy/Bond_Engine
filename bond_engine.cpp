#include<vector>
#include<cmath>
#include <algorithm>
#include <iostream>
#include <memory>
using namespace std;


class Date
{
    private:
        int year_d, month_d, day_d;

    public:
        Date(int year, int month, int day) : year_d(year), month_d(month), day_d(day) {}
        
        int daysDiff(const Date& other) const 
        {
            return (
                (year_d - other.year_d)*365 +
                (month_d - other.month_d)*30 +
                (day_d - other.day_d)
            );
        }
};

class IYieldCurve
{
    public:
        virtual double discountfactor(double time) const = 0;
        virtual double forwardrate(double time1, double time2) const = 0;
        virtual ~IYieldCurve() = default;
};

class FlatYieldCurve : public IYieldCurve
{
    private:
        double rate_d;

    public:
        explicit FlatYieldCurve(double rate) : rate_d(rate) {}
        
        double discountfactor(double time) const override 
        {
            return exp(-rate_d * time);
        }
        
        double forwardrate(double time1, double time2) const override 
        {
            return rate_d;
        }
};

struct Cashflow
{
    Date paymentdate;
    double time;
    double amount;
    string type;
};

class Bond
{
    private:
        double facevalue_d, couponrate_d;
        int numyears_d, frequency_d, settlementdays_d;

    public:
        Bond(double facevalue, double couponrate, int numyears, int frequency, int settlementdays=1)
            : facevalue_d(facevalue), couponrate_d(couponrate), numyears_d(numyears), frequency_d(frequency), settlementdays_d(settlementdays) {}

        vector<Cashflow> generateCashflows(const Date& settledate) const
        {
            vector<Cashflow> cashflows;
            double couponamount = facevalue_d * couponrate_d / frequency_d;
            
            const int days_in_year = 365;
            const int coupon_interval_days = days_in_year / frequency_d;
            
            //Adding coupon cashflows
            for(int i=1; i <= numyears_d*frequency_d; i++)
            {
                Date paymentdate(
                    settledate.year_d + i / frequency_d,
                    (settledate.month_d + (i % frequency_d) * 12 / frequency_d)%12 + 1,
                    settledate.day_d
                );

                double time = i*1.0/frequency_d;
                cashflows.push_back({paymentdate, time, couponamount, "Coupon"});
            }

            //Adding principal repayment cashflow
            Date maturitydate(
                settledate.year_d + numyears_d,
                settledate.month_d,
                settledate.day_d
            );

            cashflows.push_back({maturitydate, static_cast<double>(numyears_d), facevalue_d+couponamount, "Principal"});

            return cashflows;
        }

        double pricefromyield(double yield, const vector<Cashflow>& cashflows) const
        {
            double discountedprice = 0.0;
            double discountfactor = yield / frequency_d;

            for(const auto& cf : cashflows)
            {
                discountedprice += cf.amount/pow(1+discountfactor, cf.time*frequency_d);
            }

            return discountedprice;
        }

        pair<double, double> calculateYTM(double marketprice, const vector<Cashflow>& cashflows) const
        {
            double accural = facevalue_d * couponrate_d * 0.5 * (30.0/360.0);
            double dirtyprice = marketprice + accural;

            auto pricediff = [&](double ytm)
            {
                return pricefromyield(ytm, cashflows) - dirtyprice;
            };

            double ytm=0.0;

            try
            {
                double ytm_low = 0.0, ytm_high = 0.5;
                while (ytm_high-ytm_low>1e-8)
                {
                    double ytm_mid = (ytm_low + ytm_high) / 2.0;
                    if(pricediff(ytm_low)*pricediff(ytm_mid)<=0)
                    {
                        ytm_high = ytm_mid;
                    }
                    else
                    {
                        ytm_low = ytm_mid;
                    }
                }

                ytm = (ytm_low + ytm_high) / 2.0;
            }
            catch(...)
            {
                ytm = numeric_limits<double>::quiet_NaN();
            }

            return {ytm, accural};
        }

        struct BondAnalytics
        {
            double macauleydur, modifieddur, convexity, dv01, price;
        };

        BondAnalytics calculateAnalytics(double yield, const vector<Cashflow>& cashflows) const
        {
            BondAnalytics analytics;

            analytics.price = pricefromyield(yield, cashflows);

            double macaulay_duration = 0.0, convexity = 0.0;

            for(const auto& cf : cashflows)
            {
                double time = cf.time;
                double pv = cf.amount / pow(1 + yield / frequency_d, time * frequency_d);

                macaulay_duration += time * pv;
                convexity += time * (time + 1.0 / frequency_d) * pv;
            }

            analytics.macauleydur = macaulay_duration / analytics.price;
            analytics.modifieddur = analytics.macauleydur / (1 + yield / frequency_d);
            analytics.convexity = convexity / (analytics.price * pow(1 + yield / frequency_d, 2));
            analytics.dv01 = analytics.modifieddur * analytics.price * 0.0001;

            return analytics;
        }
};

void main()
{

}
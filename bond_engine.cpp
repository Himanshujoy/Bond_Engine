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
};

void main()
{

}
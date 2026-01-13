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

void main()
{

}
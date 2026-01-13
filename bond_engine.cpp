#include<vector>
#include<cmath>
#include <algorithm>
#include <iostream>
#include <memory>

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

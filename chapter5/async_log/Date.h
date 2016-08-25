//
// Created by fit on 16-8-25.
//

#ifndef DATE_H
#define DATE_H
#include "copyable.h"
#include "Types.h"
struct tm;

namespace muduo{
    class Date:public muduo::copyable{
    public:
        struct YearMonthDay{
            int year;//[1900..2500]
            int month;//[1..12]
            int day;//[1..31]
        };
        static const int kDaysPerWerek = 7;
        static const int kJulianDayof1970_01_01;
        data():julianDayNumber_(0){}
        Date(int year,int month,int day);
        explicit Date(int julianDayNum):julianDayNumber_(julianDayNum){}

        explicit Date( const struct tm&);
        void swap(Date &that){
            std::swap(julianDayNumber_,that.julianDayNumber_);
        }
        bool valid() const{
            return julianDayNumber_>0;
        }
        string toIsoString() const;
        struct YearMonthDay yearMonthDay() const;
        int year() const{
            return yearMonthDay().year;
        }
        int month() const{
            return yearMonthDay().month;
        }
        int day() const{
            return yearMonthDay().day;
        }
        int weekday() const{
            return (julianDayNumber_+1)%kDaysPerWerek;
        }
        int julianDayNumber() const{
            return julianDayNumber_;
        }

    private:
        int julianDayNumber_;
    };
    inline bool operator <( Date x,Date y){
        return x.julianDayNumber_< y.julianDayNumber_;
    }
    inline bool operator==(Date x,Date y){
        return x.julianDayNumber_ == y.julianDayNumber_;
    }
  inline bool operator > (Date x,Date y){
    return
}




#endif

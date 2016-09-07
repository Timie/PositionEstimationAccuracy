#include "rce/utility/RNumberConversion.h"

#include <cmath>

#include <QString>
#include <QRegularExpression>
#include <QLocale>


QString getNegativeZeroString(const QLocale &l,
                              int prec)
{
    if(prec <= 0)
    {
        return "-0";
    }
    else
    {
        QString zeros;
        for(int i = 0;
            i < prec;
            ++i)
        {
            zeros.append('0');
        }
        return QString("-0") + l.decimalPoint() + zeros;
    }
}

QRegularExpression nonZeroDigitRE()
{
    QRegularExpression numberRE("1|2|3|4|5|6|7|8|9",
                                  QRegularExpression::OptimizeOnFirstUsageOption);
    numberRE.optimize();
    return numberRE;
}

bool
isNegZero(const QString &str)
{
    static QRegularExpression numberRE = nonZeroDigitRE();
    return ((str.indexOf(numberRE) < 0) && (str[0]=='-'));
}

QString
rce::utility::
doubleToStr(double number,
            char f,
            int prec)
{
    if((f == 'f') && (number < 0))
    {
        // check rounding around zero
        QString result = QString::number(number,
                                         f,
                                         prec);
        if(isNegZero(result))
        {
            return result.mid(1);
        }
        else
        {
            return result;
        }
    }
    else
    {
        return QString::number(number,
                          f,
                          prec);
    }
}

QString
rce::utility::
doubleToStr(const QLocale &l,
            double number,
            char f,
            int prec)
{
    if((f == 'f') && (number < 0))
    {
        // check rounding around zero
        QString result = l.toString(number,
                                    f,
                                    prec);

        if(isNegZero(result))
        {
            return result.mid(1);
        }
        else
        {
            return result;
        }
    }
    else
    {
        return l.toString(number,
                          f,
                          prec);
    }
}

double
rce::utility::
preventNegZero(double value,
               int prec)
{
    if(value < 0)
    {
        double roundLimit = -0.5 * std::pow(10, -prec);
        if(value >= roundLimit)
        {
            return 0;
        }
    }

    return value;
}

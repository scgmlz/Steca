//  ***********************************************************************************************
//
//  libqcr: capture and replay Qt widget actions
//
//! @file      qcr/engine/string_ops.cpp
//! @brief     Implements namespace strOp with various string operations
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2018-
//! @author    Joachim Wuttke
//
//  ***********************************************************************************************

#include "string_ops.h"
#include "qcr/engine/qcrexception.h"
#include <QString>

bool strOp::to_b(const QString& s)
{
    int ret;
    if (s=="on")
        return true;
    else if (s=="off")
        return false;
    else
        throw QcrException("Expected 'on' or 'off', found string '" + s + "'");
}

int strOp::to_i(const QString& s)
{
    if (s=="")
        throw QcrException("Expected an integer number, found empty string");
    int ret;
    bool ok;
    ret = s.toInt(&ok);
    if (!ok)
        throw QcrException("Expected an integer number, found string '" + s + "'");
    return ret;
}

double strOp::to_d(const QString& s)
{
    if (s=="")
        throw QcrException("Expected a floating-point number, found empty string");
    double ret;
    bool ok;
    ret = s.toDouble(&ok);
    if (!ok)
        throw QcrException("Expected a floating-point number, found string '" + s + "'");
    return ret;
}

template<> bool    strOp::from_s(const QString& s) { return strOp::to_b(s); }
template<> int     strOp::from_s(const QString& s) { return strOp::to_i(s); }
template<> double  strOp::from_s(const QString& s) { return strOp::to_d(s); }
template<> QString strOp::from_s(const QString& s) { return s; }

QString strOp::to_s(bool val)
{
    return val ? "on" : "off";
}

QString strOp::to_s(int val)
{
    return QString::number(val);
}

QString strOp::to_s(double val)
{
    return QString::number(val);
}

QString strOp::to_s(QString val)
{
    return val;
}

void strOp::splitOnce(const QString& in, QString& part0, QString& part1)
{
    int j = in.indexOf(' ');
    if (j!=-1) {
        part0 = in.left(j);
        part1 = in.mid(j+1);
    } else {
        part0 = in;
        part1 = "";
    }
}

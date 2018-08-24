//  ***********************************************************************************************
//
//  libqcr: capture and replay Qt widget actions
//
//! @file      qcr/base/string_ops.cpp
//! @brief     Implements namespace strOp with various string operations
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2018-
//! @author    Joachim Wuttke
//
//  ***********************************************************************************************

#include "qcr/base/string_ops.h"
#include "qcr/base/qcrexception.h"
#include <QWidget>

namespace strOp {

bool to_b(const QString& s)
{
    if (s=="on")
        return true;
    else if (s=="off")
        return false;
    else
        throw QcrException("Expected 'on' or 'off', found string '" + s + "'");
}

int to_i(const QString& s)
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

double to_d(const QString& s)
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

template<> bool    from_s(const QString& s) { return to_b(s); }
template<> int     from_s(const QString& s) { return to_i(s); }
template<> double  from_s(const QString& s) { return to_d(s); }
template<> QString from_s(const QString& s) { return s; }

QString to_s(bool val)
{
    return val ? "on" : "off";
}

QString to_s(int val)
{
    return QString::number(val);
}

QString to_s(double val)
{
    return QString::number(val);
}

QString to_s(QString val)
{
    return val;
}

void splitOnce(const QString& in, QString& part0, QString& part1)
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

void setWidth(QWidget* w, int ndigits, bool withDot)
{
    int width = ndigits;
#ifdef Q_OS_WIN
    width += 1 + (withDot?1:0);
#endif
    w->setFixedWidth(width * w->fontMetrics().width('m'));
}

} // namespace strOp

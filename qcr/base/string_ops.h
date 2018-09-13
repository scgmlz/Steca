//  ***********************************************************************************************
//
//  libqcr: capture and replay Qt widget actions
//
//! @file      qcr/base/string_ops.h
//! @brief     Defines namespace strOp with various string operations
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2018-
//! @author    Joachim Wuttke
//
//  ***********************************************************************************************

#ifndef STRING_OPS_H
#define STRING_OPS_H

#include <QString>
class QWidget;

//! String operations

namespace strOp {

bool to_b(const QString&);
int to_i(const QString&);
double to_d(const QString&);

template<typename T>
T from_s(const QString&);

QString to_s(bool);
QString to_s(int);
QString to_s(double);
QString to_s(QString);

void splitOnce(const QString&, QString&, QString&);

void setWidth(QWidget* w, int ndigits, bool withDot);

} // namespace strOp

#endif // STRING_OPS_H

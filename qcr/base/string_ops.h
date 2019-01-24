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

//! String operations.

namespace strOp {

bool to_b(const QString&);   //!< Converts string "on" or "off" to boolean value, or throws.
int to_i(const QString&);    //!< Converts string to integer, or throws.
double to_d(const QString&); //!< Converts string to double, or throws.

template<typename T>
T from_s(const QString&); //!< Converts string to T (supports T=bool,int,double,QString).

QString to_s(bool);    //!< Converts boolean value to "on" or "off".
QString to_s(int);     //!< Converts integer value to decimal string representation.
QString to_s(double);  //!< Converts double value to floating-point string representation
QString to_s(const QString&); //!< Returns argument (needed in templated functions).

void splitOnce(const QString&, QString&, QString&); //!< Splits arg0 at first blank.

void setWidth(QWidget* w, int ndigits, bool withDot); //!< Sets widget width for number display.

} // namespace strOp

#endif // STRING_OPS_H

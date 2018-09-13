//  ***********************************************************************************************
//
//  libqcr: capture and replay Qt widget actions
//
//! @file      qcr/base/qcrexception.h
//! @brief     Defines class QcrException
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2018-
//! @author    Joachim Wuttke
//
//  ***********************************************************************************************

#ifndef QCREXCEPTION_H
#define QCREXCEPTION_H

#include <QException>
#include <QString> // no auto rm

//! The sole exception type used within Qcr.
class QcrException : public QException {
public:
    QcrException(const QString& msg) noexcept : msg_(msg) {}
    const QString& msg() const noexcept { return msg_; }
private:
    QString msg_;
};

#endif // QCREXCEPTION_H

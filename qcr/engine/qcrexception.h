//  ***********************************************************************************************
//
//  libqcr: capture and replay Qt widget actions
//
//! @file      qcr/engine/qcrexception.h
//! @brief     Defines class QCRException
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#ifndef QCREXCEPTION_H
#define QCREXCEPTION_H

#include <QException>
#include <QString> // no auto rm

//! The sole exception type used in this software.
class QCRException : public QException {
public:
    QCRException() = delete;
    QCRException(const QString& msg) noexcept : msg_(msg) {}
    const QString& msg() const noexcept { return msg_; }
private:
    QString msg_;
};

#endif // QCREXCEPTION_H

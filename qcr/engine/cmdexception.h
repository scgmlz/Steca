//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      qcr/engine/cmdexception.h
//! @brief     Defines class CmdException
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#ifndef CMDEXCEPTION_H
#define CMDEXCEPTION_H

#include <QException>
#include <QString> // no auto rm

//! The sole exception type used in this software.
class CmdException : public QException {
public:
    CmdException() = delete;
    CmdException(const QString& msg) noexcept : msg_(msg) {}
    const QString& msg() const noexcept { return msg_; }
private:
    QString msg_;
};

#endif // CMDEXCEPTION_H

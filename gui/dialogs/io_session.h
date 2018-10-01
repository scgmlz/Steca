//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      gui/dialogs/io_session.h
//! @brief     Defines functions in namespace ioSession
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#ifndef IO_SESSION_H
#define IO_SESSION_H

class QWidget;

namespace ioSession {
    void load(QWidget*);
    void save(QWidget*);
} // ioSession

#endif // IO_SESSION_H

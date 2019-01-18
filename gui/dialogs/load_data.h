//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      gui/dialogs/load_data.h
//! @brief     Defines functions in namespace loadData
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#ifndef LOAD_DATA_H
#define LOAD_DATA_H

class QWidget;

namespace loadData {
    void addFiles(QWidget*);
    void loadCorrFile(QWidget*);
} // ioSession

#endif // LOAD_DATA_H

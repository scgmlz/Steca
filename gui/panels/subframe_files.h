//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      gui/panels/subframe_files.h
//! @brief     Defines class SubframeFiles
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#ifndef SUBFRAME_FILES_H
#define SUBFRAME_FILES_H

#include "qcr/widgets/views.h"

//! Part of the main window that controls the data and correction files.
class SubframeFiles : public QcrDockWidget {
public:
    SubframeFiles();
};

#endif // SUBFRAME_FILES_H

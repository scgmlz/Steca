//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      gui/panels/subframe_dfgram.h
//! @brief     Defines class SubframeDfgram
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#ifndef SUBFRAME_DFGRAM_H
#define SUBFRAME_DFGRAM_H

#include <QTabWidget> // no need for CTabWidget, since there is only one tab

//! Part of the main window that contains a diffractogram, with associated controls.

class SubframeDfgram : public QTabWidget {
public:
    SubframeDfgram();
};

#endif // SUBFRAME_DFGRAM_H

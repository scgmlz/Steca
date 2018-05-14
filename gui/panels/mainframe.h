//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      gui/panels/mainframe.h
//! @brief     Defines class Mainframe
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#ifndef MAINFRAME_H
#define MAINFRAME_H

#include "qcr/widgets/controls.h"

//! Part of the main window that shows a detector image, with associated controls.

//! There are two tabs: data image and correction data image.

class Mainframe : public QcrTabWidget {
public:
    Mainframe();

private:
    class DataImageTab*  dataImageTab_;
    class CorrImageTab*  corrImageTab_;
    class BigtableTab*   bigtableTab_;
    class DiagramTab* diagramTab_;
    class PolefigTab* polefigTab_;
};

#endif // MAINFRAME_H

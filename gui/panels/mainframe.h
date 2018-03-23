// ************************************************************************** //
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
// ************************************************************************** //

#ifndef MAINFRAME_H
#define MAINFRAME_H

#include "gui/base/controls.h"

//! Part of the main window that shows a detector image, with associated controls.

//! There are two tabs: data image and correction data image.

class Mainframe : public CTabWidget {
public:
    Mainframe();
    const class TableWidget& tableWidget() const { return *tableWidget_; }

private:
    void render();
    class DataImageTab*  dataImageTab_;
    class CorrImageTab*  corrImageTab_;
    class TableWidget*   tableWidget_;
    class DiagramWidget* diagramWidget_;
    class PolefigWidget* polefigWidget_;
};

#endif // MAINFRAME_H

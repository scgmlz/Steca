//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      gui/dialogs/export_diagram.h
//! @brief     Defines class ExportDiagram
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#ifndef EXPORT_DIAGRAM_H
#define EXPORT_DIAGRAM_H

#include "gui/capture_and_replay/enhance_widgets.h"

//! The modal dialog for the data shown in the diagram

class ExportDiagram : private CModal, public QDialog {
public:
    ExportDiagram();
private:
    void save();
    class ExportfileDialogfield* fileField_;
};

#endif // EXPORT_DIAGRAM_H

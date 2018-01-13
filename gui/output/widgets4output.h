// ************************************************************************** //
//
//  Steca: stress and texture calculator
//
//! @file      gui/output/widgets4output.h
//! @brief     Defines classes Params, DataTable, TabSave, all used in frame.cpp.
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#ifndef WIDGETS4OUTPUT_H
#define WIDGETS4OUTPUT_H

#include "core/typ/str.h"
#include "core/typ/variant.h"
#include "core/def/special_pointers.h"
#include "gui/base/tree_views.h"
#include "gui/base/new_q.h"

//! Horizontal box with variable selection of control panels. Appears in each output dialog (Frame).

class Params : public QWidget {
public:
    enum ePanels {
        REFLECTION = 0x01,
        GAMMA = 0x02,
        POINTS = 0x04,
        INTERPOLATION = 0x08,
        DIAGRAM = 0x10,
    };
    Params(ePanels);
    class PanelReflection* panelReflection;
    class PanelGammaSlices* panelGammaSlices;
    class PanelGammaRange* panelGammaRange;
    class PanelPoints* panelPoints;
    class PanelInterpolation* panelInterpolation;
    class PanelDiagram* panelDiagram;

private:
    QBoxLayout* box_;
};

//! Base class for dialogs for saving some output to a file.

class TabSave : public QWidget {
public:
    TabSave(bool withTypes);
    str filePath(bool withSuffix);
    str separator() const;
    QAction *actBrowse, *actSave;
protected:
    QGridLayout* grid_;
    str fileSetSuffix(rcstr);
    QLineEdit *dir_, *file_;
    QRadioButton *rbDat_, *rbCsv_;
};

#endif // WIDGETS4OUTPUT_H

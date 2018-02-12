// ************************************************************************** //
//
//  Steca: stress and texture calculator
//
//! @file      gui/dialogs/frame.h
//! @brief     Defines classes Params, Frame
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#ifndef FRAME_H
#define FRAME_H

#include "core/calc/peak_info.h"
#include "gui/capture_and_replay/enhance_widgets.h"
#include <QDialog>

class QBoxLayout;
class QProgressBar;
class QTabWidget;
class QToolButton;

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
    class PanelPeak* panelPeak;
    class PanelGammaSlices* panelGammaSlices;
    class PanelGammaRange* panelGammaRange;
    class PanelPoints* panelPoints;
    class PanelInterpolation* panelInterpolation;
    class PanelDiagram* panelDiagram;

private:
    QBoxLayout* box_;
};

//! A rich modal dialog, for the polefig, diagram, and diffractogram outputs.

class Frame : public QDialog, private CModal {
public:
    Frame(const QString& name, const QString& title, Params*);
    ~Frame();
protected:
    QAction *actClose_, *actCalculate_, *actInterpolate_;
    QToolButton *btnClose_, *btnCalculate_, *btnInterpolate_;
    QProgressBar* progressBar_;
    QBoxLayout* box_;
    Params* params_;
    QTabWidget* tabs_;
    vec<PeakInfos> calcPoints_, interpPoints_;
    class DataView* table_;
    void calculate();
    void interpolate();
    virtual void displayPeak(int reflIndex, bool interpolated);
    int getReflIndex() const;
    bool getInterpolated() const;
private:
    void updatePeak();
    class TabTable* tabTable_;
};

#endif // FRAME_H

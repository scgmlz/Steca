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
#include "gui/base/controls.h"
#include <QDialog>
#include <QProgressBar>

class QBoxLayout;
class QProgressBar;
class QTabWidget;
class QToolButton;

//! Horizontal box with variable selection of control panels. Appears in each output dialog (Frame).

class Params : public QHBoxLayout {
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
};

//! A rich modal dialog, for the polefig and diagram outputs.

class Frame : public CModelessDialog {
public:
    Frame(const QString& name, const QString& title, Params*);
    ~Frame();
protected:
    CTrigger actClose_ {"actClose#", "Close"};
    CTrigger actCalculate_ {"actCalculate#", "Calculate"};
    CTrigger actInterpolate_ {"actInterpolate#", "Interpolate"};
    XTextButton btnClose_ {&actClose_};
    XTextButton btnCalculate_ {&actCalculate_};
    XTextButton btnInterpolate_ {&actInterpolate_};
    QProgressBar progressBar_;
    Params* params_;
    QTabWidget tabs_;
    vec<PeakInfos> calcPoints_, interpPoints_;
    class DataView* dataView_;
    void calculate();
    void interpolate();
    virtual void displayPeak(int reflIndex, bool interpolated);
    int getReflIndex() const;
    bool getInterpolated() const;
private:
    QVBoxLayout box_;
    class TabTable* tabTable_;
    void updatePeak();
};

#endif // FRAME_H

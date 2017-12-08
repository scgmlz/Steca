// ************************************************************************** //
//
//  Steca2: stress and texture calculator
//
//! @file      gui/panels/panel_diffractogram.h
//! @brief     Defines ...
//!
//! @homepage  https://github.com/scgmlz/Steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2017
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#ifndef PANEL_DIFFRACTOGRAM_H
#define PANEL_DIFFRACTOGRAM_H

#include "actions.h"
#include "panel.h"
#include "qcustomplot.h"
#include "typ/range.h"
#include "typ/vec.h"

namespace gui {
namespace panel {

class DiffractogramPlot;

class DiffractogramPlotOverlay : public QWidget {
private:
    using super = QWidget;
public:
    DiffractogramPlotOverlay(DiffractogramPlot&);

    void setMargins(int left, int right);

private:
    DiffractogramPlot& plot_;

    QColor addColor_, remColor_, color_, bgColor_, reflColor_;
    int marginLeft_, marginRight_;

protected:
    void enterEvent(QEvent*);
    void leaveEvent(QEvent*);
    void mousePressEvent(QMouseEvent*);
    void mouseReleaseEvent(QMouseEvent*);
    void mouseMoveEvent(QMouseEvent*);

    void paintEvent(QPaintEvent*);

    bool hasCursor_, mouseDown_;
    int cursorPos_, mouseDownPos_;

    void updateCursorRegion();
};

class DiffractogramPlot : public QCustomPlot, protected RefHub {
private:
    using super = QCustomPlot;
public:
    enum class eTool {
        NONE,
        BACKGROUND,
        PEAK_REGION,
    };

    DiffractogramPlot(TheHub&, class Diffractogram&);

public:
    void setTool(eTool);
    eTool getTool() const { return tool_; }

    void plot(typ::Curve const&, typ::Curve const&, typ::Curve const&, typ::curve_vec const&, uint);

    typ::Range fromPixels(int, int);

    void clearBg();
    void addBg(typ::Range const&);
    void remBg(typ::Range const&);
    void setNewReflRange(typ::Range const&);
    void updateBg();

    void clearReflLayer();

    QColor bgRgeColor_, reflRgeColor_;
    eFittingTab selectedFittingTab();

    void enterZoom(bool);

protected:
    void addBgItem(typ::Range const&);
    void resizeEvent(QResizeEvent*);

private:
    Diffractogram& diffractogram_;

    eTool tool_;
    bool showBgFit_;

    QCPGraph *bgGraph_, *dgramGraph_, *dgramBgFittedGraph_, *dgramBgFittedGraph2_, *guesses_,
        *fits_;

    typ::vec<QCPGraph*> reflGraph_;
    DiffractogramPlotOverlay* overlay_;
};

class Diffractogram : public PanelWidget {
private:
    using super = PanelWidget;
public:
    Diffractogram(TheHub&);

    void render();

    data::shp_Dataset dataset() const { return dataset_; }

private:
    void setDataset(data::shp_Dataset);

    data::shp_Dataset dataset_;

    DiffractogramPlot* plot_;

    typ::Curve dgram_, dgramBgFitted_, bg_;
    typ::curve_vec refls_;

    uint currReflIndex_;
    calc::shp_Reflection currentReflection_;

    QComboBox* comboNormType_;
    QRadioButton *intenSum_, *intenAvg_;
    QDoubleSpinBox* intenScale_;
    QToolButton* enableZoom_;
    Action* actZoom_;

public:
    void calcDgram();
    void calcBackground();
    void calcReflections();

    void setCurrReflNewRange(typ::Range const&);
    typ::Range currReflRange() const;
};
}
}
#endif // PANEL_DIFFRACTOGRAM_H

// ************************************************************************** //
//
//  Steca2: stress and texture calculator
//
//! @file      gui/output/output_diagrams.cpp
//! @brief     Implements class DiagramsFrame
//!
//! @homepage  https://github.com/scgmlz/Steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#include "output_diagrams.h"
#include "dialog_panels.h"
#include "fit/fit_fun.h"
#include "output_dialogs.h"
#include "session.h"
#include "thehub.h"
//#include "typ/variant.h"
#include "widgets/widget_makers.h"
#include "write_file.h"
#include "QCustomPlot/qcustomplot.h"
#include <QAction>

namespace gui {
namespace output {

// sorts xs and ys the same way, by (x,y)
static void sortColumns(qreal_vec& xs, qreal_vec& ys, uint_vec& is) {
    debug::ensure(xs.count() == ys.count());

    uint count = xs.count();

    is.resize(count);
    for_i (count)
        is[i] = i;

    std::sort(is.begin(), is.end(), [&xs, &ys](uint i1, uint i2) {
        qreal x1 = xs.at(i1), x2 = xs.at(i2);
        if (x1 < x2)
            return true;
        if (x1 > x2)
            return false;
        return ys.at(i1) < ys.at(i2);
    });

    qreal_vec r(count);

    for_i (count)
        r[i] = xs.at(is.at(i));

    xs = r;

    for_i (count)
        r[i] = ys.at(is.at(i));

    ys = r;
}

static const Params::ePanels PANELS =
    Params::ePanels(Params::REFLECTION | Params::GAMMA | Params::DIAGRAM);

// ************************************************************************** //
//  class TabPlot (file scope)
// ************************************************************************** //

class TabPlot : public QCustomPlot {
private:
public:
    TabPlot();
    void set(ReflectionInfos);
    void plot(
        qreal_vec const& xs, qreal_vec const& ys, qreal_vec const& ysLo, qreal_vec const& ysUp);
protected:
    QCPGraph *graph_, *graphLo_, *graphUp_;
};

TabPlot::TabPlot() {
    graph_ = addGraph();
    graphLo_ = addGraph();
    graphUp_ = addGraph();
}

void TabPlot::plot(
    qreal_vec const& xs, qreal_vec const& ys, qreal_vec const& ysLo, qreal_vec const& ysUp) {
    debug::ensure(xs.count() == ys.count());

    uint count = xs.count();

    graph_->clearData();
    graphUp_->clearData();
    graphLo_->clearData();

    Range rgeX, rgeY;

    for_i (count) {
        rgeX.extendBy(xs.at(i));
        rgeY.extendBy(ys.at(i));
    }

    if (!count || rgeX.isEmpty() || rgeY.isEmpty()) {
        xAxis->setVisible(false);
        yAxis->setVisible(false);
        replot();
        return;
    }

    xAxis->setRange(rgeX.min, rgeX.max);
    yAxis->setRange(rgeY.min, rgeY.max);
    xAxis->setVisible(true);
    yAxis->setVisible(true);

    graph_->setPen(QPen(Qt::blue));
    graph_->addData(xs.sup(), ys.sup());

    graphUp_->setPen(QPen(Qt::red));
    graphUp_->addData(xs.sup(), ysUp.sup());

    graphLo_->setPen(QPen(Qt::green));
    graphLo_->addData(xs.sup(), ysLo.sup());

    replot();
}

// ************************************************************************** //
//  class TabDiagramsSave (file scope)
// ************************************************************************** //

class TabDiagramsSave : public TabSave {
public:
    TabDiagramsSave(Params&);
    uint currType() const { return fileTypes_->currentIndex(); }
    bool currDiagram() const { return currentDiagram_->isChecked(); }
protected:
    QRadioButton *currentDiagram_, *allData_;
    QComboBox* fileTypes_;
};

TabDiagramsSave::TabDiagramsSave(Params& params) : TabSave(params, true) {
    auto gp = new GridPanel("To save");
    grid_->addWidget(gp, grid_->rowCount(), 0, 1, 2);
    grid_->setRowStretch(grid_->rowCount(), 1);

    auto g = gp->grid();
    g->addWidget((currentDiagram_ = radioButton("Current diagram")));
    g->addWidget((allData_ = radioButton("All data")));
    g->addWidget(textButton(actSave), 1, 1);
    g->setColumnStretch(0, 1);

    currentDiagram_->setChecked(true);
}

// ************************************************************************** //
//  class DiagramsFrame
// ************************************************************************** //

DiagramsFrame::DiagramsFrame(rcstr title, QWidget* parent)
    : Frame(title, new Params(PANELS), parent) {
    btnInterpolate_->hide();

    tabPlot_ = new TabPlot();
    tabs_->addTab("Diagram", Qt::Vertical).box().addWidget(tabPlot_);

    debug::ensure(params_->panelDiagram);
    auto pd = params_->panelDiagram;

    connect(pd->xAxis, slot(QComboBox, currentIndexChanged, int), [this]() { recalculate(); });

    connect(pd->yAxis, slot(QComboBox, currentIndexChanged, int), [this]() { recalculate(); });

    tabSave_ = new TabDiagramsSave(*params_);
    tabs_->addTab("Save", Qt::Vertical).box().addWidget(tabSave_);

    connect(tabSave_->actSave, &QAction::triggered, [this]() { saveDiagramOutput(); });

    recalculate();
}

DiagramsFrame::eReflAttr DiagramsFrame::xAttr() const {
    debug::ensure(params_->panelDiagram);
    return eReflAttr(params_->panelDiagram->xAxis->currentIndex());
}

DiagramsFrame::eReflAttr DiagramsFrame::yAttr() const {
    debug::ensure(params_->panelDiagram);
    return eReflAttr(params_->panelDiagram->yAxis->currentIndex());
}

void DiagramsFrame::displayReflection(uint reflIndex, bool interpolated) {
    Frame::displayReflection(reflIndex, interpolated);
    rs_ = calcPoints_.at(reflIndex);
    recalculate();
}

void DiagramsFrame::recalculate() {

    uint count = rs_.count();

    xs_.resize(count);
    ys_.resize(count);

    uint xi = uint(xAttr());
    uint yi = uint(yAttr());

    for_i (count) {
        auto row = rs_.at(i).data();
        xs_[i] = row.at(xi).toDouble();
        ys_[i] = row.at(yi).toDouble();
    }

    uint_vec is;
    sortColumns(xs_, ys_, is);

    auto calcErrors = [this, is](eReflAttr attr) {
        uint count = ys_.count();
        ysErrorLo_.resize(count);
        ysErrorUp_.resize(count);

        for_i (count) {
            auto row = rs_.at(is.at(i)).data(); // access error over sorted index vec
            qreal sigma = row.at(uint(attr)).toDouble();
            qreal y = ys_.at(i);
            ysErrorLo_[i] = y - sigma;
            ysErrorUp_[i] = y + sigma;
        }
    };

    ysErrorLo_.clear();
    ysErrorUp_.clear();

    if (gSession->reflections().at(getReflIndex())->peakFunction().name() != "Raw") {
        switch (yAttr()) {
        case eReflAttr::INTEN: calcErrors(eReflAttr::SIGMA_INTEN); break;
        case eReflAttr::TTH: calcErrors(eReflAttr::SIGMA_TTH); break;
        case eReflAttr::FWHM: calcErrors(eReflAttr::SIGMA_FWHM); break;
        default: break;
        }
    }

    tabPlot_->plot(xs_, ys_, ysErrorLo_, ysErrorUp_);
}

void DiagramsFrame::saveDiagramOutput() {
    str path = tabSave_->filePath(true);
    if (path.isEmpty()) {
        qWarning() << "cannot save diagram: path is empty";
        return;
    }
    str separator = tabSave_->separator();
    if (tabSave_->currDiagram())
        writeCurrentDiagramOutputFile(path, separator);
    else
        writeAllDataOutputFile(path, separator);
    qDebug() /* qInfo() TODO restore */ << "diagram saved to " << path;
}

void DiagramsFrame::writeCurrentDiagramOutputFile(rcstr filePath, rcstr separator) {
    WriteFile file(filePath);

    QTextStream stream(&file);

    debug::ensure(xs_.count() == ys_.count());
    debug::ensure(ysErrorLo_.isEmpty() || ysErrorLo_.count() == ys_.count());
    debug::ensure(ysErrorLo_.count() == ysErrorUp_.count());

    bool writeErrors = !ysErrorUp_.isEmpty();

    for_i (xs_.count()) {
        stream << xs_.at(i) << separator << ys_.at(i);
        if (writeErrors)
            stream << separator << ysErrorLo_.at(i) << separator << ysErrorUp_.at(i);
        stream << '\n';
    }
}

void DiagramsFrame::writeAllDataOutputFile(rcstr filePath, rcstr separator) {
    WriteFile file(filePath);
    QTextStream stream(&file);

    auto headers = table_->outHeaders();
    for_i (headers.count())
        stream << headers.at(to_u(i)) << separator;
    stream << '\n';

    for_i (calcPoints_.at(getReflIndex()).count()) {
        auto& row = table_->row(i);
        for_i (row.count()) {
            QVariant const& var = row.at(i);
            if (isNumeric(var))
                stream << var.toDouble();
            else
                stream << var.toString();

            stream << separator;
        }
        stream << '\n';
    }
}

} // namespace output
} // namespace gui

// ************************************************************************** //
//
//  Steca: stress and texture calculator
//
//! @file      gui/output/output_polefigures.cpp
//! @brief     Implements class PoleFiguresFrame
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#include "gui/output/output_polefigures.h"
#include "core/fit/fit_fun.h"
#include "core/session.h"
#include "gui/cfg/colors.h"
#include "gui/output/dialog_panels.h"
#include "gui/output/write_file.h"
#include "gui/output/widgets4output.h"
#include "gui/thehub.h"
#include "gui/widgets/new_q.h"
#include "gui/widgets/various_widgets.h"
#include <qmath.h>
#include <QPainter>

// ************************************************************************** //
//  local class TabGraph
// ************************************************************************** //

class TabGraph final : public QWidget {
public:
    TabGraph(Params&);
    void set(ReflectionInfos);

private:
    Params& params_;
    QGridLayout* grid_;
    void update();

    ReflectionInfos rs_;
    void paintEvent(QPaintEvent*);

    QPointF p(deg alpha, deg beta) const;
    deg alpha(QPointF const&) const;
    deg beta(QPointF const&) const;

    void circle(QPointF c, qreal r);

    void paintGrid();
    void paintPoints();

    // valid during paintEvent
    QPainter* p_;
    QPointF c_;
    qreal r_;

    bool flat_;
    qreal alphaMax_, avgAlphaMax_;

    QCheckBox* cbFlat_;
};

TabGraph::TabGraph(Params& params)
    : params_(params), flat_(false), alphaMax_(90), avgAlphaMax_(0) {
    setLayout((grid_ = newQ::GridLayout()));
    debug::ensure(params_.panelInterpolation);

    grid_->addWidget((cbFlat_ = newQ::CheckBox("no intensity")), 0, 0);

    grid_->setRowStretch(grid_->rowCount(), 1);
    grid_->setColumnStretch(grid_->columnCount(), 1);

    connect(
        params_.panelInterpolation->avgAlphaMax, slot(QDoubleSpinBox, valueChanged, double),
        [this]() { update(); });

    connect(cbFlat_, &QCheckBox::toggled, [this]() { update(); });

    update();
}

void TabGraph::set(ReflectionInfos rs) {
    rs_ = rs;
    update();
}

void TabGraph::update() {
    avgAlphaMax_ = params_.panelInterpolation->avgAlphaMax->value();
    flat_ = cbFlat_->isChecked();
    QWidget::update();
}

void TabGraph::paintEvent(QPaintEvent*) {
    int w = size().width(), h = size().height();

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.translate(w / 2, h / 2);

    p_ = &painter;
    c_ = QPointF(0, 0);
    r_ = qMin(w, h) / 2;

    paintGrid();
    paintPoints();
}

QPointF TabGraph::p(deg alpha, deg beta) const {
    qreal r = r_ * alpha / alphaMax_;

    rad betaRad = beta.toRad();
    return QPointF(r * cos(betaRad), -r * sin(betaRad));
}

deg TabGraph::alpha(QPointF const& p) const {
    return sqrt(p.x() * p.x() + p.y() * p.y()) / r_ * alphaMax_;
}

deg TabGraph::beta(QPointF const& p) const {
    deg b = rad(atan2(p.y(), p.x())).toDeg();
    return b <= 0 ? -b : 360 - b;
}

void TabGraph::circle(QPointF c, qreal r) {
    p_->drawEllipse(c, r, r);
}

void TabGraph::paintGrid() {
    QPen penMajor(Qt::gray), penMinor(Qt::lightGray);

    for (int alpha = 10; alpha <= 90; alpha += 10) {
        qreal r = r_ / alphaMax_ * alpha;
        p_->setPen(!(alpha % 30) ? penMajor : penMinor);
        circle(c_, r);
    }

    for (int beta = 0; beta < 360; beta += 10) {
        p_->setPen(!(beta % 30) ? penMajor : penMinor);
        p_->drawLine(p(10, beta), p(90, beta));
    }

    QPen penMark(Qt::darkGreen);
    p_->setPen(penMark);
    circle(c_, r_ * avgAlphaMax_ / alphaMax_);
}

void TabGraph::paintPoints() {
    qreal rgeMax = rs_.rgeInten().max;

    for (auto& r : rs_) {
        qreal inten = r.inten();
        if (!qIsFinite(inten)) // nan comes from interpolation
            continue;
        auto pp = p(r.alpha(), r.beta());
        if (flat_) {
            QColor color(Qt::blue);
            p_->setPen(color);
            p_->setBrush(color);
            circle(pp, .5);
        } else {
            inten /= rgeMax;
            QColor color = colormap::intenGraph(inten, 1);
            p_->setPen(color);
            p_->setBrush(color);
            circle(pp, inten * r_ / 60); // TODO scale to max inten
        }
    }
}

// ************************************************************************** //
//  local class TabPoleFiguresSave
// ************************************************************************** //

class TabPoleFiguresSave final : public TabSave {
public:
    TabPoleFiguresSave(Params& params);

    bool onlySelectedRefl() const;
    bool outputInten() const;
    bool outputTth() const;
    bool outputFWHM() const;

    void rawReflSettings(bool on);

private:
    QRadioButton *rbSelectedRefl_, *rbAllRefls_;
    QCheckBox *outputInten_, *outputTth_, *outputFWHM_;
};

TabPoleFiguresSave::TabPoleFiguresSave(Params& params) : TabSave(params, false) {
    auto hb = newQ::HBoxLayout();
    grid_->addLayout(hb, grid_->rowCount(), 0);
    grid_->setRowStretch(grid_->rowCount(), 1);

    auto p1 = new GridPanel("Output data");
    auto p2 = new GridPanel("To save");

    hb->addWidget(p1);
    hb->addWidget(p2);
    hb->addStretch();

    {
        auto g = p1->grid();
        g->addWidget((outputInten_ = newQ::CheckBox("Intensity pole figure")));
        g->addWidget((outputTth_ = newQ::CheckBox("Peak position pole figure")));
        g->addWidget((outputFWHM_ = newQ::CheckBox("TWHM pole figure")));
        g->setRowStretch(g->rowCount(), 1);
    }

    {
        auto g = p2->grid();
        g->addWidget((rbSelectedRefl_ = newQ::RadioButton("Selected reflection")));
        g->addWidget((rbAllRefls_ = newQ::RadioButton("All reflections")));
        g->addWidget(newQ::TextButton(actSave), 2, 1);
        g->setRowStretch(g->rowCount(), 1);
    }

    rbSelectedRefl_->setChecked(true);
    outputInten_->setChecked(true);
}

bool TabPoleFiguresSave::onlySelectedRefl() const {
    return rbSelectedRefl_->isChecked();
}

bool TabPoleFiguresSave::outputInten() const {
    return outputInten_->isChecked();
}

bool TabPoleFiguresSave::outputTth() const {
    return outputTth_->isChecked();
}

bool TabPoleFiguresSave::outputFWHM() const {
    return outputFWHM_->isChecked();
}

void TabPoleFiguresSave::rawReflSettings(bool on) {
    outputTth_->setEnabled(on);
    outputFWHM_->setEnabled(on);
}


// ************************************************************************** //
//  class PoleFiguresFrame
// ************************************************************************** //

static const Params::ePanels PANELS =
    Params::ePanels(Params::REFLECTION | Params::GAMMA | Params::POINTS | Params::INTERPOLATION);

PoleFiguresFrame::PoleFiguresFrame(rcstr title, QWidget* parent)
    : Frame(title, new Params(PANELS), parent) {
    tabGraph_ = new TabGraph(*params_);
    newQ::Tab(tabs_, "Graph")->box().addWidget(tabGraph_);

    tabSave_ = new TabPoleFiguresSave(*params_);
    newQ::Tab(tabs_, "Save")->box().addWidget(tabSave_);

    connect( tabSave_->actSave, &QAction::triggered, [this]() { savePoleFigureOutput(); });
    show();
}

void PoleFiguresFrame::displayReflection(uint reflIndex, bool interpolated) {
    Frame::displayReflection(reflIndex, interpolated);
    if (!interpPoints_.isEmpty() && !calcPoints_.isEmpty())
        tabGraph_->set((interpolated ? interpPoints_ : calcPoints_).at(reflIndex));
    tabSave_->rawReflSettings(
        gSession->reflections().at(reflIndex)->peakFunction().name() != "Raw");
}

void PoleFiguresFrame::savePoleFigureOutput() {
    auto& reflections = gSession->reflections();
    if (reflections.isEmpty()) {
        qWarning() << "cannot save pole figure: no reflection chosen";
        return;
    }
    str path = tabSave_->filePath(false);
    if (path.isEmpty()) {
        qWarning() << "cannot save pole figure: file path is empty";
        return;
    }
    if (tabSave_->onlySelectedRefl()) {
        writePoleFigureOutputFiles(path, getReflIndex());
        return;
    }
    // all reflections
    for_i (reflections.count()) // TODO collect output into one message
        writePoleFigureOutputFiles(path, i);
}

static str const OUT_FILE_TAG(".refl%1");
static int const MAX_LINE_LENGTH_POL(9);

void PoleFiguresFrame::writePoleFigureOutputFiles(rcstr filePath, uint index) {
    auto refl = gSession->reflections().at(index);
    ReflectionInfos reflInfo;
    if (getInterpolated())
        reflInfo = interpPoints_.at(index);
    else
        reflInfo = calcPoints_.at(index);
    bool withFit = refl->peakFunction().name() != "Raw";
    str path = str(filePath + OUT_FILE_TAG).arg(index + 1);
    bool check = false;
    uint numSavedFiles = 0;

    if (tabSave_->outputInten()) {
        qreal_vec output;
        for_i (reflInfo.count())
            output.append(reflInfo.at(i).inten());
        auto intenFilePath = path + ".inten";
        writeListFile(intenFilePath, reflInfo, output);
        writePoleFile(intenFilePath, reflInfo, output);
        writeErrorMask(intenFilePath, reflInfo, output);
        check = true;
        numSavedFiles += 3;
    }

    if (tabSave_->outputTth() && withFit) {
        qreal_vec output;
        for_i (reflInfo.count())
            output.append(reflInfo.at(i).tth());
        auto tthFilePath = filePath + ".tth";
        writeListFile(tthFilePath, reflInfo, output);
        writePoleFile(tthFilePath, reflInfo, output);
        check = true;
        numSavedFiles += 2;
    }

    if (tabSave_->outputFWHM() && withFit) {
        qreal_vec output;
        for_i (reflInfo.count())
            output.append(reflInfo.at(i).fwhm());
        auto fwhmFilePath = filePath + ".fwhm";
        writeListFile(fwhmFilePath, reflInfo, output);
        writePoleFile(fwhmFilePath, reflInfo, output);
        check = true;
        numSavedFiles += 2;
    }

    if (numSavedFiles > 0) {
        if (check)
            qDebug() /* qInfo() TODO restore */ << numSavedFiles << " files have been saved";
        else
            qWarning() << "something went wrong, yet " << numSavedFiles << " files have been saved";
    } else
        qWarning() << "no files saved";
}

void PoleFiguresFrame::writeErrorMask(
    rcstr filePath, ReflectionInfos reflInfo, qreal_vec const& output) {
    WriteFile file(filePath + ".errorMask");
    QTextStream stream(&file);

    for (uint j = 0, jEnd = reflInfo.count(); j < jEnd; j += 9) {
        uint max = j + MAX_LINE_LENGTH_POL;
        for (uint i = j; i < max; i++) {
            if (qIsNaN(output.at(i)))
                stream << "0"
                       << " ";
            else
                stream << "1"
                       << " ";
        }
        stream << '\n';
    }
}

void PoleFiguresFrame::writePoleFile(
    rcstr filePath, ReflectionInfos reflInfo, qreal_vec const& output) {
    WriteFile file(filePath + ".pol");
    QTextStream stream(&file);

    for (uint j = 0, jEnd = reflInfo.count(); j < jEnd; j += 9) {
        uint max = j + MAX_LINE_LENGTH_POL;
        for (uint i = j; i < max; i++) {
            if (qIsNaN(output.at(i)))
                stream << " -1 "
                       << " ";
            else
                stream << output.at(i) << " ";
        }
        stream << '\n';
    }
}

void PoleFiguresFrame::writeListFile(
    rcstr filePath, ReflectionInfos reflInfo, qreal_vec const& output) {
    WriteFile file(filePath + ".lst");
    QTextStream stream(&file);

    for_i (reflInfo.count()) {
        stream << qreal(reflInfo.at(i).alpha()) << " " << qreal(reflInfo.at(i).beta()) << " "
               << output.at(i) << '\n';
    }
}

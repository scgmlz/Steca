// ************************************************************************** //
//
//  Steca2: stress and texture calculator
//
//! @file      gui/output/frame.cpp
//! @brief     Implements class Frame
//!
//! @homepage  https://github.com/scgmlz/Steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#include "output_dialogs.h"
#include "actions.h"
#include "dialog_panels.h"
#include "calc/calc_polefigure.h"
#include "session.h"
#include "thehub.h"
#include "frame.h"
#include "widgets/widget_makers.h"
#include <QAction>
#include <QProgressBar>
#include <QScrollArea>

namespace {

// ************************************************************************** //
//  local class TabTable
// ************************************************************************** //

using output::Params;
using output::Table;

class TabTable : public output::Tab {
public:

    TabTable(Params&, QStringList const& headers, QStringList const& outHeaders, cmp_vec const&);
private:
    struct showcol_t {
        str name;
        QCheckBox* cb;
    };
    typedef vec<showcol_t> showcol_vec;

    class ShowColsWidget : public QWidget {
    private:
    public:
        ShowColsWidget(Table&, showcol_vec&);
    private:
        Table& table_;
        showcol_vec& showCols_;
        QBoxLayout* box_;
        QRadioButton *rbHidden_, *rbAll_, *rbNone_, *rbInten_, *rbTth_, *rbFWHM_;
    };
public:
    Table* table;
private:
    ShowColsWidget* showColumnsWidget_;
    showcol_vec showCols_;
};

TabTable::TabTable(
    Params& params, QStringList const& headers, QStringList const& outHeaders, cmp_vec const& cmps)
    : Tab(params) {
    debug::ensure(to_u(headers.count()) == cmps.count());
    uint numCols = to_u(headers.count());

    grid_->addWidget((table = new Table(numCols)), 0, 0);
    grid_->setColumnStretch(0, 1);

    table->setColumns(headers, outHeaders, cmps);

    for_i (numCols) {
        showcol_t item;
        item.name = headers.at(i);
        showCols_.append(item);
    }

    auto scrollArea = new QScrollArea;
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setWidget((showColumnsWidget_ = new ShowColsWidget(*table, showCols_)));

    grid_->addWidget(scrollArea, 0, 1);
}

TabTable::ShowColsWidget::ShowColsWidget(Table& table, showcol_vec& showCols)
    : table_(table), showCols_(showCols) {
    using eReflAttr = ReflectionInfo::eReflAttr;

    setLayout((box_ = vbox()));

    box_->addWidget((rbHidden_ = radioButton("")));
    rbHidden_->hide();

    box_->addWidget((rbAll_ = radioButton("all")));
    box_->addWidget((rbNone_ = radioButton("none")));
    box_->addWidget(rbInten_ = radioButton("Intensity"));
    box_->addWidget(rbTth_ = radioButton("2θ"));
    box_->addWidget(rbFWHM_ = radioButton("fwhm"));
    box_->addSpacing(8);

    for_i (showCols.count()) {
        auto& item = showCols[i];
        box_->addWidget((item.cb = check(item.name)));
    }

    auto all = [this]() {
        for (auto& col : showCols_)
            col.cb->setChecked(true);
    };

    auto none = [this]() {
        for (auto& col : showCols_)
            col.cb->setChecked(false);
    };

    auto showInten = [this, none]() {
        none();
        showCols_.at(uint(eReflAttr::INTEN)).cb->setChecked(true);
    };

    auto showTth = [this, none]() {
        none();
        showCols_.at(uint(eReflAttr::TTH)).cb->setChecked(true);
    };

    auto showFWHM = [this, none]() {
        none();
        showCols_.at(uint(eReflAttr::FWHM)).cb->setChecked(true);
    };

    auto updateRbs = [this]() {
        bool isAll = true, isNone = true, isOther = false;
        uint nInten = 0, nTth = 0, nFwhm = 0;

        for_i (showCols_.count()) {
            if (!showCols_.at(i).cb->isChecked()) {
                isAll = false;
                continue;
            }

            isNone = false;

            switch (eReflAttr(i)) {
            case eReflAttr::ALPHA:
            case eReflAttr::BETA:
                ++nInten;
                ++nTth;
                ++nFwhm;
                break;
            case eReflAttr::INTEN: ++nInten; break;
            case eReflAttr::TTH: ++nTth; break;
            case eReflAttr::FWHM: ++nFwhm; break;
            default: isOther = true; break;
            }
        }

        rbHidden_->setChecked(true);
        rbNone_->setChecked(isNone);
        rbAll_->setChecked(isAll);

        uint const PRESET_SELECTION = 1;

        rbInten_->setChecked(!isOther && PRESET_SELECTION == nInten);
        rbTth_->setChecked(!isOther && PRESET_SELECTION == nTth);
        rbFWHM_->setChecked(!isOther && PRESET_SELECTION == nFwhm);
    };

    for_i (showCols_.count()) {
        auto cb = showCols_.at(i).cb;

        connect(cb, &QCheckBox::toggled, [this, updateRbs, i](bool on) {
            if (on)
                table_.showColumn(to_i(i) + 1);
            else
                table_.hideColumn(to_i(i) + 1);

            updateRbs();
        });
    }

    connect(rbAll_, &QRadioButton::clicked, all);
    connect(rbNone_, &QRadioButton::clicked, none);
    connect(rbInten_, &QRadioButton::clicked, showInten);
    connect(rbTth_, &QRadioButton::clicked, showTth);
    connect(rbFWHM_, &QRadioButton::clicked, showFWHM);

    rbAll_->click();
}

} // anonymous namespace


// ************************************************************************** //
//  class Frame
// ************************************************************************** //

namespace output {

Frame::Frame(rcstr title, Params* params, QWidget* parent)
    : QDialog(parent) {
    setModal(true);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    setWindowTitle(title);
    setLayout((box_ = vbox()));

    debug::ensure(params);

    box_->addWidget((params_ = params));
    box_->addWidget((tabs_ = new panel::TabsPanel()));
    box_->setStretch(box_->count() - 1, 1);

    auto hb = hbox();
    box_->addLayout(hb);

    actClose_ = newTrigger("Close");
    actCalculate_ = newTrigger("Calculate");
    actInterpolate_ = newTrigger("Interpolate");

    hb->addWidget((btnClose_ = textButton(actClose_)));
    hb->addStretch(1);
    hb->addWidget((pb_ = new QProgressBar));
    hb->setStretchFactor(pb_, 333);
    hb->addStretch(1);
    hb->addWidget((btnCalculate_ = textButton(actCalculate_)));
    hb->addWidget((btnInterpolate_ = textButton(actInterpolate_)));

    pb_->hide();

    connect(actClose_, &QAction::triggered, [this]() { close(); });
    connect(actCalculate_, &QAction::triggered, [this]() { calculate(); });
    connect(actInterpolate_, &QAction::triggered, [this]() { interpolate(); });

    auto updateDisplay = [this]() { displayReflection(getReflIndex(), getInterpolated()); };

    if (params_->panelReflection) {
        connect(
            params_->panelReflection->cbRefl, slot(QComboBox, currentIndexChanged, int),
            [updateDisplay]() { updateDisplay(); });
    }

    if (params_->panelPoints) {
        debug::ensure(params_->panelReflection);
        connect(params_->panelPoints->rbInterp, &QRadioButton::toggled, [updateDisplay]() {
            updateDisplay();
        });
    }

    // tabs

    auto tabTable = new TabTable(
        *params_, ReflectionInfo::dataTags(false), ReflectionInfo::dataTags(true),
        ReflectionInfo::dataCmps());
    tabs_->addTab("Points", Qt::Vertical).box().addWidget(tabTable);

    table_ = tabTable->table;

    uint reflCount = gSession->reflections().count();
    calcPoints_.resize(reflCount);
    interpPoints_.resize(reflCount);
}

void Frame::calculate() {
    TakesLongTime __;

    calcPoints_.clear();
    interpPoints_.clear();

    auto& reflections = gSession->reflections();
    if (!reflections.isEmpty()) {
        uint reflCount = reflections.count();

        auto ps = params_->panelGammaSlices;
        debug::ensure(ps);

        uint gammaSlices = to_u(ps->numSlices->value());

        auto pr = params_->panelGammaRange;
        debug::ensure(pr);

        Range rgeGamma;
        if (pr->cbLimitGamma->isChecked())
            rgeGamma.safeSet(pr->minGamma->value(), pr->maxGamma->value());

        Progress progress(reflCount, pb_);

        for_i (reflCount)
            calcPoints_.append(
                gSession->makeReflectionInfos(
                    gSession->experiment(), *reflections.at(i),
                    gammaSlices, rgeGamma, &progress));
    }

    interpolate();
}

void Frame::interpolate() {
    TakesLongTime __;

    interpPoints_.clear();

    auto pi = params_->panelInterpolation;
    if (pi) {
        deg alphaStep = pi->stepAlpha->value();
        deg betaStep = pi->stepBeta->value();
        qreal idwRadius = pi->idwRadius->value();

        qreal avgRadius = pi->avgRadius->value();
        qreal avgAlphaMax = pi->avgAlphaMax->value();
        qreal avgTreshold = pi->avgThreshold->value() / 100.0;

        Progress progress(calcPoints_.count(), pb_);

        for_i (calcPoints_.count())
            interpPoints_.append(interpolateInfos(
                calcPoints_.at(i), alphaStep, betaStep, idwRadius, avgAlphaMax, avgRadius,
                avgTreshold, &progress));
    } else {
        for_i (calcPoints_.count())
            interpPoints_.append(ReflectionInfos());
    }

    displayReflection(getReflIndex(), getInterpolated());
}

void Frame::displayReflection(uint reflIndex, bool interpolated) {
    table_->clear();

    debug::ensure(calcPoints_.count() == interpPoints_.count());
    if (calcPoints_.count() <= reflIndex)
        return;

    for (auto& r : (interpolated ? interpPoints_ : calcPoints_).at(reflIndex))
        table_->addRow(r.data(), false);

    table_->sortData();
}

uint Frame::getReflIndex() const {
    debug::ensure(params_->panelReflection);
    int reflIndex = params_->panelReflection->cbRefl->currentIndex();
    RUNTIME_CHECK(reflIndex >= 0, "invalid reflection index");
    return to_u(reflIndex);
}

bool Frame::getInterpolated() const {
    auto pi = params_->panelPoints;
    return pi ? pi->rbInterp->isChecked() : false;
}

} // namespace output

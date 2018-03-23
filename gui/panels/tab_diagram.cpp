// ************************************************************************** //
//
//  Steca: stress and texture calculator
//
//! @file      gui/panels/tab_diagram.cpp
//! @brief     Implements class DiagramWidget, and its dependences
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#include "tab_diagram.h"
#include "core/session.h"
#include "gui/actions/toggles.h"
#include "gui/actions/triggers.h"
#include "gui/plot/plot_diagram.h"
#include <iostream>

// sorts xs and ys the same way, by (x,y)
static void sortColumns(QVector<qreal>& xs, QVector<qreal>& ys, QVector<int>& is)
{
    ASSERT(xs.count() == ys.count());

    int count = xs.count();

    is.resize(count);
    for_i (count)
        is[i] = i;

    std::sort(is.begin(), is.end(), [&xs, &ys](int i1, int i2) {
        qreal x1 = xs.at(i1), x2 = xs.at(i2);
        if (x1 < x2)
            return true;
        if (x1 > x2)
            return false;
        return ys.at(i1) < ys.at(i2);
    });

    QVector<qreal> r(count);

    for_i (count)
        r[i] = xs.at(is.at(i));
    xs = r;

    for_i (count)
        r[i] = ys.at(is.at(i));
    ys = r;
}

// ************************************************************************** //
//  local class SelectXY
// ************************************************************************** //

//! Controls to choose metadata?

class SelectXY : public QGridLayout {
public:
    SelectXY();
    CComboBox xAxis {"xAxis"};
    CComboBox yAxis {"yAxis"};
};

SelectXY::SelectXY()
{
    QStringList tags = PeakInfo::dataTags(false);
    for_i (Metadata::numAttributes(false) - Metadata::numAttributes(true))
        tags.removeLast(); // remove all tags that are not numbers
    xAxis.addItems(tags);
    yAxis.addItems(tags);

    addWidget(new QLabel("x"), 1, 0);
    addWidget(&xAxis, 1, 1);
    addWidget(new QLabel("y"), 0, 0);
    addWidget(&yAxis, 0, 1);
    //setColumnStretch(2, 1);
}

// ************************************************************************** //
//  class DiagramWidget
// ************************************************************************** //

DiagramWidget::DiagramWidget()
{
    // initializations
    plot_ = new PlotDiagram; // the main subframe
    selectXY_ = new SelectXY;

    // internal connections
    connect(&selectXY_->xAxis, qOverload<int>(&QComboBox::currentIndexChanged), [this]() {
            render(); });
    connect(&selectXY_->yAxis, qOverload<int>(&QComboBox::currentIndexChanged), [this]() {
            render(); });

    // inbound connection
    connect(gSession, &Session::sigRawFits, [this]() {
            if (isVisible())
                render(); });

    // layout
    auto* buttonBox = new QHBoxLayout;
    buttonBox->addStretch(1);
    buttonBox->addWidget(new XIconButton {&gGui->triggers->spawnXY});
    buttonBox->addWidget(new XIconButton {&gGui->triggers->exportXY});

    auto* controls = new QVBoxLayout;
    controls->addLayout(selectXY_);
    controls->addStretch(1); // ---
    controls->addLayout(buttonBox);

    auto* layout = new QHBoxLayout;
    layout->addWidget(plot_);
    layout->addLayout(controls);
    layout->setStretch(0,1000);
    setLayout(layout);
}

void DiagramWidget::render()
{
    rs_ = gSession->peakInfos();
    int count = rs_.count();

    xs_.resize(count);
    ys_.resize(count);

    int xi = int(selectXY_->xAxis.currentIndex());
    int yi = int(selectXY_->yAxis.currentIndex());

    for_i (count) {
        const row_t row = rs_.at(i).data();
        xs_[i] = row.at(xi).toDouble();
        ys_[i] = row.at(yi).toDouble();
    }

    QVector<int> is;
    sortColumns(xs_, ys_, is);

    auto _calcErrors = [this, is](eReflAttr attr) {
        int count = ys_.count();
        ysErrorLo_.resize(count);
        ysErrorUp_.resize(count);

        for_i (count) {
            const row_t row = rs_.at(is.at(i)).data(); // access error over sorted index vec
            qreal sigma = row.at(int(attr)).toDouble();
            qreal y = ys_.at(i);
            ysErrorLo_[i] = y - sigma;
            ysErrorUp_[i] = y + sigma;
        }
    };

    ysErrorLo_.clear();
    ysErrorUp_.clear();

    int iRefl = gSession->peaks().selectedIndex();
    if (!gSession->peaks().at(iRefl).isRaw()) {
        switch ((PeakInfo::eReflAttr)selectXY_->yAxis.currentIndex()) {
        case eReflAttr::INTEN:
            _calcErrors(eReflAttr::SIGMA_INTEN);
            break;
        case eReflAttr::TTH:
            _calcErrors(eReflAttr::SIGMA_TTH);
            break;
        case eReflAttr::FWHM:
            _calcErrors(eReflAttr::SIGMA_FWHM);
            break;
        default:
            break;
        }
    }

    plot_->plot(xs_, ys_, ysErrorLo_, ysErrorUp_);
    show();
}

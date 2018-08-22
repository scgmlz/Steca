//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      gui/panels/controls_baseline.cpp
//! @brief     Implements class ControlsBaseline
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#include "gui/panels/controls_baseline.h"
#include "core/session.h"
#include "gui/actions/triggers.h"
#include "gui/mainwin.h"
#include "gui/view/range_control.h"
#include "qcr/widgets/tables.h"
#include "qcr/base/debug.h" // ASSERT

//  ***********************************************************************************************
//! @class BaseRangesModel, used in BaseRangesView (local scope)

//! Model for BaseRangesView.

class BaseRangesModel : public TableModel {
public:
    BaseRangesModel() : TableModel("baseline") {}

    enum { COL_RANGE = 1, NUM_COLUMNS };

private:
    int columnCount() const final { return NUM_COLUMNS; }
    int rowCount() const final { return gSession->baseline.ranges.size(); }
    int highlighted() const final { return gSession->baseline.ranges.selectedIndex(); }
    void onHighlight(int row) final { gSession->baseline.ranges.select(row); }

    QVariant data(const QModelIndex&, int) const;
};

QVariant BaseRangesModel::data(const QModelIndex& index, int role) const
{
    int row = index.row();
    if (row < 0 || rowCount() <= row)
        return {};
    const Range& range = gSession->baseline.ranges.at(row);
    switch (role) {
    case Qt::DisplayRole: {
        int col = index.column();
        if (col < 1)
            return {};
        switch (col) {
        case COL_RANGE:
            return range.to_s();
        default:
            return {};
        }
    }
    case Qt::ForegroundRole: {
        if (range.isEmpty())
            return QColor(Qt::red);
        return QColor(Qt::black);
    }
    case Qt::BackgroundRole: {
        if (row==highlighted())
            return QColor(Qt::green);
        return QColor(Qt::white);
    }
    default:
        return {};
    }
}

//  ***********************************************************************************************
//! @class BaseRangesView (local scope)

//! List view of user-defined Bragg peaks.

class BaseRangesView : public TableView {
public:
    BaseRangesView() : TableView{new BaseRangesModel()} {}
};

//  ***********************************************************************************************
//! @class ControlsBaseline

ControlsBaseline::ControlsBaseline()
{
    auto* box = new QVBoxLayout;
    auto* hb  = new QHBoxLayout;
    auto* spinDegree = new QcrSpinBox{
        "degree", &gSession->baseline.polynomDegree, 4, false, 0, 4,
        "Degree of the polynomial used to fit the baseline"};
    hb->addWidget(new QLabel("Pol. degree:"));
    hb->addWidget(spinDegree);
    hb->addStretch(1);
    hb->addWidget(new QcrIconTriggerButton(&gGui->triggers->baserangeAdd));
    hb->addWidget(new QcrIconTriggerButton(&gGui->triggers->baserangeRemove));
    hb->addWidget(new QcrIconTriggerButton(&gGui->triggers->baserangesClear));
    box->addLayout(hb);

    box->addWidget(new BaseRangesView());
    box->addWidget(
        new RangeControl(
            "base",
            []()->const Range*{return gSession->baseline.ranges.selectedRange();},
            [](double val, bool namelyMax)->void{
                Range* r = gSession->baseline.ranges.selectedRange();
                ASSERT(r);
                r->setOne(val, namelyMax);
                gSession->onBaseline();
            }));
    box->addStretch(1);
    setLayout(box);
}

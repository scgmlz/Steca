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

#include "controls_baseline.h"
#include "core/session.h"
#include "gui/actions/triggers.h"
#include "gui/mainwin.h"
#include "gui/state.h"
#include "gui/view/range_control.h"
#include "qcr/widgets/tables.h"

//  ***********************************************************************************************
//! @class BaseRangesModel, used in BaseRangesView (local scope)

//! Model for BaseRangesView.

class BaseRangesModel : public TableModel {
public:
    BaseRangesModel() : TableModel("baseline") {}

    enum { COL_RANGE = 1, NUM_COLUMNS };

private:
    int columnCount() const final { return NUM_COLUMNS; }
    int rowCount() const final { return gSession->baseline().ranges().count(); }
    int highlighted() const final { return gSession->baseline().selectedIndex(); }
    void setHighlight(int row) final { gSession->baseline().select(row); }

    QVariant data(const QModelIndex&, int) const;
};

QVariant BaseRangesModel::data(const QModelIndex& index, int role) const
{
    int row = index.row();
    if (row < 0 || rowCount() <= row)
        return {};
    const Range& range = gSession->baseline().ranges().at(row);
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

class BaseRangesView final : public TableView {
public:
    BaseRangesView();
};

BaseRangesView::BaseRangesView()
    : TableView(new BaseRangesModel())
{
}

//  ***********************************************************************************************
//! @class ControlsBaseline

ControlsBaseline::ControlsBaseline()
    : QcrWidget{"baseline"}
    , spinDegree_ {2, "degree", 4, false, 0, 4,
              "Degree of the polynomial used to fit the baseline"}
{
    hb_.addWidget(new QLabel("Pol. degree:"));
    hb_.addWidget(&spinDegree_);
    hb_.addStretch(1);
    hb_.addWidget(new QcrIconTriggerButton(&gGui->triggers->removeBaserange));
    hb_.addWidget(new QcrIconTriggerButton(&gGui->triggers->clearBackground));
    box_.addLayout(&hb_);

    box_.addWidget(new BaseRangesView());
    box_.addWidget(new RangeControl("base", []()->Range*{
                return gSession->baseline().selectedRange(); }));
    box_.addStretch(1);
    setLayout(&box_);

    connect(&gGui->triggers->removeBaserange, &QAction::triggered, []() {
            gSession->baseline().removeSelectedRange();
            gRoot->remakeAll("clearBackground"); });
    connect(&gGui->triggers->clearBackground, &QAction::triggered, []() {
            gSession->baseline().clearRanges();
            gRoot->remakeAll("clearBackground"); });

    setRemake([this](){
            gGui->triggers->clearBackground.setEnabled(gSession->baseline().ranges().count());
        });
}

void ControlsBaseline::hideEvent(QHideEvent*)
{
    gGui->state->editingBaseline = false;
}

void ControlsBaseline::showEvent(QShowEvent*)
{
    gGui->state->editingBaseline = true;
}

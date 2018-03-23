// ************************************************************************** //
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
// ************************************************************************** //

#include "controls_baseline.h"
#include "core/session.h"
#include "gui/base/model_view.h"
#include "gui/mainwin.h"
#include "gui/actions/triggers.h"

// ************************************************************************** //
//  local class BaseRangesModel, used in BaseRangesView
// ************************************************************************** //

//! Model for BaseRangesView.

class BaseRangesModel : public TableModel {
public:
    BaseRangesModel() : TableModel("baseline") {}

    int columnCount() const final { return NUM_COLUMNS; }
    int rowCount() const final { return gSession->baseline().ranges().count(); }
    int highlighted() const final { return 0; } // dummy
    void setHighlight(int row) final {}         // no need to select and highlight ranges

    QVariant data(const QModelIndex&, int) const;

    enum { COL_RANGE = 1, NUM_COLUMNS };
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
        return QColor(Qt::white);
    }
    default:
        return {};
    }
}

// ************************************************************************** //
//  local class BaseRangesView
// ************************************************************************** //

//! List view of user-defined Bragg peaks.

class BaseRangesView final : public TableView {
public:
    BaseRangesView();
private:
    void currentChanged(const QModelIndex& current, const QModelIndex&) override final {
        gotoCurrent(current); }
};

BaseRangesView::BaseRangesView()
    : TableView(new BaseRangesModel())
{
    connect(gSession, &Session::sigBaseline, this, &BaseRangesView::onData);
}

// ************************************************************************** //
//  class ControlsBaseline
// ************************************************************************** //

ControlsBaseline::ControlsBaseline()
{
    hb_.addWidget(new QLabel("Pol. degree:"));
    hb_.addWidget(&spinDegree_);
    hb_.addStretch(1);
    hb_.addWidget(new XIconButton(&gGui->triggers->clearBackground));
    box_.addLayout(&hb_);

    box_.addWidget(new BaseRangesView());
    box_.addStretch(1);
    setLayout(&box_);

    connect(&spinDegree_, &CSpinBox::valueReleased, [](int degree_) {
            gSession->baseline().setPolynomDegree(degree_); });

    connect(gSession, &Session::sigBaseline, [this]() {
            spinDegree_.setValue(gSession->baseline().polynomDegree()); });
}

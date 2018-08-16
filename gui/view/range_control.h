//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      gui/view/range_control.h
//! @brief     Defines class RangeControl
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#ifndef RANGE_CONTROL_H
#define RANGE_CONTROL_H

#include "qcr/widgets/views.h"

class Range;

//! A horizontal row with labels and spin boxes to view and change one peak fit range.

class RangeControl : public QcrWidget {
public:
    RangeControl() = delete;
    RangeControl(const QString& _name, const std::function<Range*()>& _selectRange);
    RangeControl(const QString&,
                 const std::function<const Range*()>&,
                 const std::function<void(double,bool)>&);
    RangeControl(const RangeControl&) = delete;
private:
    std::function<Range*()> selectRange_;
};

#endif // RANGE_CONTROL_H

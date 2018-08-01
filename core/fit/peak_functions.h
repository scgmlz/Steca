//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      core/fit/peak_functions.h
//! @brief     Defines function to register specific peak function classes
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#ifndef PEAK_FUNCTIONS_H
#define PEAK_FUNCTIONS_H

#include "core/fit/registry.h"

typedef class PeakFunction* (*initializer_type)();

//! Holds initializers for all available peak functions.

class FunctionRegistry : public IRegistry<initializer_type> {
public:
    FunctionRegistry();
    void register_fct(const initializer_type f);
    PeakFunction* name2new(const QString&);
    PeakFunction* clone(const PeakFunction& old);
};

#endif // PEAK_FUNCTIONS_H

// ************************************************************************** //
//
//  Steca2: stress and texture calculator
//
//! @file      gui/main.cpp
//! @brief     Implements ...
//!
//! @homepage  https://github.com/scgmlz/Steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2017
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#include "app.h"
#include <tclap/CmdLine.h> // templated command line argument parser, in 3rdparty directory

int main(int argc, char* argv[]) {

    TCLAP::CmdLine cmd("Stress and texture calculator", ' ',
#include "../VERSION"
                       , true);
    cmd.parse(argc, argv);

    return App(argc, argv).exec();
}

// ************************************************************************** //
//
//  Steca: stress and texture calculator
//
//! @file      gui/output/output_diffractograms.h
//! @brief     Defines class DiffractogramsFrame
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#ifndef OUTPUT_DIFFRACTOGRAMS_H
#define OUTPUT_DIFFRACTOGRAMS_H

#include "frame.h"

//! The modal dialog for saving diffractograms.

class DiffractogramsFrame : public Frame {
public:
    DiffractogramsFrame(rcstr title, QWidget*);

private:
    class TabDiffractogramsSave* tabSave_;

    void save();
    void saveCurrent(rcstr filePath, rcstr separator);
    void saveAll(rcstr filePath, rcstr separator, bool oneFile);
};

#endif // OUTPUT_DIFFRACTOGRAMS_H

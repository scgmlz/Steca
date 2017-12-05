// ************************************************************************** //
//
//  Steca2: stress and texture calculator
//
//! @file      gui/output/output_diffractograms.h
//! @brief     Defines ...
//!
//! @homepage  https://github.com/scgmlz/Steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2017
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#ifndef OUTPUT_DIFFRACTOGRAMS_H
#define OUTPUT_DIFFRACTOGRAMS_H

#include "output_dialogs.h"

namespace gui {
namespace output {

class TabDiffractogramsSave : public TabSave {
    CLASS(TabDiffractogramsSave)
    SUPER(TabSave) public : TabDiffractogramsSave(TheHub&, Params&);

    uint currType() const;
    bool currentChecked() { return rbCurrent_->isChecked(); }
    bool allSequentialChecked() { return rbAllSequential_->isChecked(); }
    bool allChecked() { return rbAll_->isChecked(); }

protected:
    QRadioButton *rbCurrent_, *rbAllSequential_, *rbAll_;
    QComboBox* fileTypes_;
};


struct OutputData;
using OutputDataCollection = typ::vec<OutputData>;
using OutputDataCollections = typ::vec<OutputDataCollection>;

class DiffractogramsFrame : public Frame {
    CLASS(DiffractogramsFrame)
    SUPER(Frame) public : DiffractogramsFrame(TheHub&, rcstr title, QWidget*);

protected:
    TabDiffractogramsSave* tabSave_;

    OutputDataCollection
    collectCurves(gma_rge::rc, uint gmaSlices, data::Dataset::rc dataset, uint picNum);
    OutputData collectCurve(data::Dataset::rc dataset);

    OutputData outputCurrDiffractogram();
    OutputDataCollections outputAllDiffractograms();

    bool saveDiffractogramOutput();
    bool writeCurrDiffractogramToFile(rcstr filePath, rcstr separator);
    bool writeAllDiffractogramsToFiles(rcstr filePath, rcstr separator, bool oneFile);
};


}
}
#endif // OUTPUT_Diffractograms_H

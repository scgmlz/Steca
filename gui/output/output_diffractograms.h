// ************************************************************************** //
//
//  Steca2: stress and texture calculator
//
//! @file      gui/output/output_diffractograms.h
//! @brief     Defines class DiffractogramsFrame
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
#include "frame.h"

namespace gui {
namespace output {

struct OutputData;
using OutputDataCollection = vec<OutputData>;
using OutputDataCollections = vec<OutputDataCollection>;

class DiffractogramsFrame : public Frame {
public:
    DiffractogramsFrame(rcstr title, QWidget*);

protected:
    class TabDiffractogramsSave* tabSave_;

    OutputDataCollection
    collectCurves(Range const&, uint gmaSlices, DataSequence const& dataset, uint picNum);
    OutputData collectCurve(DataSequence const& dataset);

    OutputData outputCurrDiffractogram();
    OutputDataCollections outputAllDiffractograms();

    void saveDiffractogramOutput();
    void writeCurrDiffractogramToFile(rcstr filePath, rcstr separator);
    void writeAllDiffractogramsToFiles(rcstr filePath, rcstr separator, bool oneFile);
};

} // namespace output
} // namespace gui

#endif // OUTPUT_DIFFRACTOGRAMS_H

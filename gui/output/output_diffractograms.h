// ************************************************************************** //
//
//  Steca2:    StressTextureCalculator ver. 2
//
//! @file      output_diffractograms.h
//! @brief     Output of Diffractograms.
//!
//! @homepage  http://apps.jcns.fz-juelich.de/steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016
//! @authors   Scientific Computing Group at MLZ Garching
//! @authors   Rebecca Brydon, Jan Burle,  Antti Soininen
//! @authors   Based on the original STeCa by Christian Randau
//
// ************************************************************************** //

#ifndef OUTPUT_DIFFRACTOGRAMS_H
#define OUTPUT_DIFFRACTOGRAMS_H

#include "output_dialogs.h"

namespace gui { namespace output {
//------------------------------------------------------------------------------

class DiffractogramsParams : public Params {
  CLS(DiffractogramsParams) SUPER(Params)
public:
  DiffractogramsParams(TheHub&);
};

//------------------------------------------------------------------------------

class TabDiffractogramsSave : public TabSave {
  CLS(TabDiffractogramsSave) SUPER(TabSave)
public:
  TabDiffractogramsSave(TheHub&, Params&);

  uint currType() const;
  bool currentChecked() { return rbCurrent_->isChecked(); }
  bool allSequentialChecked() { return rbAllSequential_->isChecked(); }
  bool allChecked() { return rbAll_->isChecked(); }

protected:
  QRadioButton *rbCurrent_, *rbAllSequential_, *rbAll_;
  QComboBox *fileTypes_;
};

//------------------------------------------------------------------------------

struct OutputData;
using OutputDataCollection  = typ::vec<OutputData>;
using OutputDataCollections = typ::vec<OutputDataCollection>;

class DiffractogramsFrame : public Frame {
  CLS(DiffractogramsFrame) SUPER(Frame)
public:
  DiffractogramsFrame(TheHub&, rcstr title, QWidget*);

protected:
  TabDiffractogramsSave *tabSave_;

  OutputDataCollection collectCurves(gma_rge::rc, gma_t gmaStep, data::Dataset::rc dataset, uint picNum);
  OutputData collectCurve(data::Dataset::rc dataset);

  OutputData outputCurrDiffractogram();
  OutputDataCollections outputAllDiffractograms();

  bool saveDiffractogramOutput();
  bool writeCurrDiffractogramToFile();
  bool writeAllDiffractogramsToFiles(bool oneFile);
};

//------------------------------------------------------------------------------
}}
#endif // OUTPUT_Diffractograms_H

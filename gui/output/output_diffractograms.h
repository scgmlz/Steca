/*******************************************************************************
 * REVIEW: STeCa2 - StressTextureCalculator ver. 2
 *
 * Copyright (C) 2016 Forschungszentrum Jülich GmbH 2016
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * See the COPYING and AUTHORS files for more details.
 ******************************************************************************/

#ifndef OUTPUT_DIFFRACTOGRAMS_H
#define OUTPUT_DIFFRACTOGRAMS_H

#include "output_dialogs.h"

namespace gui { namespace output {
//------------------------------------------------------------------------------

class TabDiffractogramsSave : public TabSave {
  CLASS(TabDiffractogramsSave) SUPER(TabSave)
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
  CLASS(DiffractogramsFrame) SUPER(Frame)
public:
  DiffractogramsFrame(TheHub&, rcstr title, QWidget*);

protected:
  TabDiffractogramsSave *tabSave_;

  OutputDataCollection collectCurves(gma_rge::rc, uint gmaSlices, data::Dataset::rc dataset, uint picNum, bool averaged);
  OutputData collectCurve(data::Dataset::rc dataset, bool averaged);

  OutputData outputCurrDiffractogram();
  OutputDataCollections outputAllDiffractograms();

  bool saveDiffractogramOutput();
  bool writeCurrDiffractogramToFile(rcstr filePath, rcstr separator);
  bool writeAllDiffractogramsToFiles(rcstr filePath, rcstr separator, bool oneFile);
};

//------------------------------------------------------------------------------
}}
#endif // OUTPUT_Diffractograms_H

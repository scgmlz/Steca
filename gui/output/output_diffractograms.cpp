// ************************************************************************** //
//
//  STeCa2:    StressTextureCalculator ver. 2
//
//! @file      output_diffractograms.cpp
//!
//! @homepage  http://apps.jcns.fz-juelich.de/steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016
//! @authors   Scientific Computing Group at MLZ Garching
//! @authors   Antti Soininen, Jan Burle, Rebecca Brydon
//! @authors   Based on the original STeCa by Christian Randau
//
// ************************************************************************** //

#include "output_diffractograms.h"
#include "core_lens.h"
#include "thehub.h"
#include "types/core_async.h"
#include <QDir>
#include <QTextStream>

namespace gui { namespace output {
//------------------------------------------------------------------------------

DiffractogramsParams::DiffractogramsParams(TheHub& hub) : super(hub) {
  gpRefl_->hide();
  gpInterpolation_->hide();
}

//------------------------------------------------------------------------------

TabDiffractogramsSave::TabDiffractogramsSave(TheHub& hub, Params& params)
: super(hub, params)
{
  auto vb = vbox();
  grid_->addLayout(vb, grid_->rowCount(), 0);

  vb->addWidget(rbCurrent_       = radioButton("Current diffractogram"));
  vb->addWidget(rbAllSequential_ = radioButton("All diffractograms to sequentially numbered files"));
  vb->addWidget(rbAll_           = radioButton("All diffractograms"));
  vb->addWidget(fileTypes_       = comboBox(fileTags));

  vb->addStretch();

  rbAll_->setChecked(true);
}

uint TabDiffractogramsSave::currType() const {
  return fileTypes_->currentIndex();
}

//------------------------------------------------------------------------------

struct OutputData {

  OutputData() {
  }

  OutputData(core::Curve curve, core::Dataset dataset, core::Range gammaStripe, uint picNum) :
  curve_(curve), dataset_(dataset), gammaStripe_(gammaStripe), picNum_(picNum) {
  }

  core::Curve curve_;
  core::Dataset dataset_;
  core::Range gammaStripe_;
  uint picNum_;

  bool isValid() {
   return (!dataset_.metadata().isNull() || !curve_.isEmpty() || gammaStripe_.isValid());
  }

};

DiffractogramsFrame::DiffractogramsFrame(TheHub &hub, rcstr title, QWidget *parent)
: super(hub, title, new DiffractogramsParams(hub), parent)
{
  tabs_->removeTab(0);
  btnCalculate_->hide();
  btnInterpolate_->hide();

  tabSave_ = new TabDiffractogramsSave(hub, *params_);
  tabs_->addTab("Save").box().addWidget(tabSave_);

  connect(tabSave_->actSave(),&QAction::triggered,[this]() {
    tabSave_->clearMessage();
    saveDiffractogramOutput();
    tabSave_->showMessage();
  });
}

OutputDataCollection DiffractogramsFrame::collectCurves(core::rcRange rgeGamma, qreal gammaStep,
                                                       core::rcDataset dataset, uint picNum) {
  auto lens = hub_.lens(dataset);
  auto &map = lens->angleMap();

  core::Range rge = map.rgeGamma();
  if (rgeGamma.isValid())
    rge = rge.intersect(rgeGamma);

  OutputDataCollection outputData;
  qreal step = gammaStep;
  for_i (rge.numSlices(step)) {
    qreal min = rge.min + i * step;
    core::Range gammaStripe(min, min + step);

    auto curve = lens->makeCurve(gammaStripe, map.rgeTth());
    outputData.append(OutputData(curve,dataset,gammaStripe,picNum));
  }
  return outputData;
}

OutputData DiffractogramsFrame::collectCurve(core::rcDataset dataset) {
  auto lens = hub_.lens(dataset);
  auto &map = lens->angleMap();

  auto curve = lens->makeCurve(map.rgeGamma(), map.rgeTth());

  return OutputData(curve,dataset,map.rgeGamma(),0); // TODO current picture number
}

OutputDataCollections DiffractogramsFrame::outputAllDiffractograms() {
  core::deg gammaStep = params_->stepGamma->value();

  core::Range rgeGamma;
  if (params_->cbLimitGamma->isChecked())
    rgeGamma.safeSet(params_->limitGammaMin->value(),
                     params_->limitGammaMax->value());

  auto &datasets = hub_.collectedDatasets();
  Progress progress(datasets.count(), pb_);

  OutputDataCollections allOutputData;
  uint picNum = 1;
  for (core::shp_Dataset dataset: datasets) {
    progress.step();
    allOutputData.append(collectCurves(rgeGamma, gammaStep, *dataset, picNum));
    ++picNum;
  }

  return allOutputData;
}

OutputData DiffractogramsFrame::outputCurrDiffractogram() {
  auto dataset = hub_.selectedDataset();
  if (dataset)
    return collectCurve(*dataset);
  else
    return OutputData();
}

auto writeMetaData = [](OutputData outputData, QTextStream& stream, str separator) {
  if (outputData.picNum_ > 0)
    stream << "Picture Nr:" << separator << outputData.picNum_ << '\n';

  stream << "Comment:"  << separator << outputData.dataset_.metadata()->comment << '\n';
  stream << "Date:" << separator << outputData.dataset_.metadata()->date << '\n';
  stream << "Gamma Range max:" << separator << outputData.gammaStripe_.min << '\n';
  stream << "Gamma Range max:" << separator << outputData.gammaStripe_.max << '\n';

  for_i (core::Metadata::numAttributes(true)) {
    stream << core::Metadata::attributeTag(i) << ": " <<
                        outputData.dataset_.metadata()->attributeValue((uint)i).toDouble() << '\n';
  }
};

bool DiffractogramsFrame::writeCurrDiffractogramToFile() {
  auto outputData = outputCurrDiffractogram();
  if (!outputData.isValid()) return false;

  auto ts = static_cast<TabDiffractogramsSave*>(tabSave_);
  auto s = ts->currType();
  auto separator = tabSave_->fileSeparators[s];

  auto filePath = QDir(tabSave_->path()).absoluteFilePath(tabSave_->fileName());
  auto fileTag = tabSave_->fileTags[s];
  WriteFile file(filePath + fileTag);

  QTextStream stream(&file);

  writeMetaData(outputData,stream,separator);
  stream << "Intensity" << separator << "Tth" << '\n';
  for_i (outputData.curve_.xs().count()) {
    stream << outputData.curve_.x(i) << separator << outputData.curve_.y(i) << '\n';
  }
  tabSave_->savedMessage("File has been saved.");

  return true;
}

bool DiffractogramsFrame::writeAllDiffractogramsToFiles(bool oneFile) {
  auto outputCollections = outputAllDiffractograms();
  for (auto outputCollection : outputCollections) {
    for (auto outputData : outputCollection) {
      if (!outputData.isValid()) return false;
    }
  }

  auto ts = static_cast<TabDiffractogramsSave*>(tabSave_);
  auto s = ts->currType();
  str separator = tabSave_->fileSeparators[s];
  str fileTag = tabSave_->fileTags[s];

  if (oneFile) {
    auto filePath = QDir(tabSave_->path()).absoluteFilePath(tabSave_->fileName() + fileTag);
    WriteFile file(filePath);
    QTextStream stream(&file);

    for (auto outputCollection : outputCollections) {
      for (auto outputData : outputCollection) {
        writeMetaData(outputData,stream,separator);

        stream << "Intensity" << separator << "Tth" << '\n';
        for_i (outputData.curve_.xs().count()) {
          stream << outputData.curve_.x(i) << separator << outputData.curve_.y(i) << '\n';
        }
      }
    }
    tabSave_->savedMessage("File has been saved.");

  } else {
    int fileNumber = 1;
    for (auto outputCollection : outputCollections) {
      str fileName = QString(tabSave_->fileName() + "%1" + fileTag).arg(fileNumber);
      auto filePath = QDir(tabSave_->path()).absoluteFilePath(fileName);
      WriteFile file(filePath);
      QTextStream stream(&file);
      for (auto outputData : outputCollection) {
        writeMetaData(outputData,stream,separator);
        stream << "Intensity" << separator << "Tth" << '\n';
        for_i (outputData.curve_.xs().count()) {
          stream << outputData.curve_.x(i) << separator << outputData.curve_.y(i) << '\n';
        }
      }
      ++fileNumber;
    }
    tabSave_->savedMessage(QString("%1 files have been saved.").arg(fileNumber-1));
  }
  return true;

 }

bool DiffractogramsFrame::saveDiffractogramOutput() {
  if (tabSave_->currentChecked())
    return writeCurrDiffractogramToFile();

  if (tabSave_->allSequentialChecked())
    return writeAllDiffractogramsToFiles(false);

  if (tabSave_->allChecked())
    return writeAllDiffractogramsToFiles(true);

  return false;
}

//------------------------------------------------------------------------------
}}
// eof

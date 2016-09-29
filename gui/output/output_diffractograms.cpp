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
//! @authors   Rebecca Brydon, Jan Burle,  Antti Soininen
//! @authors   Based on the original STeCa by Christian Randau
//
// ************************************************************************** //

#include "output_diffractograms.h"
#include "thehub.h"
#include <QTextStream>

namespace gui { namespace output {
//------------------------------------------------------------------------------

TabDiffractogramsSave::TabDiffractogramsSave(TheHub& hub, Params& params)
: super(hub, params, true)
{
  auto gp = new panel::GridPanel(hub, "To save");
  grid_->addWidget(gp, grid_->rowCount(), 0, 1, 2);
  grid_->addRowStretch();

  auto g = gp->grid();
  g->addWidget((rbCurrent_       = radioButton("Current diffractogram")));
  g->addWidget((rbAllSequential_ = radioButton("All diffractograms to sequentially numbered files")));
  g->addWidget((rbAll_           = radioButton("All diffractograms")));
  g->addWidget(textButton(actSave), 2, 1);

  rbAll_->setChecked(true);
}

uint TabDiffractogramsSave::currType() const {
  return to_u(fileTypes_->currentIndex());
}

//------------------------------------------------------------------------------

struct OutputData {
  OutputData() {
  }

  OutputData(typ::Curve curve, data::Dataset dataset, gma_rge gmaStripe, uint picNum)
  : curve_(curve), dataset_(dataset), gmaStripe_(gmaStripe), picNum_(picNum) {
  }

  typ::Curve curve_;
  data::Dataset dataset_;
  gma_rge gmaStripe_;
  uint picNum_;

  bool isValid() {
   return (!dataset_.metadata().isNull() || !curve_.isEmpty() || gmaStripe_.isValid());
  }
};

static const Params::ePanels PANELS = Params::ePanels(
    Params::GAMMA);

DiffractogramsFrame::DiffractogramsFrame(TheHub &hub, rcstr title, QWidget *parent)
: super(hub, title, new Params(hub, PANELS), parent)
{
  tabs_->removeTab(0);
  btnCalculate_->hide();
  btnInterpolate_->hide();

  tabSave_ = new TabDiffractogramsSave(hub, *params_);
  tabs_->addTab("Save", Qt::Vertical).box().addWidget(tabSave_);

  connect(tabSave_->actSave,&QAction::triggered,[this]() {
    logSuccess(saveDiffractogramOutput());
  });
}

OutputDataCollection DiffractogramsFrame::collectCurves(
    gma_rge::rc rgeGma, pint gmaSlices, data::Dataset::rc dataset, uint picNum) {

  auto lens = hub_.datasetLens(dataset);

  typ::Range rge = lens->rgeGma();
  if (rgeGma.isValid())
    rge = rge.intersect(rgeGma);

  OutputDataCollection outputData;

  qreal step = rge.width() / gmaSlices;
  for_i (uint(gmaSlices)) {
    qreal min = rge.min + i * step;
    gma_rge gmaStripe(min, min + step);

    auto curve = lens->makeCurve(gmaStripe);
    outputData.append(OutputData(curve, dataset, gmaStripe, picNum));
  }
  return outputData;
}

OutputData DiffractogramsFrame::collectCurve(data::Dataset::rc dataset) {
  auto lens = hub_.datasetLens(dataset);
  auto curve = lens->makeCurve();
  return OutputData(curve, dataset, lens->rgeGma(), 0); // TODO current picture number
}

OutputDataCollections DiffractogramsFrame::outputAllDiffractograms() {
  ENSURE(params_->panelGammaSlices)
  auto ps = params_->panelGammaSlices;
  pint gmaSlices = pint(ps->numSlices->value());

  ENSURE(params_->panelGammaRange)
  auto pr = params_->panelGammaRange;
  typ::Range rgeGma;
  if (pr->cbLimitGamma->isChecked())
    rgeGma.safeSet(pr->minGamma->value(),
                   pr->maxGamma->value());

  auto& datasets = hub_.collectedDatasets();
  Progress progress(datasets.count(), pb_);

  OutputDataCollections allOutputData;
  uint picNum = 1;
  for (data::shp_Dataset dataset : datasets) {
    progress.step();
    allOutputData.append(collectCurves(rgeGma, gmaSlices, *dataset, picNum));
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
    stream << "Picture Nr: " << outputData.picNum_ << '\n';

  auto& md     = *outputData.dataset_.metadata();
  auto& rgeGma = outputData.gmaStripe_;

  stream << "Comment: "         << md.comment << '\n';
  stream << "Date: "            << md.date    << '\n';
  stream << "Gamma range min: " << rgeGma.min << '\n';
  stream << "Gamma range max: " << rgeGma.max << '\n';

  for_i (data::Metadata::numAttributes(true)) {
    stream << data::Metadata::attributeTag(i) << ": " <<
              md.attributeValue(i).toDouble() << '\n';
  }
};

bool DiffractogramsFrame::writeCurrDiffractogramToFile(rcstr filePath, rcstr separator) {
  auto outputData = outputCurrDiffractogram();
  if (!outputData.isValid())
    return false;

  WriteFile file(filePath);
  QTextStream stream(&file);

  writeMetaData(outputData, stream, separator);
  stream << "Tth" << separator << "Intensity" << '\n';

  auto& curve = outputData.curve_;

  for_i (curve.xs().count())
    stream << curve.x(i) << separator << curve.y(i) << '\n';

  return true;
}

bool DiffractogramsFrame::writeAllDiffractogramsToFiles(rcstr filePath, rcstr separator, bool oneFile) {
  auto outputCollections = outputAllDiffractograms();
  for (auto outputCollection : outputCollections) {
    for (auto outputData : outputCollection) {
      if (!outputData.isValid())
        return false;
    }
  }

  WriteFile file(filePath);
  QTextStream stream(&file);

  if (oneFile) {
    for (auto outputCollection : outputCollections) {
      for (auto outputData : outputCollection) {
        writeMetaData(outputData,stream,separator);

        stream << "Tth" << separator << "Intensity" << '\n';
        for_i (outputData.curve_.xs().count()) {
          stream << outputData.curve_.x(i) << separator << outputData.curve_.y(i) << '\n';
        }
      }
    }
  } else {
    int fileNumber = 1;
    for (auto outputCollection : outputCollections) {
      for (auto outputData : outputCollection) {
        writeMetaData(outputData,stream,separator);
        stream << "Tth" << separator << "Intensity" << '\n';
        for_i (outputData.curve_.xs().count()) {
          stream << outputData.curve_.x(i) << separator << outputData.curve_.y(i) << '\n';
        }
      }
      ++fileNumber;
    }
  }

  return true;
}

bool DiffractogramsFrame::saveDiffractogramOutput() {
  str path = tabSave_->filePath(true);
  if (path.isEmpty())
    return false;

  str separator = tabSave_->separator();

  if (tabSave_->currentChecked())
    return writeCurrDiffractogramToFile(path, separator);

  if (tabSave_->allSequentialChecked())
    return writeAllDiffractogramsToFiles(path, separator, false);

  if (tabSave_->allChecked())
    return writeAllDiffractogramsToFiles(path, separator, true);

  return false;
}

//------------------------------------------------------------------------------
}}
// eof

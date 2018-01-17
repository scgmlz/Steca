// ************************************************************************** //
//
//  Steca: stress and texture calculator
//
//! @file      gui/output/output_diffractograms.cpp
//! @brief     Implements class DiffractogramsFrame
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#include "gui/output/output_diffractograms.h"
#include "core/session.h"
#include "gui/output/dialog_panels.h"
#include "gui/output/tab_save.h"
#include "gui/output/write_file.h"
#include "gui/thehub.h"
#include "gui/base/various_widgets.h"

// ************************************************************************** //
//  local class TabDiffractogramsSave
// ************************************************************************** //

class TabDiffractogramsSave final : public TabSave {
public:
    TabDiffractogramsSave();

    int currType() const;
    bool currentChecked() { return rbCurrent_->isChecked(); }
    bool allSequentialChecked() { return rbAllSequential_->isChecked(); }
    bool allChecked() { return rbAll_->isChecked(); }

private:
    QRadioButton *rbCurrent_, *rbAllSequential_, *rbAll_;
    QComboBox* fileTypes_;
};

TabDiffractogramsSave::TabDiffractogramsSave()
    : TabSave(true) {
    auto gp = new GridPanel("To save");
    grid_->addWidget(gp, grid_->rowCount(), 0, 1, 2);
    grid_->setRowStretch(grid_->rowCount(), 1);

    QGridLayout* g = gp->grid();
    g->addWidget((rbCurrent_ = newQ::RadioButton("Current diffractogram")));
    g->addWidget(
        (rbAllSequential_ = newQ::RadioButton("All diffractograms to sequentially numbered files")));
    g->addWidget((rbAll_ = newQ::RadioButton("All diffractograms")));
    g->addWidget(newQ::TextButton(actSave), 2, 1);

    rbAll_->setChecked(true);
}

int TabDiffractogramsSave::currType() const {
    return fileTypes_->currentIndex();
}

// ************************************************************************** //
//  local class OutputData
// ************************************************************************** //

struct OutputData {
public:
    OutputData() = delete;
    OutputData(Curve curve, const Cluster& cluster, Range gmaStripe, int picNum)
        : curve_(curve), dataseq_(cluster), gmaStripe_(gmaStripe), picNum_(picNum) {}

    Curve curve_;
    const Cluster& dataseq_;
    Range gmaStripe_;
    int picNum_;

    bool isValid() const {
        return (!dataseq_.avgeMetadata().isNull() || !curve_.isEmpty() || gmaStripe_.isValid());
    }
};

namespace {

OutputData collectCurve(Cluster const& dataseq) {
    shp_SequenceLens lens = gSession->defaultClusterLens(dataseq);
    const Curve& curve = lens->makeCurve();
    return OutputData(curve, dataseq, lens->rgeGma(), 0); // TODO current picture number
}

OutputData outputCurrDiffractogram() {
    const Cluster* ret = gHub->selectedCluster();
    if (!ret)
        throw Exception("No data selected");
    return collectCurve(*ret);
}

vec<const OutputData*> collectCurves(
    const Range& rgeGma, int gmaSlices, Cluster const& dataseq, int picNum) {

    shp_SequenceLens lens = gSession->defaultClusterLens(dataseq);

    Range rge = (gmaSlices > 0) ? lens->rgeGma() : Range::infinite();
    if (rgeGma.isValid())
        rge = rge.intersect(rgeGma);

    vec<const OutputData*> ret;

    gmaSlices = qMax(1, gmaSlices);
    const qreal step = rge.width() / gmaSlices;
    for_i (gmaSlices) {
        const qreal min = rge.min + i * step;
        const Range gmaStripe(min, min + step);
        const Curve& curve = lens->makeCurve(gmaStripe);
        const OutputData* dat = new OutputData(curve, dataseq, gmaStripe, picNum);
        ret.append(dat);
    }
    return ret;
}

void writeMetaData(OutputData outputData, QTextStream& stream) {
    if (outputData.picNum_ > 0)
        stream << "Picture Nr: " << outputData.picNum_ << '\n';

    const Metadata& md = *outputData.dataseq_.avgeMetadata();
    const Range& rgeGma = outputData.gmaStripe_;

    stream << "Comment: " << md.comment << '\n';
    stream << "Date: " << md.date << '\n';
    stream << "Gamma range min: " << rgeGma.min << '\n';
    stream << "Gamma range max: " << rgeGma.max << '\n';

    for_i (Metadata::numAttributes(true)) {
        stream << Metadata::attributeTag(i, true) << ": " << md.attributeValue(i).toDouble()
               << '\n';
    }
}

} // local methods

static const Params::ePanels PANELS = Params::ePanels(Params::GAMMA);


// ************************************************************************** //
//  class DiffractogramsFrame
// ************************************************************************** //

DiffractogramsFrame::DiffractogramsFrame(rcstr title, QWidget* parent)
    : Frame(title, new Params(PANELS), parent) {
    tabs_->removeTab(0);
    btnCalculate_->hide();
    btnInterpolate_->hide();

    tabSave_ = new TabDiffractogramsSave();
    newQ::Tab(tabs_, "Save")->box().addWidget(tabSave_);

    connect(tabSave_->actSave, &QAction::triggered, [this]() { saveDiffractogramOutput(); });
    show();
}

vec<vec<const OutputData*>> DiffractogramsFrame::outputAllDiffractograms() {
    debug::ensure(params_->panelGammaSlices);
    int gmaSlices = params_->panelGammaSlices->numSlices->value();

    debug::ensure(params_->panelGammaRange);
    const PanelGammaRange* pr = params_->panelGammaRange;
    Range rgeGma;
    if (pr->cbLimitGamma->isChecked())
        rgeGma.safeSet(pr->minGamma->value(), pr->maxGamma->value());

    const Experiment& expt = gSession->experiment();
    Progress progress(expt.count(), progressBar_);

    vec<vec<const OutputData*>> ret;
    int picNum = 1;
    for (shp_Cluster cluster : expt) {
        progress.step();
        ret.append(collectCurves(rgeGma, gmaSlices, *cluster, picNum));
        ++picNum;
    }

    return ret;
}

void DiffractogramsFrame::writeCurrDiffractogramToFile(rcstr filePath, rcstr separator) {
    const OutputData& outputData = outputCurrDiffractogram();
    if (!outputData.isValid()) {
        qWarning() << "invalid output data in writeCurrDiffractogramsToFiles";
        return;
    }
    WriteFile file(filePath);
    QTextStream stream(&file);
    writeMetaData(outputData, stream);
    stream << "Tth" << separator << "Intensity" << '\n';
    const Curve& curve = outputData.curve_;
    for_i (curve.xs().count())
        stream << curve.x(i) << separator << curve.y(i) << '\n';
}

void DiffractogramsFrame::writeAllDiffractogramsToFiles(
    rcstr filePath, rcstr separator, bool oneFile) {
    const vec<vec<const OutputData*>>& outputCollections = outputAllDiffractograms();
    for (const vec<const OutputData*>& outputCollection : outputCollections) {
        for (const OutputData* outputData : outputCollection) {
            if (!outputData->isValid()) {
                qWarning() << "invalid output data in writeAllDiffractogramsToFiles";
                return;
            }
        }
    }
    WriteFile file(filePath);
    QTextStream stream(&file);
    if (oneFile) {
        for (const vec<const OutputData*>& outputCollection : outputCollections) {
            for (const OutputData* outputData : outputCollection) {
                writeMetaData(*outputData, stream);
                stream << "Tth" << separator << "Intensity" << '\n';
                for_i (outputData->curve_.xs().count()) {
                    stream << outputData->curve_.x(i) << separator
                           << outputData->curve_.y(i) << '\n';
                }
            }
        }
    } else {
        int fileNumber = 1;
        for (const vec<const OutputData*>& outputCollection : outputCollections) {
            for (const OutputData* outputData : outputCollection) {
                writeMetaData(*outputData, stream);
                stream << "Tth" << separator << "Intensity" << '\n';
                for_i (outputData->curve_.xs().count()) {
                    stream << outputData->curve_.x(i) << separator
                           << outputData->curve_.y(i) << '\n';
                }
            }
            ++fileNumber;
        }
    }
}

void DiffractogramsFrame::saveDiffractogramOutput() {
    str path = tabSave_->filePath(true);
    if (path.isEmpty())
        return;
    str separator = tabSave_->separator();

    if (tabSave_->currentChecked())
        writeCurrDiffractogramToFile(path, separator);
    else if (tabSave_->allSequentialChecked())
        writeAllDiffractogramsToFiles(path, separator, false);
    else if (tabSave_->allChecked())
        writeAllDiffractogramsToFiles(path, separator, true);
    else
        qWarning() << "Invalid call of DiffractogramsFrame::saveDiffractogramOutput";
}

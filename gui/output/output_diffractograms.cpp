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

#include "output_diffractograms.h"
#include "widgets4output.h"
#include "data/suite.h"
#include "data/metadata.h"
#include "dialog_panels.h"
#include "gui/thebub.h"
#include "write_file.h"
#include "core/session.h"
#include "widgets/various_widgets.h"
#include "widgets/new_q.h"
#include <QAction>

// ************************************************************************** //
//  local class TabDiffractogramsSave
// ************************************************************************** //

class TabDiffractogramsSave final : public TabSave {
public:
    TabDiffractogramsSave(Params&);

    uint currType() const;
    bool currentChecked() { return rbCurrent_->isChecked(); }
    bool allSequentialChecked() { return rbAllSequential_->isChecked(); }
    bool allChecked() { return rbAll_->isChecked(); }

private:
    QRadioButton *rbCurrent_, *rbAllSequential_, *rbAll_;
    QComboBox* fileTypes_;
};

TabDiffractogramsSave::TabDiffractogramsSave(Params& params)
    : TabSave(params, true) {
    auto gp = new GridPanel("To save");
    grid_->addWidget(gp, grid_->rowCount(), 0, 1, 2);
    grid_->setRowStretch(grid_->rowCount(), 1);

    auto g = gp->grid();
    g->addWidget((rbCurrent_ = newQ::RadioButton("Current diffractogram")));
    g->addWidget(
        (rbAllSequential_ = newQ::RadioButton("All diffractograms to sequentially numbered files")));
    g->addWidget((rbAll_ = newQ::RadioButton("All diffractograms")));
    g->addWidget(newQ::TextButton(actSave), 2, 1);

    rbAll_->setChecked(true);
}

uint TabDiffractogramsSave::currType() const {
    return to_u(fileTypes_->currentIndex());
}

struct OutputData {
    OutputData() {}

    OutputData(Curve curve, Suite dataseq, Range gmaStripe, uint picNum)
        : curve_(curve), dataseq_(dataseq), gmaStripe_(gmaStripe), picNum_(picNum) {}

    Curve curve_;
    Suite dataseq_;
    Range gmaStripe_;
    uint picNum_;

    bool isValid() {
        return (!dataseq_.metadata().isNull() || !curve_.isEmpty() || gmaStripe_.isValid());
    }
};

static const Params::ePanels PANELS = Params::ePanels(Params::GAMMA);

// ************************************************************************** //
//  class DiffractogramsFrame
// ************************************************************************** //

DiffractogramsFrame::DiffractogramsFrame(rcstr title, QWidget* parent)
    : Frame(title, new Params(PANELS), parent) {
    tabs_->removeTab(0);
    btnCalculate_->hide();
    btnInterpolate_->hide();

    tabSave_ = new TabDiffractogramsSave(*params_);
    newQ::Tab(tabs_, "Save")->box().addWidget(tabSave_);

    connect(tabSave_->actSave, &QAction::triggered, [this]() { saveDiffractogramOutput(); });
    show();
}

OutputDataCollection DiffractogramsFrame::collectCurves(
    Range const& rgeGma, uint gmaSlices, Suite const& dataseq, uint picNum) {

    auto lens = gSession->defaultDatasetLens(dataseq);

    Range rge = (gmaSlices > 0) ? lens->rgeGma() : Range::infinite();
    if (rgeGma.isValid())
        rge = rge.intersect(rgeGma);

    OutputDataCollection outputData;

    gmaSlices = qMax(1u, gmaSlices);
    qreal step = rge.width() / gmaSlices;
    for_i (gmaSlices) {
        qreal min = rge.min + i * step;
        Range gmaStripe(min, min + step);
        auto curve = lens->makeCurve(gmaStripe);
        outputData.append(OutputData(curve, dataseq, gmaStripe, picNum));
    }
    return outputData;
}

OutputData DiffractogramsFrame::collectCurve(Suite const& dataseq) {
    auto lens = gSession->defaultDatasetLens(dataseq);
    auto curve = lens->makeCurve();
    return OutputData(curve, dataseq, lens->rgeGma(), 0); // TODO current picture number
}

OutputDataCollections DiffractogramsFrame::outputAllDiffractograms() {
    debug::ensure(params_->panelGammaSlices);
    auto ps = params_->panelGammaSlices;
    uint gmaSlices = to_u(ps->numSlices->value());

    debug::ensure(params_->panelGammaRange);
    auto pr = params_->panelGammaRange;
    Range rgeGma;
    if (pr->cbLimitGamma->isChecked())
        rgeGma.safeSet(pr->minGamma->value(), pr->maxGamma->value());

    auto& suite = gSession->experiment();
    Progress progress(suite.count(), pb_);

    OutputDataCollections allOutputData;
    uint picNum = 1;
    for (QSharedPointer<Suite> dataseq : suite) {
        progress.step();
        allOutputData.append(collectCurves(rgeGma, gmaSlices, *dataseq, picNum));
        ++picNum;
    }

    return allOutputData;
}

OutputData DiffractogramsFrame::outputCurrDiffractogram() {
    auto dataseq = gHub->selectedSuite();
    if (dataseq)
        return collectCurve(*dataseq);
    else
        return OutputData();
}

auto writeMetaData = [](OutputData outputData, QTextStream& stream) {
    if (outputData.picNum_ > 0)
        stream << "Picture Nr: " << outputData.picNum_ << '\n';

    auto& md = *outputData.dataseq_.metadata();
    auto& rgeGma = outputData.gmaStripe_;

    stream << "Comment: " << md.comment << '\n';
    stream << "Date: " << md.date << '\n';
    stream << "Gamma range min: " << rgeGma.min << '\n';
    stream << "Gamma range max: " << rgeGma.max << '\n';

    for_i (Metadata::numAttributes(true)) {
        stream << Metadata::attributeTag(i, true) << ": " << md.attributeValue(i).toDouble()
               << '\n';
    }
};

void DiffractogramsFrame::writeCurrDiffractogramToFile(rcstr filePath, rcstr separator) {
    auto outputData = outputCurrDiffractogram();
    if (!outputData.isValid()) {
        qWarning() << "invalid output data in writeCurrDiffractogramsToFiles";
        return;
    }
    WriteFile file(filePath);
    QTextStream stream(&file);
    writeMetaData(outputData, stream);
    stream << "Tth" << separator << "Intensity" << '\n';
    auto& curve = outputData.curve_;
    for_i (curve.xs().count())
        stream << curve.x(i) << separator << curve.y(i) << '\n';
}

void DiffractogramsFrame::writeAllDiffractogramsToFiles(
    rcstr filePath, rcstr separator, bool oneFile) {
    auto outputCollections = outputAllDiffractograms();
    for (auto outputCollection : outputCollections) {
        for (auto outputData : outputCollection) {
            if (!outputData.isValid()) {
                qWarning() << "invalid output data in writeAllDiffractogramsToFiles";
                return;
            }
        }
    }
    WriteFile file(filePath);
    QTextStream stream(&file);
    if (oneFile) {
        for (auto outputCollection : outputCollections) {
            for (auto outputData : outputCollection) {
                writeMetaData(outputData, stream);

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
                writeMetaData(outputData, stream);
                stream << "Tth" << separator << "Intensity" << '\n';
                for_i (outputData.curve_.xs().count()) {
                    stream << outputData.curve_.x(i) << separator << outputData.curve_.y(i) << '\n';
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

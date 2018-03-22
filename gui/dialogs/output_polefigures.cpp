// ************************************************************************** //
//
//  Steca: stress and texture calculator
//
//! @file      gui/dialogs/output_polefigures.cpp
//! @brief     Implements class PoleFiguresFrame
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#include "core/session.h"
#include "gui/base/file_dialog.h"
#include "gui/dialogs/dialog_panels.h"
#include "gui/dialogs/output_polefigures.h"
#include "gui/dialogs/tab_save.h"
#include "gui/mainwin.h"
#include "gui/panels/plot_polefig.h"
#include <qmath.h>
#include <QPainter>


// ************************************************************************** //
//  local class TabPoleFiguresSave
// ************************************************************************** //

//! Tab in PoleFiguresFrame, to save the pole figure data.

class TabPoleFiguresSave : public TabSave {
public:
    TabPoleFiguresSave();

    bool onlySelectedRefl() const;
    bool outputInten() const;
    bool outputTth() const;
    bool outputFWHM() const;

    void rawReflSettings(bool on);

private:
    CCheckBox outputInten_ {"outputInten#", "Intensity pole figure"};
    CCheckBox outputTth_ {"outputTth#", "Peak position pole figure"};
    CCheckBox outputFWHM_ {"outputFWHM#", "TWHM pole figure"};
    CRadioButton rbSelectedRefl_ {"rbSelectedRefl#", "Selected peak"};
    CRadioButton rbAllRefls_ {"rbAllRefls#", "All peaks"};
};

TabPoleFiguresSave::TabPoleFiguresSave()
    : TabSave(false)
{
    auto hb = new QHBoxLayout();
    grid_->addLayout(hb, grid_->rowCount(), 0);
    grid_->setRowStretch(grid_->rowCount(), 1);

    auto p1 = new GridPanel("Output data");
    auto p2 = new GridPanel("To save");

    hb->addWidget(p1);
    hb->addWidget(p2);
    hb->addStretch();

    {
        QGridLayout* g = &p1->grid_;
        g->addWidget(&outputInten_);
        g->addWidget(&outputTth_);
        g->addWidget(&outputFWHM_);
        g->setRowStretch(g->rowCount(), 1);
    }

    {
        QGridLayout* g = &p2->grid_;
        g->addWidget(&rbSelectedRefl_);
        g->addWidget(&rbAllRefls_);
        g->setRowStretch(g->rowCount(), 1);
    }

    rbSelectedRefl_.setChecked(true);
    outputInten_.setChecked(true);
}

bool TabPoleFiguresSave::onlySelectedRefl() const
{
    return rbSelectedRefl_.isChecked();
}

bool TabPoleFiguresSave::outputInten() const
{
    return outputInten_.isChecked();
}

bool TabPoleFiguresSave::outputTth() const
{
    return outputTth_.isChecked();
}

bool TabPoleFiguresSave::outputFWHM() const
{
    return outputFWHM_.isChecked();
}

void TabPoleFiguresSave::rawReflSettings(bool on)
{
    outputTth_.setEnabled(on);
    outputFWHM_.setEnabled(on);
}


// ************************************************************************** //
//  class PoleFiguresFrame
// ************************************************************************** //

static const Params::ePanels PANELS =
    Params::ePanels(Params::REFLECTION | Params::GAMMA | Params::POINTS | Params::INTERPOLATION);

PoleFiguresFrame::PoleFiguresFrame()
    : Frame("polfig#", "Pole figure", new Params(PANELS))
{
    {
        auto* tab = new QWidget();
        tabs_.addTab(tab, "Graph");
        tab->setLayout(new QVBoxLayout());
        plot_ = new PlotPolefig();
        tab->layout()->addWidget(plot_);
    }
    {
        auto* actSave = new CTrigger("exportSave", "Save");
        auto* tab = new QWidget();
        tabs_.addTab(tab, "Save");
        tab->setLayout(new QVBoxLayout());
        tabSave_ = new TabPoleFiguresSave();
        tab->layout()->addWidget(tabSave_);
        tab->layout()->addWidget(new XTextButton(actSave));
        connect(actSave, &QAction::triggered, [this]() { savePoleFigureOutput(); });
    }
    show();
}

PoleFiguresFrame::~PoleFiguresFrame()
{
    delete tabSave_;
    delete plot_;
}

void PoleFiguresFrame::displayPeak(int reflIndex, bool interpolated)
{
    Frame::displayPeak(reflIndex, interpolated);
    if (!interpPoints_.isEmpty() && !calcPoints_.isEmpty())
        plot_->set((interpolated ? interpPoints_ : calcPoints_).at(reflIndex));
    tabSave_->rawReflSettings(!gSession->peaks().at(reflIndex).isRaw());
}

void PoleFiguresFrame::savePoleFigureOutput()
{
    int reflCount = gSession->peaks().count();
    ASSERT(reflCount); // user should not get here if no peak is defined
    QString path = tabSave_->filePath(false);
    if (path.isEmpty())
        THROW("cannot save pole figure: file path is empty");
    if (tabSave_->onlySelectedRefl()) {
        writePoleFigureOutputFiles(path, getReflIndex());
        return;
    }
    // all peaks
    for_i (reflCount) // TODO collect output into one message
        writePoleFigureOutputFiles(path, i);
}

static QString const OUT_FILE_TAG(".refl%1");
static int const MAX_LINE_LENGTH_POL(9);

void PoleFiguresFrame::writePoleFigureOutputFiles(const QString& filePath, int index)
{
    PeakInfos reflInfo;
    if (getInterpolated())
        reflInfo = interpPoints_.at(index);
    else
        reflInfo = calcPoints_.at(index);
    bool withFit = !gSession->peaks().at(index).isRaw();
    QString path = QString(filePath + OUT_FILE_TAG).arg(index + 1);
    bool check = false;
    int numSavedFiles = 0;

    if (tabSave_->outputInten()) {
        QVector<qreal> output;
        for_i (reflInfo.count())
            output.append(reflInfo.at(i).inten());
        const QString intenFilePath = path + ".inten";
        writeListFile(intenFilePath, reflInfo, output);
        writePoleFile(intenFilePath, reflInfo, output);
        writeErrorMask(intenFilePath, reflInfo, output);
        check = true;
        numSavedFiles += 3;
    }

    if (tabSave_->outputTth() && withFit) {
        QVector<qreal> output;
        for_i (reflInfo.count())
            output.append(reflInfo.at(i).tth());
        const QString tthFilePath = filePath + ".tth";
        writeListFile(tthFilePath, reflInfo, output);
        writePoleFile(tthFilePath, reflInfo, output);
        check = true;
        numSavedFiles += 2;
    }

    if (tabSave_->outputFWHM() && withFit) {
        QVector<qreal> output;
        for_i (reflInfo.count())
            output.append(reflInfo.at(i).fwhm());
        const QString fwhmFilePath = filePath + ".fwhm";
        writeListFile(fwhmFilePath, reflInfo, output);
        writePoleFile(fwhmFilePath, reflInfo, output);
        check = true;
        numSavedFiles += 2;
    }

    if (numSavedFiles > 0) {
        if (check)
            qDebug() /* qInfo() TODO restore */ << numSavedFiles << " files have been saved";
        else
            qWarning() << "something went wrong, yet " << numSavedFiles << " files have been saved";
    } else
        qWarning() << "no files saved";
}

void PoleFiguresFrame::writeErrorMask(
    const QString& filePath, PeakInfos reflInfo, const QVector<qreal>& output)
{
    QFile* file = file_dialog::openFileConfirmOverwrite("file", this, filePath);
    if (!file)
        return;
    QTextStream stream(file);

    for (int j = 0, jEnd = reflInfo.count(); j < jEnd; j += 9) {
        int max = j + MAX_LINE_LENGTH_POL;
        for (int i = j; i < max; i++) {
            if (qIsNaN(output.at(i)))
                stream << "0 ";
            else
                stream << "1 ";
        }
        stream << '\n';
    }
}

void PoleFiguresFrame::writePoleFile(
    const QString& filePath, PeakInfos reflInfo, const QVector<qreal>& output)
{
    QFile* file = file_dialog::openFileConfirmOverwrite("file", this, filePath);
    if (!file)
        return;
    QTextStream stream(file);

    for (int j = 0, jEnd = reflInfo.count(); j < jEnd; j += 9) {
        int max = j + MAX_LINE_LENGTH_POL;
        for (int i = j; i < max; i++) {
            if (qIsNaN(output.at(i)))
                stream << " -1  ";
            else
                stream << output.at(i) << " ";
        }
        stream << '\n';
    }
}

void PoleFiguresFrame::writeListFile(
    const QString& filePath, PeakInfos reflInfo, const QVector<qreal>& output)
{
    QFile* file = file_dialog::openFileConfirmOverwrite("file", this, filePath);
    QTextStream stream(file);

    for_i (reflInfo.count()) {
        stream << qreal(reflInfo.at(i).alpha()) << " " << qreal(reflInfo.at(i).beta()) << " "
               << output.at(i) << '\n';
    }
}

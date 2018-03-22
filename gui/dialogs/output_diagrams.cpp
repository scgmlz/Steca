// ************************************************************************** //
//
//  Steca: stress and texture calculator
//
//! @file      gui/dialogs/output_diagrams.cpp
//! @brief     Implements class DiagramsFrame
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#include "gui/dialogs/output_diagrams.h"
#include "core/session.h"
#include "gui/base/file_dialog.h"
#include "gui/mainwin.h"
#include "gui/dialogs/data_table.h"
#include "gui/dialogs/dialog_panels.h"
#include "gui/dialogs/tab_save.h"
#include "gui/panels/plot_diagram.h"

static const Params::ePanels PANELS =
    Params::ePanels(Params::REFLECTION | Params::GAMMA | Params::DIAGRAM);

// sorts xs and ys the same way, by (x,y)
static void sortColumns(QVector<qreal>& xs, QVector<qreal>& ys, QVector<int>& is)
{
    ASSERT(xs.count() == ys.count());

    int count = xs.count();

    is.resize(count);
    for_i (count)
        is[i] = i;

    std::sort(is.begin(), is.end(), [&xs, &ys](int i1, int i2) {
        qreal x1 = xs.at(i1), x2 = xs.at(i2);
        if (x1 < x2)
            return true;
        if (x1 > x2)
            return false;
        return ys.at(i1) < ys.at(i2);
    });

    QVector<qreal> r(count);

    for_i (count)
        r[i] = xs.at(is.at(i));
    xs = r;

    for_i (count)
        r[i] = ys.at(is.at(i));
    ys = r;
}

// ************************************************************************** //
//  local class TabDiagramsSave
// ************************************************************************** //

//! Tab in DiagramsFrame, to save a data table.

class TabDiagramsSave : public TabSave {
public:
    TabDiagramsSave();
    bool currDiagram() const { return currentDiagram_.isChecked(); }
private:
    GridPanel gp_ {"To save"};
    CRadioButton currentDiagram_ {"currentDiagram#", "Current diagram"};
    CRadioButton allData_ {"allData#", "All data"};
};

TabDiagramsSave::TabDiagramsSave()
    : TabSave(true)
{
    grid_->addWidget(&gp_, grid_->rowCount(), 0, 1, 2);
    grid_->setRowStretch(grid_->rowCount(), 1);

    gp_.grid_.addWidget(&currentDiagram_);
    gp_.grid_.addWidget(&allData_);
    gp_.grid_.setColumnStretch(0, 1);

    currentDiagram_.setChecked(true);
}

// ************************************************************************** //
//  class DiagramsFrame
// ************************************************************************** //

DiagramsFrame::DiagramsFrame()
    : Frame("diagrams#", "Diagrams", new Params(PANELS))
{
    btnInterpolate_.hide();

    {
        auto* tab = new QWidget();
        tabs_.addTab(tab, "Diagram");
        tab->setLayout(new QVBoxLayout());
        plot_ = new PlotDiagram();
        tab->layout()->addWidget(plot_);
    }

    ASSERT(params_->panelDiagram);
    const PanelDiagram*const pd = params_->panelDiagram;

    connect(&pd->xAxis, qOverload<int>(&QComboBox::currentIndexChanged), [this]() {
            recalculate(); });
    connect(&pd->yAxis, qOverload<int>(&QComboBox::currentIndexChanged), [this]() {
            recalculate(); });

    {
        auto* actSave = new CTrigger("exportSave", "Save");
        auto* tab = new QWidget();
        tabs_.addTab(tab, "Save");
        tab->setLayout(new QVBoxLayout());
        tabSave_ = new TabDiagramsSave();
        tab->layout()->addWidget(tabSave_);
        tab->layout()->addWidget(new XTextButton(actSave));
        connect(actSave, &QAction::triggered, [this]() { saveDiagramOutput(); });
    }

    recalculate();
    show();
}

DiagramsFrame::~DiagramsFrame()
{
    delete tabSave_;
    delete plot_;
}

DiagramsFrame::eReflAttr DiagramsFrame::xAttr() const
{
    ASSERT(params_->panelDiagram);
    return eReflAttr(params_->panelDiagram->xAxis.currentIndex());
}

DiagramsFrame::eReflAttr DiagramsFrame::yAttr() const
{
    ASSERT(params_->panelDiagram);
    return eReflAttr(params_->panelDiagram->yAxis.currentIndex());
}

void DiagramsFrame::displayPeak(int reflIndex, bool interpolated)
{
    Frame::displayPeak(reflIndex, interpolated);
    rs_ = calcPoints_.at(reflIndex);
    recalculate();
}

void DiagramsFrame::recalculate()
{
    int count = rs_.count();

    xs_.resize(count);
    ys_.resize(count);

    int xi = int(xAttr());
    int yi = int(yAttr());

    for_i (count) {
        const row_t row = rs_.at(i).data();
        xs_[i] = row.at(xi).toDouble();
        ys_[i] = row.at(yi).toDouble();
    }

    QVector<int> is;
    sortColumns(xs_, ys_, is);

    auto _calcErrors = [this, is](eReflAttr attr) {
        int count = ys_.count();
        ysErrorLo_.resize(count);
        ysErrorUp_.resize(count);

        for_i (count) {
            const row_t row = rs_.at(is.at(i)).data(); // access error over sorted index vec
            qreal sigma = row.at(int(attr)).toDouble();
            qreal y = ys_.at(i);
            ysErrorLo_[i] = y - sigma;
            ysErrorUp_[i] = y + sigma;
        }
    };

    ysErrorLo_.clear();
    ysErrorUp_.clear();

    if (!gSession->peaks().at(getReflIndex()).isRaw()) {
        switch (yAttr()) {
        case eReflAttr::INTEN:
            _calcErrors(eReflAttr::SIGMA_INTEN);
            break;
        case eReflAttr::TTH:
            _calcErrors(eReflAttr::SIGMA_TTH);
            break;
        case eReflAttr::FWHM:
            _calcErrors(eReflAttr::SIGMA_FWHM);
            break;
        default:
            break;
        }
    }

    plot_->plot(xs_, ys_, ysErrorLo_, ysErrorUp_);
}

void DiagramsFrame::saveDiagramOutput()
{
    QString path = tabSave_->filePath(true);
    if (path.isEmpty()) {
        qWarning() << "cannot save diagram: path is empty";
        return;
    }
    QString separator = tabSave_->separator();
    if (tabSave_->currDiagram())
        writeCurrentDiagramOutputFile(path, separator);
    else
        writeAllDataOutputFile(path, separator);
}

void DiagramsFrame::writeCurrentDiagramOutputFile(const QString& filePath, const QString& separator)
{
    QFile* file = file_dialog::openFileConfirmOverwrite("file", this, filePath);
    if (!file)
        return;
    QTextStream stream(file);

    ASSERT(xs_.count() == ys_.count());
    ASSERT(ysErrorLo_.isEmpty() || ysErrorLo_.count() == ys_.count());
    ASSERT(ysErrorLo_.count() == ysErrorUp_.count());

    bool writeErrors = !ysErrorUp_.isEmpty();

    for_i (xs_.count()) {
        stream << xs_.at(i) << separator << ys_.at(i);
        if (writeErrors)
            stream << separator << ysErrorLo_.at(i) << separator << ysErrorUp_.at(i);
        stream << '\n';
    }
}

void DiagramsFrame::writeAllDataOutputFile(const QString& filePath, const QString& separator)
{
    QFile* file = file_dialog::openFileConfirmOverwrite("file", this, filePath);
    if (!file)
        return;
    QTextStream stream(file);

    const QStringList& headers = dataView_->outHeaders();
    for_i (headers.count())
        stream << headers.at(i) << separator;
    stream << '\n';

    for_i (calcPoints_.at(getReflIndex()).count()) {
        const row_t& row = dataView_->row(i);
        for_i (row.count()) {
            const QVariant& var = row.at(i);
            if (isNumeric(var))
                stream << var.toDouble();
            else
                stream << var.toString();
            stream << separator;
        }
        stream << '\n';
    }
}

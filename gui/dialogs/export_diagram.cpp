//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      gui/dialogs/export_diagram.cpp
//! @brief     Implements class ExportDiagram
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#include "gui/dialogs/export_diagram.h"
#include "core/data/export.h"
#include "core/session.h"
#include "gui/mainwin.h"
//#include "qcr/base/debug.h"

namespace {

void writeDiagram(QTextStream& stream, const QString& separator)
{
    // get data

    const int idxX = int(gSession->params.diagramX.val());
    const int idxY = int(gSession->params.diagramY.val());
    std::vector<double> xs, ys, ysSigma;
    gSession->allPeaks.currentInfoSequence()->getValuesAndSigma(idxX, idxY, xs, ys, ysSigma);

    // write header

    QStringList tags = PeakInfo::dataTags(true);
    stream << "# " << tags[idxX] << separator << tags[idxY];
    if (ysSigma.size() > 0)
        stream << separator << "sigma_"  << tags[idxY];
    stream << '\n';

    // write data

    for (size_t i = 0; i < xs.size(); ++i) {
        stream << xs[i] << separator << ys[i];
        if (ysSigma.size() > 0)
            stream << separator << ysSigma[i];
        stream << '\n';
    }
}

} // namespace

//  ***********************************************************************************************
//! @class ExportDiagram

ExportDiagram::ExportDiagram()
    : DialogSave(gGui, "ExportDiagram", "Diagram data export", data_export::defaultFormats)
{
}

void ExportDiagram::writeCurrent(QTextStream& stream)
{
    writeDiagram(stream, data_export::separator(format()));
}

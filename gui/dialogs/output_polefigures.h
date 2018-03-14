// ************************************************************************** //
//
//  Steca: stress and texture calculator
//
//! @file      gui/dialogs/output_polefigures.h
//! @brief     Defines class PoleFiguresFrame
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#ifndef OUTPUT_POLEFIGURES_H
#define OUTPUT_POLEFIGURES_H

#include "frame.h"

//! The modal dialog for generating and saving pole figures
class PoleFiguresFrame : public Frame {
public:
    PoleFiguresFrame();
    ~PoleFiguresFrame();

private:
    class TabGraph* tabGraph_;
    class TabPoleFiguresSave* tabSave_;

    void displayPeak(int reflIndex, bool interpolated);

    void savePoleFigureOutput();
    void writePoleFigureOutputFiles(const QString& filePath, int index);
    void writePoleFile(const QString& filePath, PeakInfos, const vec<qreal>&);
    void writeListFile(const QString& filePath, PeakInfos, const vec<qreal>&);
    void writeErrorMask(const QString& filePath, PeakInfos, const vec<qreal>&);
};

#endif // OUTPUT_POLEFIGURES_H

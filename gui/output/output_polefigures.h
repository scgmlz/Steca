// ************************************************************************** //
//
//  Steca: stress and texture calculator
//
//! @file      gui/output/output_polefigures.h
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
    PoleFiguresFrame(rcstr title, QWidget*);

private:
    class TabGraph* tabGraph_;
    class TabPoleFiguresSave* tabSave_;

    void displayReflection(int reflIndex, bool interpolated);

    void savePoleFigureOutput();
    void writePoleFigureOutputFiles(rcstr filePath, int index);
    void writePoleFile(rcstr filePath, ReflectionInfos, vec<qreal> const&);
    void writeListFile(rcstr filePath, ReflectionInfos, vec<qreal> const&);
    void writeErrorMask(rcstr filePath, ReflectionInfos, vec<qreal> const&);
};

#endif // OUTPUT_POLEFIGURES_H

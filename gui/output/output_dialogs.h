// ************************************************************************** //
//
//  Steca2: stress and texture calculator
//
//! @file      gui/output/output_dialogs.h
//! @brief     Defines ...
//!
//! @homepage  https://github.com/scgmlz/Steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2017
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#ifndef OUTPUT_DIALOGS_H
#define OUTPUT_DIALOGS_H

#include "typ/variant.h"
#include "def/special_pointers.h"
#include "panels/panel.h"
#include "widgets/tree_views.h"

class QCheckBox;

namespace gui {
namespace output {

class Params : public QWidget {
public:
    enum ePanels {
        REFLECTION = 0x01,
        GAMMA = 0x02,
        POINTS = 0x04,
        INTERPOLATION = 0x08,
        DIAGRAM = 0x10,
    };
    Params(ePanels);
    ~Params();
    class PanelReflection* panelReflection;
    class PanelGammaSlices* panelGammaSlices;
    class PanelGammaRange* panelGammaRange;
    class PanelPoints* panelPoints;
    class PanelInterpolation* panelInterpolation;
    class PanelDiagram* panelDiagram;
    str saveDir, saveFmt;
    void readSettings();
    void saveSettings() const;
    QBoxLayout* box_;
};


class Table : public TreeView {
public:
    Table(uint numDataColumns);
    void setColumns(QStringList const& headers, QStringList const& outHeaders, cmp_vec const&);
    QStringList const outHeaders() { return outHeaders_; }
    void clear();
    void addRow(row_t const&, bool sort);
    void sortData();
    uint rowCount() const;
    row_t const& row(uint) const;
    scoped<class TableModel*> model_;
    QStringList outHeaders_;
};


class Tab : public QWidget { // TODO: better names for panel::Tab and output::Tab
public :
    Tab(Params&);
protected:
    Params& params_;
    QGridLayout* grid_;
};


class TabTable : public Tab {
public:
    TabTable(Params&, QStringList const& headers, QStringList const& outHeaders,
             cmp_vec const&);
private:
    struct showcol_t {
        str name;
        QCheckBox* cb;
    };
    typedef vec<showcol_t> showcol_vec;

    class ShowColsWidget : public QWidget {
    private:
    public:
        ShowColsWidget(Table&, showcol_vec&);
    private:
        Table& table_;
        showcol_vec& showCols_;
        QBoxLayout* box_;
        QRadioButton *rbHidden_, *rbAll_, *rbNone_, *rbInten_, *rbTth_, *rbFWHM_;
    };
public:
    Table* table;
private:
    ShowColsWidget* showColumnsWidget_;
    showcol_vec showCols_;
};


class TabSave : public Tab {
public:
    TabSave(Params&, bool withTypes);
    str filePath(bool withSuffix);
    str separator() const;
    QAction *actBrowse, *actSave;
protected:
    str fileSetSuffix(rcstr);
    QLineEdit *dir_, *file_;
    QRadioButton *rbDat_, *rbCsv_;
};

} // namespace output
} // namespace gui

#endif // OUTPUT_DIALOGS_H

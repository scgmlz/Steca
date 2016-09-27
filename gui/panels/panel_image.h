// ************************************************************************** //
//
//  STeCa2:    StressTextureCalculator ver. 2
//
//! @file      panel_image.h
//! @brief     Image panel.
//!
//! @homepage  http://apps.jcns.fz-juelich.de/steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016
//! @authors   Scientific Computing Group at MLZ Garching
//! @authors   Rebecca Brydon, Jan Burle,  Antti Soininen
//! @authors   Based on the original STeCa by Christian Randau
//
// ************************************************************************** //

#ifndef PANEL_IMAGE_H
#define PANEL_IMAGE_H

#include "panel.h"
#include "calc/calc_lens.h"

namespace gui { namespace panel {
//------------------------------------------------------------------------------

class ImagePanel;

class ImageWidget : public QWidget, protected RefHub {
  CLS(ImageWidget) SUPER(QWidget)
public:
  ImageWidget(TheHub&, ImagePanel&);

  void setPixmap(QPixmap const&);
  void setShowOverlay(bool);
  void setScale(preal);

  QSize sizeHint() const;

protected:
  ImagePanel &dataset_;
  bool     showOverlay_;
  QPixmap  original_, scaled_;
  preal    scale_;

  void paintEvent(QPaintEvent*);
};

//------------------------------------------------------------------------------

class ImagePanel : public TabsPanel {
  CLS(ImagePanel) SUPER(TabsPanel)
public:
  ImagePanel(TheHub&);

  void setImageScale(preal);

private:
  QPixmap makeBlankPixmap();
  QPixmap makePixmap(calc::shp_ImageLens);

  void    setDataset(data::shp_Dataset);
  void    render();

  data::shp_Dataset dataset_;
  ImageWidget *dataImageWidget_, *corrImageWidget_;

  uint n = 0;
  QSpinBox *spinN;
  QLabel   *labelN;
};

//------------------------------------------------------------------------------
}}
#endif // PANEL_IMAGE_H

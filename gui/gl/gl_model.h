// ************************************************************************** //
//
//  STeCa2:    StressTexCalculator ver. 2
//
//! @file      gl_model.h
//! @brief     OpenGl model
//!
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016
//! @authors   Scientific Computing Group at MLZ Garching
//! @authors   Original version: Christian Randau
//! @authors   Version 2: Antti Soininen, Jan Burle, Rebecca Brydon
//
// ************************************************************************** //

// Original code Copyright 2015-6 Ian G Burleigh
// MIT-licence

#ifndef GL_MODEL_H
#define GL_MODEL_H

#include "core_defs.h"
#include <qopengl.h>

namespace gui { namespace gl {
//------------------------------------------------------------------------------

class Mesh {
public:
  Mesh();

private:
  QVector<GLfloat> glData;
};

//------------------------------------------------------------------------------
}}
#endif // GL_MODEL_H

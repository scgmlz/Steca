/*******************************************************************************
 * STeCa2 - StressTextureCalculator ver. 2
 *
 * Copyright (C) 2016 Forschungszentrum Jülich GmbH 2016
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * See the COPYING and AUTHORS files for more details.
 ******************************************************************************/

#ifndef FILEDIALOG_H
#define FILEDIALOG_H

#include "typ/typ_str.h"
#include "typ/typ_strlst.h"

class QWidget;

namespace gui { namespace file_dialog {
//------------------------------------------------------------------------------

str     openFileName(QWidget*, rcstr caption, rcstr dir, rcstr filter);
str     saveFileName(QWidget*, rcstr caption, rcstr dir, rcstr filter);
str_lst openFileNames(QWidget*, rcstr caption, rcstr dir, rcstr filter);

str     saveDirName(QWidget*, rcstr caption, rcstr dir);

//------------------------------------------------------------------------------
}}
#endif // FILEDIALOG_H

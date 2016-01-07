#include "file.h"

#include <memory>
#include "loadcaress.h"

namespace core {

File::File(): File("") {
}

File::File(rcstr fileName): info(fileName) {
}

void File::load() THROWS {
  if (!info.exists())
    raiseError("File " % info.filePath() % " does not exist");
  loadCaress(info.filePath(),datasets);
}

Files::Files() {
}

Files::~Files() {
  for (auto file: *this)
    delete file;
}

}

// eof

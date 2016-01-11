#include "core_file.h"

#include <memory>
#include "core_loadcaress.h"

namespace core {

File::File(): File(str::null) {
}

File::File(rcstr fileName): info(fileName) {
  TR(" f" << this)
}

File::~File() {
  TR("~f" << this)
}

void File::load() THROWS {
  RUNTIME_CHECK(info.exists(), "File " % info.filePath() % " does not exist");
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

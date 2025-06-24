#include "fileDialog.h"

const char* FileDialog::getFile()
{

    return tinyfd_openFileDialog(
        "Open Model",
        "",
        0,
        NULL,
        NULL,
        false);

}
#ifndef RECONSTRUCT_H
#define RECONSTRUCT_H

#include "../Extract/Extract.h"

int Reconstruct(GdkPixbuf* input, Cluster* **grid, Cluster* **word_list,
        Solution** solutions, int word_count);

#endif

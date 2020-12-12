##include "misc.h"

int gobble(const char* pattern, size_t plen, size_t i, const char m, const char mr, size_t *j) {
    if (i >= plen || j == NULL) {
        return -1;
    }

    size_t skip = 0;
    for (; i < plen; i++) {
        const char c = pattern[i];

        if ((mr != '\0') && (c == mr)) {
            skip++;
        }

        if (c == m) {
            if (skip > 0) { skip--; continue;}

            *j = i;
            return 0;
        }
    }

    return -1;
}

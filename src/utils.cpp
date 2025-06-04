#define DBG_MACRO_NO_WARNING
#include <dbg.h>
#include "utils.h"

#include <algorithm>
#include <random>

std::string fileDialog(const nfdu8filteritem_t *filters, const nfdfiltersize_t count) {
    NFD_Init();
    nfdu8char_t *outPath;
    nfdopendialogu8args_t args = {nullptr};
    args.filterList = filters;
    args.filterCount = count;
    std::string filename;
    if (const nfdresult_t result = NFD_OpenDialogU8_With(&outPath, &args); result == NFD_OKAY) {
        filename = outPath;
        std::ranges::replace(filename, '\\', '/');
        NFD_FreePathU8(outPath);
    } else if (result == NFD_CANCEL) {
        // no-op
    } else {
        printf("Error: %s\n", NFD_GetError());
    }
    NFD_Quit();
    return filename;
}

int randomInt() {
    static std::mt19937 gen{std::random_device{}()};
    static std::uniform_int_distribution<int> distribution;
    return distribution(gen);
}

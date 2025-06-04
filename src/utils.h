#ifndef UTILS_H
#define UTILS_H

#include <nfd.h>

#include <string>

std::string fileDialog(const nfdu8filteritem_t *filters, const nfdfiltersize_t count);

int randomInt();

#endif

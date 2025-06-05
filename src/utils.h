#ifndef UTILS_H
#define UTILS_H

#include <nfd.h>

#include <filesystem>
#include <optional>
#include <string>

auto fileDialog(const nfdu8filteritem_t *filters, const nfdfiltersize_t count) -> std::optional<std::string>;

int randomInt();

std::string normalize_path(const std::string &path);

std::string join_paths(const std::string &base, const std::string &relative);

std::string get_directory(const std::string &filepath);

std::string get_filename(const std::string &filepath);

#endif

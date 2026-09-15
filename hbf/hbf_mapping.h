// Shared helpers for HBF page-placement tables.
//
// A placement table contains one `page,channel` pair per line. Whitespace is
// also accepted as a separator and lines beginning with '#' are ignored.

#ifndef HBF_MAPPING_H
#define HBF_MAPPING_H

#include <errno.h>
#include <ctype.h>
#include <limits.h>
#include <map>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>

static inline bool hbf_placement_error(std::string *error,
                                       const std::string &message) {
  if (error != NULL) *error = message;
  return false;
}

// Parse the entire file before replacing `table`. Explicit placement is a
// correctness contract: malformed or incomplete configuration must not be
// converted into a different placement policy by best-effort parsing.
static inline bool hbf_load_placement_table(
    const char *path, unsigned num_channels,
    std::map<unsigned long long, unsigned> *table, std::string *error) {
  if (error != NULL) error->clear();
  if (table == NULL)
    return hbf_placement_error(error, "null destination table");
  if (path == NULL || path[0] == '\0')
    return hbf_placement_error(error, "placement table path is empty");
  if (num_channels == 0)
    return hbf_placement_error(error, "placement channel count is zero");

  FILE *fp = fopen(path, "r");
  if (fp == NULL) {
    return hbf_placement_error(
        error, std::string("could not open placement table '") + path +
                   "': " + strerror(errno));
  }

  std::map<unsigned long long, unsigned> parsed;
  char line[1024];
  unsigned line_number = 0;
  while (fgets(line, sizeof(line), fp) != NULL) {
    ++line_number;
    size_t line_length = strlen(line);
    if (line_length == sizeof(line) - 1 && line[line_length - 1] != '\n' &&
        !feof(fp)) {
      fclose(fp);
      return hbf_placement_error(
          error, "placement line " + std::to_string(line_number) +
                     " exceeds 1023 bytes");
    }

    char *p = line;
    while (*p != '\0' && isspace((unsigned char)*p)) ++p;
    if (*p == '\0' || *p == '#') continue;

    if (*p == '-') {
      fclose(fp);
      return hbf_placement_error(
          error, "negative page on placement line " +
                     std::to_string(line_number));
    }

    errno = 0;
    char *end = NULL;
    unsigned long long page = strtoull(p, &end, 0);
    if (end == p || errno == ERANGE) {
      fclose(fp);
      return hbf_placement_error(
          error, "invalid page on placement line " +
                     std::to_string(line_number));
    }

    bool had_whitespace = false;
    while (*end != '\0' && isspace((unsigned char)*end)) {
      had_whitespace = true;
      ++end;
    }
    if (*end == ',') {
      ++end;
      while (*end != '\0' && isspace((unsigned char)*end)) ++end;
    } else if (!had_whitespace) {
      fclose(fp);
      return hbf_placement_error(
          error, "missing page/channel separator on placement line " +
                     std::to_string(line_number));
    }
    if (*end == '-') {
      fclose(fp);
      return hbf_placement_error(
          error, "negative channel on placement line " +
                     std::to_string(line_number));
    }

    errno = 0;
    char *channel_end = NULL;
    unsigned long channel = strtoul(end, &channel_end, 0);
    if (channel_end == end || errno == ERANGE || channel > UINT_MAX) {
      fclose(fp);
      return hbf_placement_error(
          error, "invalid channel on placement line " +
                     std::to_string(line_number));
    }
    while (*channel_end != '\0' &&
           isspace((unsigned char)*channel_end))
      ++channel_end;
    if (*channel_end != '\0' && *channel_end != '#') {
      fclose(fp);
      return hbf_placement_error(
          error, "trailing data on placement line " +
                     std::to_string(line_number));
    }
    if (channel >= num_channels) {
      fclose(fp);
      return hbf_placement_error(
          error, "channel out of range on placement line " +
                     std::to_string(line_number));
    }

    auto inserted = parsed.insert(std::make_pair(page, (unsigned)channel));
    if (!inserted.second && inserted.first->second != channel) {
      fclose(fp);
      return hbf_placement_error(
          error, "conflicting duplicate page on placement line " +
                     std::to_string(line_number));
    }
  }

  if (ferror(fp)) {
    fclose(fp);
    return hbf_placement_error(
        error, std::string("failed while reading placement table '") + path +
                   "'");
  }
  fclose(fp);
  if (parsed.empty())
    return hbf_placement_error(error, "placement table has no entries");

  table->swap(parsed);
  return true;
}

#endif  // HBF_MAPPING_H

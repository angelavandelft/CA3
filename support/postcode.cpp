/*
 * Skeleton code for use with Computer Architecture 2024 assignment 3,
 * LIACS, Leiden University.
 */

#include "postcode.h"
#include "timing.h"

#include <fstream>
#include <iostream>

constexpr const size_t BUFSIZE = 128;


static bool
find_separators(char *line, size_t sep[], int n_sep)
{
  int pos = 0;
  int f = 0;

  for (char *c = line; *c != '\0'; ++c, ++pos)
    {
      if (*c == ',')
        {
          if (f >= n_sep)
            return false;

          sep[f] = pos;
          ++f;
        }
    }

  if (f >= n_sep)
    return false;
  if (f < n_sep - 1)
    return false;

  sep[f] = pos;

  return true;
}

static inline size_t
field_length(size_t sep[], int field)
{
  int start = 0;
  if (field-1 >= 0)
    start = sep[field-1] + 1;

  return sep[field] - start;
}


bool
read_postcode_table(const std::string &filename,
                    std::vector<Record> &postcode_table)
{
  std::ifstream postcode_file(filename);
  if (!postcode_file)
    {
      std::cerr << "cannot open " << filename << std::endl;
      return false;
    }

  char buffer[BUFSIZE] = { 0, };

  /* read header and ignore */
  postcode_file.getline(buffer, BUFSIZE);

  int line_nr = 1;
  while (postcode_file.getline(buffer, BUFSIZE))
    {
      line_nr++;

      size_t sep[Record::N_FIELDS];
      if (! find_separators(buffer, sep, Record::N_FIELDS))
        {
          std::cerr << line_nr << ": does not contain 4 fields" << std::endl;
          return false;
        }

      /* Sanity check lengths; postcode is stored w/o null terminator */
      if (field_length(sep, 0) > Record::POSTCODE_LEN ||
          field_length(sep, 1) >= Record::STREET_LEN ||
          field_length(sep, 2) >= Record::CITY_LEN ||
          field_length(sep, 3) >= Record::PROVINCE_LEN)
        {
          std::cerr << line_nr << ": field length longer than expected" << std::endl;
          return false;
        }

      /* Add record to table */
      Record &last(postcode_table.emplace_back());
      strncpy(last.postcode.data(), buffer, field_length(sep, 0));
      strncpy(last.street.data(), buffer + sep[0] + 1, field_length(sep, 1));
      strncpy(last.city.data(), buffer + sep[1] + 1, field_length(sep, 2));
      strncpy(last.province.data(), buffer + sep[2] + 1, field_length(sep, 3));
    }

  return true;
}

bool
read_query(const std::string &filename,
           std::vector<Record> &result_table)
{
  std::ifstream query_file(filename);
  if (!query_file)
    {
      std::cerr << "cannot open " << filename << std::endl;
      return false;
    }

  char buffer[BUFSIZE] = { 0, };

  while (query_file.getline(buffer, BUFSIZE))
    {
      Record &last(result_table.emplace_back());

      if (strnlen(buffer, BUFSIZE) < Record::POSTCODE_LEN)
        {
          std::cerr << "Error: " << filename
                    << ": line found that cannot contain a postcode" << std::endl;
          return false;
        }

      memcpy(last.postcode.data(), buffer, Record::POSTCODE_LEN);
    }

  return true;
}


void
dump_results(const std::vector<Record> &result_table)
{
  for (auto &result : result_table)
    {
      std::array<char, Record::POSTCODE_LEN+1> postcode_buffer;
      strncpy(postcode_buffer.data(), result.postcode.data(), Record::POSTCODE_LEN);
      postcode_buffer[Record::POSTCODE_LEN] = 0;

      std::cout << postcode_buffer.data();
      if (result.city[0] == '\0')
        std::cout << ": not found." << std::endl;
      else
        std::cout << ": " << result.street.data() << ", "
                  << result.city.data() << std::endl;
    }
}

bool
check_all_found(const std::vector<Record> &result_table)
{
  for (auto &result : result_table)
    {
      if (result.city[0] == '\0')
        {
          std::cerr << "Error: not all postcodes were found!" << std::endl;
          return false;
        }
    }

  return true;
}


bool
test_query_function(RunQueryFunction func,
                    const size_t blockSize,
                    const std::vector<Record> &postcode_table,
                    std::vector<Record> &result_table)
{
  func(postcode_table, result_table, blockSize);
  dump_results(result_table);
  return check_all_found(result_table);
}

bool
bench_query_function(RunQueryFunction func,
                     std::string_view func_name,
                     const long int n_repeat,
                     const size_t blockSize,
                     const std::vector<Record> &postcode_table,
                     const std::vector<Record> &result_table_blank,
                     std::string_view postcode_filename,
                     std::string_view query_filename)
{
  std::vector<Record> result_table;
  for (long int Z = 0; Z < n_repeat; ++Z)
    {
      result_table.clear();
      std::copy(result_table_blank.begin(), result_table_blank.end(),
                std::back_inserter(result_table));


      struct timespec start_time, end_time;

      get_time(&start_time);

      func(postcode_table, result_table, blockSize);

      get_time(&end_time);

#if ENABLE_TIMING
      /* Output result in CSV for each processing. */
      /* database,query,func name,block size,compute time */
      std::cerr << postcode_filename << ","
                << query_filename << ","
                << func_name << ","
                << blockSize << ","
                << get_elapsed_ms(&end_time, &start_time) << std::endl;
#endif /* ENABLE_TIMING */
    }

  return check_all_found(result_table);
}

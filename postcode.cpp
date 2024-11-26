/*
 * Skeleton code for use with Computer Architecture 2024 assignment 3,
 * LIACS, Leiden University.
 */

#include "postcode.h"

#include <unordered_map>
#include <algorithm>

#include <iostream>

static void
run_query_scan(const std::vector<Record> &postcode_table,
               std::vector<Record> &result_table,
               const size_t blockSize)
{
  for (auto &result : result_table)
    {
      auto it = std::find(postcode_table.begin(), postcode_table.end(),
                          result.postcode);
      if (it != postcode_table.end())
        {
          strncpy(result.street.data(), it->street.data(), Record::STREET_LEN);
          strncpy(result.city.data(), it->city.data(), Record::CITY_LEN);
        }
    }
}

static void
run_query_hash(const std::vector<Record> &postcode_table,
               std::vector<Record> &result_table,
               const size_t blockSize)
{
  std::unordered_map<Record::Postcode, size_t> hash;

  /* Setting the number of buckets in advance has a positive effect
   * on performance. The hash function provided is near perfect.
   */
  hash.rehash(postcode_table.size());

  for (size_t i = 0; i < postcode_table.size(); ++i)
    hash[postcode_table[i].postcode] = i;

  for (auto &result : result_table)
    {
      auto it = hash.find(result.postcode);
      if (it != hash.end())
        {
          strncpy(result.street.data(), postcode_table[it->second].street.data(), Record::STREET_LEN);
          strncpy(result.city.data(), postcode_table[it->second].city.data(), Record::CITY_LEN);
        }
    }
}

/* Add additional variants here. In support/postcode.h you can find
 * a binary_search routine you can use. You can add the new routines
 * in the table search_modes below to test and benchmark these.
 */

struct SearchMode{
  std::string name;
  RunQueryFunction func;
} search_modes[] = {
  {"scan", run_query_scan},
  {"hash", run_query_hash},

  /* This must be the last entry. */
  {"", NULL}
};

int
usage (int argc, char **argv)
{
  std::cerr << "Usage: " << argv[0]
            << " <postcode_file> <query_file> [-b block] <n_repeat>" << std::endl;
  std::cerr << "       " << argv[0]
            << " <postcode_file> <query_file> [-b block] -t <mode>" << std::endl;
  std::cerr << std::endl;
  std::cerr << "Options: " << std::endl;
  std::cerr << "  -b <block> Set block size." << std::endl;
  std::cerr << "  -t <mode>  Use the search function <mode>, and print the" << std::endl
            << "             results of the query to stdout." << std::endl;
  return EXIT_FAILURE;
}

int
main (int argc, char **argv)
{
  if (argc < 4)
    return usage(argc, argv);

  std::string postcode_filename(argv[1]);
  std::string query_filename(argv[2]);

  std::string test_mode = "";
  long int n_repeat = 0,
           block_size = 0;

  /* Parse options */
  int i = 3;
  for(; i < argc && *argv[i]=='-'; i++)
    {
      switch(argv[i][1])
        {
          case 't':
            test_mode = argv[++i];
            break;

          case 'b':
            block_size = strtol(argv[++i], NULL, 10);
            if (errno > 0 || block_size == 0)
              {
                std::cerr << "Invalid block size: " << argv[i] << std::endl;
                return EXIT_FAILURE;
              }

            break;

          default:
            std::cerr << "Unknown option: " << argv[i] << std::endl;
            return usage(argc, argv);
        }
    }

  if (test_mode == "")
    {
      /* Set repeat count */
      if (argv[i] == NULL)
        return usage(argc, argv);

      n_repeat = strtol(argv[i], NULL, 10);
      if (errno > 0 || n_repeat == 0)
        {
          std::cerr << "Invalid repeat amount: " << argv[i] << std::endl;
          return EXIT_FAILURE;
        }
    }

  /* Load files */
  std::vector<Record> postcode_table;
  if (! read_postcode_table(postcode_filename, postcode_table))
    return EXIT_FAILURE;

  std::cerr << "loaded " << postcode_table.size() << " entries." << std::endl;

  std::vector<Record> result_table_blank;
  if (! read_query(query_filename, result_table_blank))
    return EXIT_FAILURE;

  bool result = false;
  if (test_mode != "")
    {
      /* Run single function and print results */
      SearchMode *mode = &search_modes[0];
      for(; mode->func != NULL; mode++)
        {
          if(mode->name == test_mode)
            break;
        }
      if(mode->func == NULL)
        {
          std::cerr << "Unknown mode: " << test_mode << std::endl;
          return EXIT_FAILURE;
        }

      result = test_query_function(mode->func, block_size,
                                   postcode_table, result_table_blank);
      return result ? EXIT_SUCCESS : EXIT_FAILURE;
    }

  /* Run all functions */
  SearchMode *mode = &search_modes[0];
  for(; mode->func != NULL; mode++)
    {
      result |= bench_query_function(mode->func,
                                     mode->name, n_repeat, block_size,
                                     postcode_table, result_table_blank,
                                     postcode_filename, query_filename);
    }
  return result ? EXIT_SUCCESS : EXIT_FAILURE;
}

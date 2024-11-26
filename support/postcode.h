/*
 * Skeleton code for use with Computer Architecture 2024 assignment 3,
 * LIACS, Leiden University.
 */

#include <string>
#include <vector>
#include <array>
#include <functional>

#include <cstring>

/*
 * Record for Postcode entries.
 */
struct Record
{
  static constexpr size_t POSTCODE_LEN = 6;
  static constexpr size_t STREET_LEN = 60;
  static constexpr size_t CITY_LEN = 46;
  static constexpr size_t PROVINCE_LEN = 16;

  static constexpr size_t N_FIELDS = 4;

  using Postcode = std::array<char, POSTCODE_LEN>;

  Postcode postcode;
  std::array<char, STREET_LEN> street;
  std::array<char, CITY_LEN> city;
  std::array<char, PROVINCE_LEN> province;
};

static_assert(sizeof(Record) == 128, "Want Record struct to be 128 bytes");


inline bool postcode_compare(const Record::Postcode &a,
                             const Record::Postcode &b)
{
  return (strncmp(a.data(), b.data(), Record::POSTCODE_LEN) < 0);
}

inline bool operator<(const Record &a, const Record::Postcode &postcode)
{
  return postcode_compare(a.postcode, postcode);
}

inline bool operator==(const Record &a, const Record::Postcode &postcode)
{
  return (strncmp(a.postcode.data(), postcode.data(), Record::POSTCODE_LEN) == 0);
}

inline bool operator<(const Record::Postcode &postcode, const Record &b)
{
  return postcode_compare(postcode, b.postcode);
}


/*
 * Binary search function that returns match via iterator.
 * Adapted from:
 * https://en.cppreference.com/w/cpp/algorithm/binary_search
 */
template<class ForwardIt, class T>
static bool
binary_search(ForwardIt first, ForwardIt last,
              const T& value, ForwardIt &it)
{
  first = std::lower_bound(first, last, value);
  if (!(first == last) && !(value < *first))
    {
      it = first;
      return true;
    }
  /* else */
  return false;
}


/*
 * Hash function for "Postcode" that can be used with std::unordered_map.
 */
template<>
struct std::hash<Record::Postcode>
{
  /* Hash function inspired by https://stackoverflow.com/q/40172468 */
  inline std::size_t operator()(Record::Postcode const &p) const noexcept
  {
    size_t result =
        p[0] * 1757600
        + p[1] * 67600
        + p[2] * 2600
        + p[3] * 100
        + p[4] * 10
        + p[5] * 1;
    return result - 7722455528u;
  }
};


/*
 * Helper functions
 */

using RunQueryFunction =
    std::function<void(const std::vector<Record> &, std::vector<Record> &, const size_t)>;

bool  read_postcode_table(const std::string &filename,
                          std::vector<Record> &postcode_table);
bool  read_query(const std::string &filename,
                 std::vector<Record> &result_table);

void  dump_results(const std::vector<Record> &result_table);
bool  check_all_found(const std::vector<Record> &result_table);

bool  test_query_function(RunQueryFunction func,
                          const size_t blockSize,
                          const std::vector<Record> &postcode_table,
                          std::vector<Record> &result_table);

bool  bench_query_function(RunQueryFunction func,
                           std::string_view func_name,
                           const long int n_repeat,
                           const size_t blockSize,
                           const std::vector<Record> &postcode_table,
                           const std::vector<Record> &result_table_blank,
                           std::string_view postcode_filename,
                           std::string_view query_filename);

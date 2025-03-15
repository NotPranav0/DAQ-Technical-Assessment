#include <catch2/catch.hpp>
#include <solution.hpp>

#include <string>
#include <unordered_map>

TEST_CASE("Link test: DBCManager instantiation", "[link]") {
  std::unordered_map<std::string, std::string> dbcPaths;
  DBCManager manager(dbcPaths);
  SUCCEED("Successfully linked against the solution library");
}

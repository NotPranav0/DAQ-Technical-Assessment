#pragma once

#include <cassert>
#include <cstdint>
#include <dbcppp/Network.h>
#include <fstream>
#include <iostream>
#include <memory>
#include <regex>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

// DBC structure to wrap individual DBC files.
struct DBC {
  using Id = uint64_t;
  using MessagePtr = std::unique_ptr<dbcppp::IMessage>;
  std::unordered_map<Id, MessagePtr> messages;
  std::unique_ptr<dbcppp::INetwork> network;

  // Constructor that loads and parses a DBC file.
  DBC(const std::string &path);
};

// DBCManager loads and stores multiple DBC objects and provides decoding
// functionality.
class DBCManager {
public:
  // Construct the manager from a map: interface name -> DBC file path.
  DBCManager(const std::unordered_map<std::string, std::string> &dbcPaths);

  // Decode a given message and write the results to the provided output stream.
  void decodeAndOutput(const std::string &interface, uint64_t id,
                       const std::vector<uint8_t> &data,
                       const std::string &time, std::ostream &out) const;

private:
  std::unordered_map<std::string, DBC> dbcs;
};

// LogProcessor encapsulates reading the log file and using DBCManager to decode
// entries.
class LogProcessor {
public:
  // Construct the log processor with input/output streams and a reference to a
  // DBCManager.
  LogProcessor(std::istream &in, std::ostream &out,
               const DBCManager &dbcManager);

  // Process the log file line by line.
  void process();

private:
  std::istream &in;
  std::ostream &out;
  const DBCManager &dbcManager;
  std::regex logPattern;
};

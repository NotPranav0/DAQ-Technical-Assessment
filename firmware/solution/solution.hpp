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

// Structure for storing a parsed log frame.
struct LogFrame {
  std::string time;
  std::string interface;
  uint64_t id;
  std::vector<uint8_t> data;
};

// Structure for holding signal attributes.
struct SignalAttributes {
  std::string name;
  uint64_t multiplexerSwitchValue;
  uint64_t startBit;
  uint64_t bitSize;
  dbcppp::ISignal::EByteOrder byteOrder;
  std::string valueType;
  double factor;
  double offset;
  double minimum;
  double maximum;
  std::string unit;
};

// DBC structure to wrap individual DBC files.
struct DBC {
  using Id = uint64_t;
  using MessagePtr = std::unique_ptr<dbcppp::IMessage>;
  std::unordered_map<Id, MessagePtr> messages;
  std::unique_ptr<dbcppp::INetwork> network;

  // Constructor that loads and parses a DBC file.
  DBC(const std::string &path);
};

// DBCManager loads and stores multiple DBC objects, provides decoding
// functionality, and exposes signal attributes.
class DBCManager {
public:
  // Construct the manager from a map: interface name -> DBC file path.
  DBCManager(const std::unordered_map<std::string, std::string> &dbcPaths);

  // Decode a given message and write the results to the provided output stream.
  void decodeAndOutput(const std::string &interface, uint64_t id,
                       const std::vector<uint8_t> &data,
                       const std::string &time, std::ostream &out) const;

  // Retrieve the signal attributes for a given interface and CAN id.
  std::vector<SignalAttributes>
  getSignalAttributes(const std::string &interface, uint64_t id) const;

private:
  std::unordered_map<std::string, DBC> dbcs;
};

// LogProcessor encapsulates reading the log file, parsing entries,
// and storing the parsed log frames.
class LogProcessor {
public:
  // Construct the log processor with input/output streams and a reference to a
  // DBCManager.
  LogProcessor(std::istream &in, std::ostream &out,
               const DBCManager &dbcManager);

  // Process the log file line by line and store the parsed frames.
  void process();

  // Retrieve all parsed log frames.
  const std::vector<LogFrame> &getParsedFrames() const;

private:
  std::istream &in;
  std::ostream &out;
  const DBCManager &dbcManager;
  std::regex logPattern;
  std::vector<LogFrame> parsedFrames; // Container for parsed log entries.
};

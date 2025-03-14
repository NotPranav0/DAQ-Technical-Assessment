#include <cassert>
#include <cstdint>
#include <dbcppp/Network.h>
#include <fstream>
#include <iostream>
#include <regex>
#include <string>
#include <unordered_map>
#include <vector>

// DBC structure to wrap individual DBC files.
struct DBC {
  using Id = uint64_t;
  using MessagePtr = std::unique_ptr<dbcppp::IMessage>;
  std::unordered_map<Id, MessagePtr> messages;
  std::unique_ptr<dbcppp::INetwork> network;

  DBC(const std::string &path) {
    std::ifstream dbc_file(path);
    if (!dbc_file.is_open()) {
      throw std::runtime_error("Failed to open DBC file: " + path);
    }
    network = dbcppp::INetwork::LoadDBCFromIs(dbc_file);
    if (!network) {
      throw std::runtime_error("Failed to parse DBC file: " + path);
    }
    for (const dbcppp::IMessage &msg : network->Messages()) {
      messages[msg.Id()] = msg.Clone();
    }
  }
};

// DBCManager loads and stores multiple DBC objects and provides decoding
// functionality.
class DBCManager {
public:
  // Construct the manager from a map: interface name -> DBC file path.
  DBCManager(const std::unordered_map<std::string, std::string> &dbcPaths) {
    for (const auto &entry : dbcPaths) {
      dbcs.emplace(entry.first, DBC(entry.second));
    }
  }

  // Decode a given message and write the results to the provided output stream.
  void decodeAndOutput(const std::string &interface, uint64_t id,
                       const std::vector<uint8_t> &data,
                       const std::string &time, std::ostream &out) const {
    auto dbcIt = dbcs.find(interface);
    if (dbcIt == dbcs.end())
      return;

    const DBC &dbc = dbcIt->second;
    auto msgIt = dbc.messages.find(id);
    if (msgIt == dbc.messages.end())
      return;

    // Loop through all signals in the message.
    for (const auto &sig : msgIt->second->Signals()) {
      auto raw = sig.Decode(data.data());
      auto phys = sig.RawToPhys(raw);
      out << "(" << time << "): " << sig.Name() << ": " << phys << std::endl;
    }
  }

private:
  std::unordered_map<std::string, DBC> dbcs;
};

// LogProcessor encapsulates reading the log file and using DBCManager to decode
// entries.
class LogProcessor {
public:
  LogProcessor(std::istream &in, std::ostream &out,
               const DBCManager &dbcManager)
      : in(in), out(out), dbcManager(dbcManager),
        logPattern(R"(\((\d+\.\d+)\) (\w+) ([0-9A-Fa-f]+)#([0-9A-Fa-f]*))") {}

  // Process the log file line by line.
  void process() {
    std::string line;
    while (std::getline(in, line)) {
      std::smatch match;
      if (std::regex_search(line, match, logPattern)) {
        std::string time = match[1].str();
        std::string interface = match[2].str();
        uint64_t id = std::stoull(match[3].str(), nullptr, 16);
        std::string data_str = match[4].str();
        if (data_str.empty())
          continue;
        std::vector<uint8_t> data;
        for (size_t i = 0; i < data_str.size(); i += 2) {
          data.push_back(static_cast<uint8_t>(
              std::stoul(data_str.substr(i, 2), nullptr, 16)));
        }
        dbcManager.decodeAndOutput(interface, id, data, time, out);
      }
    }
  }

private:
  std::istream &in;
  std::ostream &out;
  const DBCManager &dbcManager;
  std::regex logPattern;
};

int main() {
  // Map of CAN interfaces to their corresponding DBC file paths.
  std::unordered_map<std::string, std::string> dbcPaths = {
      {"can0", "../../dbc-files/ControlBus.dbc"},
      {"can1", "../../dbc-files/SensorBus.dbc"},
      {"can2", "../../dbc-files/TractiveBus.dbc"}};

  DBCManager dbcManager(dbcPaths);

  std::ifstream logFile("../../dump.log");
  if (!logFile.is_open()) {
    std::cerr << "Failed to open log file" << std::endl;
    return 1;
  }

  std::ofstream outFile("../../output.txt");
  if (!outFile.is_open()) {
    std::cerr << "Failed to open output file" << std::endl;
    return 1;
  }

  LogProcessor processor(logFile, outFile, dbcManager);
  processor.process();

  return 0;
}

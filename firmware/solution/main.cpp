#include <cassert>
#include <cstdint>
#include <dbcppp/Network.h>
#include <fstream>
#include <iostream>
#include <regex>
#include <string>
#include <unordered_map>

struct DBC {
  using Id = uint64_t;
  using MessagePtr = std::unique_ptr<dbcppp::IMessage>;
  std::unordered_map<Id, MessagePtr> messages;
  std::unique_ptr<dbcppp::INetwork> network;
  DBC(std::string path) {
    std::ifstream dbc_file(path);
    network = dbcppp::INetwork::LoadDBCFromIs(dbc_file);
    for (const dbcppp::IMessage &msg : network->Messages()) {
      messages[msg.Id()] = msg.Clone();
    }
  }
};

auto main(void) -> int {
  std::unordered_map<std::string, DBC> dbcs;
  dbcs.emplace("can0", DBC("../../dbc-files/ControlBus.dbc"));
  dbcs.emplace("can1", DBC("../../dbc-files/SensorBus.dbc"));
  dbcs.emplace("can2", DBC("../../dbc-files/TractiveBus.dbc"));

  std::ifstream logFile("../../dump.small.log");
  if (!logFile.is_open()) {
    std::cerr << "Failed to open log file" << std::endl;
    return 1;
  }

  std::ofstream outFile("../../output.txt");
  if (!outFile.is_open()) {
    std::cerr << "Failed to open output file" << std::endl;
    return 1;
  }

  std::regex logPattern(
      R"(\((\d+\.\d+)\) (\w+) ([0-9A-Fa-f]+)#([0-9A-Fa-f]*))");
  std::string line;
  while (std::getline(logFile, line)) {
    std::smatch match;
    if (std::regex_search(line, match, logPattern)) {
      std::string time = match[1].str();
      std::string interface = match[2].str();
      uint64_t id = std::stoull(match[3].str(), nullptr, 16);
      // data is a string of hexadecimal values, need to convert to a byte array
      std::string data_str = match[4].str();
      if (data_str.empty()) {
        continue;
      }
      std::vector<uint8_t> data;
      for (size_t i = 0; i < data_str.size(); i += 2) {
        data.push_back(std::stoul(data_str.substr(i, 2), nullptr, 16));
      }

      auto dbc = dbcs.find(interface);
      if (dbc != dbcs.end()) {
        auto msg = dbc->second.messages.find(id);
        if (msg != dbc->second.messages.end()) {
          for (const auto &sig : msg->second->Signals()) {
            auto raw = sig.Decode(&data[0]);
            auto phys = sig.RawToPhys(raw);
            outFile << "(" << time << "): " << sig.Name() << ": " << phys
                    << std::endl;
          }
        }
      }
    }
  }
  return 0;
}
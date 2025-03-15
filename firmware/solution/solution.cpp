#include "solution.hpp"

// DBC implementation
DBC::DBC(const std::string &path) {
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

// DBCManager implementation
DBCManager::DBCManager(
    const std::unordered_map<std::string, std::string> &dbcPaths) {
  for (const auto &entry : dbcPaths) {
    dbcs.emplace(entry.first, DBC(entry.second));
  }
}

void DBCManager::decodeAndOutput(const std::string &interface, uint64_t id,
                                 const std::vector<uint8_t> &data,
                                 const std::string &time,
                                 std::ostream &out) const {
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

// LogProcessor implementation
LogProcessor::LogProcessor(std::istream &in, std::ostream &out,
                           const DBCManager &dbcManager)
    : in(in), out(out), dbcManager(dbcManager),
      logPattern(R"(\((\d+\.\d+)\) (\w+) ([0-9A-Fa-f]+)#([0-9A-Fa-f]*))") {}

void LogProcessor::process() {
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

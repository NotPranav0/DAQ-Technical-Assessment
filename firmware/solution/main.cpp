#include <dbcppp/Network.h>
#include <fstream>
#include <iostream>
#include <regex>
#include <string>

auto main(void) -> int {
  std::ifstream logFile("../../dump.small.log");
  if (!logFile.is_open()) {
    std::cerr << "Failed to open log file" << std::endl;
    return 1;
  }

  std::regex logPattern(
      R"(\((\d+\.\d+)\) (\w+) ([0-9A-Fa-f]+)#([0-9A-Fa-f]*))");
  std::string line;
  while (std::getline(logFile, line)) {
    std::smatch match;
    if (std::regex_search(line, match, logPattern)) {
      std::cout << "Time: " << match[1] << " Node: " << match[2]
                << " ID: " << match[3] << " Data: " << match[4] << std::endl;
    }
  }

  return 0;
}

// auto main(void) -> int {
//   std::ifstream dbc_file("../../dbc-files/SensorBus.dbc");
//   std::unique_ptr<dbcppp::INetwork> network =
//       dbcppp::INetwork::LoadDBCFromIs(dbc_file);

//   std::cout << "Network version: " << network->Version() << std::endl;

//   for (const dbcppp::IMessage &msg : network->Messages()) {
//     std::cout << "Message: " << msg.Name() << " (ID: " << msg.Id() << ")"
//               << std::endl;

//     for (const dbcppp::ISignal &sig : msg.Signals()) {
//       std::cout << "  Signal: " << sig.Name()
//                 << " Start bit: " << sig.StartBit()
//                 << " Size: " << sig.BitSize() << " Factor: " << sig.Factor()
//                 << " Offset: " << sig.Offset() << " Unit: " << sig.Unit()
//                 << std::endl;
//     }
//   }

//   return 0;
// }
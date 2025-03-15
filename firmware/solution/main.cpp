#include "solution.hpp"
#include <fstream>
#include <iostream>
#include <string>
#include <unordered_map>

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

#include <dbcppp/Network.h>
#include <fstream>
#include <iostream>

auto main(void) -> int {
  std::ifstream dbc_file("../../dbc-files/SensorBus.dbc");
  std::unique_ptr<dbcppp::INetwork> network =
      dbcppp::INetwork::LoadDBCFromIs(dbc_file);

  std::cout << "Network version: " << network->Version() << std::endl;

  for (const dbcppp::IMessage &msg : network->Messages()) {
    std::cout << "Message: " << msg.Name() << " (ID: " << msg.Id() << ")"
              << std::endl;

    for (const dbcppp::ISignal &sig : msg.Signals()) {
      std::cout << "  Signal: " << sig.Name()
                << " Start bit: " << sig.StartBit()
                << " Size: " << sig.BitSize() << " Factor: " << sig.Factor()
                << " Offset: " << sig.Offset() << " Unit: " << sig.Unit()
                << std::endl;
    }
  }

  return 0;
}
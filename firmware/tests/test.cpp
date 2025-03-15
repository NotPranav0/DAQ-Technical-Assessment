#include "solution.hpp"
#include <catch2/catch.hpp>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

// Helper function to load a file into a string
std::string loadFileContent(const std::string &filePath) {
  std::ifstream in(filePath);
  REQUIRE(in.is_open());
  std::stringstream buffer;
  buffer << in.rdbuf();
  return buffer.str();
}

TEST_CASE("LogProcessor parses valid log entries correctly", "[LogProcessor]") {
  // Load the test log file.
  std::string logContent = loadFileContent("../../tests/TestLog.log");
  std::istringstream logStream(logContent);
  std::ostringstream dummyOut;

  // Create a DBCManager with two DBC files.
  std::unordered_map<std::string, std::string> dbcPaths = {
      {"can0", "../../tests/TestDBC1.dbc"},
      {"can1", "../../tests/TestDBC2.dbc"}};
  DBCManager manager(dbcPaths);

  // Process the log file.
  LogProcessor processor(logStream, dummyOut, manager);
  processor.process();

  // Retrieve parsed log frames.
  const auto &frames = processor.getParsedFrames();

  // The log file contains 4 lines but two are invalid (empty data and invalid
  // log entry). Therefore, we expect exactly 2 valid parsed frames.
  REQUIRE(frames.size() == 2);

  // Verify first valid frame: should come from can0 with data "6400".
  const LogFrame &frame1 = frames[0];
  CHECK(frame1.time == "123.456");
  CHECK(frame1.interface == "can0");
  CHECK(frame1.id == 0x100);
  std::vector<uint8_t> expectedData1 = {0x64, 0x00};
  CHECK(frame1.data == expectedData1);

  // Verify second valid frame: from can1 with data "0A00".
  const LogFrame &frame2 = frames[1];
  CHECK(frame2.time == "124.000");
  CHECK(frame2.interface == "can1");
  CHECK(frame2.id == 0x100);
  std::vector<uint8_t> expectedData2 = {0x0A, 0x00};
  CHECK(frame2.data == expectedData2);
}

TEST_CASE("DBCManager extracts signal attributes correctly for can0",
          "[DBCManager]") {
  // Use the TestDBC1.dbc for can0.
  std::unordered_map<std::string, std::string> dbcPaths = {
      {"can0", "../../tests/TestDBC1.dbc"},
  };
  DBCManager manager(dbcPaths);

  // Retrieve signal attributes for message ID 0x100 (256 decimal) on can0.
  auto attributes = manager.getSignalAttributes("can0", 0x100);
  REQUIRE(!attributes.empty());
  // We expect one signal.
  REQUIRE(attributes.size() == 1);
  const SignalAttributes &attr = attributes[0];

  CHECK(attr.name == "Speed");
  CHECK(attr.startBit == 0);
  CHECK(attr.bitSize == 16);
  CHECK(attr.byteOrder == dbcppp::ISignal::EByteOrder::LittleEndian);
  CHECK(attr.factor == Approx(0.1));
  CHECK(attr.offset == Approx(0.0));
  CHECK(attr.minimum == Approx(0.0));
  CHECK(attr.maximum == Approx(250.0));
  CHECK(attr.unit == "km/h");
}

TEST_CASE("DBCManager extracts signal attributes correctly for can1",
          "[DBCManager]") {
  // Use the TestDBC2.dbc for can1.
  std::unordered_map<std::string, std::string> dbcPaths = {
      {"can1", "../../tests/TestDBC2.dbc"}};
  DBCManager manager(dbcPaths);

  auto attributes = manager.getSignalAttributes("can1", 0x100);
  REQUIRE(!attributes.empty());
  REQUIRE(attributes.size() == 1);
  const SignalAttributes &attr = attributes[0];

  CHECK(attr.name == "Temp");
  CHECK(attr.startBit == 0);
  CHECK(attr.bitSize == 16);
  CHECK(attr.byteOrder == dbcppp::ISignal::EByteOrder::LittleEndian);
  // Factor of 1 and offset of 0 for temperature.
  CHECK(attr.factor == Approx(1.0));
  CHECK(attr.offset == Approx(0.0));
  CHECK(attr.minimum == Approx(0.0));
  CHECK(attr.maximum == Approx(100.0));
  CHECK(attr.unit == "C");
}

TEST_CASE("LogProcessor handles invalid log entries gracefully",
          "[LogProcessor]") {
  // Provide a log input with only invalid lines.
  std::string invalidLog = "This is not a valid log entry\nAnother bad line\n";
  std::istringstream logStream(invalidLog);
  std::ostringstream dummyOut;

  // Even if we supply a valid DBC file map, there should be no parsed frames.
  std::unordered_map<std::string, std::string> dbcPaths = {
      {"can0", "../../tests/TestDBC1.dbc"},
  };
  DBCManager manager(dbcPaths);
  LogProcessor processor(logStream, dummyOut, manager);
  processor.process();

  const auto &frames = processor.getParsedFrames();
  CHECK(frames.empty());
}

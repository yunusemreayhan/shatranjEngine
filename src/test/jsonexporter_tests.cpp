
#include "gtest/gtest.h"

#include "json_game_exporter.h"

TEST(JsonExporterTests, Test1) {
    JsonExporter exporter("dummygameplay", "/home/yunus/Desktop", 8, 3);
    exporter.add_move({"fen1", "m1", 0}, {{}}, "test");
    exporter.add_move({"fen2", "m2", 0}, {{}, {}}, "test");
    exporter.add_move({"fen3", "m3", 0}, {{}, {}, {}}, "test");
    exporter.write();
}

#include "parser.hpp"

int main() {
    DownstreamConfig config(".config.csv");
    Parser parser(config);
    parser.writeLPFile(config.lpFilePath);
}

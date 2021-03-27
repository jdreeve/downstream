#ifndef SCHEDULE_WRITER_H
#define SCHEDULE_WRITER_H

#include "settings.hpp"
#include "edge.hpp"
#include "parser.hpp"

using namespace std;

class ScheduleWriter{
    public:
        ScheduleWriter(){
            parser = Parser();

        }

    private:
        Parser parser;
        vector<Edge> edges;
};

#endif

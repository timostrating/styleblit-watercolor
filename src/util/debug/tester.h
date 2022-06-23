#pragma once

#include <fstream>
#include <vector>

//struct FrameData
//{
//    double ms;
//};

class FrameTester
{
    std::ofstream   _outputStream;
    std::vector<double> _data;

    FrameTester() { }

public:
    static FrameTester& getInstance()
    {
        static FrameTester instance;
        return instance;
    }

    ~FrameTester()
    {
        std::cout << "\tWriting results of run to file." << std::endl;
        writeResultsToFile();
    }

    void addResult(double data)
    {
        _data.push_back(data);
    }

    void writeResultsToFile()
    {
        _outputStream.open("data.csv");
        _outputStream << "id,name\n";

        int index = 0;
        for (const double v : _data) {
            _outputStream << index++ << "," << v << "\n";
        }

        _outputStream.close();
    }
};


//#if FRAME_TESTING == 1
	#define ADD_TEST_RESULT(v) FrameTester::getInstance().addResult(v)
//#else
//	#define ADD_TEST_RESULT(v)
//#endif
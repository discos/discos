#include <iostream>
#include <cstdio>
#include <memory>
#include <stdexcept>
#include <array>
#include <boost/filesystem.hpp>

std::string exec(const std::string command)
{
    std::array<char, 128> buffer;
    std::string result;

    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(command.c_str(), "r"), pclose);
    if(!pipe)
    {
        throw std::runtime_error("popen() failed!");
    }
    while(fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr)
    {
        result += buffer.data();
    }
    return result;
}

int main()
{
    std::string binpath = boost::filesystem::canonical("/proc/self/exe").parent_path().string();

    std::string output_tab = exec(binpath + "/testSched");
    std::string output_spaces = exec(binpath + "/testSched schedule_ws.tmpl");
    if(output_tab != output_spaces)
    {
        std::cout << "testSched and testSchedSpaces have different outputs, FAILURE!" << std::endl;
        return -1;
    }
    std::cout << "testSched and testSchedSpaces have the same output, OK!" << std::endl;
    return 0;
}

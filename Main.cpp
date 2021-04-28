//
// Created by Yuri Grigorian on 27/04/2021.
//

#include "NumberWithUnits.hpp"
#include <sstream>
#include <fstream>
#include <cfloat>

using namespace ariel;

int main(){

    std::ofstream myfile;
    myfile.open ("myTestFile.txt");
    myfile << "1 km = 1000 m" << std::endl;
    myfile << "1 m = 100 cm" << std::endl;
    myfile << "1 kg = 1000 g" << std::endl;
    myfile << "1 ton = 1000 kg" << std::endl;
    myfile << "1 hour = 60 min" << std::endl;
    myfile << "1 min = 60 sec" << std::endl;
    myfile << "1 day = 24 hour" << std::endl;
    myfile << "1 month = 30 day" << std::endl;
    myfile << "1 year = 12 month" << std::endl;
    myfile.close();
    std::ifstream units_file{"myTestFile.txt"};
    NumberWithUnits::read_units(units_file);


    NumberWithUnits min1=NumberWithUnits(1,"min");
    NumberWithUnits min2=NumberWithUnits(2,"min");
    NumberWithUnits min4=NumberWithUnits(4,"min");
    NumberWithUnits min0=NumberWithUnits(0,"min");
    NumberWithUnits min_9=NumberWithUnits(-9,"min");
    NumberWithUnits hour1=NumberWithUnits(1,"hour");
    NumberWithUnits hour0=NumberWithUnits(0,"hour");
    NumberWithUnits hour_888=NumberWithUnits(-888,"hour");
    NumberWithUnits hour3=NumberWithUnits(3,"hour");
    NumberWithUnits hourMAX=NumberWithUnits(DBL_MAX,"hour");
    NumberWithUnits day1=NumberWithUnits(1,"day");
    NumberWithUnits day0=NumberWithUnits(0,"day");
    NumberWithUnits day_3=NumberWithUnits(-3,"day");
    NumberWithUnits day12=NumberWithUnits(12,"day");
    NumberWithUnits sec100=NumberWithUnits(100,"sec");
    NumberWithUnits sec0=NumberWithUnits(0,"sec");
    NumberWithUnits ton10=NumberWithUnits(10,"ton");
    NumberWithUnits kg1=NumberWithUnits(1,"kg");

    std::vector<std::string> path;
    std::unordered_map<std::string, int> temp;

    NumberWithUnits::get_path_to_unit(path, temp, "day", "sec");

    for(auto &s : path){
        std::cout << s << ",";
    }

    std::cout << std::endl;
}
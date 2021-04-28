//
// Created by Yuri Grigorian on 19/04/2021.
//

#ifndef NUMBER_WITH_UNITS_A_NUMBERWITHUNITS_HPP
#define NUMBER_WITH_UNITS_A_NUMBERWITHUNITS_HPP

#include <string>
#include <iostream>
#include <unordered_map>
#include <vector>

#define EPS_COMPARE 0.001

namespace ariel{

    class NumberWithUnits{
        std::string unit;
        double val;

        // Better to use a graph ds for this since we need cross type conversions.
        static std::unordered_map<std::string, std::unordered_map<std::string, double>> links;

        static void add_unit(const std::string &source_unit, const std::string &target_unit, double scale);

        public:

            static std::vector <std::string> &
            get_path_to_unit(std::vector <std::string> &path,
                             std::unordered_map<std::string, int> &temp,
                             const std::string &source_unit, const std::string &target_unit);

            static void ensure_unit_supported(const std::string &unit);

            static void read_units(std::ifstream &ifs);

            NumberWithUnits(double val, const std::string &unit);
            NumberWithUnits(int val, const std::string &unit);
            NumberWithUnits(const NumberWithUnits &source, const std::string &unit_type);

            double get_val() const;
            const std::string& get_unit() const;

            NumberWithUnits operator+(NumberWithUnits const &n2) const;
            NumberWithUnits operator+() const;


            NumberWithUnits operator-(NumberWithUnits const &n2) const;
            NumberWithUnits operator-() const;

            NumberWithUnits& operator+=(NumberWithUnits const &n2);
            NumberWithUnits& operator-=(NumberWithUnits const &n2);

            bool operator==(NumberWithUnits const &n2) const;
            bool operator!=(NumberWithUnits const &n2) const;

            bool operator<( NumberWithUnits const &n2) const;
            bool operator<=( NumberWithUnits const &n2) const;
            bool operator>( NumberWithUnits const &n2) const;
            bool operator>=( NumberWithUnits const &n2) const;

            NumberWithUnits& operator++();
            NumberWithUnits& operator--();
            friend NumberWithUnits operator++(NumberWithUnits const &n1, int _);
            friend NumberWithUnits operator--(NumberWithUnits const &n1, int _);

            friend NumberWithUnits operator* (double factor, NumberWithUnits const &n1);
            friend NumberWithUnits operator* (int factor, NumberWithUnits const &n1);
            friend NumberWithUnits operator* (NumberWithUnits const &n1, double factor );
            friend NumberWithUnits operator* (NumberWithUnits const &n1, int factor);

            friend std::ostream& operator<<( std::ostream &output, NumberWithUnits const &n1);
            friend std::istream& operator>>( std::istream &input, NumberWithUnits &n1);

    };

}
#endif //NUMBER_WITH_UNITS_A_NUMBERWITHUNITS_HPP

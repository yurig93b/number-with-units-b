//
// Created by Yuri Grigorian on 19/04/2021.
//

#include "NumberWithUnits.hpp"
#include <regex>
#include <string>
#include <iostream>
#include <tgmath.h>

#include <sstream>
#include <fstream>

using namespace ariel;

std::unordered_map <std::string, std::unordered_map<std::string, double>> NumberWithUnits::links = {};

void NumberWithUnits::ensure_unit_supported(const std::string &unit) {
    if (links.find(unit) == links.end()) {
        throw std::runtime_error("Unknown unit " + unit);
    }
}

NumberWithUnits::NumberWithUnits(double val, const std::string &unit) {
    NumberWithUnits::ensure_unit_supported(unit);
    this->unit = unit;
    this->val = val;
}

NumberWithUnits::NumberWithUnits(int val, const std::string &unit) {
    NumberWithUnits::ensure_unit_supported(unit);
    this->unit = unit;
    this->val = (double) val;
}


// Non optimal path finder, using graph is better.
std::vector <std::string> &
NumberWithUnits::get_path_to_unit(std::vector <std::string> &path,
                                  std::unordered_map<std::string, int> &temp,
                                  const std::string &source_unit, const std::string &target_unit) {

    temp.insert({source_unit, 0});

    std::unordered_map < std::string, std::unordered_map < std::string, double >> ::iterator it;
    it = NumberWithUnits::links.find(source_unit);

    if (it != NumberWithUnits::links.end()) {
        auto &units = it->second;

        // Found target conversion unit
        std::unordered_map<std::string, double>::iterator inner_it = units.find(target_unit);
        if (inner_it != units.end()) {
            path.push_back(target_unit);
            path.push_back(source_unit);
            return path;
        }

        // Traverse child units
        for (auto const &inner_unit : units) {
            // If not seen this unit, visit it.
            if (temp.find(inner_unit.first) == temp.end()) {
                NumberWithUnits::get_path_to_unit(path, temp, inner_unit.first, target_unit);
                if (!path.empty()) {
                    path.push_back(source_unit);
                    break;
                }
            }
        }
    }
    return path;
}

NumberWithUnits::NumberWithUnits(const NumberWithUnits &source, const std::string &unit_type) {
    // If convert to self
    if (source.get_unit() == unit_type) {
        this->val = source.get_val();
        this->unit = source.get_unit();
        return;
    }

    std::unordered_map < std::string, std::unordered_map < std::string, double >> ::iterator
    it;
    // Try to find direct conversion
    it = links.find(source.get_unit());
    if (it == links.end()) {
        throw std::runtime_error("Unknown conversion from type.");
    }

    std::unordered_map<std::string, double>::iterator inner_it;
    inner_it = it->second.find(unit_type);
    if (inner_it != it->second.end()) {
        this->unit = unit_type;
        this->val = source.get_val() * inner_it->second;
        return;
    }

    // Find indirect path to unit
    std::vector <std::string> path{};
    std::unordered_map<std::string, int> temp{};

    NumberWithUnits::get_path_to_unit(path, temp, source.get_unit(), unit_type);

    if (path.empty()) {
        throw std::runtime_error("No path from " + source.get_unit() + " to " + unit_type);
    }

    std::reverse(path.begin(), path.end());
    NumberWithUnits temp_unit(source, source.get_unit());
    for (auto &next_unit_type: path) {
        temp_unit = NumberWithUnits(temp_unit, next_unit_type);
    }

    this->val = temp_unit.get_val();
    this->unit = temp_unit.get_unit();
}

const std::string &NumberWithUnits::get_unit() const {
    return this->unit;
}

double NumberWithUnits::get_val() const {
    return this->val;
}

void NumberWithUnits::add_unit(const std::string &source_unit, const std::string &target_unit, double scale) {
    if (links.find(source_unit) == links.end()) {
        links.insert({source_unit, std::unordered_map<std::string, double>()});
    }

    links[source_unit].insert({target_unit, scale});
}


// Read units from file line by line, uses regex to extract the fields.
void NumberWithUnits::read_units(std::ifstream &ifs) {
    std::string line;
    const std::regex base_regex("^1\\s*(\\w+)\\s*=\\s*(\\d*?\\.?\\d+?)\\s*(\\w+)\\s*$");

    while (std::getline(ifs, line)) {
        std::smatch base_match;

        std::string source_unit;
        std::string target_unit;
        double factor = 0;

        if (std::regex_match(line, base_match, base_regex)) {
            source_unit = base_match[1].str();
            target_unit = base_match[3].str();
            factor = std::stod(base_match[2].str());

            NumberWithUnits::add_unit(source_unit, target_unit, factor);
            NumberWithUnits::add_unit(target_unit, source_unit, 1 / factor);
        } else {
            throw std::runtime_error(std::string("Malformed unit line ") + line);
        }

    }
}

NumberWithUnits NumberWithUnits::operator+(NumberWithUnits const &n2) const {
    NumberWithUnits n2_converted = NumberWithUnits(n2, this->get_unit());
    return NumberWithUnits(this->get_val() + n2_converted.get_val(), this->get_unit());
}

NumberWithUnits NumberWithUnits::operator+() const {
    return NumberWithUnits(this->get_val(), this->get_unit());
}

NumberWithUnits &NumberWithUnits::operator+=(NumberWithUnits const &n2) {
    NumberWithUnits n2_converted = NumberWithUnits(n2, this->get_unit());
    this->val += n2_converted.get_val();
    return *this;
}

NumberWithUnits NumberWithUnits::operator-(NumberWithUnits const &n2) const {
    NumberWithUnits n2_converted = NumberWithUnits(n2, this->get_unit());
    return NumberWithUnits(this->get_val() - n2_converted.get_val(), this->get_unit());
}

NumberWithUnits NumberWithUnits::operator-() const {
    return NumberWithUnits(-this->get_val(), this->get_unit());
}

NumberWithUnits &NumberWithUnits::operator-=(NumberWithUnits const &n2) {
    NumberWithUnits n2_converted = NumberWithUnits(n2, this->get_unit());

    this->val -= n2_converted.get_val();
    return *this;
}

bool NumberWithUnits::operator==(NumberWithUnits const &n2) const {
    NumberWithUnits n2_converted = NumberWithUnits(n2, this->get_unit());
    return this->get_unit() == n2_converted.get_unit() && fabs(this->get_val() - n2_converted.get_val()) < EPS_COMPARE;
}

bool NumberWithUnits::operator!=(NumberWithUnits const &n2) const {
    NumberWithUnits n2_converted = NumberWithUnits(n2, this->get_unit());
    return this->get_unit() != n2_converted.get_unit() || this->get_val() != n2_converted.get_val();
}

bool NumberWithUnits::operator<(NumberWithUnits const &n2) const {
    NumberWithUnits n2_converted = NumberWithUnits(n2, this->get_unit());
    return this->get_val() < n2_converted.get_val();
}

bool NumberWithUnits::operator<=(NumberWithUnits const &n2) const {
    NumberWithUnits n2_converted = NumberWithUnits(n2, this->get_unit());
    return this->get_val() <= n2_converted.get_val();
}

bool NumberWithUnits::operator>(NumberWithUnits const &n2) const {
    NumberWithUnits n2_converted = NumberWithUnits(n2, this->get_unit());
    return this->get_val() > n2_converted.get_val();
}

bool NumberWithUnits::operator>=(NumberWithUnits const &n2) const {
    NumberWithUnits n2_converted = NumberWithUnits(n2, this->get_unit());
    return this->get_val() >= n2_converted.get_val();
}

NumberWithUnits &NumberWithUnits::operator++() {
    ++(this->val);
    return *this;
}

NumberWithUnits &NumberWithUnits::operator--() {
    --(this->val);
    return *this;
}

NumberWithUnits ariel::operator++(NumberWithUnits const &n1, int _) {
    // For some reason the compiler passes n1 as const. need to mutate it to non const (the only way i found working).
    NumberWithUnits &mod_n1 = const_cast<NumberWithUnits &>(n1); // NOLINT
    return NumberWithUnits(mod_n1.val++, mod_n1.get_unit());
}

NumberWithUnits ariel::operator--(NumberWithUnits const &n1, int _) {
    // For some reason the compiler passes n1 as const. need to mutate it to non const (the only way i found working).
    NumberWithUnits &mod_n1 = const_cast<NumberWithUnits &>(n1); // NOLINT
    return NumberWithUnits(mod_n1.val--, mod_n1.get_unit());
}


NumberWithUnits ariel::operator*(double factor, NumberWithUnits const &n1) {
    return NumberWithUnits(n1.get_val() * factor, n1.get_unit());
}

NumberWithUnits ariel::operator*(int factor, NumberWithUnits const &n1) {
    return NumberWithUnits(n1.get_val() * factor, n1.get_unit());
}

NumberWithUnits ariel::operator*(NumberWithUnits const &n1, double factor) {
    return NumberWithUnits(n1.get_val() * factor, n1.get_unit());
}

NumberWithUnits ariel::operator*(NumberWithUnits const &n1, int factor) {
    return NumberWithUnits(n1.get_val() * factor, n1.get_unit());
}

std::ostream &ariel::operator<<(std::ostream &output, NumberWithUnits const &n1) {
    output << n1.get_val() << "[" << n1.get_unit() << "]";
    return output;
}

std::istream &ariel::operator>>(std::istream &input, NumberWithUnits &n1) {
    std::string entry;

    char c = '\0';
    while (input.get(c)) {
        entry += c;
        if (c == ']') { break; }
    }

    const std::regex base_regex("\\s*([-+]?\\d*?\\.?\\d+?)\\s*\\[?\\s*(\\w+)\\s*]\\s*");

    std::string source_unit;
    double val = 0;
    std::smatch base_match;

    if (std::regex_match(entry, base_match, base_regex)) {
        val = std::stod(base_match[1].str());
        source_unit = base_match[2].str();

        NumberWithUnits::ensure_unit_supported(source_unit);
        n1.val = val;
        n1.unit = source_unit;
    } else {
        throw std::runtime_error(std::string("Malformed unit entry ") + entry);
    }

    return input;
}


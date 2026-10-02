//-----------------------------------------------------------------------------
// zx-81 decompile
// Copyright (C) Paulo Custodio, 2023-2026
// License: The Artistic License 2.0, http://www.perlfoundation.org/artistic_license_2_0
//-----------------------------------------------------------------------------

#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <utility>

class Labels {
public:
    explicit Labels();
    void clear();

    void add(const std::string& name, int value);
    int get(const std::string& name) const;

    // get existing or create new label for value
    std::string get_add(int value);

    bool find(const std::string& name, int& out_value) const;
    bool find(int value, std::string& out_name) const;

    std::vector<std::pair<std::string, int>> sorted_by_name() const;
    std::vector<std::pair<std::string, int>> sorted_by_value() const;

private:
    std::unordered_map<std::string, int> by_name;
    std::unordered_map<int, std::string> by_value;

    std::vector<std::pair<std::string, int>> all_unsorted() const;
    virtual void init() = 0;
    virtual std::string generate_label(int value) const = 0;
};

class AsmLabels : public Labels {
public:
    explicit AsmLabels();

private:
    void init() override;
    std::string generate_label(int value) const override;
};

class BasicLabels : public Labels {
public:
    explicit BasicLabels();

private:
    void init() override;
    std::string generate_label(int value) const override;
};

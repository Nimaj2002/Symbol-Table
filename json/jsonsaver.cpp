#include "jsonsaver.hpp"

using json = nlohmann::json;
ifstream outputFile;
string fileName = "data.json";

void tableToJson(Env *top)
{
    json j;

    std::ifstream inputFile(fileName); // Open the existing file
    if (inputFile.good())
    {
        try
        {
            inputFile >> j; // Read existing JSON data from the file
            inputFile.close();
        }
        catch (json::parse_error &e)
        {
            std::ofstream file(fileName, std::ios::trunc);
            file.close();
        }
    }

    // string bl = to_string(prev->block);
    if ((top->table.empty() != true))
    {
        for (auto pair : top->table)
        {
            if ((top->prev == nullptr))
            {
                j["0"][pair.first] = pair.second;
                j["0"]["prev"] = "null";
            }
            else
            {
                j[to_string(top->block)][pair.first] = pair.second;
                j[to_string(top->block)]["prev"] = to_string(top->prev->block);
            }
        }
    }
    else
    {
        return;
    }

    // Write updated JSON back to the file
    std::ofstream outputFile(fileName);
    if (outputFile.is_open())
    {
        outputFile << j.dump(4); // Write JSON to the file with indentation of 4 spaces
        outputFile.close();
    }
    else
    {
        cout << "ERROR";
    }
}
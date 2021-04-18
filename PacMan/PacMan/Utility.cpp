#include "Utility.h"
#include <algorithm>
#include <string>
#include <fstream>
#include <iostream>
#include <conio.h>

using namespace std;

//constructors
Utility::Utility() {};

//methods
string Utility::TransformString(const string& text, const int operation)
{
    string temp = text;
    switch (operation)
    {
    case 0: // to upper
        transform(temp.begin(), temp.end(), temp.begin(), ::toupper);
        return temp;
    case 1: // to lower
        transform(temp.begin(), temp.end(), temp.begin(), ::tolower);
        return temp;
    }
    return "";
}

void Utility::ReplaceString(string& text, const string from, const char to)
{
    size_t start_pos = 0;
    string to_string = { to };
    while ((start_pos = text.find(from, start_pos)) != std::string::npos) {
        text.replace(start_pos, from.length(), to_string);
        start_pos += to_string.length(); // ...
    }
}

void Utility::ReplaceString(string& text, const string from, const string to)
{
    size_t start_pos = 0;
    while ((start_pos = text.find(from, start_pos)) != std::string::npos) {
        text.replace(start_pos, from.length(), to);
        start_pos += to.length(); // ...
    }
}

void Utility::ReplaceString(string& text, const char from, const char to)
{
    size_t start_pos = 0;
    string to_string = { to };
    while ((start_pos = text.find(from, start_pos)) != std::string::npos) {
        text.replace(start_pos, 1, to_string);
        start_pos += to_string.length(); // ...
    }
}

string Utility::Spacer(const string& format, const int block_width)
{
    // used to center the "format" string -> cout the return on either side of the string to print
    int spacer = ((block_width - (int)format.size()) / 2);
    string spaces = "";
    for (int i = 0; i < spacer; i++)
    {
        spaces = spaces + " ";
    }
    return spaces;
}

string Utility::GetMenuFromFile(const string& file_name)
{
    ifstream file(file_name);
    string file_line, marker, content = "false";
    bool process_lines = true;
    int number_options = 0;

    if (file) {
        // file exists and is open 
        content = "";
        while (getline(file, file_line))
        {
            int this_scene;

            marker = "#scene:";
            if (file_line.find(Utility::TransformString(marker, 1), 0) != std::string::npos)
            {

                // set the scene number
                this_scene = stoi(file_line.substr(Utility::TransformString(marker, 1).size(), 2));

                if (this_scene != 0) {

                    content += file_line.substr(Utility::TransformString(marker, 1).size(), (file_line.size() - marker.size())) + "*";

                    marker = "title:";
                    getline(file, file_line);
                    if (file_line.find(Utility::TransformString(marker, 1), 0) != std::string::npos) {
                        content += file_line.substr(Utility::TransformString(marker, 1).size(), (file_line.size() - marker.size())) + "\n";
                        number_options++;
                    }

                }
            }

        }
    }
    file.close();

    // returns a new line seperated string with the menu option, adding the number of options at the head
    return to_string(number_options) + "\n" + content;
}

string Utility::GetTemplateFromFile(const string& file_name)
{
    ifstream file(file_name);
    string file_line, marker, content = "false";
    bool process_lines = true;

    if (file) {
        content = "";
        string start_marker = "#scene:0";
        string end_marker = "#end_scene";

        while (getline(file, file_line))
        {
            if (file_line.find(Utility::TransformString(start_marker, 1), 0) != std::string::npos)
            {
                while (getline(file, file_line))
                {
                    if (file_line.find(Utility::TransformString(end_marker, 1), 0) != std::string::npos)
                    {
                        break;
                    }
                    else
                    {
                        content += file_line + '\n';
                    }
                }
            }
        }
    }
    file.close();

    return content;

}

template <typename Type, bool number>
Type Utility::ProcessOption(string label)
{
    if (number)
    {
        do
        {
            int int_input;
            cout << endl;
            cout << label;
            cin >> int_input;

            if (cin.fail()) {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << "  You need to enter a vlaid number!" << endl;
                cout << "  Press a any key to continue.";
                int input = _getch();
                system("cls");
            }
            else
            {
                return int_input;
            }

        } while (true);
    }
    else
    {
        do
        {
            string string_input;
            cout << endl;
            cout << label;
            getline(cin, string_input);

            if (string_input.length() < 1) {
                cout << "  You need to enter valid valid text!" << endl;
                cout << "  Press a any key to continue.";
                int input = _getch();
                system("cls");
            }
            else
            {
                return string_input;
            }

        } while (true);
    }
    
    return 0;
    
}
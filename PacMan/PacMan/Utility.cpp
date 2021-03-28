#include "Utility.h"
#include <algorithm>
#include <string>
#include <fstream>


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
//string Utility::LoadSceneFromFile(string filename, int scene_to_load)
//{
//    Utility utility; // access to utilities
//
//    ifstream file(filename);
//    string file_line, marker, content = "";
//    bool process_lines = true;
//
//    if (file) {
//         file exists and is open 
//        while (getline(file, file_line))
//        {
//
//            check for right scene
//            marker = "#scene:";
//            if (file_line.find(Utility::TransformString(marker, 1), 0) != std::string::npos) { // if line is scene identifier
//
//                if (scene_to_load == stoi(file_line.substr(Utility::TransformString(marker, 1).size(), 2))) // and if the sence is the one I'm looking for
//                {
//                    this is the scene we are looking for so let's process it until your get to the end of the level info
//                    while (getline(file, file_line))
//                    {
//                         get the next line and set the relevant info in the level info
//
//                        marker = "title:";
//                        if (file_line.find(Utility::TransformString(marker, 1), 0) != std::string::npos) {
//                            content += file_line + "\n";
//                            continue;
//                        }
//
//                        marker = "pellet_points:";
//                        if (file_line.find(Utility::TransformString(marker, 1), 0) != std::string::npos) {
//                            content += file_line + "\n";
//                            continue;
//                        }
//
//                        marker = "ghost_points:";
//                        if (file_line.find(Utility::TransformString(marker, 1), 0) != std::string::npos) {
//                            content += file_line + "\n";
//                            continue;
//                        }
//
//                        marker = "all_ghosts_bonus:";
//                        if (file_line.find(Utility::TransformString(marker, 1), 0) != std::string::npos) {
//                            content += file_line + "\n";
//                            continue;
//                        }
//
//                        marker = "edible_ghost duration:";
//                        if (file_line.find(Utility::TransformString(marker, 1), 0) != std::string::npos) {
//                            content += file_line + "\n";
//                            continue;
//                        }
//
//                        marker = "chase_duration:";
//                        if (file_line.find(Utility::TransformString(marker, 1), 0) != std::string::npos) {
//                            content += file_line + "\n";
//                            continue;
//                        }
//
//                        marker = "run_duration:";
//                        if (file_line.find(Utility::TransformString(marker, 1), 0) != std::string::npos) {
//                            content += file_line + "\n";
//                            continue;
//                        }
//
//                        marker = "roam_duration:";
//                        if (file_line.find(Utility::TransformString(marker, 1), 0) != std::string::npos) {
//                            content += file_line + "\n";
//                            continue;
//                        }
//
//                        marker = "roam_count:";
//                        if (file_line.find(Utility::TransformString(marker, 1), 0) != std::string::npos) {
//                            content += file_line + "\n";
//                            continue;
//                        }
//
//                        marker = "level_map:";
//                        if (file_line.find(Utility::TransformString(marker, 1), 0) != std::string::npos) {
//                             get map info into the map string ending when you reach end of map
//                            marker = "#end_scene";
//                            while (getline(file, file_line))
//                            {
//                                if (file_line.find(Utility::TransformString(marker, 1), 0) == std::string::npos)
//                                    content += file_line;
//                                else
//                                    break;
//                            }
//                        }
//
//                        marker = "#scene:"; // check to see if reached the next scene;
//                        if (file_line.find(Utility::TransformString(marker, 1), 0) != std::string::npos) {
//                            break;
//                        }
//
//                    }
//                }
//                process_lines = false;
//            }
//        }
//        file.close();
//    }
//
//    return (content);
//}
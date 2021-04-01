#pragma once
#include "Stage.h"
#include "EnumsAndStatics.h"

using namespace std;

class Scene : public Stage
{
    class Editor* p_editor = nullptr; // fwd declare class
    int top_left_modifier = 5;

public:
    // level map and scene
    int this_scene = 0;
    char** p_map = nullptr;
    string** p_options = nullptr;
    string scene_errors = "";
    int error_count = 0;
    bool resize_scene = false;

	//constructors
	Scene();

    //constructors
    ~Scene();

	// methods
    void CreateLevelScene(int& current_scene);
    string LoadSceneFromFile(string filename, int scene_to_load);
    Coord MapSize(const string& map);
    void DrawLevel();
    void ShowKey();
    string** GetSceneValues();
    void DeleteOptions();
    bool PenIsValid(char pen);
    bool SaveToFile();
    string CreatesceneString(int scene);
    bool ValiditeScene();
    bool HasNoDeadEnd(int row, int col);
    bool HasOuterWalls(int row, int col);
    void ResizeScene();
    string ProcessTextOption(string label);
    int ProcessNumberOption(string label);
    void ResizeMap(int new_rows, int new_cols);

    // setters
    void SetEditor(Editor* p_editor);

};
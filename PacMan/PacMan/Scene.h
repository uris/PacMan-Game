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
    bool cols_from_left = true;
    bool rows_fom_bottom = true;
    bool add_cols = false;
    bool add_rows = false;
    bool remove_cols = false;
    bool remove_rows = false;

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
    int AddRemoveRows(bool add);
    int AddRemoveColumns(bool add);
    void DeallocateMapArray();
    bool AlPelletsReacheable();
    void SetPositionToAccessible(Coord new_position, char** temp);

    // setters
    void SetEditor(Editor* p_editor);

};
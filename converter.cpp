#include <iostream>
#include <Windows.h>
#include <stdbool.h>
#include <filesystem>
#include <algorithm>
#include <string>

#include "./utils.h"

#include <SketchUpAPI/slapi.h>
#include <SketchUpAPI/geometry.h>
#include <SketchUpAPI/initialize.h>
#include <SketchUpAPI/unicodestring.h>
#include <SketchUpAPI/model/model.h>
#include <SketchUpAPI/model/entities.h>
#include <SketchUpAPI/model/face.h>
#include <SketchUpAPI/model/edge.h>
#include <SketchUpAPI/model/vertex.h>
#include <SketchUpAPI/model/material.h>
#include <SketchUpAPI/model/scene.h>
#include <vector>

using namespace std;

class CSUString {
 public:
  CSUString() {
    SUSetInvalid(su_str_);
    SUStringCreate(&su_str_);
  }

  ~CSUString() {
    SUStringRelease(&su_str_);
  }

  operator SUStringRef*() {
    return &su_str_;
  }

  string utf8() {
    size_t length;
    SUStringGetUTF8Length(su_str_, &length);
    string string;
    string.resize(length + 1);
    size_t returned_length;
    SUStringGetUTF8(su_str_, length, &string[0], &returned_length);
    return string;
  }

 private:
  // Disallow copying for simplicity
  CSUString(const CSUString& copy);
  CSUString& operator=(const CSUString& copy);

  SUStringRef su_str_;
};

bool CreateDirectory(const string& path) {
    if (::CreateDirectoryA(path.c_str(), nullptr) || ::GetLastError() == ERROR_ALREADY_EXISTS) {
        return true;
    }
    return false;
}

string GetFileName(const string& filePath) {
    // 使用 filesystem::path 解析文件路径
    filesystem::path path(filePath);
    // 使用 filesystem::path::stem() 获取文件名称（不包括扩展名）
    string fileName = path.stem().string();
    return fileName;
}

// Utility function to get a material's name
static string GetMaterialName(SUMaterialRef material) {
  CSUString name;
  SU_CALL(SUMaterialGetName(material, name));
  return name.utf8();
}

int ExportSceneMaterials(SUModelRef model, string directory_path) {
  size_t count = 0;
  SU_CALL(SUModelGetNumMaterials(model, &count));
  vector<SUMaterialRef> materials(count);
  SU_CALL(SUModelGetMaterials(model, count, &materials[0], &count));
  cout << "材质数量：" << count << endl;
  for (size_t i = 0; i < count; i++) {
    SUMaterialRef material = materials[i];
    string material_name = GetMaterialName(material);
    material_name.erase(remove(material_name.begin(), material_name.end(), '\0'), material_name.end());
    string path_name = directory_path + "/" + material_name.append(".skm");
    cout << "序号：" << i << " 文件地址：" << path_name << endl;
    SU_CALL(SUMaterialWriteToFile(material, path_name.c_str()));
  }
  return 0;
}

// Utility function to get a scene's name
static string GetSceneName(SUSceneRef scene) {
  CSUString name;
  SU_CALL(SUSceneGetName(scene, name));
  return name.utf8();
}

int ExportSceneMaterialJSON(SUModelRef model, string folder_str) {
  size_t num_scenes = 0;
  SU_CALL(SUModelGetNumScenes(model, &num_scenes));
  if (num_scenes > 0) {
    string name_str;
    vector<SUSceneRef> scenes(num_scenes);
    SU_CALL(SUModelGetScenes(model, num_scenes, &scenes[0], &num_scenes));
    for (size_t i = 0; i < num_scenes; ++i) {
      SUSceneRef scene = scenes[i];
      string scene_name = GetSceneName(scene);
      cout << "序号：" << i << " 场景名称：" << scene_name << endl;
      size_t hidden_count = 0;
      SUCameraRef camera = SU_INVALID;
      SU_CALL(SUSceneGetCamera(scene, &camera));
      SU_CALL(SUSceneGetNumHiddenEntities(scene, &hidden_count));
      cout << "序号：" << i << " 隐藏数量：" << hidden_count << endl;
    }
  }
  

  // SUEntitiesRef entities;
  // SUModelGetEntities(model, &entities);
  return 0;
}

int main(int argc, char* argv[]) {
  // 打印参数数量
  SetConsoleOutputCP(CP_UTF8);

  if (argv[1] == nullptr) {
    cout << "Failed creating model from a file, Please provide the model path follow the command" << endl;
    return 1;
  }

  // Always initialize the API before using it
  SUInitialize();

  filesystem::path path(argv[0]);
  filesystem::path directory = path.parent_path();

  string file_path = argv[1];
  string file_name = GetFileName(file_path);
  string output_root = directory.string() + "/output";
  string output_folder = output_root + "/" + file_name;
  string output_material = output_folder + "/material";
  // string folder_path = "/material";
  // string directory_path = "./output/" + file_name + folder_path;
  if (CreateDirectory(output_root)) {
      cout << "Root path created successfully." << endl;
  } else {
      cout << "Failed to create root path." << endl;
  }
  if (CreateDirectory(output_folder)) {
      cout << "Folder path created successfully." << endl;
  } else {
      cout << "Failed to create folder path." << endl;
  }
  if (CreateDirectory(output_material)) {
      cout << "Material path created successfully." << endl;
  } else {
      cout << "Failed to create material path." << endl;
  }

  // 从文件里加载model
  SUModelRef model = SU_INVALID;
  SUModelLoadStatus status;
  SUResult res = SUModelCreateFromFileWithStatus(&model, argv[1], &status);
  
  if (res != SU_ERROR_NONE) {
    cout << "Failed creating model from a file" << endl;
    return 1;
  }
  if (status == SUModelLoadStatus_Success_MoreRecent) {
    cout
        << "This model was created in a more recent SketchUp version than that of the SDK. "
           "It contains data which will not be read. Saving the model over the original file may "
           "lead to permanent data loss."
        << endl;
  }

  // 导出场景中的skm材质文件
  ExportSceneMaterials(model, output_material);
  // 导出场景的材质json
  // ExportSceneMaterialJSON(model, output_material);
  // Must release the model or there will be memory leaks
  SUModelRelease(&model);
  // Always terminate the API when done using it
  SUTerminate();
  return 0;
}
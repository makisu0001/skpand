
#include <iostream>
#include <windows.h>
#include <io.h>
#include <fcntl.h>
#include <locale>
#include <codecvt>
#include <string>
#include <filesystem>

using namespace std;

int main(int argc, char* argv[]) {
    if (argv[1] == nullptr) {
        cout << "Please input skm file path" << endl;
        return 1;
    }

    // filesystem::path cwd = std::filesystem::current_path();
    filesystem::path path(argv[0]);
    filesystem::path directory = path.parent_path();

    // 带有空格路径和参数的程序
    // std::string exePath = "\"\"D:\\Work Space\\skip\\main.exe\"";
    // std::string pythonPath = "\"\"D:\\Work Space\\skip\\render\\blender.exe\" -b -P \"D:\\Work Space\\skip\\main.py\" -- ";
    // std::string programArgs = "\"D:\\SKP Files\\shafa.skp\"\"";
    string exePath = "\"\"" + directory.string() + "\\main.exe\"";
    string pythonPath = "\"\"" + directory.string() + "\\render\\blender.exe\" -b -P " + "\"" + directory.string() + "\\main.py\" -- ";
    string programArgs = "\"" + string(argv[1]) + "\"\"";

    // 构造完整的命令
    string exeCommand = "cmd /c " + exePath + " " + programArgs;
    string pythonCommand = "cmd /c " + pythonPath + " " + programArgs;

    cout<<exeCommand<<endl;
    // 执行命令
    system(exeCommand.c_str());
    system(pythonCommand.c_str());

    return 0;
}
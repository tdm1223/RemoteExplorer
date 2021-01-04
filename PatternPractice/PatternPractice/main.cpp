#include <iostream>
#include <vector>
#include <string>
#include<map>
#include "Action.h"

using namespace std;

void readScript(const string fileName, map<char, Action*>& key_binding)
{
    FILE* script_file = nullptr;
    script_file = fopen(fileName.c_str(), "r");

    int num_bindings = 0;
    fscanf(script_file, "%d", &num_bindings);

    for (int i = 0; i < num_bindings; i++)
    {
        char cmd_buffer[255];
        char key_script;
        fscanf(script_file, "%s %c", cmd_buffer, &key_script);
        cout << cmd_buffer << " " << key_script << endl;

        key_binding[key_script] = GetActionPtr(string(cmd_buffer));
    }

    fclose(script_file);
}

int main()
{
    map<char, Action*> key_binding;

    readScript("key_binding_script.txt", key_binding);

    while (true)
    {
        char input_key;
        cin >> input_key;

        if (key_binding.count(input_key) > 0)
        {
            key_binding[input_key]->Execute();
        }
    }
    return 0;
}
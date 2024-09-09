# These are optional setup files to place inside a .vscode folder

## c_cpp_properties.json
```json
{
  "configurations": [
    {
      "name": "Architools",
      "includePath": [
        "${workspaceFolder}/**",
        "C:/Users/<user>/devkitBase/include/**"
      ],
      "defines": [],
      "cStandard": "c11",
      "cppStandard": "c++20",
      "intelliSenseMode": "windows-msvc-x64"
    }
  ],
  "version": 4
}
```

## settings.json
```json
{
  "C_Cpp.errorSquiggles": "disabled"
}
```
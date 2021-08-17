# Changelog
## Modification (2021.08.11)
### About source code
1. Add a WSRequestHandler_AverMediaParseJson.cpp to handle custom request "AverMedia_Parse_Json"
2. Change the settings-dialog.ui serverPort default value
    > 4444 -> 24172
3. Change Config::ServerPort default value
    > 4444 -> 24172
4. Change macro
    > #define SECTION_NAME "WebsocketAPI"
    -> #define SECTION_NAME "WebsocketAPI_AVT"
    
    > #define blog(level, msg, ...) blog(level, "[obs-websocket] " msg, ##__VA_ARGS__) 
    -> #define blog(level, msg, ...) blog(level, "[obs-websocket-avt] " msg, ##__VA_ARGS__)
6. Change default locale
    > OBS_MODULE_USE_DEFAULT_LOCALE("obs-websocket", "en-US") -> OBS_MODULE_USE_DEFAULT_LOCALE("obs-websocket-avt", "en-US")

### About CMakeLists
1. Change project name
    > "obs-websocket" -> "obs-websocket-avt"
2. Add WSRequestHandler_AverMediaParseJson.cpp to cmake variable obs-websocket_SOURCES

### About translation
Change all string in *.ini
> "Websocket" -> "AVerMedia Websocket" 
---
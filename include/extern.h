#include "entity.h"
#include "global.h"

extern std::unique_ptr<GlobalClass> Global;
extern std::unique_ptr<EditorClass> Editor;
extern std::unique_ptr<SettingsClass> Settings;
extern Inputs* P1Inputs;
extern Entity* Camera;
extern ZipData* LoadedData;
extern Uint64 lastTime;
extern Uint64 currentTime;
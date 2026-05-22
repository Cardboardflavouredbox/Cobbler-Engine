#include "entity.h"
#include "global.h"

extern std::unique_ptr<GlobalClass> Global;
extern std::unique_ptr<EditorClass> Editor;
extern std::unique_ptr<SettingsClass> Settings;
extern Inputs* P1Inputs;
extern std::shared_ptr<Entity> Camera;
extern Uint64 lastTime;
extern Uint64 currentTime;
extern std::unordered_map<std::string, void (*)()> SpawnEntities;
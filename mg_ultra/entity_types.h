#ifndef __ENTITY_TYPES__
#define __ENTITY_TYPES__

using namespace std;

#include <string>

enum EntityTypes {
	ETNoType,
	ETGeneric,
	ETGameControl,
	ETConsole,
	ETCamera,
	ETPlayer,
	ETPlayerBullets,
	ETProfile,
	ETBackgroundMaster,
	ETBackgroundLayer,
	ETAnonymousSpawner,
	ETDirectionalLight,
	ETEnemy,
	ETMusic,
	ETWeather,
	ETBulletSpawner,
	ETEnemyBullet,
	ETDialogue,
	ETDebugCycle,
	UIManager,
	UIElement,
	ETFocusedMenu,
	ETPowerUp,
	ETVariableTracker,
	ETAnonymousSoundSource,
	ETBulletClearer
};

//String to Entity type
//returns -1 on failure
int stringToEntType(string name);

#endif

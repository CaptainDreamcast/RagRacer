#include "gamescreen.h"

#include <prism/blitz.h>

#include "gamelogic.h"
#include "roadhandler.h"
#include "playerhandler.h"
#include "beginningenemyhandler.h"
#include "hazardhandler.h"
#include "enemyhandler.h"
#include "uihandler.h"
#include "finishhandler.h"

class GameScreen {
public:
	GameScreen() {
		instantiateActor(getGameLogicHandler());
		instantiateActor(getRoadHandler());
		instantiateActor(getPlayerHandler());
		instantiateActor(getBeginningEnemyHandler());
		instantiateActor(getEnemyHandler());
		instantiateActor(getHazardHandler());
		instantiateActor(getUIHandler());
		instantiateActor(getFinishHandler());

		addFadeIn(20, NULL);
	}

	void update() {}
};

EXPORT_SCREEN_CLASS(GameScreen);

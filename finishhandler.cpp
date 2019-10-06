#include "finishhandler.h"

#include <prism/blitz.h>

#include "roadhandler.h"
#include "enemyhandler.h"
#include "gamelogic.h"

class FinishHandler {
public:

	int mIsActive = 0;
	int mEntityID;
	double t = 0.5;
	double z = INFINITY;

	FinishHandler() {}

	void updateStart() {
		if (mIsActive) return;
		if (getEnemiesLeft()) return;

		mEntityID = addBlitzEntity(makePosition(0, 0, 1));
		addBlitzMugenAnimationComponent(mEntityID, getGameSprites(), getGameAnimations(), 4400);

		z = getRoadZ() + 250000;
		mIsActive = 1;
	}

	void updateDisplay() {
		if (!mIsActive) return;
		double scale;
		Position tPos;
		getObjectPositionOnRoad(t, z, scale, tPos);
		setBlitzEntityPosition(mEntityID, tPos);
		setBlitzEntityScale2D(mEntityID, scale);
	}

	void update() {
		updateStart();
		updateDisplay();
	}

};

EXPORT_ACTOR_CLASS(FinishHandler);

double getFinishZ()
{
	return gFinishHandler->z;
}

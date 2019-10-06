#include "beginningenemyhandler.h"

#include <prism/blitz.h>

#include "gamelogic.h"
#include "roadhandler.h"
#include "uihandler.h"
#include "enemyhandler.h"

class BeginningEnemy {
public:
	double t;
	double z;
	double dz;
	double speedZ;
	int mEntityID;
	BeginningEnemy(double t, double z, double speedZ, int isStarting) {
		this->t = t;
		this->z = z;
		this->speedZ = speedZ;
		if (isStarting) {
			dz = 0;
		}
		else {
			dz = speedZ * 2;
		}
		mEntityID = addBlitzEntity(makePosition(0, 0, 1));
		addBlitzMugenAnimationComponent(mEntityID, getGameSprites(), getGameAnimations(), 6000);
		updateEnemyAppearance();
	}

	~BeginningEnemy() {
		removeBlitzEntity(mEntityID);
	}

	void updateEnemyAppearance() {
		double scale;
		Position tPos;
		getObjectPositionOnRoad(t, z, scale, tPos);
		setBlitzEntityPosition(mEntityID, tPos);
		setBlitzEntityScale2D(mEntityID, scale);
	}

	void update() {
		updateEnemyAppearance();
		if (!canMove()) return;

		dz += 5 * 2;
		dz = min(dz, speedZ * 2);
		z += dz;
	}
};

class BeginningEnemyHandler {
public:
	std::vector<std::unique_ptr<BeginningEnemy>> mEnemies;
	
	BeginningEnemyHandler() {
		for (int i = 0; i < 8; i++) {
			mEnemies.push_back(make_unique<BeginningEnemy>(i * (1.0 / 7.0), 0, randfrom(350.0, 400.0), 1));
		}
	}

	void update() {
		if (!mEnemies.size()) return;

		if (getGameTime() == 170) {
			explodeRightMost();
		}

		int enemyVisible = 0;
		for (size_t i = 0; i < mEnemies.size(); i++) {
			mEnemies[i]->update();
			auto scale = getBlitzEntityScale(mEnemies[i]->mEntityID);
			if (scale.x) {
				enemyVisible = 1;
			}
		}

		if (!enemyVisible) {
			mEnemies.clear();
		}
	}

	void explodeRightMost() {
		auto x = addMugenAnimation(getMugenAnimation(getGameAnimations(), 8000), getGameSprites(), getBlitzEntityPosition(mEnemies[6]->mEntityID) + makePosition(0,-40,1));
		playSoundEffectGame(1, 2);
		setMugenAnimationNoLoop(x);
		setMugenAnimationBaseDrawScale(x, 4);
		decreaseEnemiesLeft();
		mEnemies[6]->z = 100000;
	}
};

EXPORT_ACTOR_CLASS(BeginningEnemyHandler);

void explodeBeginningEnemy()
{
	gBeginningEnemyHandler->explodeRightMost();
}

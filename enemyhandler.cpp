#include "enemyhandler.h"

#include <prism/blitz.h>

#include "gamelogic.h"
#include "roadhandler.h"
#include "playerhandler.h"
#include "uihandler.h"

class Enemy {
public:
	double ddt = 0;
	double dt = 0;
	double t;
	double z;
	double dz;
	double speedZ;
	int mEntityID;
	double originalZ;

	int mDeathActive = 0;
	int mTimeSinceDeath = 0;

	Enemy(double t, double z, double speedZ) {
		this->t = t;
		this->z = z;
		this->originalZ = z;
		this->speedZ = speedZ;
		dz = speedZ * 2;
	
		mEntityID = addBlitzEntity(makePosition(0, 0, 1));
		addBlitzMugenAnimationComponent(mEntityID, getGameSprites(), getGameAnimations(), 6000);
		updateEnemyAppearance();
	}

	~Enemy() {
		removeBlitzEntity(mEntityID);
	}

	void updateEnemyAppearance() {
		double scale;
		Position tPos;
		getObjectPositionOnRoad(t, z, scale, tPos);
		setBlitzEntityPosition(mEntityID, tPos);
		setBlitzEntityScale2D(mEntityID, scale);
	}

	void updateMovement() {
		updateEnemyAppearance();
		dz += 5 * 2;
		dz = min(dz, speedZ * 2);
		z += dz;

		auto playerPos = getPlayerRoadPosition();
		z = max(playerPos.z + getPlayerMaxSpeedZ() + 400, z);
		z = min(playerPos.z + 40000, z);
	}

	void updateDeath() {
		if (mDeathActive != 1) return;

		if (randfrom(0, 1)) {
			int id = addBlitzEntity(getBlitzEntityPosition(mEntityID) + makePosition(randfrom(-20, 20), randfrom(-20, 20), randfrom(0.1, 1.0)) - makePosition(0, 30, 0));
			addBlitzMugenAnimationComponent(id, getGameSprites(), getGameAnimations(), 8000);
			setBlitzMugenAnimationNoLoop(id);
		}

		if (mTimeSinceDeath > 180) {
			mDeathActive = 2;
		}
		mTimeSinceDeath++;
	}

	int isCollidingWithPlayer() {
		auto playerPos = getPlayerRoadPosition();
		auto x = getBlitzEntityPositionX(mEntityID);
		auto z = this->z;

		double distX;
		if (x < 160) {
			distX = max(0.0, playerPos.x - x);
		}
		else {
			distX = max(0.0, x - playerPos.x);
		}
		auto distZ = abs(z - playerPos.z);
		if (distZ < 1000 && distX < 30) {
			return 1;
		}

		return 0;
	}

	void bump() {
		const auto pos = t;
		const auto playerPos = getPlayerT();
		if (pos < playerPos) {
			addPlayerImpulse(makePosition(-30, 0, 0));
			ddt += 0.4;
		}
		else {
			addPlayerImpulse(makePosition(30, 0, 0));
			ddt -= 0.4;
		}

		playSoundEffectGame(1, 0);
	}

	void updateBump() {
		if (isCollidingWithPlayer()) {
			bump();
		}
	}

	void updateExploding() {
		if (mDeathActive) return;
		if (t <= -0.1 || t >= 1.1) {
			explode();
		}
	}

	void updatePhysics() {
		if (mDeathActive) return;

		dt += ddt;
		t += dt;

		ddt = 0;
		dt *= 0.9;
		if (abs(dt) < 1) dt = 0;
	}

	void updateCurveAcceleration() {
		const double curveStrength = getCurveStrength(z);
		const double velocityStrength = dz / 400.0;
		static const double CURVE_ACCEL_FACTOR = 0.01;
		ddt += curveStrength * velocityStrength * CURVE_ACCEL_FACTOR;
	}

	void updateSteering() {		
		if (t > 0.9) {
			ddt += -0.02;
			changeBlitzMugenAnimation(mEntityID, 6001);
		}
		else if(t < 0.1) {
			ddt += 0.02;
			changeBlitzMugenAnimation(mEntityID, 6002);
		}
		else if (!getCurveStrength(z)) {
			changeBlitzMugenAnimation(mEntityID, 6000);
		}
	}

	void update() {
		updateMovement();
		updateBump();
		updateExploding();
		updateDeath();
		updateCurveAcceleration();
		updateSteering();
		updatePhysics();
	}

	void explode() {
		mTimeSinceDeath = 0;
		mDeathActive = 1;
		playSoundEffectGame(1, 2);
		decreaseEnemiesLeft();
		increaseTime(getCurrentEnemyIndex());
		addPlayerPart();
	}

	int getOver() {
		return mDeathActive == 2;
	}
};

class EnemyHandler {
public:
	unique_ptr<Enemy> mActiveEnemy = nullptr;
	int mCurrentEnemyIndex = -1;
	int mEnemiesLeft = 8;

	EnemyHandler() {}

	void updateAddingNewEnemy() {
		if (!mEnemiesLeft) return;
		if (mActiveEnemy != nullptr) return;
		mCurrentEnemyIndex++;
		mActiveEnemy = make_unique<Enemy>(randfrom(0, 1), getPlayerRoadPosition().z + 40000, getPlayerMaxSpeedZ() - randfrom(50, 80));
	}

	void updateRemovingEnemy() {
		if (!mActiveEnemy) return;

		if (mActiveEnemy->getOver()) {
			mActiveEnemy = nullptr;
		}
	}

	void update() {
		updateAddingNewEnemy();
		if (mActiveEnemy) mActiveEnemy->update();
		updateRemovingEnemy();
	}
};

EXPORT_ACTOR_CLASS(EnemyHandler);

int getCurrentEnemyIndex()
{
	return gEnemyHandler->mCurrentEnemyIndex;
}

int getEnemiesLeft()
{
	return gEnemyHandler->mEnemiesLeft;
}

void decreaseEnemiesLeft()
{
	gEnemyHandler->mEnemiesLeft--;
}

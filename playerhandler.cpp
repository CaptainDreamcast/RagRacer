#include "playerhandler.h"

#include <prism/blitz.h>

#include "gamelogic.h"
#include "roadhandler.h"
#include "uihandler.h"
#include "finishhandler.h"
#include "storyscreen.h"

static void fadeOutroFinishedCB(void*) {
	setCurrentStoryDefinitionFile("OUTRO");
	setNewScreen(getStoryScreen());
}

static void gotoOutroCB(void*) {
	addFadeOut(120, fadeOutroFinishedCB);
}

class PlayerHandler {
public:
	double ddx = 0;
	double dx = 0;
	double ddz = 0;
	double dZ = 0;
	double mSpeedZ = 0;
	int mEntityID;
	int mStage = 0;
	int mPartAmount = 0;

	const Position START_POSITION = makePosition(160, 240 - 10, 5);

	PlayerHandler() {
		mEntityID = addBlitzEntity(START_POSITION);
		addBlitzMugenAnimationComponent(mEntityID, getGameSprites(), getGameAnimations(), 5000);
		addBlitzPhysicsComponent(mEntityID);
		updateAnimationSpeed();
	}

	void updateAnimation(int tDirection) {
		int baseActionNo = 5000;
		baseActionNo += 10 * mStage;
		baseActionNo += tDirection;
		changeBlitzMugenAnimationIfDifferent(mEntityID, baseActionNo);
	}

	void updateMovement() {
		if (!canMove()) return;

		const double SPEED = 1.5;
		int directionAnimationOffset = 0;
		if (hasPressedLeft()) {
			if (getCurveStrength(getRoadZ()) < 0) {
				ddx -= SPEED * 0.5;
			} else ddx -= SPEED;
			directionAnimationOffset = 1;
		}
		else if (hasPressedRight()) {
			if (getCurveStrength(getRoadZ()) >= 0) {
				ddx += SPEED * 0.5;
			} else ddx += SPEED;
			directionAnimationOffset = 2;
		}
		updateAnimation(directionAnimationOffset);
	}

	void updateXMovement() {
		dx += ddx;
		addBlitzEntityPositionX(getBlitzCameraHandlerEntityID(), dx);
		addBlitzEntityPositionX(mEntityID, dx);

		ddx = 0;
		dx *= 0.9;
		if (abs(dx) < 1) dx = 0;
	}

	double getSpeedLimit() {
		if (mStage == 2) {
			return 400.0;
		}
		else if (mStage == 1) {
			return 300.0;
		}
		else {
			return 200.0;
		}
	}

	void updateAnimationSpeed() {
		if (mSpeedZ == 0) {
			setBlitzMugenAnimationAnimationStepDuration(mEntityID, INF);
			return;
		}

		int lowest = 20;
		int highest = 2;

		double t = mSpeedZ / 400.0;
		setBlitzMugenAnimationAnimationStepDuration(mEntityID, int(lowest + (highest - lowest) * t));
	}

	void updateSpeed() {
		if (canMove() && hasPressedA()) {
			mSpeedZ += 5;
		}
		else {
			mSpeedZ *= 0.99;
			if (mSpeedZ < 1) mSpeedZ = 0;
		}

		mSpeedZ = std::min(mSpeedZ, getSpeedLimit());
		updateAnimationSpeed();
	}

	int mDeathActive = 0;
	Position mDeathPosition;
	int mTimeSinceDeath;

	void updateDeath() {
		if (!mDeathActive) return;

		if (randfrom(0, 1)) {
			int id = addBlitzEntity(getBlitzEntityPosition(mEntityID) + makePosition(randfrom(-20, 20), randfrom(-20, 20), randfrom(0.1, 1.0)) - makePosition(0, 30, 0));
			addBlitzMugenAnimationComponent(id, getGameSprites(), getGameAnimations(), 8000);
			setBlitzMugenAnimationNoLoop(id);
		}

		if (mTimeSinceDeath > 180) {
			mDeathActive = 0;
		}
		mTimeSinceDeath++;
	}

	void explode() {
		mTimeSinceDeath = 0;
		mDeathPosition = getBlitzEntityPosition(mEntityID);
		mDeathActive = 1;
		playSoundEffectGame(1, 2);

		setBlitzEntityPosition(mEntityID, START_POSITION);
		setBlitzCameraHandlerPosition(makePosition(0, 0, 0));
		mSpeedZ = 0;
	}

	void updateCurveAcceleration() {
		const double curveStrength = getCurveStrength(getRoadZ());
		const double velocityStrength = mSpeedZ / 400.0;
		static const double CURVE_ACCEL_FACTOR = 1.5;
		ddx += curveStrength * velocityStrength * CURVE_ACCEL_FACTOR;
	}

	int isInFinish = 0;
	TextureData mWhiteTexture;

	void updateFinish() {
		if (isInFinish) return;
		if (getRoadZ() > getFinishZ() - 1000) {
			mWhiteTexture = getEmptyWhiteTexture();
			auto elem = playOneFrameAnimationLoop(makePosition(0, 0, 88), &mWhiteTexture);
			setAnimationSize(elem, makePosition(320, 240, 1), makePosition(0, 0, 0));
			setAnimationColor(elem, 0, 0, 0);
			addMugenAnimation(getMugenAnimation(getGameAnimations(), 4500), getGameSprites(), makePosition(160, 120, 89));
			addTimerCB(180, gotoOutroCB, nullptr);
			playSoundEffectGame(1, 5);
			isInFinish = 1;
		}
	}

	int isPlayingMusic = 0;
	void updatePlayingMusic() {
		if (!isPlayingMusic && canMove()) {
			streamMusicFile("tracks/3.ogg");
			isPlayingMusic = 1;
		}
	}

	void update() {
		updateMovement();
		updateSpeed();
		updateDeath();
		updateXMovement();
		updateCurveAcceleration();
		updateFinish();
		updatePlayingMusic();
	}

	void addPart() {
		playSoundEffectGame(1, 4);
		mPartAmount++;
		if (!mStage && mPartAmount >= 3) {
			mStage++;
			streamMusicFile("tracks/4.ogg");
			mPartAmount = 0;
		} else if (mStage == 1 && mPartAmount >= 3) {
			mStage++;
			streamMusicFile("tracks/5.ogg");
			mPartAmount = 0;
		}
	}

};

EXPORT_ACTOR_CLASS(PlayerHandler);

double getPlayerSpeedZ() {
	return gPlayerHandler->mSpeedZ;
}

double getPlayerMaxSpeedZ() {
	return gPlayerHandler->getSpeedLimit();
}

Position getPlayerRoadPosition()
{
	auto pos = getBlitzEntityPosition(gPlayerHandler->mEntityID);
	return makePosition(pos.x, pos.y, getRoadZ());
}

void setPlayerExplode()
{
	gPlayerHandler->explode();
}

void addPlayerPart()
{
	gPlayerHandler->addPart();
}

void addPlayerImpulse(const Position& tAcc)
{
	gPlayerHandler->ddx += tAcc.x;
}

double getPlayerT()
{
	return (getBlitzEntityPositionX(gPlayerHandler->mEntityID) + 20) / 360.0;
}

int getPlayerStage()
{
	return gPlayerHandler->mStage;
}

int getPlayerParts()
{
	return gPlayerHandler->mPartAmount;
}

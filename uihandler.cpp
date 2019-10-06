#include "uihandler.h"

#include <prism/blitz.h>

#include "gamelogic.h"
#include "playerhandler.h"
#include "enemyhandler.h"
#include "gamescreen.h"
#include "gameoverscreen.h"

static void continueWithCounterCB(void*);

class UIHandler {
public:
	MugenAnimationHandlerElement* mPlacing;
	int mTimeTextID;
	int mPartsLeftID;

	int mTimeLeft = 30;
	int mTicks = 0;
	vector<int> mTimeBonuses = { 20, 20, 20, 20, 20, 20, 20, 20, 20 };

	int mTimeBonusTextID;
	int mCanMove = 0;
	int mTimeSinceTimeBonus = INF;
	int mGameTime = 0;

	UIHandler() {
		addMugenAnimation(getMugenAnimation(getGameAnimations(), 4200), getGameSprites(), makePosition(310, 10, 80));
		mPlacing = addMugenAnimation(getMugenAnimation(getGameAnimations(), 4109), getGameSprites(), makePosition(10, 10 + 21, 80));

		mTimeTextID = addMugenTextMugenStyle("30", makePosition(155, 50, 81), makeVector3DI(1, 0, 0));
		setMugenTextScale(mTimeTextID, 2);
		mPartsLeftID = addMugenTextMugenStyle("0/3", makePosition(250, 50, 81), makeVector3DI(1, 0, 1));
		setMugenTextScale(mPartsLeftID, 2);

		mTimeBonusTextID = addMugenTextMugenStyle("", makePosition(160, 150, 81), makeVector3DI(1, 0, 0));
		setMugenTextScale(mTimeBonusTextID, 2);

		MugenAnimationHandlerElement* goID = addMugenAnimation(getMugenAnimation(getGameAnimations(), 4210), getGameSprites(), makePosition(160, 120, 82));
		setMugenAnimationNoLoop(goID);

		playSoundEffectGame(1, 3);
	}
	
	void updateTexts() {
		stringstream ss;
		if (getPlayerStage() == 0) {
			ss << getPlayerParts() << "/3";
		}
		else if (getPlayerStage() == 1) {
			ss << getPlayerParts() << "/3";
		}
		else {
			ss << "FULL";
		}
		changeMugenText(mPartsLeftID, ss.str().c_str());

		ss = stringstream();
		ss << mTimeLeft;
		changeMugenText(mTimeTextID, ss.str().c_str());

		changeMugenAnimation(mPlacing, getMugenAnimation(getGameAnimations(), 4100 + getEnemiesLeft() + 1));
	}
	
	void updateTimeLeft() {
		if (!mCanMove) return;
		mTicks++;
		if (mTicks >= 40) {
			mTicks = 0;
			mTimeLeft--;
			if (mTimeLeft < 0) {
				mTimeLeft = 0;
			}
		}
	}

	void updateTimeBonusText() {
		if (mTimeSinceTimeBonus == 90) {
			changeMugenText(mTimeBonusTextID, "");
			mTimeSinceTimeBonus = INF;
		}
		mTimeSinceTimeBonus++;
	}

	void updateCanMove() {
		if (mGameTime > 360) {
			mCanMove = mTimeLeft;
		}
	}

	void updateGameTime() {
		mGameTime++;
	}

	MugenAnimationHandlerElement* continueElement;
	int isInContinue = 0;
	int isInCountdown = 0;
	int mContinueText;
	int mCounterText;
	int mCountdownValue = 10;
	int mCountdownTicks = 0;

	void continueWithCounter() {
		mCountdownValue = 10;
		mCountdownTicks = 0;
		isInCountdown = 1;
		playSoundEffectGame(1, 1);
		removeMugenAnimation(continueElement);
		mContinueText = addMugenTextMugenStyle("CONTINUE?", makePosition(160, 100, 80), makeVector3DI(1, 0, 0));
		mCounterText = addMugenTextMugenStyle("10", makePosition(160, 120, 80), makeVector3DI(1, 0, 0));
	}

	void updateCountdown() {
		if (!isInCountdown) return;

		if (hasPressedStartFlank()) {
			setNewScreen(getGameScreen());
		}

		mCountdownTicks++;
		if (mCountdownTicks >= 60) {
			if (!mCountdownValue) {
				setNewScreen(getGameOverScreen());
			}
			else {
				mCountdownValue--;
				stringstream ss;
				ss << mCountdownValue;
				changeMugenText(mCounterText, ss.str().c_str());
				mCountdownTicks = 0;
			}
		}
	}

	void updateContinue() {
		if (mTimeLeft || isInContinue) return;
		isInContinue = 1;
		playSoundEffectGame(1, 6);
		continueElement = addMugenAnimation(getMugenAnimation(getGameAnimations(), 4300), getGameSprites(), makePosition(160, 120, 82));
		addTimerCB(180, continueWithCounterCB, nullptr);
	}

	void update() {
		updateTexts();
		updateTimeLeft();
		updateTimeBonusText();
		updateCanMove();
		updateGameTime();
		updateContinue();
		updateCountdown();
	}
};

EXPORT_ACTOR_CLASS(UIHandler);

static void continueWithCounterCB(void*) {
	gUIHandler->continueWithCounter();
}

void increaseTime(int tEnemyIndex)
{
	if (!gUIHandler->mTimeLeft) return;
	MugenAnimationHandlerElement* element = addMugenAnimation(getMugenAnimation(getGameAnimations(), 4220), getGameSprites(), makePosition(160, 110, 82));
	setMugenAnimationNoLoop(element);
	gUIHandler->mTimeLeft += gUIHandler->mTimeBonuses[tEnemyIndex];

	stringstream ss;
	ss << "TIME BONUS: +" << gUIHandler->mTimeBonuses[tEnemyIndex] << "!";
	changeMugenText(gUIHandler->mTimeBonusTextID, ss.str().c_str());
	gUIHandler->mTimeSinceTimeBonus = 0;

}

int canMove()
{
	return gUIHandler->mCanMove;
}

int getGameTime()
{
	return gUIHandler->mGameTime;
}

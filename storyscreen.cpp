#include "storyscreen.h"

#include <assert.h>

#include <prism/blitz.h>
#include <prism/stlutil.h>

#include "titlescreen.h"
#include "gamescreen.h"

using namespace std;

static struct {
	MugenDefScript mScript;
	MugenDefScriptGroup* mCurrentGroup;
	MugenSpriteFile mSprites;

	MugenAnimation* mOldAnimation;
	MugenAnimation* mAnimation;
	MugenAnimationHandlerElement* mAnimationID;
	MugenAnimationHandlerElement* mOldAnimationID;

	Position mOldAnimationBasePosition;
	Position mAnimationBasePosition;

	int mSpeakerID;
	int mTextID;

	int mIsStoryOver;

	char mDefinitionPath[1024];
} gStoryScreenData;

static int isImageGroup() {
	string name = gStoryScreenData.mCurrentGroup->mName;
	char firstW[100];
	sscanf(name.data(), "%s", firstW);

	return !strcmp("Image", firstW);
}

static void increaseGroup() {
	gStoryScreenData.mCurrentGroup = gStoryScreenData.mCurrentGroup->mNext;
}

static void loadImageGroup() {
	if (gStoryScreenData.mOldAnimationID != nullptr) {
		removeMugenAnimation(gStoryScreenData.mOldAnimationID);
		destroyMugenAnimation(gStoryScreenData.mOldAnimation);
	}

	if (gStoryScreenData.mAnimationID != nullptr) {
		setMugenAnimationBasePosition(gStoryScreenData.mAnimationID, &gStoryScreenData.mOldAnimationBasePosition);
	}

	gStoryScreenData.mOldAnimationID = gStoryScreenData.mAnimationID;
	gStoryScreenData.mOldAnimation = gStoryScreenData.mAnimation;


	int group = getMugenDefNumberVariableAsGroup(gStoryScreenData.mCurrentGroup, "group");
	int item = getMugenDefNumberVariableAsGroup(gStoryScreenData.mCurrentGroup, "item");
	gStoryScreenData.mAnimation = createOneFrameMugenAnimationForSprite(group, item);
	gStoryScreenData.mAnimationID = addMugenAnimation(gStoryScreenData.mAnimation, &gStoryScreenData.mSprites, makePosition(0, 0, 0));
	setMugenAnimationBasePosition(gStoryScreenData.mAnimationID, &gStoryScreenData.mAnimationBasePosition);

	increaseGroup();
}


static int isTextGroup() {
	string name = gStoryScreenData.mCurrentGroup->mName;
	char firstW[100];
	sscanf(name.data(), "%s", firstW);

	return !strcmp("Text", firstW);
}

static void loadTextGroup() {
	if (gStoryScreenData.mTextID != -1) {
		removeMugenText(gStoryScreenData.mTextID);
		removeMugenText(gStoryScreenData.mSpeakerID);
	}

	char* speaker = getAllocatedMugenDefStringVariableAsGroup(gStoryScreenData.mCurrentGroup, "speaker");
	char* text = getAllocatedMugenDefStringVariableAsGroup(gStoryScreenData.mCurrentGroup, "text");

	gStoryScreenData.mSpeakerID = addMugenText(speaker, makePosition(40 / 2, 348 / 2, 3), 1);

	gStoryScreenData.mTextID = addMugenText(text, makePosition(30 / 2, 380 / 2, 3), 1);
	setMugenTextBuildup(gStoryScreenData.mTextID, 1);
	setMugenTextTextBoxWidth(gStoryScreenData.mTextID, 560 / 2);

	freeMemory(speaker);
	freeMemory(text);

	increaseGroup();
}

static int isTitleGroup() {
	string name = gStoryScreenData.mCurrentGroup->mName;
	char firstW[100];
	sscanf(name.data(), "%s", firstW);

	return !strcmp("Title", firstW);
}

static void goToTitle(void* tCaller) {
	(void)tCaller;
	setNewScreen(getTitleScreen());
}

static void loadTitleGroup() {
	gStoryScreenData.mIsStoryOver = 1;

	addFadeOut(30, goToTitle, NULL);
}

static int isGameGroup() {
	string name = gStoryScreenData.mCurrentGroup->mName;
	char firstW[100];
	sscanf(name.data(), "%s", firstW);

	return !strcmp("Game", firstW);
}

static void goToGame(void* tCaller) {
	(void)tCaller;
	setNewScreen(getGameScreen());
}

static void loadGameGroup() {
	gStoryScreenData.mIsStoryOver = 1;

	addFadeOut(30, goToGame, NULL);
}

static void loadNextStoryGroup() {
	int isRunning = 1;
	while (isRunning) {
		if (isImageGroup()) {
			loadImageGroup();
		}
		else if (isTextGroup()) {
			loadTextGroup();
			break;
		}
		else if (isTitleGroup()) {
			loadTitleGroup();
			break;
		}
		else if (isGameGroup()) {
			loadGameGroup();
			break;
		}
		else {
			logError("Unidentified group type.");
			//logErrorString(gStoryScreenData.mCurrentGroup->mName);
			abortSystem();
		}
	}
}

static void findStartOfStoryBoard() {
	gStoryScreenData.mCurrentGroup = gStoryScreenData.mScript.mFirstGroup;

	while (gStoryScreenData.mCurrentGroup && "STORYSTART" != gStoryScreenData.mCurrentGroup->mName) {
		gStoryScreenData.mCurrentGroup = gStoryScreenData.mCurrentGroup->mNext;
	}

	assert(gStoryScreenData.mCurrentGroup);
	gStoryScreenData.mCurrentGroup = gStoryScreenData.mCurrentGroup->mNext;
	assert(gStoryScreenData.mCurrentGroup);

	gStoryScreenData.mAnimationID = nullptr;
	gStoryScreenData.mOldAnimationID = nullptr;
	gStoryScreenData.mTextID = -1;

	gStoryScreenData.mOldAnimationBasePosition = makePosition(0, 0, 1);
	gStoryScreenData.mAnimationBasePosition = makePosition(0, 0, 2);

	loadNextStoryGroup();
}



static void loadStoryScreen() {
	gStoryScreenData.mIsStoryOver = 0;

	stringstream scriptPath;
	scriptPath << "game/" << gStoryScreenData.mDefinitionPath << ".def";
	loadMugenDefScript(&gStoryScreenData.mScript, scriptPath.str().data());

	scriptPath = stringstream();
	scriptPath << "game/" << gStoryScreenData.mDefinitionPath << ".sff";
	gStoryScreenData.mSprites = loadMugenSpriteFileWithoutPalette(scriptPath.str());

	findStartOfStoryBoard();

	if (!strcmp("INTRO", gStoryScreenData.mDefinitionPath)) {
		streamMusicFile("tracks/6.ogg");
	}
	else {
		streamMusicFile("tracks/5.ogg");
	}
}


static void updateText() {
	if (gStoryScreenData.mIsStoryOver) return;
	if (gStoryScreenData.mTextID == -1) return;

	if (hasPressedAFlankSingle(0) || hasPressedAFlankSingle(1)) {
		if (isMugenTextBuiltUp(gStoryScreenData.mTextID)) {
			loadNextStoryGroup();
		}
		else {
			setMugenTextBuiltUp(gStoryScreenData.mTextID);
		}
	}
}

static void updateStoryScreen() {

	updateText();
}


Screen gStoryScreen;

Screen* getStoryScreen() {
	gStoryScreen = makeScreen(loadStoryScreen, updateStoryScreen);
	return &gStoryScreen;
}

void setCurrentStoryDefinitionFile(const char* tPath) {
	strcpy(gStoryScreenData.mDefinitionPath, tPath);
}

#include "titlescreen.h"

#include <prism/blitz.h>

#include "gamescreen.h"
#include "storyscreen.h"

static struct {
	MugenSpriteFile mSprites;
	MugenSounds mSounds;
} gTitleScreenData;

static void loadTitleScreen() {
	gTitleScreenData.mSprites = loadMugenSpriteFileWithoutPalette("game/TITLE.sff");
	addMugenAnimation(createOneFrameMugenAnimationForSprite(1, 0), &gTitleScreenData.mSprites, makePosition(0, 0, 1));
	addFadeIn(30, NULL, NULL);
	setWrapperTitleScreen(getTitleScreen());
}


static void gotoGameScreen(void* tCaller) {
	(void)tCaller;
	setCurrentStoryDefinitionFile("INTRO");
	setNewScreen(getStoryScreen());
}

static void updateTitleScreen() {

	if (hasPressedAFlank() || hasPressedStartFlank()) {
		addFadeOut(30, gotoGameScreen, NULL);
	}
}

static Screen gTitleScreen;

Screen* getTitleScreen() {
	gTitleScreen = makeScreen(loadTitleScreen, updateTitleScreen);
	return &gTitleScreen;
};

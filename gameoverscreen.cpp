#include "gameoverscreen.h"

#include <prism/blitz.h>

#include "gamescreen.h"
#include "titlescreen.h"

static struct {
	MugenSpriteFile mSprites;

} gGameOverScreenData;

static void loadGameOverScreen() {
	gGameOverScreenData.mSprites = loadMugenSpriteFileWithoutPalette("game/GAMEOVER.sff");
	addMugenAnimation(createOneFrameMugenAnimationForSprite(1, 0), &gGameOverScreenData.mSprites, makePosition(0, 0, 1));
	addFadeIn(30, NULL, NULL);
	streamMusicFile("tracks/6.ogg");
}


static void gotoGameScreen(void* tCaller) {
	(void)tCaller;
	setNewScreen(getTitleScreen());
}

static void updateGameOverScreen() {

	if (hasPressedAFlank() || hasPressedStartFlank()) {
		addFadeOut(30, gotoGameScreen, NULL);
	}
}

static Screen gGameOverScreen;

Screen* getGameOverScreen() {
	gGameOverScreen = makeScreen(loadGameOverScreen, updateGameOverScreen);
	return &gGameOverScreen;
};

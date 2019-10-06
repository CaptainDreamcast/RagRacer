#include "gamelogic.h"

#include <prism/blitz.h>

class GameLogicHandler {
public:
	MugenSpriteFile mSprites;
	MugenAnimations mAnimations;
	MugenSounds mSounds;

	GameLogicHandler() {
		mSprites = loadMugenSpriteFileWithoutPalette("game/GAME.sff");
		mAnimations = loadMugenAnimationFile("game/GAME.air");
		mSounds = loadMugenSoundFile("game/GAME.snd");
	}

	void update() {}
};

EXPORT_ACTOR_CLASS(GameLogicHandler);

MugenSpriteFile* getGameSprites()
{
	return &gGameLogicHandler->mSprites;
}

MugenAnimations* getGameAnimations()
{
	return &gGameLogicHandler->mAnimations;
}

void playSoundEffectGame(int tGroup, int tElement)
{
	tryPlayMugenSound(&gGameLogicHandler->mSounds, tGroup, tElement);
}

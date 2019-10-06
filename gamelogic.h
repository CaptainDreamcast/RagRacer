#pragma once

#include <prism/actorhandler.h>
#include <prism/mugenanimationreader.h>

ActorBlueprint getGameLogicHandler();
MugenSpriteFile* getGameSprites();
MugenAnimations* getGameAnimations();

void playSoundEffectGame(int tGroup, int tElement);
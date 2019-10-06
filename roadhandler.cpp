#include "roadhandler.h"

#include <prism/blitz.h>

#include "gamelogic.h"
#include "playerhandler.h"
#include "enemyhandler.h"

class Curve {
public:

	double mStartZ;
	double mEndZ;
	double mStrength;
	Curve(double startZ, double endZ, double strength) {
		mStartZ = startZ;
		mEndZ = endZ;
		mStrength = strength;
	}

};

class RoadHandler {
public:
	int mCurrentZ = 0;

	static const int SCANLINE_AMOUNT = 103;
	MugenAnimationHandlerElement* mScanlines[SCANLINE_AMOUNT];

	unique_ptr<Curve> mActiveCurve = nullptr;

	RoadHandler() {
		for (int i = 0; i < SCANLINE_AMOUNT; i++) {
			mScanlines[i] = addMugenAnimation(getMugenAnimation(getGameAnimations(), 1000 + i), getGameSprites(), makePosition(160, 239 - i, 2));
			setMugenAnimationCameraPositionReference(mScanlines[i] , getBlitzCameraHandlerPositionReference());
		}
		addMugenAnimation(getMugenAnimation(getGameAnimations(), 3000), getGameSprites(), makePosition(160, 0, 1));
		addMugenAnimation(getMugenAnimation(getGameAnimations(), 4000), getGameSprites(), makePosition(160, 240, 1));
	}


	void updateSingleScanline(int i, int z, double dx) {
		int fgFile = (z % 2400) < 1200;
		int actionNo = (fgFile + 1) * 1000 + i;
		changeMugenAnimation(mScanlines[i], getMugenAnimation(getGameAnimations(), actionNo));
		const auto pos = getMugenAnimationPosition(mScanlines[i]);
		setMugenAnimationPosition(mScanlines[i], makePosition(160 + dx, pos.y, pos.z));
	}

	int isInCurve(double z) {
		if (!mActiveCurve) return 0;

		return z >= mActiveCurve->mStartZ && z <= mActiveCurve->mEndZ;
	}

	const double CURVE_FACTOR = 6.0;

	int ddz = 4;

	void updateScanlines() {
		int dz = 0;
		int z = mCurrentZ;
		double dx = 0;
		for (int i = 0; i < SCANLINE_AMOUNT; i++) {
			updateSingleScanline(i, z, dx);
			if (isInCurve(z)) {
				dx += mActiveCurve->mStrength * CURVE_FACTOR * (1.0 - (i / (double)SCANLINE_AMOUNT));
			}
			dz += ddz;
			z += dz;
		}
	}

	void updateRemovingCurve() {
		if (!mActiveCurve) return;

		if (mCurrentZ > mActiveCurve->mEndZ) {
			mActiveCurve = nullptr;
		}
	}

	void updateAddingCurve() {
		if (mActiveCurve) return;
		if (!getEnemiesLeft()) return;

		double startZ = mCurrentZ + randfrom(40000, 50000);
		double endZ = startZ + randfrom(100000, 200000);
		mActiveCurve = make_unique<Curve>(startZ, endZ, randfromInteger(0, 1) ? randfrom(0.5, 0.75) : randfrom(-0.75, -0.5));
	}

	void updateActiveCurve() {
		updateRemovingCurve();
		updateAddingCurve();
	}

	void update() {
		updateScanlines();
		updateActiveCurve();
		mCurrentZ += int(getPlayerSpeedZ() * 2);
	}

	void getObjectPositionOnRoad(double queryT, double queryZ, double & oScale, Position & oPos)
	{
		if (queryZ < mCurrentZ) {
			oScale = 0;
			oPos = makePosition(0, 0, 0);
			return;
		}

		int dz = 0;
		int z = mCurrentZ;
		double dx = 0;
		for (int i = 0; i < SCANLINE_AMOUNT; i++) {
			if (queryZ <= z) {
				double t = i / (double)SCANLINE_AMOUNT;
				oScale = 0.1 + 0.9 * (1.0 - t);

				double center = 160 + dx;
				double lowerWidth = 360;
				double higherWidth = 4;
				double lowerX = center - (lowerWidth / 2);
				double higherX = center - (higherWidth / 2);
				double startX = lowerX + t * (higherX - lowerX);
				double width = lowerWidth + t * (higherWidth - lowerWidth);
				oPos = makePosition(startX + queryT * width, 239 - i, 5 - t);
				return;
			}
			if (isInCurve(z)) {
				dx += mActiveCurve->mStrength * CURVE_FACTOR * (1.0 - (i / (double)SCANLINE_AMOUNT));
			}
			dz += ddz;
			z += dz;
		}

		oScale = 0;
		oPos = makePosition(0, 0, 0);
	}
};

EXPORT_ACTOR_CLASS(RoadHandler);

void getObjectPositionOnRoad(double t, double z, double & oScale, Position & oPos)
{
	return gRoadHandler->getObjectPositionOnRoad(t, z, oScale, oPos);
}

double getRoadZ()
{
	return gRoadHandler->mCurrentZ;
}

double getCurveStrength(double z)
{
	if (!gRoadHandler->mActiveCurve) return 0.0;
	if (z >= gRoadHandler->mActiveCurve->mStartZ && z <= gRoadHandler->mActiveCurve->mEndZ) {
		return gRoadHandler->mActiveCurve->mStrength;
	}
	return 0.0;
}


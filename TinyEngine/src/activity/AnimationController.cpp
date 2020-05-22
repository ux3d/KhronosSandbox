#include "AnimationController.h"

#include "../math/Math.h"

AnimationController::AnimationController()
{
}

AnimationController::~AnimationController()
{
}

void AnimationController::setStartTime(float startTime)
{
	this->startTime = glm::max(0.0f, startTime);
}

float AnimationController::getStartTime() const
{
	return startTime;
}

void AnimationController::setStopTime(float stopTime)
{
	this->stopTime = glm::max(startTime, stopTime);
}

float AnimationController::getStopTime() const
{
	return stopTime;
}

void AnimationController::setCurrentTime(float currentTime)
{
	this->currentTime = glm::clamp(currentTime, startTime, stopTime);
}

float AnimationController::getCurrentTime() const
{
	return currentTime;
}

void AnimationController::setPlay(bool play)
{
	this->play = play;
}

bool AnimationController::isPlay() const
{
	return play;
}

void AnimationController::setAnimationType(AnimationType animationType)
{
	this->animationType = animationType;

	switch (animationType)
	{
	case PLAY_ONCE:
	case PLAY_LOOP:
	case PLAY_PING_PONG:
		animationSpeed = 1.0f;
		break;
	case PLAY_ONCE_REVERSE:
	case PLAY_LOOP_REVERSE:
		animationSpeed = 1.0f;
		break;
	}
}

AnimationType AnimationController::getAnimationType() const
{
	return animationType;
}

//

void AnimationController::updateCurrentTime(float deltaTime)
{
	if (!play)
	{
		return;
	}

	currentTime += animationSpeed * deltaTime;
	if (currentTime < startTime)
	{
		if (animationType == PLAY_ONCE_REVERSE)
		{
			currentTime = startTime;
			play = false;
		}
		else if (animationType == PLAY_LOOP_REVERSE)
		{
			currentTime = stopTime - (startTime - currentTime);
		}
		else if (animationType == PLAY_PING_PONG)
		{
			currentTime = startTime + (startTime - currentTime);
			animationSpeed *= -1.0f;
		}
	}
	else if (currentTime > stopTime)
	{
		if (animationType == PLAY_ONCE)
		{
			currentTime = stopTime;
			play = false;
		}
		else if (animationType == PLAY_LOOP)
		{
			currentTime = startTime + (currentTime - stopTime);
		}
		else if (animationType == PLAY_PING_PONG)
		{
			currentTime = stopTime - (currentTime - stopTime);
			animationSpeed *= -1.0f;
		}
	}

	this->currentTime = glm::clamp(currentTime, startTime, stopTime);
}

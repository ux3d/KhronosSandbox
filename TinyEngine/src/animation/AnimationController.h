#ifndef ANIMATION_ANIMATIONCONTROLLER_H_
#define ANIMATION_ANIMATIONCONTROLLER_H_

enum AnimationType {
	PLAY_ONCE,
	PLAY_ONCE_REVERSE,
	PLAY_LOOP,
	PLAY_LOOP_REVERSE,
	PLAY_PING_PONG
};

class AnimationController {
private:
	float startTime = 0.0f;
	float stopTime = 1.0f;

	float currentTime = 0.0f;

	float animationSpeed = 1.0f;

	bool play = false;

	AnimationType animationType = PLAY_LOOP;

public:
	AnimationController();
	~AnimationController();

	void setStartTime(float startTime);
	float getStartTime() const;

	void setStopTime(float stopTime);
	float getStopTime() const;

	void setCurrentTime(float currentTime);
	float getCurrentTime() const;

	void setPlay(bool play);
	bool isPlay() const;

	void setAnimationType(AnimationType animationType);
	AnimationType getAnimationType() const;

	//

	void updateCurrentTime(float deltaTime);

};

#endif /* ANIMATION_ANIMATIONCONTROLLER_H_ */

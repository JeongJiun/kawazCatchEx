#pragma once
#ifndef __HelloCocos__MainScene__
#define __HelloCocos__MainScene__


#include <random>
#include "cocos2d.h"
class MainScene :public cocos2d::Layer
{
protected:
	MainScene();
	virtual ~MainScene();
	bool init() override;

public:
	// 게임상태
	enum class GameState
	{
		READY,
		PLAYING,
		ENDING,
		RESULT
	};

	static cocos2d::Scene* createScene();
	void update(float dt);
	void onEnterTransitionDidFinish() override;
	void addReadyLabel();

	CREATE_FUNC(MainScene);

	CC_SYNTHESIZE(int, _score, Score);
	CC_SYNTHESIZE(float, _second, Second);
	CC_SYNTHESIZE(bool, _isCrash, IsCrash);
	CC_SYNTHESIZE(GameState, _state, State);
	CC_SYNTHESIZE(cocos2d::Vector<cocos2d::Sprite *>, _fruits, Fruits);
	CC_SYNTHESIZE(int, _highScore, HighScore);
	CC_SYNTHESIZE(bool, _isHighScore, IsHighScore);
	CC_SYNTHESIZE(std::mt19937, _engine, Engine);
	// _player변수과 getPlayer()함수, setPlayer(Sprite*)함수가 자동적으로 실장된다.
	CC_SYNTHESIZE_RETAIN(cocos2d::Sprite*, _player, Player);
	CC_SYNTHESIZE_RETAIN(cocos2d::Label*, _scoreLabel, ScoreLabel);
	CC_SYNTHESIZE_RETAIN(cocos2d::Label*, _secondLabel, SecondLabel);
	CC_SYNTHESIZE_RETAIN(cocos2d::SpriteBatchNode*, _fruitsBatchNode, FruitBatchNode);
	CC_SYNTHESIZE_RETAIN(cocos2d::Label *, _highscoreLabel, HighscoreLabel);


private:

	// 과일 종류를 표시
	enum class FruitType
	{
		APPLE,
		GRAPE,
		ORANGE,
		BANANA,
		CHERRY,
		GOLDEN,
		BOMB,
		// 최대수
		COUNT
	};

	/** 화면에 과일을 새로이 배치하여 그것을 반환
	* @return 새롭게 작성한 과일
	*/
	cocos2d::Sprite* addFruit();

	/** 맵에서 과일을 삭제
	* @param fruit 삭제하는 과일
	* @return 정상적으로 삭제가 되었는가.
	*/
	bool removeFruit(cocos2d::Sprite *fruit);

	/** 과일을 취득
	* @param Sprite* 취득하는 과일
	*/
	void catchFruit(cocos2d::Sprite* fruit);

	/** 게임 종료시 불려지는 함수
	*/
	void onResult();

	/**	폭탄을 잡았을때
	*
	*/
	void onCatchBomb();

	/** min ~ max의 난수를 float으로 반환
	* @param min 최소치
	* @param max 최대치
	* @retun min <= n < max의 정수치
	*/
	float generateRandom(float min, float max);

	/** 폭탄 잡았을때 이펙트
	* @param position 표시할 위치
	*/
	void addBombEffect(cocos2d::Vec2 position);
};

#endif /* defined__HelloCocos__MainScene__*/
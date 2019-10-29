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
	// ���ӻ���
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
	// _player������ getPlayer()�Լ�, setPlayer(Sprite*)�Լ��� �ڵ������� ����ȴ�.
	CC_SYNTHESIZE_RETAIN(cocos2d::Sprite*, _player, Player);
	CC_SYNTHESIZE_RETAIN(cocos2d::Label*, _scoreLabel, ScoreLabel);
	CC_SYNTHESIZE_RETAIN(cocos2d::Label*, _secondLabel, SecondLabel);
	CC_SYNTHESIZE_RETAIN(cocos2d::SpriteBatchNode*, _fruitsBatchNode, FruitBatchNode);
	CC_SYNTHESIZE_RETAIN(cocos2d::Label *, _highscoreLabel, HighscoreLabel);


private:

	// ���� ������ ǥ��
	enum class FruitType
	{
		APPLE,
		GRAPE,
		ORANGE,
		BANANA,
		CHERRY,
		GOLDEN,
		BOMB,
		// �ִ��
		COUNT
	};

	/** ȭ�鿡 ������ ������ ��ġ�Ͽ� �װ��� ��ȯ
	* @return ���Ӱ� �ۼ��� ����
	*/
	cocos2d::Sprite* addFruit();

	/** �ʿ��� ������ ����
	* @param fruit �����ϴ� ����
	* @return ���������� ������ �Ǿ��°�.
	*/
	bool removeFruit(cocos2d::Sprite *fruit);

	/** ������ ���
	* @param Sprite* ����ϴ� ����
	*/
	void catchFruit(cocos2d::Sprite* fruit);

	/** ���� ����� �ҷ����� �Լ�
	*/
	void onResult();

	/**	��ź�� �������
	*
	*/
	void onCatchBomb();

	/** min ~ max�� ������ float���� ��ȯ
	* @param min �ּ�ġ
	* @param max �ִ�ġ
	* @retun min <= n < max�� ����ġ
	*/
	float generateRandom(float min, float max);

	/** ��ź ������� ����Ʈ
	* @param position ǥ���� ��ġ
	*/
	void addBombEffect(cocos2d::Vec2 position);
};

#endif /* defined__HelloCocos__MainScene__*/
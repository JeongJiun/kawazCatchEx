#include "MainScene.h"
#include "TitleScene.h"
#include "SimpleAudioEngine.h"

USING_NS_CC;


// ������ ȭ���ܺ����� margin
const int FRUIT_TOP_MARGIN = 30;

// ���ϼӵ�
const float FALLING_DURATION = 2.2f;

// ������ ������
const int FRUIT_SPAWN_RATE = 2;

// Ȳ�� ���� �⺻ ������ �ʱ�ġ
const float GOLDEN_FRUIT_PROBABILITY_BASE = 0.01;

// ��ź ������ �ʱ�ġ
const float BOMB_PROBABILITY_BASE = 0.05;
// Ȳ�ݰ��� ������ ����ġ
const float GOLDEN_FRUIT_PROBABILITY_RATE = 0.00;
// ��ź ������ ����ġ
const float BOMB_PROBABILITY_RATE = 0.00;
// �Ϲ� ���� ��
const int NORMAL_FRUIT_COUNT = 5;
// �ð� ����
const float TIME_LIMIT_SECOND = 14.0f;

// Ȳ�� ������ ������� ����
const int GOLDEN_FRUIT_SCORE = 5;

// ��ź�� �������
const int BOMB_PENALTY_SCORE = 4;

// �Ϲ� ���� ������ �ʱ�ġ
const float FRUIT_SPAWN_INCREASE_BASE = 0.1;
// �Ϲ� ���� ������ ������
const float FRUIT_SPAWN_INCREASE_RATE = 0.0f;
// ���� ������ �ִ�ġ
const float MAXIMUM_SPAWN_PROBABILITY = 0.5;

// �ְ� ���� ���
const char* HIGHSCORE_KEY = "highscore_key";

MainScene::MainScene()
	:_player(NULL)
	, _score(0)
	, _scoreLabel(NULL)
	, _second(TIME_LIMIT_SECOND)
	, _secondLabel(NULL)
	// , _state(GameState::PLAYING)
	, _state(GameState::READY)
	, _isCrash(false)
	, _fruitsBatchNode(NULL)
	, _highscoreLabel(NULL)
	, _highScore(0)
	, _isHighScore(false)
{
	// ���� �ʱ�ȭ
	std::random_device rdev;
	_engine.seed(rdev());
}


MainScene::~MainScene()
{
	CC_SAFE_RELEASE_NULL(_player); // _player�� release�Ͽ� �޸� ���� ����
	CC_SAFE_RELEASE_NULL(_scoreLabel);
	CC_SAFE_RELEASE_NULL(_secondLabel);
	CC_SAFE_RELEASE_NULL(_highscoreLabel);
	CC_SAFE_RELEASE_NULL(_fruitsBatchNode);
}

Scene* MainScene::createScene()
{
	auto scene = Scene::create();
	auto layer = MainScene::create();
	scene->addChild(layer);
	return scene;
}

bool MainScene::init()
{
	if (!Layer::init()) {
		return false;
	}

	auto director = Director::getInstance();

	auto size = director->getWinSize();

	auto background = Sprite::create("background.png");

	background->setPosition(Vec2(size.width / 2.0, size.height / 2.0));

	this->addChild(background);

	this->setPlayer(Sprite::create("player.png")); // Sprite�� �����ؼ� _player�� ����

	_player->setPosition(Vec2(size.width / 2.0, size.height - 445)); // _player ��ġ ����

	this->addChild(_player); // ���� _player�� ��ġ

	// ��ġ�� ���
	auto listener = EventListenerTouchOneByOne::create();
	listener->onTouchBegan = [](Touch* touch, Event* event) {
		// ��ġ �Ǿ��� ���� ó��

		return true;
	};

	listener->onTouchMoved = [this](Touch* touch, Event* event) {

		if (!this->getIsCrash())
		{
			// ��ġ�� �ߴ� �Ǿ������� ó��
			// ���� ��ġ ��ġ���� ���̸� ���ͷ� ���
			Vec2 delta = touch->getDelta();

			// ���� ���� ȭ���� ��ǥ�� ���
			Vec2 position = _player->getPosition();

			// ���� ��ǥ + �̵����� ���ο� ��ǥ�� �Ѵ�.
			Vec2 newPosition = position + delta;

			auto winSize = Director::getInstance()->getWinSize(); // ȭ�� ����� ���

			newPosition = newPosition.getClampPoint(Vec2(0, position.y), Vec2(winSize.width, position.y));

			if (newPosition.x < 0) { // ���� ���ο� ��ġ�� ȭ�� ���ʳ� ���� �����̶��
				newPosition.x = 0; // x��ǥ�� 0���� �Ѵ�.
			}
			else if (newPosition.x > winSize.width) { // ���� ������ �� ���� �������̸�
				newPosition.x = winSize.width; // x��ǥ�� ȭ�� ������� �Ѵ�.
			}

			// ���ι������δ� �̵����� �ʵ��� ��.
			newPosition.y = winSize.height - 445;

			_player->setPosition(newPosition);
		}
	};

	director->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, this);

	

	auto scoreLabel = Label::createWithTTF(StringUtils::toString(_score)
		, "Marker Felt.ttf"
		, 16);

	this->setScoreLabel(scoreLabel);
	
	scoreLabel->enableShadow(Color4B::BLACK, Size(0.5f, 0.5f));
	scoreLabel->enableOutline(Color4B::BLACK, 1.5f);
	scoreLabel->setPosition(Vec2(size.width / 2.0f * 1.5f, size.height - 40));
	this->addChild(_scoreLabel);

	// ���ھ� ���
	auto scoreLabelHeader = Label::createWithTTF("SCORE", "Marker Felt.ttf", 16);

	scoreLabelHeader->enableShadow(Color4B::BLACK, Size(0.5f, 0.5f), 3);
	scoreLabelHeader->enableOutline(Color4B::BLACK, 1.5f);
	scoreLabelHeader->setPosition(Vec2(size.width / 2.0f * 1.5f, size.height - 20));
	this->addChild(scoreLabelHeader);

	// Ÿ�̸�
	int second = static_cast<int>(_second);
	auto secondLabel = Label::createWithTTF(StringUtils::toString(second), "Marker Felt.ttf", 16);
	secondLabel->enableShadow(Color4B::BLACK, Size(0.5f, 0.5f), 3);
	secondLabel->enableOutline(Color4B::BLACK, 1.5f);
	secondLabel->setPosition(Vec2(size.width / 2.0f, size.height - 40));
	this->setSecondLabel(secondLabel);
	this->addChild(secondLabel);

	// Ÿ�̸� ���
	auto secondLabelHeader = Label::createWithTTF("TIME", "Marker Felt.ttf", 16);
	secondLabelHeader->enableShadow(Color4B::BLACK, Size(0.5f, 0.5f), 3);
	secondLabelHeader->enableOutline(Color4B::BLACK, 1.5f);
	secondLabelHeader->setPosition(Vec2(size.width / 2.0f, size.height - 20));
	this->addChild(secondLabelHeader);

	// ���̽��ھ� �� �߰�
	auto highscore = UserDefault::getInstance()->getIntegerForKey(HIGHSCORE_KEY);
	auto highscoreLabel = Label::createWithTTF(StringUtils::toString(static_cast<int>(highscore)), "Marker Felt.ttf", 16);
	highscoreLabel->enableShadow(Color4B::BLACK, Size(0.5, 0.5), 3);
	highscoreLabel->enableOutline(Color4B::BLACK, 1.5);
	highscoreLabel->setPosition(Vec2(size.width / 2.0 * 0.5, size.height - 40));
	this->setHighscoreLabel(highscoreLabel);
	this->addChild(_highscoreLabel);
	this->setHighScore(highscore);

	// ���̽��ھ� ����� �߰�
	auto highscoreLabelHeader = Label::createWithTTF("HIGHSCORE", "Marker Felt.ttf", 16);
	highscoreLabelHeader->enableShadow(Color4B::BLACK, Size(0.5, 0.5), 3);
	highscoreLabelHeader->enableOutline(Color4B::BLACK, 1.5);
	highscoreLabelHeader->setPosition(Vec2(size.width / 2.0 * 0.5, size.height - 20));
	this->addChild(highscoreLabelHeader);

	// BatchNode�ʱ�ȭ
	auto fruitsBatchNode = SpriteBatchNode::create("fruits.png");
	this->addChild(fruitsBatchNode);
	this->setFruitBatchNode(fruitsBatchNode);

	// update�� �Ҷ����� ������ �����ϱ� ���� ���
	this->scheduleUpdate();

	return true;
}

Sprite* MainScene::addFruit()
{
	// ȭ������ ���
	auto winSize = Director::getInstance()->getWinSize();
	// ���� ������ ����
	// int fruitType = rand() % static_cast<int>(FruitType::COUNT);

	int fruitType = 0;
	float r = this->generateRandom(0, 1);
	int pastSecond = TIME_LIMIT_SECOND - _second;
	float goldenFruitProbability = GOLDEN_FRUIT_PROBABILITY_BASE + GOLDEN_FRUIT_PROBABILITY_RATE * pastSecond;
	float bombProbabliity = BOMB_PROBABILITY_BASE + BOMB_PROBABILITY_RATE * pastSecond;

	if (r <= goldenFruitProbability)
	{
		fruitType = static_cast<int>(FruitType::GOLDEN);
	}
	else if (r <= goldenFruitProbability + bombProbabliity)
	{
		fruitType = static_cast<int>(FruitType::BOMB);
	}
	else
	{
		fruitType = round(this->generateRandom(0, NORMAL_FRUIT_COUNT - 1));
	}

	// ���� �ۼ�
	// std::string filename = StringUtils::format("fruit%d.png", fruitType);
	// auto fruit = Sprite::create(filename);
	
	// �ؽ��� ����� ����
	auto textureSize = _fruitsBatchNode->getTextureAtlas()->getTexture()->getContentSize();
	// �ؽ��� ������ ������ �ϳ��ϳ� ����
	auto fruitWidth = textureSize.width / static_cast<int>(FruitType::COUNT);
	auto fruit = Sprite::create("fruits.png", Rect(fruitWidth * fruitType, 0, fruitWidth, textureSize.height));
	
	fruit->setTag(fruitType); // ���� ������ �±׷μ� ����

	auto fruitSize = fruit->getContentSize(); // ������ ũ�⸦ ���
	float fruitXPos = rand() % static_cast<int>(winSize.width); // X���� ������ ��ġ�� ����

	fruit->setPosition(Vec2(fruitXPos, winSize.height - FRUIT_TOP_MARGIN - fruitSize.height / 2.0));

	// this->addChild(fruit);
	// BatchNode�� ������ �߰�
	_fruitsBatchNode->addChild(fruit);
	_fruits.pushBack(fruit); // _fruits���Ϳ� ������ �߰�.

	// ���Ͽ� �������� �߰�
	auto ground = Vec2(fruitXPos, -150);

	// x�ʿ� ���� ground ��ġ�� ����
	auto fall = MoveTo::create(FALLING_DURATION, ground);

	auto remove = CallFuncN::create([this](Node *node) {
		
		auto sprite = dynamic_cast<Sprite*> (node);

		this->removeFruit(sprite);
	});

	// fall�� remove�� �����ؼ� �����ϱ� ���� �׼�
	auto sequence = Sequence::create(fall, remove, NULL);

	fruit->runAction(sequence);

	auto swing = Repeat::create(Sequence::create(RotateTo::create(0.25f, -30.0f), RotateTo::create(0.25f, 30.0f), NULL), 2);
	fruit->setScale(0);
	fruit->runAction(Sequence::create(ScaleTo::create(0.25f, 1.0f),
		swing,
		RotateTo::create(0.0f, 0.125f),
		fall,
		remove,
		NULL));

	return fruit;
}

bool MainScene::removeFruit(Sprite *fruit)
{
	// _fruits�� fruit�� ���ԵǾ��ִ°��� Ȯ��
	if (_fruits.contains(fruit)) {
		// �� ��带 ����
		fruit->removeFromParent();
		// _fruits�迭���� ����
		_fruits.eraseObject(fruit);

		return true;
	}

	return false;
}

void MainScene::update(float dt)
{

	if (_state == GameState::PLAYING) 
	{
		// �� ������ ����
		// int random = rand() % FRUIT_SPAWN_RATE;

		float pastTime = TIME_LIMIT_SECOND - _second;
		float p = FRUIT_SPAWN_INCREASE_BASE * (1 + powf(FRUIT_SPAWN_INCREASE_RATE, pastTime));
		p = MIN(p, MAXIMUM_SPAWN_PROBABILITY); // p�� �ִ�ġ�� ��ġ �ʵ���

		float random = this->generateRandom(0, 1);
		if (random < p) { // �������� �̵Ǹ�
			this->addFruit();
		}

		for (auto fruit : _fruits)
		{
			auto busketPosition = _player->getPosition() - Vec2(0, 10);
			bool isHit = fruit->getBoundingBox().containsPoint(busketPosition);

			if (isHit)
			{
				this->catchFruit(fruit);

				break;
			}
		}

		// ���� �ð� ���̱�
		_second -= dt;

		// �����ð� ����
		int second = static_cast<int>(_second);
		_secondLabel->setString(StringUtils::toString(second));

		if (_second < 0)
		{
			_state = GameState::ENDING;
			
			// ���� ���� ǥ��
			auto finish = Sprite::create("finish.png");
			auto winSize = Director::getInstance()->getWinSize();
			finish->setPosition(Vec2(winSize.width / 2.0f, winSize.height / 2.0f));
			finish->setScale(0);
			CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("finish.mp3");

			// �׼� �ۼ�
			auto appear = EaseExponentialIn::create(ScaleTo::create(0.25f, 1.0f));
			auto disappear = EaseExponentialIn::create(ScaleTo::create(0.25f, 0.0f));

			finish->runAction(Sequence::create(appear,
				DelayTime::create(2.0f),
				disappear,
				DelayTime::create(1.0f),
				CallFunc::create([this] {
			
				_state = GameState::RESULT;
				this->onResult();
				}),
				NULL));

			this->addChild(finish);
		}

	}
}

void MainScene::catchFruit(cocos2d::Sprite *fruit)
{
	// ������� ȿ����
	auto audioEngine = CocosDenshion::SimpleAudioEngine::getInstance();

	if (this->getIsCrash())
	{
		return;
	}

	// ���� Ÿ�� ��� 
	FruitType fruitType = static_cast<FruitType>(fruit->getTag());

	switch (fruitType)
	{
		case MainScene::FruitType::GOLDEN:
			_score += GOLDEN_FRUIT_SCORE;
			audioEngine->playEffect("catch_golden.mp3");
			break;

		case MainScene::FruitType::BOMB:

			// ��ź ����
			this->onCatchBomb();
			audioEngine->playEffect("catch_bomb.mp3");
			this->addBombEffect(fruit->getPosition());

			break;

		default:
			_score += 1;
			audioEngine->playEffect("catch_fruit.mp3");
			break;

	}

	// ���̽��ھ� ����
	if (_highScore > 0 && !_isHighScore && _score > _highScore) {
		// ���� ���� ������ ������, �� �ѹ� ���� ����
		_isHighScore = true;
		audioEngine->playEffect("highscore.mp3");
	}

	this->removeFruit(fruit);
	_scoreLabel->setString(StringUtils::toString(_score));
}

void MainScene::onResult()
{
	_state = GameState::RESULT;
	auto winSize = Director::getInstance()->getWinSize();

	auto replayButton = MenuItemImage::create("replay_button.png", "replay_button_pressed.png", [](Ref* ref) {
		auto scene = MainScene::createScene();
		Director::getInstance()->replaceScene(scene);
	});

	auto titleButton = MenuItemImage::create("title_button.png", "title_button_pressed.png", [](Ref* ref) {
		// Ÿ��Ʋ�� ���ư��� ��ư ������ ��
		auto scene = TitleScene::createScene();
		auto transition = TransitionCrossFade::create(1.0f, scene);
		Director::getInstance()->replaceScene(transition);

	});

	// �޴� �ۼ�
	auto menu = Menu::create(replayButton, titleButton, NULL);
	menu->alignItemsVerticallyWithPadding(30);
	menu->setPosition(Vec2(winSize.width / 2.0f, winSize.height / 2.0f));
	this->addChild(menu);

	// ���̽��ھ� ����
	auto highscore = UserDefault::getInstance()->getIntegerForKey(HIGHSCORE_KEY);
	if (_score > highscore) {
		_highscoreLabel->setString(StringUtils::toString(_score));
		UserDefault::getInstance()->setIntegerForKey(HIGHSCORE_KEY, _score);
	}

	// BGM���
	CocosDenshion::SimpleAudioEngine::getInstance()->playBackgroundMusic("result.mp3", true);

}

void MainScene::onEnterTransitionDidFinish()
{
	Layer::onEnterTransitionDidFinish();

	// BGM ���
	CocosDenshion::SimpleAudioEngine::getInstance()->playBackgroundMusic("main.mp3", true);

	// READY ������ ����
	this->addReadyLabel();
}

void MainScene::onCatchBomb()
{
	_isCrash = true;

	Vector<SpriteFrame*> frames;

	auto playerSize = _player->getContentSize();
	const int animationFrameCount = 3;

	for (int i = 0; i < animationFrameCount; ++i)
	{
		auto rect = Rect(playerSize.width * i, 0, playerSize.width, playerSize.height);

		auto frame = SpriteFrame::create("player_crash.png", rect);
		frames.pushBack(frame);
	}

	auto animaion = Animation::createWithSpriteFrames(frames, 10.0f / 60.0f);
	animaion->setLoops(3);
	animaion->setRestoreOriginalFrame(true);
	_player->runAction(Sequence::create(Animate::create(animaion), CallFunc::create([this] {
		_isCrash = false;

	}),
		
		NULL));

	_score = MAX(0, _score - BOMB_PENALTY_SCORE);

	CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("crash.mp3");
}

void MainScene::addReadyLabel()
{
	auto winSize = Director::getInstance()->getWinSize();
	auto center = Vec2(winSize.width / 2.0f, winSize.height / 2.0f);

	// Ready���� ����
	auto ready = Sprite::create("ready.png");
	ready->setScale(0); // ���� ũ�⸦ 0%�� ����
	ready->setPosition(center);
	this->addChild(ready);
	
	// Start ���� ����
	/*auto start = Sprite::create("start.png");
	start->runAction(Sequence::create(CCSpawn::create(EaseIn::create(ScaleTo::create(0.5f, 0.5f), 0.5f),
		FadeOut::create(0.5f), // 0.5�ʿ� ���ļ� ũ�� �������
		NULL),
		RemoveSelf::create(), // �ڽ��� ����
		NULL));
	*/

	// Ready�� ���ϸ��̼��� �߰�
	ready->runAction(Sequence::create(ScaleTo::create(0.25f, 1.0f), // 0.25�ʿ� ���ļ� Ȯ��
		DelayTime::create(1.0f),  // 1�� ��ٸ�
		CallFunc::create([this, center] {
		auto start = Sprite::create("start.png");
		start->setPosition(center);
		start->runAction(Sequence::create(CCSpawn::create(EaseIn::create(ScaleTo::create(0.5f, 0.5f), 0.5f),
			FadeOut::create(0.5f), // 0.5�ʿ� ���ļ� ũ�� �������
			NULL),
			RemoveSelf::create(), // �ڽ��� ����
			NULL));
		this->addChild(start);
		_state = GameState::PLAYING; // ���ӻ��¸� PLAYING���� ����
		CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("start.mp3");
	}),
		RemoveSelf::create(), // �ڽ��� ����
		NULL));
}

float MainScene::generateRandom(float min, float max)
{
	std::uniform_real_distribution<float> dest(min, max);
	return dest(_engine);
}

void MainScene::addBombEffect(cocos2d::Vec2 position)
{
	auto effect = Sprite::create("bomb_effect.png");
	auto size = effect->getContentSize();
	const int frameCount = 3;
	auto frameWidth = size.width / frameCount;

	effect->setTextureRect(Rect(0, 0, frameWidth, size.height));
	effect->setPosition(position);
	effect->setOpacity(0);
	Vector<SpriteFrame *> frames;
	for (int i = 0; i < frameCount; ++i) {
		auto frame = SpriteFrame::create("bomb_effect.png",
			Rect(frameWidth * i, 0, frameWidth, size.height));
		frames.pushBack(frame);
	}
	auto animation = Animation::createWithSpriteFrames(frames, 0.05);
	effect->runAction(Sequence::create(FadeIn::create(0.1),
		Animate::create(animation),
		FadeOut::create(0.1),
		RemoveSelf::create(),
		NULL));
	this->addChild(effect);
}
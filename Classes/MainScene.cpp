#include "MainScene.h"
#include "TitleScene.h"
#include "SimpleAudioEngine.h"

USING_NS_CC;


// 과일의 화면상단부터의 margin
const int FRUIT_TOP_MARGIN = 30;

// 낙하속도
const float FALLING_DURATION = 2.2f;

// 과일의 출현률
const int FRUIT_SPAWN_RATE = 2;

// 황금 과일 기본 출현률 초기치
const float GOLDEN_FRUIT_PROBABILITY_BASE = 0.01;

// 폭탄 출현률 초기치
const float BOMB_PROBABILITY_BASE = 0.05;
// 황금과일 출현률 증가치
const float GOLDEN_FRUIT_PROBABILITY_RATE = 0.00;
// 폭탄 출현률 증가치
const float BOMB_PROBABILITY_RATE = 0.00;
// 일반 과일 수
const int NORMAL_FRUIT_COUNT = 5;
// 시간 제한
const float TIME_LIMIT_SECOND = 14.0f;

// 황금 과일을 잡았을때 점수
const int GOLDEN_FRUIT_SCORE = 5;

// 폭탄를 잡았을때
const int BOMB_PENALTY_SCORE = 4;

// 일반 과일 출현률 초기치
const float FRUIT_SPAWN_INCREASE_BASE = 0.1;
// 일반 과일 출현률 증가률
const float FRUIT_SPAWN_INCREASE_RATE = 0.0f;
// 과일 출현률 최대치
const float MAXIMUM_SPAWN_PROBABILITY = 0.5;

// 최고 점수 취득
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
	// 난수 초기화
	std::random_device rdev;
	_engine.seed(rdev());
}


MainScene::~MainScene()
{
	CC_SAFE_RELEASE_NULL(_player); // _player를 release하여 메모리 릭을 방지
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

	this->setPlayer(Sprite::create("player.png")); // Sprite를 생성해서 _player에 각납

	_player->setPosition(Vec2(size.width / 2.0, size.height - 445)); // _player 위치 설정

	this->addChild(_player); // 씬에 _player를 배치

	// 터치를 취득
	auto listener = EventListenerTouchOneByOne::create();
	listener->onTouchBegan = [](Touch* touch, Event* event) {
		// 터치 되었을 때의 처리

		return true;
	};

	listener->onTouchMoved = [this](Touch* touch, Event* event) {

		if (!this->getIsCrash())
		{
			// 터치가 중단 되었을때의 처리
			// 이전 터치 위치와의 차이를 벡터로 취득
			Vec2 delta = touch->getDelta();

			// 현재 게임 화면의 좌표를 취득
			Vec2 position = _player->getPosition();

			// 현재 좌표 + 이동량을 새로운 좌표로 한다.
			Vec2 newPosition = position + delta;

			auto winSize = Director::getInstance()->getWinSize(); // 화면 사이즈를 취득

			newPosition = newPosition.getClampPoint(Vec2(0, position.y), Vec2(winSize.width, position.y));

			if (newPosition.x < 0) { // 만약 새로운 위치가 화면 왼쪽끝 보다 왼쪽이라면
				newPosition.x = 0; // x좌표를 0으로 한다.
			}
			else if (newPosition.x > winSize.width) { // 만약 오른쪽 끝 보다 오른쪽이면
				newPosition.x = winSize.width; // x좌표를 화면 사이즈로 한다.
			}

			// 세로방향으로는 이동하지 않도록 함.
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

	// 스코어 헤드
	auto scoreLabelHeader = Label::createWithTTF("SCORE", "Marker Felt.ttf", 16);

	scoreLabelHeader->enableShadow(Color4B::BLACK, Size(0.5f, 0.5f), 3);
	scoreLabelHeader->enableOutline(Color4B::BLACK, 1.5f);
	scoreLabelHeader->setPosition(Vec2(size.width / 2.0f * 1.5f, size.height - 20));
	this->addChild(scoreLabelHeader);

	// 타이머
	int second = static_cast<int>(_second);
	auto secondLabel = Label::createWithTTF(StringUtils::toString(second), "Marker Felt.ttf", 16);
	secondLabel->enableShadow(Color4B::BLACK, Size(0.5f, 0.5f), 3);
	secondLabel->enableOutline(Color4B::BLACK, 1.5f);
	secondLabel->setPosition(Vec2(size.width / 2.0f, size.height - 40));
	this->setSecondLabel(secondLabel);
	this->addChild(secondLabel);

	// 타이머 헤더
	auto secondLabelHeader = Label::createWithTTF("TIME", "Marker Felt.ttf", 16);
	secondLabelHeader->enableShadow(Color4B::BLACK, Size(0.5f, 0.5f), 3);
	secondLabelHeader->enableOutline(Color4B::BLACK, 1.5f);
	secondLabelHeader->setPosition(Vec2(size.width / 2.0f, size.height - 20));
	this->addChild(secondLabelHeader);

	// 하이스코어 라벨 추가
	auto highscore = UserDefault::getInstance()->getIntegerForKey(HIGHSCORE_KEY);
	auto highscoreLabel = Label::createWithTTF(StringUtils::toString(static_cast<int>(highscore)), "Marker Felt.ttf", 16);
	highscoreLabel->enableShadow(Color4B::BLACK, Size(0.5, 0.5), 3);
	highscoreLabel->enableOutline(Color4B::BLACK, 1.5);
	highscoreLabel->setPosition(Vec2(size.width / 2.0 * 0.5, size.height - 40));
	this->setHighscoreLabel(highscoreLabel);
	this->addChild(_highscoreLabel);
	this->setHighScore(highscore);

	// 하이스코어 헤더에 추가
	auto highscoreLabelHeader = Label::createWithTTF("HIGHSCORE", "Marker Felt.ttf", 16);
	highscoreLabelHeader->enableShadow(Color4B::BLACK, Size(0.5, 0.5), 3);
	highscoreLabelHeader->enableOutline(Color4B::BLACK, 1.5);
	highscoreLabelHeader->setPosition(Vec2(size.width / 2.0 * 0.5, size.height - 20));
	this->addChild(highscoreLabelHeader);

	// BatchNode초기화
	auto fruitsBatchNode = SpriteBatchNode::create("fruits.png");
	this->addChild(fruitsBatchNode);
	this->setFruitBatchNode(fruitsBatchNode);

	// update를 할때마다 프레임 실행하기 위해 등록
	this->scheduleUpdate();

	return true;
}

Sprite* MainScene::addFruit()
{
	// 화면사이즈를 취득
	auto winSize = Director::getInstance()->getWinSize();
	// 과일 종류를 선택
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

	// 과일 작성
	// std::string filename = StringUtils::format("fruit%d.png", fruitType);
	// auto fruit = Sprite::create(filename);
	
	// 텍스쳐 사이즈를 꺼냄
	auto textureSize = _fruitsBatchNode->getTextureAtlas()->getTexture()->getContentSize();
	// 텍스쳐 폭으로 과일을 하나하나 꺼냄
	auto fruitWidth = textureSize.width / static_cast<int>(FruitType::COUNT);
	auto fruit = Sprite::create("fruits.png", Rect(fruitWidth * fruitType, 0, fruitWidth, textureSize.height));
	
	fruit->setTag(fruitType); // 과일 종류를 태그로서 지정

	auto fruitSize = fruit->getContentSize(); // 과일의 크기를 취득
	float fruitXPos = rand() % static_cast<int>(winSize.width); // X축의 랜덤한 위치를 선택

	fruit->setPosition(Vec2(fruitXPos, winSize.height - FRUIT_TOP_MARGIN - fruitSize.height / 2.0));

	// this->addChild(fruit);
	// BatchNode에 과일을 추가
	_fruitsBatchNode->addChild(fruit);
	_fruits.pushBack(fruit); // _fruits벡터에 과일을 추가.

	// 과일에 움직임을 추가
	auto ground = Vec2(fruitXPos, -150);

	// x초에 걸쳐 ground 위치로 낙하
	auto fall = MoveTo::create(FALLING_DURATION, ground);

	auto remove = CallFuncN::create([this](Node *node) {
		
		auto sprite = dynamic_cast<Sprite*> (node);

		this->removeFruit(sprite);
	});

	// fall과 remove를 연속해서 실행하기 위한 액션
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
	// _fruits에 fruit이 포함되어있는가를 확인
	if (_fruits.contains(fruit)) {
		// 새 노드를 삭제
		fruit->removeFromParent();
		// _fruits배열에서 삭제
		_fruits.eraseObject(fruit);

		return true;
	}

	return false;
}

void MainScene::update(float dt)
{

	if (_state == GameState::PLAYING) 
	{
		// 매 프레임 실행
		// int random = rand() % FRUIT_SPAWN_RATE;

		float pastTime = TIME_LIMIT_SECOND - _second;
		float p = FRUIT_SPAWN_INCREASE_BASE * (1 + powf(FRUIT_SPAWN_INCREASE_RATE, pastTime));
		p = MIN(p, MAXIMUM_SPAWN_PROBABILITY); // p가 최대치를 넘치 않도록

		float random = this->generateRandom(0, 1);
		if (random < p) { // 램덤수가 이되면
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

		// 남은 시간 줄이기
		_second -= dt;

		// 남은시간 갱신
		int second = static_cast<int>(_second);
		_secondLabel->setString(StringUtils::toString(second));

		if (_second < 0)
		{
			_state = GameState::ENDING;
			
			// 종료 문자 표시
			auto finish = Sprite::create("finish.png");
			auto winSize = Director::getInstance()->getWinSize();
			finish->setPosition(Vec2(winSize.width / 2.0f, winSize.height / 2.0f));
			finish->setScale(0);
			CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("finish.mp3");

			// 액션 작성
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
	// 잡았을때 효과음
	auto audioEngine = CocosDenshion::SimpleAudioEngine::getInstance();

	if (this->getIsCrash())
	{
		return;
	}

	// 과일 타입 취득 
	FruitType fruitType = static_cast<FruitType>(fruit->getTag());

	switch (fruitType)
	{
		case MainScene::FruitType::GOLDEN:
			_score += GOLDEN_FRUIT_SCORE;
			audioEngine->playEffect("catch_golden.mp3");
			break;

		case MainScene::FruitType::BOMB:

			// 폭탄 취득시
			this->onCatchBomb();
			audioEngine->playEffect("catch_bomb.mp3");
			this->addBombEffect(fruit->getPosition());

			break;

		default:
			_score += 1;
			audioEngine->playEffect("catch_fruit.mp3");
			break;

	}

	// 하이스코어 판정
	if (_highScore > 0 && !_isHighScore && _score > _highScore) {
		// 제일 높은 점수를 냈을때, 딱 한번 사운드 실행
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
		// 타이틀로 돌아가는 버튼 눌렀을 때
		auto scene = TitleScene::createScene();
		auto transition = TransitionCrossFade::create(1.0f, scene);
		Director::getInstance()->replaceScene(transition);

	});

	// 메뉴 작성
	auto menu = Menu::create(replayButton, titleButton, NULL);
	menu->alignItemsVerticallyWithPadding(30);
	menu->setPosition(Vec2(winSize.width / 2.0f, winSize.height / 2.0f));
	this->addChild(menu);

	// 하이스코어 갱신
	auto highscore = UserDefault::getInstance()->getIntegerForKey(HIGHSCORE_KEY);
	if (_score > highscore) {
		_highscoreLabel->setString(StringUtils::toString(_score));
		UserDefault::getInstance()->setIntegerForKey(HIGHSCORE_KEY, _score);
	}

	// BGM재생
	CocosDenshion::SimpleAudioEngine::getInstance()->playBackgroundMusic("result.mp3", true);

}

void MainScene::onEnterTransitionDidFinish()
{
	Layer::onEnterTransitionDidFinish();

	// BGM 재생
	CocosDenshion::SimpleAudioEngine::getInstance()->playBackgroundMusic("main.mp3", true);

	// READY 연출을 실행
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

	// Ready문자 연출
	auto ready = Sprite::create("ready.png");
	ready->setScale(0); // 최초 크기를 0%로 설정
	ready->setPosition(center);
	this->addChild(ready);
	
	// Start 문자 연출
	/*auto start = Sprite::create("start.png");
	start->runAction(Sequence::create(CCSpawn::create(EaseIn::create(ScaleTo::create(0.5f, 0.5f), 0.5f),
		FadeOut::create(0.5f), // 0.5초에 걸쳐서 크게 만들어짐
		NULL),
		RemoveSelf::create(), // 자신을 삭제
		NULL));
	*/

	// Ready에 에니메이션을 추가
	ready->runAction(Sequence::create(ScaleTo::create(0.25f, 1.0f), // 0.25초에 걸쳐서 확대
		DelayTime::create(1.0f),  // 1초 기다림
		CallFunc::create([this, center] {
		auto start = Sprite::create("start.png");
		start->setPosition(center);
		start->runAction(Sequence::create(CCSpawn::create(EaseIn::create(ScaleTo::create(0.5f, 0.5f), 0.5f),
			FadeOut::create(0.5f), // 0.5초에 걸쳐서 크게 만들어짐
			NULL),
			RemoveSelf::create(), // 자신을 삭제
			NULL));
		this->addChild(start);
		_state = GameState::PLAYING; // 게임상태를 PLAYING으로 변경
		CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("start.mp3");
	}),
		RemoveSelf::create(), // 자신을 삭제
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
#include "TitleScene.h"
#include "MainScene.h"
#include "SimpleAudioEngine.h"

USING_NS_CC;

Scene* TitleScene::createScene()
{
	auto scene = Scene::create();
	auto layer = TitleScene::create();
	scene->addChild(layer);
	return scene;
}

TitleScene::TitleScene()
{

}


TitleScene::~TitleScene()
{
}

bool TitleScene::init()
{
	if (!Layer::init())
	{
		return false;
	}

	auto diretor = Director::getInstance();
	auto winSize = diretor->getWinSize();

	// 배경 추가
	auto background = Sprite::create("title_background.png");
	background->setPosition(Vec2(winSize.width / 2.0f, winSize.height / 2.0f));
	this->addChild(background);

	// 로고 추가
	auto logo = Sprite::create("title_logo.png");
	logo->setPosition(Vec2(winSize.width / 2.0f, winSize.height / 2.0f));
	this->addChild(logo);

	// Touch to Start 추가
	auto touchToStart = Sprite::create("title_start.png");
	touchToStart->setPosition(Vec2(winSize.width / 2.0f, 90.0f));
	this->addChild(touchToStart);
	// 점멸하도록 하는 액션
	auto blink = Sequence::create(FadeTo::create(0.5, 127)
		, FadeTo::create(0.5, 255)
		, NULL);
	touchToStart->runAction(RepeatForever::create(blink));
	this->addChild(touchToStart);

	// 화면을 터치했을때 메인화면에 이동
	auto listener = EventListenerTouchOneByOne::create();
	listener->onTouchBegan = [this](Touch* touch, Event* event)
	{
		CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("decide.mp3");
		// 몇번이고 눌러지지 않도록 하는 처리, 한번 눌리면 다음 동작을 무효화 시킴
		this->getEventDispatcher()->removeAllEventListeners();
		// 0.5초 기다리고 CallFunc을 부른다.
		auto delay = DelayTime::create(0.5f);
		// 게임이 시작되는 액션
		auto startGame = CallFunc::create([] {
			auto scene = MainScene::createScene();
			auto transition = TransitionPageTurn::create(0.5f, scene, true);
			Director::getInstance()->replaceScene(transition);
		});
		this->runAction(Sequence::create(delay, startGame, NULL));

		return true;
	};
}

void TitleScene::onEnterTransitionDidFinish()
{
	// bgm재생
	CocosDenshion::SimpleAudioEngine::getInstance()->playBackgroundMusic("title.mp3", true);
}

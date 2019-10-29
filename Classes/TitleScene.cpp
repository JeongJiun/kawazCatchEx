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

	// ��� �߰�
	auto background = Sprite::create("title_background.png");
	background->setPosition(Vec2(winSize.width / 2.0f, winSize.height / 2.0f));
	this->addChild(background);

	// �ΰ� �߰�
	auto logo = Sprite::create("title_logo.png");
	logo->setPosition(Vec2(winSize.width / 2.0f, winSize.height / 2.0f));
	this->addChild(logo);

	// Touch to Start �߰�
	auto touchToStart = Sprite::create("title_start.png");
	touchToStart->setPosition(Vec2(winSize.width / 2.0f, 90.0f));
	this->addChild(touchToStart);
	// �����ϵ��� �ϴ� �׼�
	auto blink = Sequence::create(FadeTo::create(0.5, 127)
		, FadeTo::create(0.5, 255)
		, NULL);
	touchToStart->runAction(RepeatForever::create(blink));
	this->addChild(touchToStart);

	// ȭ���� ��ġ������ ����ȭ�鿡 �̵�
	auto listener = EventListenerTouchOneByOne::create();
	listener->onTouchBegan = [this](Touch* touch, Event* event)
	{
		CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("decide.mp3");
		// ����̰� �������� �ʵ��� �ϴ� ó��, �ѹ� ������ ���� ������ ��ȿȭ ��Ŵ
		this->getEventDispatcher()->removeAllEventListeners();
		// 0.5�� ��ٸ��� CallFunc�� �θ���.
		auto delay = DelayTime::create(0.5f);
		// ������ ���۵Ǵ� �׼�
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
	// bgm���
	CocosDenshion::SimpleAudioEngine::getInstance()->playBackgroundMusic("title.mp3", true);
}

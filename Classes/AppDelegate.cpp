#include "AppDelegate.h"
#include "SimpleAudioEngine.h"
#include "cocos2d.h"
#include "HelloWorldScene.h"
#include "CCLobbyView.h"

USING_NS_CC;
using namespace CocosDenshion;

AppDelegate::AppDelegate() {

}

AppDelegate::~AppDelegate() 
{
}

bool AppDelegate::applicationDidFinishLaunching() {
    // initialize director
    CCDirector* pDirector = CCDirector::sharedDirector();
    CCEGLView* pEGLView = CCEGLView::sharedOpenGLView();

    pDirector->setOpenGLView(pEGLView);

    CCSize screenSize = pDirector->getWinSize();

    //set design screen size to the iPad and ExactFit (image is stretched to fill screen
    CCEGLView::sharedOpenGLView()->setDesignResolutionSize(768, 1024, kResolutionShowAll);
    //if device's screen width is greater than 768, use high definition images
    if (screenSize.width > 768) {
        cocos2d::CCFileUtils::sharedFileUtils()->addSearchPath("hd");
//        CCFileUtils::sharedFileUtils()->setResourceDirectory("hd");
        pDirector->setContentScaleFactor(2);
        //or else, use standard definition
    } else {
        cocos2d::CCFileUtils::sharedFileUtils()->addSearchPath("sd");
//        CCFileUtils::sharedFileUtils()->setResourceDirectory("sd");
        pDirector->setContentScaleFactor(1);
    }
    
    //preload sound effects
    SimpleAudioEngine::sharedEngine()->preloadEffect( (CCFileUtils::sharedFileUtils()->fullPathForFilename("hit.wav")).c_str() );
    SimpleAudioEngine::sharedEngine()->preloadEffect( (CCFileUtils::sharedFileUtils()->fullPathForFilename("score.wav")).c_str() );
    // turn on display FPS
    pDirector->setDisplayStats(true);

    // set FPS. the default value is 1.0/60 if you don't call this
    pDirector->setAnimationInterval(1.0 / 60);

    // create a scene. it's an autorelease object
//    CCScene *pScene = HelloWorld::scene();
    CCScene *pScene = CCLobbyView::scene();

    // run
    pDirector->runWithScene(pScene);

    return true;
}

// This function will be called when the app is inactive. When comes a phone call,it's be invoked too
void AppDelegate::applicationDidEnterBackground() {
    CCDirector::sharedDirector()->stopAnimation();

    // if you use SimpleAudioEngine, it must be pause
    // SimpleAudioEngine::sharedEngine()->pauseBackgroundMusic();
}

// this function will be called when the app is active again
void AppDelegate::applicationWillEnterForeground() {
    CCDirector::sharedDirector()->startAnimation();

    // if you use SimpleAudioEngine, it must resume here
    // SimpleAudioEngine::sharedEngine()->resumeBackgroundMusic();
}

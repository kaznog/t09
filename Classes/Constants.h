//
//  Constants.h
//  AirHockey
//
//  Created by noguchika on 2014/02/05.
//
//

#ifndef AirHockey_Constants_h
#define AirHockey_Constants_h

const unsigned char PHOTON_SENDDATATYPE_GAMEDATA = 0;
const unsigned char PHOTON_SENDDATATYPE_CLIENT   = 1;
const unsigned char PHOTON_SENDDATATYPE_SCORE    = 2;
const unsigned char PHOTON_SENDDATATYPE_READY    = 3;
const unsigned char PHOTON_SENDDATATYPE_CHAT     = 4;
const unsigned char PHOTON_SENDDATATYPE_MISC     = 5;
// type of transfer for inGame-data ()
const unsigned char PHOTON_UNRELIABLE   = 0;
const unsigned char PHOTON_RELIABLE     = 1;

// Keys for PlayerStatus-Events
const unsigned char STATUS_PLAYER_POS_X = 0;
const unsigned char STATUS_PLAYER_POS_Y = 1;
const unsigned char STATUS_PLAYER_VEC_X = 2;
const unsigned char STATUS_PLAYER_VEC_Y = 3;
#if( CC_TARGET_PLATFORM == CC_PLATFORM_IOS )
static const char f_AmericanTypewriter[] = "American Typewriter";
#else
static const char f_AmericanTypewriter[] = "fonts/American Typewriter.ttf";
#endif

#define FONT_SIZE(__SIZE) (cocos2d::CCEGLView::sharedOpenGLView()->getDesignResolutionSize().width / 480 * __SIZE)

const float kCCRoomTableCellFontSize = 24.0f;

#endif

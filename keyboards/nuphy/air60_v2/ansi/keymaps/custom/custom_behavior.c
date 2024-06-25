#include "custom_behavior.h"
#include "ansi.h"
#include "user_kb.h"
#include "sendstring_hungarian.h"

#define PASSWORD "Bullshit password"


#define NO_MOD                  0
#define ALTGR_MOD               1
#define SHIFT_MOD               2

#define OFFSET(key)             (key - FIRST_CUSTOM_KEY)

#define ANY_SHIFT_ACTIVE        (LSHIFT_ACTIVE || RSHIFT_ACTIVE)
#define IS_KEY_SHIFTED(key)     (KEY_MODIFIED[OFFSET(key)] &= SHIFT_MOD)
#define IS_KEY_ALTGRED(key)     (KEY_MODIFIED[OFFSET(key)] &= ALTGR_MOD)
#define SHIFT_KEY(key)          (KEY_MODIFIED[OFFSET(key)] |= SHIFT_MOD)
#define ALTGR_KEY(key)          (KEY_MODIFIED[OFFSET(key)] |= ALTGR_MOD)
#define CLEAR_MOD_ON_KEY(key)   (KEY_MODIFIED[OFFSET(key)] = NO_MOD)
#define ACTIVE_SHIFT            ((LSHIFT_ACTIVE) ? KC_LSFT : KC_RSFT)


//Custom Keycodes flags

bool LSHIFT_ACTIVE = false;
bool RSHIFT_ACTIVE = false;
bool ALTGR_ACTIVE = false;
bool SHIFT_NEEDED = false;
bool ALTGR_NEEDED = false;
uint8_t KEY_MODIFIED[OFFSET(LAST_CUSTOM_KEY)];
uint16_t PRESSED_CUSTOM_KEYS[10];


//MACRO global variables
uint32_t timeMouse = 0 ;
uint32_t timeMouseLED = 0 ;
uint16_t MouseJigglerInterval = 1000;
bool MouseJigglerEnabled = false;
bool MouseJigglerLED = false;


tap_dance_action_t tap_dance_actions[] = {
    [TD_A] = ACTION_TAP_DANCE_DOUBLE(KC_A, KC_QUOT),
    [TD_E] = ACTION_TAP_DANCE_DOUBLE(KC_E, KC_SCLN),
    [TD_I] = ACTION_TAP_DANCE_DOUBLE(KC_I, KC_NUBS),
    [TD_O] = ACTION_TAP_DANCE_DOUBLE(KC_O, KC_EQL),
    [TD_U] = ACTION_TAP_DANCE_DOUBLE(KC_U, KC_RBRC),
    [TD_OO] = ACTION_TAP_DANCE_DOUBLE(KC_0, KC_LBRC),
    [TD_UU] = ACTION_TAP_DANCE_DOUBLE(KC_MINS, KC_BSLS),
};

bool rgb_matrix_indicators_advanced_user(uint8_t led_min, uint8_t led_max)
{
    if (get_highest_layer(layer_state) > 0)
    {
        uint8_t layer = get_highest_layer(layer_state);
        for (uint8_t row = 0; row < MATRIX_ROWS; ++row)
        {
            for (uint8_t col = 0; col < MATRIX_COLS; ++col)
            {
                uint8_t index = g_led_config.matrix_co[row][col];
                if (index >= led_min && index < led_max && index != NO_LED &&
                keymap_key_to_keycode(layer, (keypos_t){col,row}) > KC_TRNS)
                {
                    rgb_matrix_set_color(index, RGB_RED);
                }
            }
        }
    }
    if (MouseJigglerEnabled)
    {
        if (timer_elapsed(timeMouseLED) > 500) 
        {
            MouseJigglerLED = !MouseJigglerLED;
            timeMouseLED = timer_read();
        }
        if (MouseJigglerLED)
        {
            rgb_matrix_set_color(6, RGB_RED);   //NUM 6
            rgb_matrix_set_color(58, RGB_RED);  //SPACE
        }
        else{
            rgb_matrix_set_color(6, RGB_OFF);   //NUM 6
            rgb_matrix_set_color(58, RGB_OFF);  //SPACE
        }
    }
    return false;
}

void matrix_scan_user(void) {
    if (MouseJigglerEnabled && timer_elapsed(timeMouse) >= MouseJigglerInterval)
    {
        timeMouse = timer_read();
        uint16_t random =rand();
        MouseJigglerInterval =((random%60)+1)*1000;
        switch(random%4)
        {
            case 0:
                SEND_STRING(SS_TAP(X_MS_UP));
                SEND_STRING(SS_TAP(X_MS_DOWN));
                break;
            case 1:
                SEND_STRING(SS_TAP(X_MS_DOWN));
                SEND_STRING(SS_TAP(X_MS_UP));
                break;
            case 2:
                SEND_STRING(SS_TAP(X_MS_LEFT));
                SEND_STRING(SS_TAP(X_MS_RIGHT));
                break;
            case 3:
                SEND_STRING(SS_TAP(X_MS_RIGHT));
                SEND_STRING(SS_TAP(X_MS_LEFT));
        }
    }
}

bool process_record_user(uint16_t keycode, keyrecord_t *record)
{
    bool retVal = true;
    switch (keycode)
    {
//MODIFIERS
        //Key {LSFT}
        case KC_LSFT:
            if (record->event.pressed)
            {
                register_code(KC_LSFT);
                LSHIFT_ACTIVE = true;
            }
            else
            {
                LSHIFT_ACTIVE = false; 
            }
            retVal = false;
            break;

        //Key {RSFT}
        case KC_RSFT:
            if (record->event.pressed)
            {
                register_code(KC_RSFT);
                RSHIFT_ACTIVE = true;
            }
            else
            {
                RSHIFT_ACTIVE = false;
            }
            retVal = false;
            break;

        //Key {ALTGR}
        case KC_RALT:
            if (record->event.pressed)
            {
                register_code(KC_RALT);
                ALTGR_ACTIVE = true;
            }
            else
            {
                ALTGR_ACTIVE = false;
            }
            retVal = false;
            break;

// MACROS
        case MOUS_JIG:
            if (record->event.pressed)
            {
                MouseJigglerEnabled ^= 1;
                user_config.sleep_mode =  (MouseJigglerEnabled ? 0 : 1);
                timeMouse = timer_read();
                timeMouseLED = timer_read();
            }
            retVal = false;
            break;

        case PW_DPLY:
            if (record->event.pressed)
            {
                SEND_STRING(PASSWORD);
            }
            retVal = false;
            break;


// Custom Key Codes Starting now
    // ROW 5 Entire row, except 5 and backspace 

        // Key {ESC} --> [~,`]
        case CKC_GRV:
            if (record->event.pressed)
            {
                ALTGR_NEEDED = true;
                register_code(KC_RALT);
                if(ANY_SHIFT_ACTIVE)
                {
                    register_code(KC_7);
                    SHIFT_KEY(CKC_1);
                }
                else
                {
                    register_code(KC_1);
                }
            }
            else
            {
                ALTGR_NEEDED = false;
                if(IS_KEY_SHIFTED(CKC_1))
                {
                    unregister_code(KC_7);
                    CLEAR_MOD_ON_KEY(CKC_1);
                }
                else
                {
                    unregister_code(KC_1);
                }
            }
            retVal = false;
            break;

        // Key {1} --> [1, !]
        case CKC_1:
            if (record->event.pressed)
            {
                if(ANY_SHIFT_ACTIVE)
                {
                    register_code(KC_4);
                    SHIFT_NEEDED = true;
                    SHIFT_KEY(CKC_1);
                }
                else
                {
                    register_code(KC_1);
                }
            }
            else
            {
                if(IS_KEY_SHIFTED(CKC_1))
                {
                    unregister_code(KC_4);
                    SHIFT_NEEDED = false;
                    CLEAR_MOD_ON_KEY(CKC_1);
                }
                else
                {
                    unregister_code(KC_1);
                }
            }
            retVal = false;
            break;

        // Key {2} --> [2, @]
        case CKC_2:
            if (record->event.pressed)
            {
                if(ANY_SHIFT_ACTIVE)
                {
                    unregister_code(ACTIVE_SHIFT);
                    register_code(KC_RALT);
                    register_code(KC_V);
                    ALTGR_NEEDED = true;
                    SHIFT_KEY(CKC_2);
                }
                else
                {
                    register_code(KC_2);
                }
            }
            else
            {
                if(IS_KEY_SHIFTED(CKC_2))
                {
                    register_code(ACTIVE_SHIFT);
                    unregister_code(KC_RALT);
                    unregister_code(KC_V);
                    ALTGR_NEEDED = false;
                    CLEAR_MOD_ON_KEY(CKC_2);
                }
                else
                {
                    unregister_code(KC_2);
                }
            }
            retVal = false;
            break;

        // Key {3} --> [3, #]
        case CKC_3:
            if (record->event.pressed)
            {
                if(ANY_SHIFT_ACTIVE)
                {
                    unregister_code(ACTIVE_SHIFT);
                    register_code(KC_RALT);
                    register_code(KC_X);
                    ALTGR_NEEDED = true;
                    SHIFT_KEY(CKC_3);
                }
                else
                {
                    register_code(KC_3);
                }
            }
            else
            {
                if(IS_KEY_SHIFTED(CKC_3))
                {
                    register_code(ACTIVE_SHIFT);
                    unregister_code(KC_RALT);
                    unregister_code(KC_X);
                    ALTGR_NEEDED = false;
                    CLEAR_MOD_ON_KEY(CKC_3);
                }
                else
                {
                    unregister_code(KC_3);
                }
            }
            retVal = false;
            break;

        // Key {4} --> [4, $]
        case CKC_4:
            if (record->event.pressed)
            {
                if(ANY_SHIFT_ACTIVE)
                {
                    unregister_code(ACTIVE_SHIFT);
                    register_code(KC_RALT);
                    register_code(KC_SCLN);
                    ALTGR_NEEDED = true;
                    SHIFT_KEY(CKC_4);
                }
                else
                {
                    register_code(KC_4);
                }
            }
            else
            {
                if(IS_KEY_SHIFTED(CKC_4))
                {
                    register_code(ACTIVE_SHIFT);
                    unregister_code(KC_RALT);
                    unregister_code(KC_SCLN);
                    ALTGR_NEEDED = false;
                    CLEAR_MOD_ON_KEY(CKC_4);
                }
                else
                {
                    unregister_code(KC_4);
                }
            }
            retVal = false;
            break;

        //Key {6} --> [6, ^]
        case CKC_6:
            if (record->event.pressed)
            {
                if(ANY_SHIFT_ACTIVE)
                {
                    unregister_code(ACTIVE_SHIFT);
                    register_code(KC_RALT);
                    register_code(KC_3);
                    ALTGR_NEEDED = true;
                    SHIFT_KEY(CKC_6);
                }
                else
                {
                    register_code(KC_6);
                }
            }
            else
            {
                if(IS_KEY_SHIFTED(CKC_6))
                {
                    register_code(ACTIVE_SHIFT);
                    unregister_code(KC_RALT);
                    unregister_code(KC_3);
                    ALTGR_NEEDED = false;
                    CLEAR_MOD_ON_KEY(CKC_6);
                }
                else
                {
                    unregister_code(KC_6);
                }
            }
            retVal = false;
            break; 

        //Key {7} --> [7, &]
        case CKC_7:
            if (record->event.pressed)
            {
                if(ANY_SHIFT_ACTIVE)
                {
                    unregister_code(ACTIVE_SHIFT);
                    register_code(KC_RALT);
                    register_code(KC_C);
                    ALTGR_NEEDED = true;
                    SHIFT_KEY(CKC_7);
                }
                else
                {
                    register_code(KC_7);
                }
            }
            else
            {
                if(IS_KEY_SHIFTED(CKC_7))
                {
                    register_code(ACTIVE_SHIFT);
                    unregister_code(KC_RALT);
                    unregister_code(KC_C);
                    ALTGR_NEEDED = false;
                    CLEAR_MOD_ON_KEY(CKC_7);
                }
                else
                {
                    unregister_code(KC_7);
                }
            }
            retVal = false;
            break;

        //Key {8} --> [8, *]
        case CKC_8:
            if (record->event.pressed)
            {
                if(ANY_SHIFT_ACTIVE)
                {
                    unregister_code(ACTIVE_SHIFT);
                    register_code(KC_RALT);
                    register_code(KC_SLSH);
                    ALTGR_NEEDED = true;
                    SHIFT_KEY(CKC_8);
                }
                else
                {
                    register_code(KC_8);
                }
            }
            else
            {
                if(IS_KEY_SHIFTED(CKC_8))
                {
                    register_code(ACTIVE_SHIFT);
                    unregister_code(KC_RALT);
                    unregister_code(KC_SLSH);
                    ALTGR_NEEDED = false;
                    CLEAR_MOD_ON_KEY(CKC_8);
                }
                else
                {
                    unregister_code(KC_8);
                }
            }
            retVal = false;
            break;

        //Key {9} --> [9, (]
        case CKC_9:
            if (record->event.pressed)
            {
                if(ANY_SHIFT_ACTIVE)
                {
                    register_code(KC_8);
                    SHIFT_NEEDED = true;
                    SHIFT_KEY(CKC_9);
                }
                else
                {
                    register_code(KC_9);
                }
            }
            else
            {
                if(IS_KEY_SHIFTED(CKC_9))
                {
                    unregister_code(KC_8);
                    SHIFT_NEEDED = false;
                    CLEAR_MOD_ON_KEY(CKC_9);
                }
                else
                {
                    unregister_code(KC_9);
                }
            }
            retVal = false;
            break;

        //Key {0} --> [0, )]
        case CKC_0:
            if (record->event.pressed)
            {
                if(ANY_SHIFT_ACTIVE)
                {
                    register_code(KC_9);
                    SHIFT_NEEDED = true;
                    SHIFT_KEY(CKC_0);
                }
                else
                {
                    register_code(KC_GRV);
                }
            }
            else
            {
                if(IS_KEY_SHIFTED(CKC_0))
                {
                    unregister_code(KC_9);
                    SHIFT_NEEDED = false;
                    CLEAR_MOD_ON_KEY(CKC_0);
                }
                else
                {
                    unregister_code(KC_GRV);
                }
            }
            retVal = false;
            break;

        //Key {-} --> [-, _]
        case CKC_MINS:
            if (record->event.pressed)
            {
                if(ANY_SHIFT_ACTIVE)
                {
                    register_code(KC_SLSH);
                    SHIFT_NEEDED = true;
                    SHIFT_KEY(CKC_MINS);
                }
                else
                {
                    register_code(KC_SLSH);
                }
            }
            else
            {
                if(IS_KEY_SHIFTED(CKC_MINS))
                {
                    unregister_code(KC_SLSH);
                    SHIFT_NEEDED = false;
                    CLEAR_MOD_ON_KEY(CKC_MINS);
                }
                else
                {
                    unregister_code(KC_SLSH);
                }
            }
            retVal = false;
            break;

        //Key {=} --> [=, +]
        case CKC_EQL:
            if (record->event.pressed)
            {
                if(ANY_SHIFT_ACTIVE)
                {
                    register_code(KC_3);
                    SHIFT_KEY(CKC_EQL);
                }
                else
                {
                    register_code(KC_LSFT);
                    register_code(KC_7);
                }
                SHIFT_NEEDED = true;
            }
            else
            {
                if(IS_KEY_SHIFTED(CKC_EQL))
                {
                    unregister_code(KC_3);
                    CLEAR_MOD_ON_KEY(CKC_EQL);
                }
                else
                {
                    unregister_code(KC_7);
                }
                SHIFT_NEEDED = false;
            }
            retVal = false;
            break;

    // ROW 4 --> {[} {]} {\}
        //Key {[} --> [[, {]
        case CKC_LBRC:
            if (record->event.pressed)
            {
                if(ANY_SHIFT_ACTIVE)
                {
                    unregister_code(ACTIVE_SHIFT);
                    register_code(KC_RALT);
                    register_code(KC_B);
                    ALTGR_NEEDED = true;
                    SHIFT_KEY(CKC_LBRC);
                }
                else
                {
                    register_code(KC_RALT);
                    register_code(KC_F);
                    ALTGR_NEEDED = true;
                }
            }
            else
            {
                if(IS_KEY_SHIFTED(CKC_LBRC))
                {
                    unregister_code(KC_B);
                    register_code(ACTIVE_SHIFT);
                    CLEAR_MOD_ON_KEY(CKC_LBRC);
                    ALTGR_NEEDED = false;
                } 
                else
                {
                    unregister_code(KC_F);
                    ALTGR_NEEDED = false;
                }
            }
            retVal = false;
            break;

        //Key {]} --> [], }]
        case CKC_RBRC:
            if (record->event.pressed)
            {
                if(ANY_SHIFT_ACTIVE)
                {
                    unregister_code(ACTIVE_SHIFT);
                    register_code(KC_RALT);
                    register_code(KC_N);
                    SHIFT_KEY(CKC_RBRC);
                    ALTGR_NEEDED = true;
                }
                else
                {
                    register_code(KC_RALT);
                    register_code(KC_G);
                    ALTGR_NEEDED = true;
                }
            }
            else
            {
                if(IS_KEY_SHIFTED(CKC_RBRC))
                {
                    unregister_code(KC_N);
                    register_code(ACTIVE_SHIFT);
                    CLEAR_MOD_ON_KEY(CKC_RBRC);
                    ALTGR_NEEDED = false;
                }
                else
                {
                    unregister_code(KC_G);
                    ALTGR_NEEDED = false;
                }
            }
            retVal = false;
            break;

        //Key {\} --> [\, |]
        case CKC_BSLS:
            if (record->event.pressed)
            {
                if(ANY_SHIFT_ACTIVE)
                {
                    unregister_code(ACTIVE_SHIFT);
                    register_code(KC_RALT);
                    register_code(KC_W);
                    SHIFT_KEY(CKC_BSLS);
                    ALTGR_NEEDED = true;
                } 
                else
                {
                    register_code(KC_RALT);                                  
                    register_code(KC_Q);
                    ALTGR_NEEDED = true;
                }
            }
            else
            {
                if(IS_KEY_SHIFTED(CKC_BSLS)) {
                    unregister_code(KC_W);
                    register_code(ACTIVE_SHIFT);
                    CLEAR_MOD_ON_KEY(CKC_BSLS);
                    ALTGR_NEEDED = false;
                }
                else
                {
                    unregister_code(KC_Q);
                    ALTGR_NEEDED = false;
                }
            }
            retVal = false;
            break;

    //ROW 3 --> {;}{'}
        //Key {;} --> [;, :]
        case CKC_SCLN:
            if (record->event.pressed)
            {
                if(ANY_SHIFT_ACTIVE)
                {
                    register_code(KC_DOT);
                    SHIFT_KEY(CKC_SCLN);
                    SHIFT_NEEDED = true;
                }
                else
                {
                    register_code(KC_RALT);
                    register_code(KC_COMM);
                    ALTGR_NEEDED = true;
                }
            }
            else
            {
                if(IS_KEY_SHIFTED(CKC_SCLN))
                {
                    unregister_code(KC_DOT);
                    CLEAR_MOD_ON_KEY(CKC_SCLN);
                    SHIFT_NEEDED = false;
                }
                else
                {
                    unregister_code(KC_COMM);
                    ALTGR_NEEDED = false;
                }
            }
            retVal = false;
            break;

        //Key {'} --> [', "]
        case CKC_QUOT:
            if (record->event.pressed)
            {
                if(ANY_SHIFT_ACTIVE)
                {
                    register_code(KC_2);
                    SHIFT_KEY(CKC_QUOT);
                    SHIFT_NEEDED = true;
                }
                else
                {
                    register_code(KC_LSFT);
                    register_code(KC_1);
                    SHIFT_NEEDED = true;
                }
            }
            else
            {
                if(IS_KEY_SHIFTED(CKC_QUOT))
                {
                    unregister_code(KC_2);
                    CLEAR_MOD_ON_KEY(CKC_QUOT);
                    SHIFT_NEEDED = false;
                }
                else
                {
                    unregister_code(KC_1);
                    SHIFT_NEEDED = false;
                }
            }
            retVal = false;
            break;

    //ROW 2 {,} {.} {/}
        //Key {,} --> [,, <]
        case CKC_COMM:
            if (record->event.pressed)
            {
                if(ANY_SHIFT_ACTIVE)
                {
                    unregister_code(ACTIVE_SHIFT);
                    register_code(KC_RALT);
                    register_code(KC_M);
                    ALTGR_NEEDED = true;
                    SHIFT_KEY(CKC_COMM);
                }
                else
                {
                    register_code(KC_COMM);
                }
            }
            else
            {
                if(IS_KEY_SHIFTED(CKC_COMM))
                {
                    unregister_code(KC_M);
                    register_code(ACTIVE_SHIFT);
                    ALTGR_NEEDED = false;
                    CLEAR_MOD_ON_KEY(CKC_COMM);
                }
                else
                {
                    unregister_code(KC_COMM);
                }
            }
            retVal = false;
            break;

        //Key {.} --> [., >]
        case CKC_DOT:
            if (record->event.pressed)
            {
                if(ANY_SHIFT_ACTIVE)
                {
                    unregister_code(ACTIVE_SHIFT);
                    register_code(KC_RALT);
                    ALTGR_NEEDED = true;
                    SHIFT_KEY(CKC_DOT);
                } 
                register_code(KC_DOT);
            }
            else
            {
                if(IS_KEY_SHIFTED(CKC_DOT))
                {
                    register_code(ACTIVE_SHIFT);
                    ALTGR_NEEDED = false;
                    CLEAR_MOD_ON_KEY(CKC_DOT);
                }
                unregister_code(KC_DOT);
            }
            retVal = false;
            break;

        //Key {/} --> [/, ?]
        case CKC_SLSH:
            if (record->event.pressed)
            {
                if(ANY_SHIFT_ACTIVE)
                {
                    register_code(KC_COMM);
                    SHIFT_KEY(CKC_SLSH);
                    SHIFT_NEEDED = true;
                }
                else
                {
                    register_code(KC_LSFT);
                    register_code(KC_6);
                    SHIFT_NEEDED = true;
                }
            }
            else
            {
                if(IS_KEY_SHIFTED(CKC_SLSH))
                {
                    unregister_code(KC_COMM);
                    CLEAR_MOD_ON_KEY(CKC_SLSH);
                    SHIFT_NEEDED = false;
                }
                else
                {
                    unregister_code(KC_6);
                    SHIFT_NEEDED = false;
                }
            }
            retVal = false;
            break;
    };

    //Check if left or right SFT or ALTGR needs to be released
    if(!ANY_SHIFT_ACTIVE && !SHIFT_NEEDED)
    {
        if ((get_mods() & MOD_BIT(KC_LSFT)) == MOD_BIT(KC_LSFT))
        {
            unregister_code(KC_LSFT);
        }
        if ((get_mods() & MOD_BIT(KC_RSFT)) == MOD_BIT(KC_RSFT))
        {
            unregister_code(KC_RSFT);
        }
    }
    if (!ALTGR_ACTIVE && !ALTGR_NEEDED)
    {
        unregister_code(KC_RALT);
    }
    return retVal;
}
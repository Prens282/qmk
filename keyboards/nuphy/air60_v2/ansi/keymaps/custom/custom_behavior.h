#pragma once

#include "quantum.h"
#ifdef VIA_ENABLE
#    include "via.h"
#endif




enum custom_keycodes_user{
    FIRST_CUSTOM_KEY = SAFE_RANGE,
	CKC_GRV, CKC_1, CKC_2, CKC_3, CKC_4, CKC_6, CKC_7, CKC_8, CKC_9, CKC_0, CKC_MINS, CKC_EQL, 				//ROW 5 - entire row  except 5 (it is the same), 0 and - -those are tapdances
	CKC_LBRC, CKC_RBRC, CKC_BSLS,																		//ROW 4 - Last 3 keys
	CKC_SCLN, CKC_QUOT,																				//ROW 3 - last 2 keys
	CKC_COMM, CKC_DOT, CKC_SLSH,																		//ROW 2 - last 3 keys

    //DO NOT PUT ANYTHING AFTER THIS
    LAST_CUSTOM_KEY
};

enum custom_macros_user{
    MOUS_JIG = LAST_CUSTOM_KEY,																		// Mouse jiggler 
    PW_DPLY,                                                                                        // Password deploy
};

enum TD_Keys{
    TD_A,
    TD_E,
    TD_I,
    TD_O,
    TD_OO,
    TD_U,
    TD_UU
};

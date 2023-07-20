#include "rb5_sendkeys.h"

/*
        Pre-defined special keys
*/
KS_PREDEF_KEYS prekeys[] =
    {
        {	VK_LBUTTON,		"lbutton"        },
        {	VK_RBUTTON, 	        "rbutton"        },
        {	VK_CANCEL,		"cancel"         },
        {	VK_MBUTTON,		"mbutton"        },
        {	VK_BACK,		"back"           },
        {	VK_TAB,			"tab"            },
        {	VK_CLEAR,		"clear"          },
        {	VK_RETURN,		"enter"          },
        {	VK_RETURN,		"return"         },
        {	VK_SHIFT,		"shift"          },
        {	VK_CONTROL,		"ctrl"           },
        {	VK_CONTROL,		"control"        },
        {	VK_MENU,		"alt"            },
        {	VK_PAUSE,		"pause"          },
        {	VK_CAPITAL,		"capslock"       },
        {	VK_ESCAPE,		"esc"            },
        {	VK_SPACE,		"space"          },
        {	VK_PRIOR,		"pageup"         },
        {	VK_NEXT,		"pagedown"       },
        {	VK_END,			"end"            },
        {	VK_HOME,		"home"           },
        {	VK_LEFT,		"left"           },
        {	VK_UP,			"up"             },
        {	VK_RIGHT,		"right"          },
        {	VK_DOWN,		"down"           },
        {	VK_SELECT,		"select"         },
        {	VK_PRINT,		"print"          },
        {	VK_EXECUTE,		"execute"        },
        {	VK_SNAPSHOT,	        "snapshot"       },
        {	VK_INSERT,		"insert"         },
        {	VK_DELETE,		"delete"         },
        {	VK_HELP,		"help"           },
        {	VK_LWIN,		"lwin"           },
        {	VK_RWIN,		"rwin"           },
        {	VK_APPS,		"apps"           },
        {	VK_F1,			"f1"             },
        {	VK_F2,			"f2"             },
        {	VK_F3,			"f3"             },
        {	VK_F4,			"f4"             },
        {	VK_F5,			"f5"             },
        {	VK_F6,			"f6"             },
        {	VK_F7,			"f7"             },
        {	VK_F8,			"f8"             },
        {	VK_F9,			"f9"             },
        {	VK_F10,			"f10"            },
        {	VK_F11,			"f11"            },
        {	VK_F12,			"f12"            },
        {	VK_F13,			"f13"            },
        {	VK_F14,			"f14"            },
        {	VK_F15,			"f15"            },
        {	VK_F16,			"f16"            },
        {	VK_NUMLOCK,		"numlock"        },
        {	VK_SCROLL,		"scroll"         },
        {	VK_LSHIFT,		"lshift"         },
        {	VK_RSHIFT,		"rshift"         },
        {	VK_LCONTROL,	        "lcontrol"       },
        {	VK_RCONTROL,	        "rcontrol"       },
        {	VK_LMENU,		"lalt"           },
        {	VK_RMENU,		"ralt"           }
    };


/*
        SENDKEYS MACRO LANGUAGE COMPILER AND EXECUTOR --------------------------
*/
int ks_scantoken( char* s, KS_TOKEN* t )
{
	char*	startpos;
	int		ret			= 1;
	int		i;

	t->keyCode                      = 0;
	t->hold				= 0;
	t->keyFlag			= 0;

	switch ( *s )
	{
		case '\0':
			return 0;

		case '^':
			i = ks_scantoken( s + 1, t );
			t->hold		= 1;
			return i + 1;

		case '(':

			s++;

			startpos = s;
			while ( *s != ')' && *s != '\0' )
				s++;

			*s = '\0';

			strlwr( startpos );

			for ( i = 0; i < ( sizeof( prekeys ) / sizeof( prekeys[0] ) ); i++ )
			{
				if ( strcmp( prekeys[i].keyName, startpos ) == 0 )
				{
					t->keyCode = prekeys[i].keyCode;
					break;
				}
			}

			/* Do also allow key codes here... */
			if ( i == ( sizeof( prekeys ) / sizeof( prekeys[0] ) ) )
			{
				t->keyCode = VkKeyScan( atoi( startpos ) );
				t->keyFlag = (BYTE)(t->keyCode >> 8);
				t->keyCode = (BYTE)(t->keyCode);
			}

			return (int) (s - startpos) + 1 + 1;

		case '~':
			s++;
			ret++;

		default:
			switch ( *s )
			{
				case '.':
					t->keyCode = 0xBE;
					break;
				case ',':
					t->keyCode = 0xBC;
					break;
				case '+':
					t->keyCode = VK_ADD;
					break;
				case '-':
					t->keyCode = VK_SUBTRACT;
					break;
				case '*':
					t->keyCode = VK_MULTIPLY;
					break;
				case '/':
					t->keyCode = VK_DIVIDE;
					break;
				case ' ':
					t->keyCode = VK_SPACE;
					break;
				default:
					t->keyCode = VkKeyScan( *s );

					t->keyFlag = (BYTE)(t->keyCode >> 8);
					t->keyCode = (BYTE)(t->keyCode);
					break;
			}
	}

	return ret;
}

void ks_press( KS_TOKEN* t )
{
	if ( t->keyCode == 0 ) return;

	//SHIFT pressed?
	if ( ( t->keyFlag & 1 ) == 1 )
		keybd_event(VK_SHIFT,0,0,0);

	//CTRL pressed?
	if ( ( t->keyFlag & 2 ) == 2 )
		keybd_event(VK_CONTROL,0,0,0);

	//ALT pressed?
	if ( ( t->keyFlag & 4 ) == 4 )
		keybd_event(VK_MENU,0,0,0);

	keybd_event((char)t->keyCode,0,0,0);
}

void ks_release( KS_TOKEN* t )
{
	if ( t->keyCode == 0 ) return;

	keybd_event((char)t->keyCode,0,KEYEVENTF_KEYUP,0);

	//SHIFT pressed?
	if ( ( t->keyFlag & 1 ) == 1 )
		keybd_event(VK_SHIFT,0,KEYEVENTF_KEYUP,0);

	//CTRL pressed?
	if ( ( t->keyFlag & 2 ) == 2 )
		keybd_event(VK_CONTROL,0,KEYEVENTF_KEYUP,0);

	//ALT pressed?
	if ( ( t->keyFlag & 4 ) == 4 )
		keybd_event(VK_MENU,0,KEYEVENTF_KEYUP,0);
}

int sendkeys( char* s )
{
	KS_TOKEN	ks;

	KS_TOKEN	*hold		= (KS_TOKEN*)NULL;
	int			hold_cnt	= 0;
	int			step		= 0;

	while ( ( step = ks_scantoken( s, &ks ) ) > 0 )
	{
		s += step;

		ks_press( &ks );

		if ( !ks.hold )
			ks_release( &ks );

		if ( ks.hold )
		{
			if ( hold_cnt == 0 )
			{
				hold = (KS_TOKEN*)malloc( 10 * sizeof( KS_TOKEN ) );
			}
			else if ( ( hold_cnt % 10 ) == 0 )
			{
				hold = (KS_TOKEN*)realloc( (KS_TOKEN*)hold, (((hold_cnt+1) / 10) * 10)
				                           * sizeof( KS_TOKEN ) );
			}

			memcpy( &(hold[ hold_cnt ]), &ks, sizeof( KS_TOKEN ) );
			hold_cnt++;
		}
		else if ( hold_cnt > 0 )
		{
			for ( hold_cnt--; hold_cnt >= 0; hold_cnt-- )
			{
				hold[ hold_cnt ].hold = 0;
				ks_release( &(hold[ hold_cnt ]) );
			}

			free( hold );
			hold_cnt = 0;
		}
	}

	if ( hold_cnt > 0 )
	{
		for ( ; hold_cnt >= 0; hold_cnt-- )
		{
			hold[ hold_cnt ].hold = 0;
			ks_release( &(hold[ hold_cnt ]) );
		}

		free( hold );
		hold_cnt = 0;
	}

	return 0;
}


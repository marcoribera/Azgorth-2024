/*
        Created by *Edwards
        For FantasiaShard.com

        Edwards@FantasiaShard.com

        2010-12-18
*/

use uo;
use os;
use util;

include ":gumps:gumps";
include ":gumps:gumps_ex";
//include ":onlineCount:settings";

function SelectCharacter( mobile, cmdlevel:=0 )

        var gump := GFCreateGump();

	GFResizePic( gump, 0, 0, GFCfgConst( "Defaults", "BackGround" ), 310, 380 );

	GFResizePic( gump, 15, 15, GFCfgConst( "Defaults", "ForeGround" ), 280, 25 );
	GFResizePic( gump, 15, 45, GFCfgConst( "Defaults", "ForeGround" ), 280, 320 );

        GFAddAlphaRegion( gump, 0, 0, 310, 380 );

	GFTextLine( gump, 30, 15, 1720, "Selecionar Jogador" );

	var names_list := GetCharactersList( cmdlevel ),
	    num_players := names_list.Size(),
            list_keys := names_list.Keys(),
	    counter := 0,
	    y_pos := 60;

	GFPage( gump, 1 );
	foreach name in ( list_keys )

                GFTextLine( gump, 25, y_pos, 2100, name );
		GFAddButton( gump, 261, y_pos+3, 2117, 2118, GF_CLOSE_BTN, _name_iter );

		y_pos += 20;
		counter += 1;

		if( counter >= 14 && _name_iter < num_players )
			GFAddButton( gump, 280, 320, 0xA92, 0xA93, GF_PAGE_BTN, gump.cur_page+1 );
			GFPage( gump, gump.cur_page+1 );
			GFAddButton( gump, 280, 60, 0xA90, 0xA91, GF_PAGE_BTN, gump.cur_page-1 );
			counter := 0;
			y_pos := 60;
		endif
		SleepMS(2);
	endforeach

	var input := GFSendGump( mobile, gump );
	if( !input )
		SendSysMessage( mobile, "Cancelled." );
		return 0;
	endif

	input := input[0];

        var result := list_keys[input],
            serial := names_list[result];

        var character := SystemFindObjectBySerial( serial, SYSFIND_SEARCH_OFFLINE_MOBILES );

	return character;
endfunction

function GetCharactersList( cmdlevel );

	var the_list := dictionary; // Used to store names in ABC order
 	foreach acct_name in ( ListAccounts() )
		var account := FindAccount( acct_name );
		var i := 1;
      while( i <= 5 )
 	 	 	var character := account.GetCharacter(i);
 	 	 	if( character )
 	 	 	 	if( character.cmdlevel <= cmdlevel )
					var name := character.getName();
 	 	 	 	 	the_list.Insert( name, character.serial );
 	 	 	 	endif
 	 	 	endif
 	 	 	i += 1;
 	 	 	SleepMS(5);
      endwhile
		SleepMS(5);
	endforeach

        //Make sure bots are include in the email listing...
        /*var name_list := OC_GetnameList();
        foreach name in name_list
                the_list.Insert( name, RandomInt(500)+1000 );
		SleepMS(5);
        endforeach*/

	return the_list;
endfunction

/* $Id: gumps_ex.inc 375 2006-06-17 19:26:32Z austinheilman $
 *
 * Purpose
 * TODO
 *
 * Function List
 * GFExtractData()
 * GFOptimizeGump()
 * GFTextCoords()
 * GFTextMid()
 * GFTextRight()
 * GFTextWidth()
 * GFWordWrap()
 * CheckBoxGump cria um gump com varias checkbox onde pode retornar varios valores clicados
 * RadioGump cria um gump com varias radiosbuttons, mas apenas um botao pode ser retornado
 */
use uo;
use os;
use basic;
use cfgfile;
use polsys;

include ":gumps:gumps";

// CONSTants for GFTextCoords()
CONST ALIGN_CENTER	:= 0x1;
CONST ALIGN_RIGHT	:= 0x2;

function GFCloseGump(mobile, pid, ret_val:=0)
	var packet := CreatePacket(0xBF, 13);
	packet.SetInt16(1, 13);
	packet.SetInt16(3, 4);
	packet.SetInt32(5, pid);
	packet.SetInt32(9, ret_val);

	return packet.SendPacket(mobile);
endfunction


function GFCloseGumpTimed(mobile, pid, timer, ret_val:=0)
	return Start_Script(":gumps:autoClose", array{mobile, pid, timer, ret_val});
endfunction



/*
 * GetCfgConst(elem_name, prop_name)
 *
 * Purpose
 * Rather than have a lot of CONSTants at the top for
 * different gump images, they are placed in GumpInfo.cfg
 * and retrieved with this function. 
 * The idea is that it is easier to update GumpInfo.cfg than
 * to recompile all scripts that use these includes.
 *
 * Parameters
 * elem_name:	The elem of the config to look at.
 * prop_name:	The name of the CONSTant to retrieve.
 *
 * Return value
 * An integer for the gump graphical number
 *
 */
function GFCfgConst(elem_name, prop_name)
	var cfg := ReadConfigFile(":gumps:config/GumpInfo");
	if ( cfg.errortext )
		return XGFError("GetCfgConst():: Unable to open GumpInfo.cfg -> "+cfg.errortext);
	endif
	
	var elem := cfg[elem_name];
	if ( elem.errortext )
		return XGFError("GetCfgConst():: Unable to find GumpInfo.cfg["+elem_name+"] ->"+elem.errortext);
	endif
	
	var value := GetConfigInt(elem, prop_name);
	if ( !value )
		return XGFError("GetCfgConst():: Unable to find property ["+prop_name+"] in elem ["+elem_name+"]");
	endif
	
	return value;
endfunction

/*
 * GFGetCfgConst(elem_name, prop_name)
 *
 * Purpose
 * Rather than have a lot of CONSTants at the top for
 * different gump images, they are placed in GumpInfo.cfg
 * and retrieved with this function.
 * The idea is that it is easier to update GumpInfo.cfg than
 * to recompile all scripts that use these includes.
 *
 * Parameters
 * elem_name:	The elem of the config to look at.
 * prop_name:	The name of the CONSTant to retrieve.
 *
 * Return value
 * An integer for the gump graphical number
 *
 */
function GFGetCfgConst(elem_name, prop_name)
	var cfg := ReadConfigFile(":gumps:GumpInfo");
	if ( cfg.errortext )
		return XGFError("GFGetCfgConst():: Unable to open GumpInfo.cfg -> "+cfg.errortext);
	endif

	var elem := cfg[elem_name];
	if ( elem.errortext )
		return XGFError("GFGetCfgConst():: Unable to find GumpInfo.cfg["+elem_name+"] ->"+elem.errortext);
	endif

	var value := GetConfigInt(elem, prop_name);
	if ( !value )
		return XGFError("GFGetCfgConst():: Unable to find property ["+prop_name+"] in elem ["+elem_name+"]");
	endif

	return value;
endfunction

/*
 * GFExtractData(input, data_id)
 *
 * Purpose
 * Retrieves the information for a text entry's data and
 * removes the "#: " from the begining of it.
 *
 * Parameters
 * input:	The variable/information returned from GFSendGump();
 * data_id:	An index value returned by GFTextEntry()
 *
 * Return value
 * A string containing the text entry's input value.
 *
 */
function GFExtractData(input, data_id)
	var key_string := CStr(data_id)+": ";
	var data_string := input[data_id];
	var ret_val := data_string[Len(key_string)+1,Len(data_string)];

	return ret_val;
endfunction


/* 
 * GFOptimizeGump(gump)
 *
 * Purpose
 * Finds duplicate page number entries and combines their information.
 *
 * Parameters
 * gump:	The gump to optimize
 *
 * Return value
 * Returns 1
 *
 */
function GFOptimizeGump(byref gump)
	//Arrays cant do [0] for page 0
	var cur_page := 0;
	var gump_pages := dictionary;
	gump_pages[0] := array;
	
	//Break the pages apart.
	foreach entry in (gump.layout)
		var l_entry := Lower(entry);
		if ( l_entry[1, 5] == "page " )
			//  12345   6
			// "Page " "#"
			var page_num := CInt(l_entry[6, Len(l_entry)]);
			if ( !gump_pages.Exists(page_num) )
				gump_pages[page_num] := array;
				cur_page := page_num;
			else
				cur_page := page_num;
				continue;
			endif
		endif
		gump_pages[cur_page].Append(entry);
		SleepMS(2);
	endforeach
	
	//Put the gump back together again.
	gump.layout := array{}; // Wipe it out
	foreach page_array in (gump_pages)
		 // Goes through the dictionary which contains each page's array.
		foreach entry in (page_array)
			// Goes through the array containing layout data for that page.
			(gump.layout).Append(entry);
			SleepMS(2);
		endforeach
		SleepMS(2);
	endforeach
	
	return 1;
endfunction


/*
 * GFTextCoords(type, gump_text, x_pos, width)
 *
 * Purpose
 * Returns the X coordinate to pass to GFTextLine() that will align
 * the text.
 * 
 * Parameters
 * type:	The type of horizontal alignment to do.
 *		ALIGN_CENTER - Aligns text between the x_pos and width position.
 *		ALIGN_RIGHT -  Aligns text so that it ends on the x_pos. (width not needed)
 * gump_text:	The text to find the x coordinate for.
 * x_pos:	The x position to align the text off of.
 * width:	When center aligning the text, specify the width of the area it will fit into.
 *
 * Return value
 * Returns the x coordinate to place the text on.
 *	
 */
function GFTextCoords(type:=ALIGN_CENTER, gump_text, x_pos, width:=0)
	x_pos := CInt(x_pos);
	width := CInt(width);
	if ( type == ALIGN_CENTER )
		return CInt(x_pos+CInt((width-GFTextWidth(gump_text))/2));
	elseif ( type == ALIGN_RIGHT )
		return CInt(x_pos-GFTextWidth(gump_text));
	endif
endfunction


/*
 * GFTextMid(gump, x, y, width, color, text)
 *
 * Purpose
 * Aligns text between the x position and width position.
 * 
 * Parameters
 * gump:	Reference to the gump
 * x:		The top-left spot of the X axis.
 * y:		The top-left spot of the Y axis.
 * width:	The right-most point the text will fit into.
 * hue:		The decimal number representing the Label's hue
 * text:	The string that is to be exposed.
 *
 * Return value	
 * Return value is the result from GFTextLine()
 *
 */
function GFTextMid(byref gump, x, y, width, color, text)
	var x_pos := GFTextCoords(ALIGN_CENTER, text, x, CStr(width));
	return GFTextLine(gump, x_pos, y, color, CStr(text));
endfunction


/*
 * GFTextRight(gump, x, y, color, text)
 *
 * Purpose
 * Aligns text between the x position and width position.
 * 
 * Parameters
 * gump:	Reference to the gump
 * x:		The right-most coordinate the text will end on.
 * y:		The top-left spot of the Y axis.
 * hue:		The decimal number representing the Label's hue
 * text:	The string that is to be exposed.
 *
 * Return value	
 * Return value is the result from GFTextLine()
 *
 */
function GFTextRight(byref gump, x, y, color, text)
	var x_pos := GFTextCoords(ALIGN_RIGHT, CStr(text), x);
	return GFTextLine(gump, x_pos, y, color, CStr(text));
endfunction


/*
 * GFGetTextWidth(char)
 *
 * Purpose
 * To figure how the width in pixels of text on a gump.
 *
 * Parameters
 * gump_text:	The text to get the pixel width of.
 *
 * NOTE: This needs to be expanded to support fonts - and possibly be config driven?
 *
 * Return value
 * The width, in pixels, of the text's width on a gump.
 *
 */
function GFTextWidth(gump_text)
	var gft_length := struct;
	gft_length.+charsf :=
	"||||||||||||||"+	//  2
	"`',.;:::::::::"+	//  3
	"!!!!!!!!!!!!!!"+	//  4
	"il1[]]]]]]]]]\""+	//  5
	"bcejot�(){}==="+	//  6
	"adfpqrszI�~~~~"+	//  7
	"ghnuvy�<>-----"+	//  8
	"xJ578$?++++++ "+	//  9
	"kCE2369^/*****"+	// 10
	"mwABDFOP��40%%"+	// 11
	"GLNQSZ@#######"+	// 12
	"HRT&__________"+	// 13
	"K�UYYYYYYYYYYY"+	// 14
	"XXXXXXXXXXXXXX"+	// 15
	"VVVVVVVVVVVVVV"+	// 16
	"MMMMMMMMMMMMMM"+	// 18
	"WWWWWWWWWWWWWW";	// 21
	gft_length.+width := array
	{
		2, 3, 4, 5, 6, 7, 8, 9, 10, 11,
		12, 13, 14, 15, 16, 18, 21
	};

	var points:=0;
	var j := Len(gump_text);

	for ( j; j>0; j:=j-1 )
		var i := Find(gft_length.charsf, gump_text[j], 1) - 1;
		if ( i>=0 )
			points := points + gft_length.width[CInt(i/14)+1];
		else
			print("XGFTextWidth: Unknown Character = '"+gump_text[j]+"'");
		endif
	endfor

	return (points);
endfunction


/*
 * GFWordWrap(text, pixels)
 *
 * Purpose
 * Breaks a string up into an array of strings.
 * Each string will be less than or equal to 'pixels' in width.
 *
 * Parameters
 * text:	The text to break up.
 * pixels:	The maximum width a line of text can be.
 *
 * Return value
 * An array of strings
 *
 */
function GFWordWrap(text, pixels)
	var wrap_lines := array{};
	var word_list := SplitWords(text);
	//var num_words := word_list.Size();
	
	var space_size := GFTextWidth(" ");
	
	var cur_line := "";
	var cur_length := 0;
	var start_new := 0;
	
	foreach word in word_list
		var word_width := GFTextWidth(word);
				
		if ( word == "[P]" )
			// [P] will force a line break
			word := "";
			word_width := 0;
			start_new := 1;
		elseif ( (word_width+cur_length) > pixels )
			start_new := 1;
		endif
		
		if ( start_new )
			start_new := 0;
			wrap_lines.Append(cur_line);
			cur_line := "";
			cur_length := 0;
		endif
		if ( word )
			cur_line := cur_line + word+" ";
			cur_length := cur_length+word_width+space_size;
		endif
						
		SleepMS(2);
	endforeach
	
	//Make sure nothing got left behind.
	if ( wrap_lines[wrap_lines.Size()] != cur_line )
		wrap_lines.Append(cur_line);
	endif
	
	return wrap_lines;	
endfunction

/* CheckBoxGump cria um gump com varias checkbox onde pode retornar varios valores clicados
   RadioGump cria um gump com varias radiosbuttons, mas apenas um botao pode ser retornado */

/*function CheckBoxGump(mobile, sizex, sizey, prompt:="", options, opts := 0)

	var gump := GFCreateGump(140, 100);
	GFClosable(gump, 0);
	GFResizePic(gump, 0, 0, 3600, sizex, sizey);
	GFResizePic(gump, 15, 15, 0x13BE, sizex-30, sizey-30);
	
	var y_pos := 20;
	foreach line in (GFWordWrap(prompt, 360))
		GFTextLine(gump, 20, y_pos, 1153, line);
		y_pos := y_pos+20;
		SleepMS(2);
	endforeach
	
  GFSetRadioGroup(gump, 1);
  var indice := 1;
  y_pos := 30;
  var first := 1;
  foreach text in options
    GFTextLine(gump, 60, y_pos + indice*20, 1153, text);
    if (opts)
      first := opts[indice];
    endif
    GFCheckBox(gump, 30, y_pos + indice*20, 210, 211, first, indice);
    first := 0;
    indice := indice+1;
  endforeach
	
  GFAddButton(gump, sizex-70, sizey-60, 4502, 4502, 1, 999 );
	GFTextLine(gump, sizex-110, sizey-50, 1153, "Proximo");
  
  var ret := array;
  
	var input := GFSendGump(mobile, gump);
	if ( input[0] == 999 )
		 foreach key in (input.keys)
        if (options[key]  )
          ret.append(options[key]);
        endif
     endforeach
     return ret;
  else
      return 0;
	endif	

endfunction

function RadioGump(mobile, sizex, sizey, prompt:="", options)

	var gump := GFCreateGump(140, 100);
	GFClosable(gump, 0);
	GFResizePic(gump, 0, 0, 3600, sizex, sizey);
	GFResizePic(gump, 15, 15, 0x13BE, sizex-30, sizey-30);
	
	var y_pos := 20;
	foreach line in (GFWordWrap(prompt, 360))
		GFTextLine(gump, 20, y_pos, 1153, line);
		y_pos := y_pos+20;
		SleepMS(2);
	endforeach
	
  GFSetRadioGroup(gump, 1);
  var indice := 1;
  y_pos := 30;
  var first := 1;
  foreach text in options
    GFTextLine(gump, 60, y_pos + indice*20, 1153, text);
    GFRadioButton(gump, 30, y_pos + indice*20, 210, 211, first, indice);
    first := 0;
    indice := indice+1;
  endforeach
	
  GFAddButton(gump, sizex-70, sizey-60, 4502, 4502, 1, 999 );
	GFTextLine(gump, sizex-110, sizey-50, 1153, "Proximo");
  
	var input := GFSendGump(mobile, gump);
	if ( input[0] == 999 )
		 foreach key in (input.keys)
        if (options[key]  )
          return options[key];
        endif
     endforeach
  else
      return 0;
	endif	
  
endfunction*/

function GFHtmlShadowed(byref gump, x, y, width, height, color, text, size := 5);
	GFHTMLArea(gump, x-1, y-1, width, height, "<BASEFONT SIZE={} COLOR=#111111>{}".format(size, text));
   GFHTMLArea(gump, x+1, y+1, width, height, "<BASEFONT SIZE={} COLOR=#111111>{}".format(size, text));
   GFHTMLArea(gump, x+1, y-1, width, height, "<BASEFONT SIZE={} COLOR=#111111>{}".format(size, text));
   GFHTMLArea(gump, x-1, y+1, width, height, "<BASEFONT SIZE={} COLOR=#111111>{}".format(size, text));
   GFHTMLArea(gump, x, y, width, height, "<BASEFONT SIZE={} COLOR={}>{}".format(size, color, text));
endfunction
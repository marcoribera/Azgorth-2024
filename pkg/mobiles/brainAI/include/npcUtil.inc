/* $Id: npcUtil.inc 776 2005-11-01 08:16:27Z muaddiblsd $
 *
 */
/*===============================================================
* Current Version
* NPCUTIL.INC - v1.0
* Updated 8/31/2005 4:23AM
*
* -- Revision v1.0 --
* Austin:
*  Created include file
===============================================================*/

use uo;
use os;
use polsys;
use cfgfile;
use file;
use polsys;
use storage;
include ":containers:storageAreas";


// * Global Variables *
//const CP_NOCREATE   := 0x0;
//const CP_CREATE     := 0x1;

//const CP_DEFAULT_AREA   := "World Bank";
//const CP_DEFAULT_PREFIX := "[Root Container] ";

/*
 * Set the 'box_type' variable to the object name of the container type
 * you want to be used for bank box.
 * Example: "pouch" "metalchest" "barrel"
 *
 * This is ONLY used when a new root container is created. It can be changed on
 * the fly inside of a script for when you want to use specific container
 * types for special storage areas.
 *
 * Ideally, you should make an object type with the name 'BankBox' to better
 * control the number of items that can be put in the storage container.
 */
//var container_type  := "BankBox";

// Toggles the bank related debug messages on and off.
//var STORAGE_DEBUG   := 1;
/*
 * NPC_ParseTemplateName(template_name)
 *
 * Purpose
 * Finds the appropriate npcdesc.cfg to load.
 * This enables npcs to be placed in packages and not just the ::npcdesc file.
 * ReadConfigFile(":*:npcdesc") does not work in this case because packaged npcs
 * have the package name at the begining of their .npctemplate member. Reading the
 * specific config file - helps to avoid possible elem name collisions.
 *
 * Parameters
 * template_name:	Name of the npc template to parse
 *
 * Return value
 * Returns struct .package .template
 *
 */
function NPC_ParseTemplateName(template_name)
	if ( template_name.IsA(POLCLASS_NPC) )
		template_name := template_name.npctemplate;
	endif

	//Find positions of ":" characters
	var colon_a := Find(template_name, ":", 1);
	var colon_b := Find(template_name, ":", 2);

	var parsed := struct{"package", "template"};

	if ( !colon_a || !colon_b )
		//Not a complete package name - use default npcdesc.cfg
		parsed.package := "";
		parsed.template := template_name;
	else
		parsed.package := template_name[colon_a+1, colon_b-2];
		parsed.template := template_name[colon_b+1, Len(template_name)];
	endif

	return parsed;
endfunction

/*
 * NPC_GetNPCConfig(template)
 *
 * Purpose
 * Retrieves a config elem for an npc template.
 *
 * Parameters
 * info:	Template info from NPC_ParseTemplateName()
 *
 * Return value
 * Config elem reference
 *
 */


function NPC_GetNPCConfig(template)
	if ( !template.package )
		template := NPC_ParseTemplateName(template);
	endif

	var npc_cfg := ReadConfigFile(":"+template.package+":npcdesc");
	if ( npc_cfg.errortext )
		return error{"errortext":="Error::NPC_GetNPCConfig() - Could not open :"+template.package+":npcdesc ->"+npc_cfg.errortext};
	endif

	var cfg_elem := npc_cfg[template.template];
	if ( cfg_elem.errortext )
		return error{"errortext":="Error::NPC_GetNPCConfig() - Could not find elem ["+template.template+"] ->"+cfg_elem.errortext};
	endif

	return cfg_elem;
endfunction


/*
 * NPC_GetAllTemplateNames()
 *
 * Purpose
 * Retrieves a list of all npc templates in the shard's npcdesc.cfg files
 *
 * Parameters
 *
 * Return value
 * Returns an array
 *
 */

 /*
function NPC_GetAllTemplateNames()
	var elem_names := array{};
	elem_names := elem_names + GetConfigStringKeys(ReadConfigFile("::npcdesc"));
	foreach package in (Packages())
		if ( package.npcdesc )
			var pkg_name := ":"+package.name+":";
			foreach template_name in (GetConfigStringKeys(ReadConfigFile(pkg_name+"npcdesc")))
				elem_names.Append(pkg_name+template_name);
				sleepms(2);
			endforeach
		endif
		sleepms(2);
	endforeach

	return elem_names;
endfunction
*/

/* $Id: storageAreas.inc 128 2006-05-27 17:36:42Z austinheilman $
 *
 * Purpose
 * Assists in the use of storage areas.
 *
 * Author: Austin
 *
 * [ Function List ]
 * CP_GetStorageContainerForMobile(container_id, storage_area_name:=0)
 * CP_GetStorageAreaContainer(mobile, storage_area_name:=0, create:=CP_NOCREATE)
 * CP_CreateStorageAreaContainer(container_id, storage_area_name:=0)
 * CP_RegisterStorageArea(storage_area_name)
 * CP_RemoveStorageAreaContainer(container_id, storage_area_name)
 * CP_StorageDebugMsg(text)
 *
 * [ Internal Function List ]
 *
 */


/*
 * CP_GetMobileContainerID(mobile)
 *
 * Purpose
 * Sets up the container_id string for a mobile's root container.
 *
 * Parameters
 * mobile:	Mobile whos box will be retrieved.
 *
 * Return value
 * String
 *
 */

/*
function CP_GetMobileContainerID (mobile)
	if ( mobile.IsA(POLCLASS_CORPSE) )
		return "Owner="+Hex(mobile.ownerserial);
	else
		return "Owner="+Hex(mobile.serial);
	endif
endfunction
*/

/*
 * GetStorageContainerForMobile(mobile, storage_area_name, create)
 *
 * Purpose
 *
 * Parameters
 * mobile:		Owner of the container that will be retrieved (uses .serial of it)
 * storage_area_name:	Name of the storage area to retrieve the container from.
 * create:		Controls if the container will be created if it does not already exist.
 *
 * Return Value
 * A container object
 *
 */
 /*
function CP_GetStorageContainerForMobile(mobile, storage_area_name:=0, create:=CP_NOCREATE)
	if ((!storage_area_name) || (storage_area_name == error))
		storage_area_name := CP_DEFAULT_AREA;
	endif

	var container_id := CP_GetMobileContainerID(mobile);
	var root_container := CP_GetStorageAreaContainer(container_id, storage_area_name, create);

	return root_container;
endfunction
*/

/*
 * CP_GetStorageAreaContainer(container_id, storage_area_name, create)
 *
 * Purpose
 * Retrieves a root container that matches the id sent.
 *
 * Parameters
 * container_id:	Name of the root container to retrieve.
 * storage_area_name:	Name of the storage area to retrieve the container from.
 * create:		Controls if the container will be created if it does not already exist.
 *
 * Return value
 * A container object
 *
 */
 /*
function CP_GetStorageAreaContainer(container_id, storage_area_name:=0, create:=CP_NOCREATE)
	if ( (!storage_area_name) || (storage_area_name == error) )
		storage_area_name := CP_DEFAULT_AREA;
	endif
	var container_name := CP_DEFAULT_PREFIX + container_id;

	var storage_area := FindStorageArea(storage_area_name);
	var root_container := FindRootItemInStorageArea(storage_area, container_name);
	if ( (!root_container) && (create) )
		root_container := CP_CreateStorageAreaContainer(container_id, storage_area_name);
	endif

	return root_container;
endfunction
*/

/*
 * CP_CreateStorageAreaContainer(container_id, storage_area_name)
 *
 * Purpose
 * Creates a root container in a storage area.
 *
 * Parameters
 * container_id:	Name of the container to create.
 * storage_area_name:	Name of the storage area to create the container in.
 *
 * Return value
 * A container object
 *
 */
 /*
function CP_CreateStorageAreaContainer(container_id, storage_area_name:=0)
	if ( (!storage_area_name) || (storage_area_name == error) )
		storage_area_name := CP_DEFAULT_AREA;
	endif

	var storage_area := FindStorageArea(storage_area_name);
	if ( !storage_area )
		// The storage area does not exist - so create it.
		storage_area := CreateStorageArea(storage_area_name);
	endif

	var container_name := CP_DEFAULT_PREFIX + container_id;

	var root_container := CreateRootItemInStorageArea(storage_area, container_name, container_type);
	if ( root_container.errortext )
		CP_StorageDebugMsg("Error::CP_CreateStorageAreaContainer() - "+root_container.errortext);
	endif

	return root_container;
endfunction
*/

/*
 * CP_RemoveStorageAreaContainer(container_id, storage_area_name)
 *
 * Purpose
 * Removes a root container from a storage area.
 * Also deletes all of its contents.
 *
 * Parameters
 * container_id:	Name of the container to remove.
 * storage_area_name:	Name of the storage area to remove the container from.
 *
 * Return value
 * Returns the result of the deletion.
 *
 */
 /*
function CP_RemoveStorageAreaContainer(container_id, storage_area_name:=0)
	if ( (!storage_area_name) || (storage_area_name == error) )
		storage_area_name := CP_DEFAULT_AREA;
	endif

	var storage_area := FindStorageArea(storage_area_name);
	var container_name := CP_DEFAULT_PREFIX + container_id;

	CP_StorageDebugMsg("Removed root container. Storage area: ["+storage_area_name+"] ID: ["+container_id+"]");

	return DestroyRootItemInStorageArea(storage_area, container_name);
endfunction
*/

/*
 * CP_GetStorageAreaNames()
 *
 * Purpose
 * Returns a list of storage area names
 *
 * Parameters
 *
 * Return value
 * Array
 *
 */
 /*
function CP_GetStorageAreaNames()
	var name_list := array;
	foreach storage_area in (StorageAreas())
		// Used to fix unused var report by ecompile: storage_area;
		storage_area := storage_area;
		name_list.Append(_storage_area_iter);
		SleepMS(2);
	endforeach

	return name_list;
endfunction
*/

/*
 * CP_GetRootContainersInStorageArea(storage_area_name)
 *
 * Purpose
 *
 * Parameters
 *
 * Return value
 * Array
 *
 */

 /*
function CP_GetRootContainersInStorageArea(storage_area_name)
	var storage := StorageAreas();
	var root_list := array;
	foreach container in (storage[storage_area_name])
		root_list.Append(container);
		SleepMS(2);
	endforeach

	return root_list;
endfunction
*/

/*
 * CP_StorageDebugMsg(text)
 *
 * Purpose
 *
 * Parameters
 *
 * Return value
 *
 */
 /*
function CP_StorageDebugMsg(report_text)
	if ( STORAGE_DEBUG )
		var script_name := GetProcess(GetPid()).name;
		LogToFile("::log/storage.log", "["+script_name+"]: "+report_text, LOG_DATETIME);
	endif

	return 1;
endfunction
*/

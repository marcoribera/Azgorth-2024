// ###########################################
// # 
// # AddNPC
// # remNPC
// # editNPC
// # listNPC
// # ContractNpc
// # listContractedNPC
// # DismissNPC
// # CheckPrivilegies
// # GetReports
// # 
// ###########################################

include ":datafile:include/datafile";
include ":unicos:item_template";

function AddNpc(guild, npc)
   if (typeof(guild) == "String")
      guild := FindGuild(guild);
   endif

   if (!guild) return; endif

   var new_npc := struct{
      "template" := npc.template,
      "equipment" := npc.equipment,
      "bossStruct" := npc.bossStruct,
      "price" := npc.price,
      "name" := npc.name
   };
   if (!new_npc.template) 
      return 0;
   elseif (!new_npc.equipment)
      return 0;
   elseif (!new_npc.price)
      return 0;
   elseif (!new_npc.name)
      return 0;
   endif

   var npclist := listNPC(guild);
   npclist.append(new_npc);

   guild.setProp("npcs", npclist);
   return 1;
endfunction

function listNPC(guild)
   if (typeof(guild) == "String")
      guild := FindGuild(guild);
   endif

   if (!guild) return; endif

   var npcs := guild.GetProp("npcs");
   if (!npcs) npcs := array{}; endif

   return npcs;
endfunction

function remNPC(guild, index)
   if (typeof(guild) == "String")
      guild := FindGuild(guild);
   endif
   if (typeof(index) == "Integer") 
      var npc_list := listNPC(guild);
      npc_list.Erase(index);
      guild.setProp("npcs", npc_list);
   endif
endfunction

function listContractedNPC(guild)
   if (typeof(guild) == "String")
      guild := FindGuild(guild);
   endif

   if (!guild) return; endif

   var npcs_active := guild.GetProp("npcs_active");
   if (!npcs_active) npcs_active := array{}; endif

   return npcs_active;
endfunction

function DismissNPC(guild, npc_i, npc := 0)
   var npcs := listContractedNPC(guild);
   if (!npc || !npc.isA(POLCLASS_MOBILE))
      npc := SystemFindObjectBySerial(npcs[npc_i].serial);
   endif

   npcs.Erase(npc_i);
   guild.setProp("npcs_active", npcs);

   npc.Disable("invul");
   SetObjProperty(npc, "GuardKill", 1);
   SetObjProperty(npc, "NoCorpse", 1);
   ApplyRawDamage(npc, 500);
endfunction



function ContractNPC(guild, npc_i, who, loc)
   if (typeof(guild) == "String")
      guild := FindGuild(guild);
   endif

   if (!guild) return; endif

   var npcs := listNpc(guild);
   var template := npcs[npc_i];
   if (!template) return; endif

    var npc_list := listContractedNPC(guild);
    if(len(npc_list) < 15)
       var agent := CreateNPCFromTemplate(template.template, loc.x, loc.y, loc.z+2);
      	if (agent.npctemplaye["merchant"])
          SetObjProperty(agent, "noequip", 1);
      	endif

		if (agent.name != "Vendedor")

			agent.script := ":ghaia:guard";
			SetObjproperty(agent, "script", agent.script);
			RestartScript(agent);

			SetObjProperty(agent, "guild", guild.guildid);
       		SetObjProperty(agent, "contract_by", who.serial);
       		SetObjProperty(agent, "conf_ref",npc_i);

			foreach item_ref in (template.equipment)
				sleepms(2);
				var item := CreateIteminBackpackFromTemplate(agent, item_ref);
				var equipped := EquipItem(agent, item);
       		endforeach

			PrintText(agent, "As suas ordens");

			npc_list.append(struct{
				"serial" := agent.serial,
				"price" := template.price,
				"ref" := npc_i
			});
			guild.setProp("npcs_active", npc_list);
	   		return 1;
		endif

			SetObjProperty(agent, "guild", guild.guildid);
			SetObjProperty(agent, "contract_by", who.serial);
			SetObjProperty(agent, "conf_ref",npc_i);


			foreach item_ref in (template.equipment)
				sleepms(2);
				var item := CreateIteminBackpackFromTemplate(agent, item_ref);
				var equipped := EquipItem(agent, item);
			endforeach
/*
       if (template.bossStruct.keys().size() >= 1)
          SetBoss(agent, template.bossStruct);
       endif
*/
			PrintText(agent, "As suas ordens");

			npc_list.append(struct{
				"serial" := agent.serial,
				"price" := template.price,
				"ref" := npc_i
			});
			guild.setProp("npcs_active", npc_list);
    else
        SendSysMessageEx(who, "Quantidade maxima de agentes atingida.");
    endif

   return 1;
endfunction

function CreateIteminBackpackFromTemplate(player, selling)

	var purchase := CreateItemInBackPack(player, selling, selling.amount);
	if ( purchase == error )
		purchase := CreateItemAtLocation(player.x, player.y, player.z, selling, selling.amount);
		if ( purchase == error )
			SendSysMessage(player, "Error - Unable to create item! ->"+purchase.errortext);
		endif
	endif
	if (selling.amount == 1)
		SetName(purchase, selling.desc);
	endif
	foreach prop in selling
		//sendsysmessage(player, " " + _prop_iter + " "+ selling[_prop_iter]);
		purchase.set_member(_prop_iter, selling[_prop_iter]);
	endforeach

	if (purchase.isBook())
		var bookinfo := GetObjProperty(purchase, "bookinfo");
		purchase.setAuthor(bookinfo.Author );
		purchase.setTitle(bookinfo.Title);
		if (bookinfo.readonly == 1)
			purchase.ReadOnly(1);
		else
			purchase.ReadOnly(0);
		endif
		var book_data_file := DFOpenDataFile(":sysbook:books", DF_CREATE);
		var book_elem := DFFindElement(book_data_file, Hex(purchase.serial), DF_CREATE);
		book_elem.SetProp("Contents", bookinfo.contents);
	endif
	purchase.movable := 1;
endfunction

function CreateItemTemplate(item)
	if ( !item.IsA(POLCLASS_ITEM) )
		var temp := error;
		temp.+errortext := "Error: CreateItemTemplate() -> Object passed is not an item.";
		return temp;
	endif
	
	var template := GetItemDescriptor(CInt(item.objtype));
	if ( template == error )
		return template;
	endif
	
	template.+Amount;
	foreach property in template
		var item_property := item.get_member(Lower(CStr(_property_iter)));
		if ( item_property != error )
			template[_property_iter] := item_property;
		endif
		sleepms(2);
	endforeach

	//precisa por as props que nao ficam em cfgs manualmente
	if (item.xeast != error)
		template["xeast"] := item.xeast;
	endif
	if ( item.xwest != error)
		template["xwest"] := item.xwest;
	endif
	if ( item.ynorth != error )
		template["ynorth"] := item.ynorth;
	endif
	if ( item.ysouth != error )
		template["ysouth"] := item.ysouth;
	endif
	if ( item.gumpwidth != error)
		template["gumpwidth"] := item.gumpwidth;
	endif
	if ( item.gumpheight != error)
		template["gumpheight"] := item.gumpheight;
	endif

	if ( item.ar_mod != error )
		template["ar_mod"] := item.ar_mod;
	endif

	if ( item.onhitscript != error )
		template["onhitscript"] := item.onhitscript;
	endif

	if (item.quality != error)
		template["quality"] := item.quality;
	endif

	if (item.hp != error )
		template["hp"] := item.hp;
	endif

	if (item.maxhp_mod != error)
		template["maxhp_mod"] := item.maxhp_mod;
	endif

	if ( item.usescript != error)
		template["usescript"] := item.usescript;
	endif

	if ( item.equipscript != error)
		template["equipscript"] := item.equipscript;
	endif

	if ( item.unequipscript != error )
		template["unequipscript"] := item.unequipscript;
	endif

	if ( item.dmg_mod != error)
		template["dmg_mod"] := item.dmg_mod;
	endif

	if (item.hitscript != error )
		template["hitscript"] := item.hitscript;
	endif

	foreach name in GetObjPropertyNames(item);
		template.CProps.insert(name, GetObjProperty(item, name));
		sleepms(2);
	endforeach
	
	if (item.isBook())
		var bookinfo := item.extractInfo();
		if (!item.isWritable())
			bookinfo.+readonly := 1;
		else
			bookinfo.+readonly := 0;
		endif
		
		template.Cprops.insert("bookinfo", bookinfo);
	endif

	if (item.isA(POLCLASS_WEAPON))
		template["damage_cold_mod"] := item.damage_cold_mod;
		template["damage_fire_mod"] := item.damage_fire_mod;
		template["damage_energy_mod"] := item.damage_energy_mod;
		template["damage_physical_mod"] := item.damage_physical_mod;
		template["damage_poison_mod"] := item.damage_poison_mod;
	endif

	if (item.isa(POLCLASS_ITEM))
		template["resist_cold_mod"] := item.resist_cold_mod;
		template["resist_fire_mod"] := item.resist_fire_mod;
		template["resist_energy_mod"] := item.resist_energy_mod;
		template["resist_physical_mod"] := item.resist_physical_mod;
		template["resist_poison_mod"] := item.resist_poison_mod;
	endif

	return template;
endfunction
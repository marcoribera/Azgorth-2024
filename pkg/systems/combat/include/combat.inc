
use uo;
include ":brainai:npcUtil";
include ":combat:weaponInfo";
include ":faccao:faccao";
include ":ghaia:ghaiaUtil";

function DistanceChecks(attacker, defender)
	var cur_range := Distance(attacker, defender);
	var wpn_a_elem2 := GetWpnCfgInfo(attacker, attacker.weapon);
	if ( wpn_a_elem2.MaxRange == error )
		return 1;
	endif
	if ( cur_range > wpn_a_elem2.MaxRange )
		CombatDbgMsg(attacker, "Oponente esta muito longe.");
		return 0;
	elseif ( cur_range < wpn_a_elem2.MinRange )
		CombatDbgMsg(attacker, "Oponente esta muito perto.");
		return 0;
	else
		return 1;
	endif
endfunction

function GetWpnCfgInfo(mobile, weapon)

	var npc_cfg2	:= ReadConfigFile("::npcdesc");
	var item_cfg2	:= ReadConfigFile(":*:itemdesc");

	if ( mobile.IsA(POLCLASS_NPC) && weapon.objtype == 0xF020 )
		var npctemplate := mobile.npctemplate;
		npctemplate := npctemplate[10, len(npctemplate)-9];
		return npc_cfg2[npctemplate];
	else
		return item_cfg2[weapon.objtype];
	endif

endfunction
  
function PlayHitSounds(attacker, defender)
	var wpn_a_elem2 := GetWpnCfgInfo(attacker, attacker.weapon);
	var npc_cfg2	:= ReadConfigFile("::npcdesc");

	if ( wpn_a_elem2.HitSound )
		PlaySoundEffect(attacker, wpn_a_elem2.HitSound);
	else
		//NPC intrinsic weapon
		PlaySoundEffect(attacker, wpn_a_elem2.AttackHitSound);
	endif
	
	var damaged_sound;
	if ( defender.npctemplate )
		var npctemplate := defender.npctemplate;
		npctemplate := npctemplate[10, len(npctemplate)-9];
		damaged_sound := npc_cfg2[npctemplate].DamageSound;		
	else
		var sounds;
		if ( defender.gender == 0 )
			// Male
			sounds := array(341, 342, 343, 345, 346);
		else
			// Female
			sounds := array(332, 333, 334, 335, 336);
		endif
		damaged_sound := sounds[RandomInt(sounds.size())+1];
	endif
	
	PlaySoundEffect(defender, damaged_sound);
endfunction

/*function PlayMissSound(attacker)
	if ( wpn_a_elem.MissSound )
		PlaySoundEffect(attacker, wpn_a_elem.MissSound);
	else
		//NPC intrinsic weapon
		PlaySoundEffect(attacker, wpn_a_elem.AttackMissSound);
	endif
endfunction
*/

function PlayAttackAnimation(attacker)
	var attack_anim := 0x0c; // Look around for npcs?
	var wpn_a_elem2 := GetWpnCfgInfo(attacker, attacker.weapon);
	if ( wpn_a_elem2.Anim )
		var anim_list := GetConfigStringArray(wpn_a_elem2, "Anim");
		attack_anim := anim_list[RandomInt(anim_list.size())+1];
	endif
	if ( attacker.graphic != 400 || attacker.graphic != 401)
		attack_anim := 0x0c;
	endif
	
	PerformAction(attacker, CInt(attack_anim));
	
	return attack_anim;
endfunction
  
function GetRawDmg(attacker)
  
    if (attacker.npctemplate)
      var npcelem := NPC_GetNPCConfig(attacker.npctemplate);
      var dmg := GetConfigString(npcelem, "AttackDamage");
      return RandomDiceRoll(dmg);
    else
      var dmg := CS_GetWeaponInfo(attacker.weapon, "Damage");
      return RandomDiceRoll(dmg);
    endif
  
endfunction

/*function PlayHitSounds(attacker, defender := 0)
	var wpn_a_elem2 := GetItemCFGInfo(attacker, attacker.weapon);
   	var metamorfose := GetObjProperty(attacker, "metamorfose");


   if (metamorfose)
		var npcelem := NPC_GetNPCConfig(":ghaia:{}".format(metamorfose));
      PlaySoundEffect(attacker, npcelem.AttackSound);
	elseif ( wpn_a_elem2.HitSound )
		PlaySoundEffect(attacker, wpn_a_elem2.HitSound);
	else
		//NPC intrinsic weapon
		var npcelem := NPC_GetNPCConfig(attacker.npctemplate);
		PlaySoundEffect(attacker, npcelem.AttackSound);
	endif
	
	if (defender)
		var hurt_sound;
      var metamorfose_2 := GetObjProperty(defender, "metamorfose");
      if (metamorfose_2)
         hurt_sound := NPC_GetNPCConfig(":ghaia:{}".format(metamorfose_2)).HurtSound;
		elseif ( defender.npctemplate )
			var npcelem := NPC_GetNPCConfig(defender.npctemplate);
			hurt_sound  := npcelem.HurtSound;		
		else
			var sounds;
			if ( defender.gender == 0 )
				// Male
				sounds := array(341, 342, 343, 345, 346);
			else
				// Female
				sounds := array(332, 333, 334, 335, 336);
			endif
			hurt_sound := sounds.randomentry();
		endif
		
		PlaySoundEffect(defender, hurt_sound);
	endif
endfunction
*/
/*
function PlayMissSound(attacker, wpn_elem)
	if ( wpn_elem.MissSound )
		PlaySoundEffect(attacker, wpn_elem.MissSound);
	else
		//NPC intrinsic weapon
		var npcelem := NPC_GetNPCConfig(attacker.npctemplate);
		if (!npcelem.MissSound)
			var miss_sound := array{569, 570, 571};
			PlaySoundEffect(attacker, miss_sound.randomentry());
		else
			PlaySoundEffect(attacker, npcelem.MissSound);
		endif
	endif
endfunction
*/

function PlayMissSound(attacker)
	var miss_sound := array{569, 570, 571};
	PlaySoundEffect(attacker, miss_sound.randomentry());
endfunction

function CalculateHitChancehab(attacker, targ)

	var wpn_a, wpn_d;
	var wpn_a_elem, wpn_d_elem;

	wpn_a := attacker.weapon;
	wpn_d := targ.weapon;
	
	wpn_a_elem := GetWpnCfgInfo(attacker, wpn_a);
	wpn_d_elem := GetWpnCfgInfo(targ, wpn_d);

	var skill_a;
	var skill_d;
	if ( !attacker.npctemplate )
		skill_a := CDbl(AP_GetSkill(attacker, wpn_a.attribute));

	/*	if ( GetEquipmentByLayer(attacker, LAYER_MOUNT))
			skill_a := skill_a / 2;
		endif
	*/
		if (wpn_a.attribute == WRESTLING) // Ajustar se for soco pra melhor melee combat
			if (AP_GetSkill(attacker, MELEECOMBAT1H) > AP_GetSkill(attacker, MELEECOMBAT2H))
				skill_a := AP_GetSkill(attacker, MELEECOMBAT1H);
			else
				skill_a := AP_GetSkill(attacker, MELEECOMBAT2H);
			endif
		endif

	else
		skill_a := CDbl(AP_GetSkill(attacker, WRESTLING));
	endif

	if ( GetObjProperty(attacker, "metamorfose") != error)
		skill_a := cint(GetObjProperty(attacker, "skill_a"));
	endif

	if ( !targ.npctemplate )
		skill_d := CDbl(AP_GetSkill(targ, wpn_d.attribute));
		if ( GetEquipmentByLayer(targ, LAYER_MOUNT))
			skill_d := skill_d / 2;
		endif
		if (wpn_d.attribute == WRESTLING)
			if (AP_GetSkill(targ, MELEECOMBAT1H) > AP_GetSkill(targ, MELEECOMBAT2H))
				skill_d := AP_GetSkill(targ, MELEECOMBAT1H);
			else
				skill_d := AP_GetSkill(targ, MELEECOMBAT2H);
			endif		
		endif
	else
		skill_d := CDbl(AP_GetSkill(targ, WRESTLING));
	endif

	if ( GetObjProperty(targ, "metamorfose") != error)
		skill_d := cint(GetObjProperty(targ, "skill_d"));
	endif

	var hit_chance := 50.0 + ((skill_a - skill_d) / 1.5);

	hit_chance := hit_chance + CDbl( GetObjProperty(attacker, "hitbonus") ) + attacker.hitchance_mod - targ.evasionchance_mod;
	hit_chance := hit_chance + cint( GetObjProperty(attacker, "#attackbonus" ));	


	if (getcooldown(targ, "evasion_chance"))
		hit_chance := hit_chance - 10;
	endif

	if (TemHabilidade(attacker, "Postura Ofensiva"))
		hit_chance := hit_chance + 10;
	endif
	if (TemHabilidade(targ, "Postura Ofensiva"))
		hit_chance := hit_chance + 5;
	endif
	if (TemHabilidade(attacker, "Postura Defensiva"))
		hit_chance := hit_chance - 5;
	endif
	if (TemHabilidade(targ, "Postura Defensiva"))
		hit_chance := hit_chance - 10;
	endif

	if (TemHabilidade(attacker, "Poder da Alianca"))
		var guild := FindGuild(attacker);
		var members := guild.members;
		var guildbonus := 0;

		foreach mob in members
			guildbonus := guildbonus +1 ;
		endforeach

		hit_chance := hit_chance + guildbonus;
		//hit_chance := hit_chance + attacker.party.members.size() * 2;
	endif

	if ( GetObjProperty(attacker, "armapreferida") == attacker.weapon.objtype )
		hit_chance := hit_chance + 5;
	endif

	if ( cint(GetObjProperty(attacker, "#attackbonus")) != 0 )
		EraseObjProperty(attacker, "#attackbonus");
	endif

	if ( isBloodied(attacker) )
		if (TemHabilidade(targ, "Iludir o Fraco") )
			hit_chance := hit_chance - 10;
		endif
		if (GetCooldown(attacker, "Ataques Enlouquecidos") )
			hit_chance := hit_chance + 100;
		endif
	endif
	if ( isBloodied(targ) )
		if (TemHabilidade(targ, "Esquiva Instintiva") )
			hit_chance := hit_chance - 10;
		endif
		if (GetCooldown(targ, "Ataques Enlouquecidos") )
			hit_chance := hit_chance + 100;
		endif
	endif
	if (GetCooldown(attacker, "desafioheroico") > 0 )
		if ( GetObjProperty(attacker, "desafiante") != targ.serial)
			hit_chance := hit_chance - 10;
		endif
	endif

	if (GetCooldown(attacker, "semdor") > 0) //penaliza ataque
		hit_chance := hit_chance - (GetObjProperty(attacker, "#semdorbonus"));
	endif
	if (GetCooldown(targ, "semdor") > 0) //beneficia defesa
		hit_chance := hit_chance - (GetObjProperty(targ, "#semdorbonus"));
	endif

	
	if (GetCooldown(targ, "resistenciaheroica") )
		var hostiles := ListHostiles(targ, 3);
		if ( hostiles.size() > 1)
			hit_chance := hit_chance  - 20;
		endif
	endif

	if (temhabilidade(attacker, "Lobo Solitario") )
		var hostiles := ListHostiles(attacker, 3);
		if ( hostiles.size() > 1)
			hit_chance := hit_chance + 15;
		endif
	endif

	var tiroCerteiro := Cint(getObjProperty(attacker.weapon, "tirocerteiro"));
	if ( tiroCerteiro > 0)
		tiroCerteiro := tiroCerteiro - 1;
		if (tiroCerteiro == 0)
			EraseObjProperty(attacker.weapon, "tirocerteiro");
		else
			SetObjProperty(attacker.weapon, "tirocerteiro", tiroCerteiro);
		endif
		hit_chance := hit_chance + 100.0;
	endif

	if (TemHabilidade(targ, "Luta Defensiva"))
		hit_chance := hit_chance -10;	
	endif

	if (hit_chance < 5.0)
		hit_chance := 5.0;
	endif

	if (targ.acctname  )
		if (!targ.warmode)
			hit_chance := 100;
		endif
	endif

	return hit_chance;

endfunction

function CalculateRawDamage(attacker, weapon)

	if (attacker.npctemplate)
		var npctemplate := NPC_ParseTemplateName(attacker.npctemplate);
		var cfg := ReadConfigFile(":brainai:npcdesc");
		var dmg := cfg[npctemplate.template].AttackDamage;
		//printtextabove(attacker, "dmg dice " + dmg);
		return ( RandomDiceRoll(dmg) );
	else
		var weaponcfg := ReadConfigFile(":*:itemdesc");
		var weaponelem := FindConfigElem(weaponcfg, weapon.objtype);
		var dmg;
		if ( GetObjProperty(attacker, "metamorfose") != error)
      dmg := RandomDiceRoll( GetObjProperty(attacker, "dicedmg"));
		else
    dmg :=  RandomDiceRoll(cstr(weaponelem.Damage));
    endif
		return (  dmg + weapon.dmg_mod );
	endif

endfunction

function CheckForAmmo(who, bow, qty := 1)
	// var aljavas := array{0x791c, 0x2fb7};
	// var aljava;
	// if (aljava.objtype in aljavas)
	// 	aljava := GetEquipmentByLayer(who, 0x14);
	// else
	// 	aljava := GetEquipmentByLayer(who, 0xc);
	// endif

	var aljava := GetEquipmentByLayer(who, 20);
	
	if(aljava.objtype != 0x791c || aljava.objtype != 0x2fb7)
		aljava := GetEquipmentByLayer(who, 12);
	endif

	var ammo := GetObjProperty(bow, "ammo");
	if (!ammo)
		SendSysMessageEx(who, "Você não definiu o tipo de flecha que vai usar. [clique 2x no arco fora do modo agressivo e na flecha ]");
		return 0;
	elseif ( ConsumeSubstance( aljava, ammo, qty ) == error)
		SendSysMessageEx(who, "Você não definiu o tipo de flecha que vai usar ou ela acabou.");
		return 0;
	endif

   return ammo;
endfunction

function CheckifIsAlly(who, mobiles)
   var party := who.party.members;
   var hostiles := ListHostiles(who, 20, LH_FLAG_INCLUDE_HIDDEN);
   
   if (typeof(mobiles) != "Array")
      if (mobiles in hostiles)
         return 0;
      elseif (mobiles.npctemplate)
         if (mobiles.master && (mobiles.master != who && mobiles.master in party))
            return 0;
         elseif(mobiles.alignment != 2 && (who.connected || who.alignment == 2))
            return 0;
         endif
      elseif (who.murderer)
         return 0;
      endif

      return 1;
   endif

   var newarray := array{};
   foreach mobile in mobiles
      sleepms(2);
      if (mobile in hostiles)
          newarray.append(mobile);
      elseif (mobile.npctemplate)
         if (mobile.master == who || mobile.master in party)
            continue;
         elseif (GetObjProperty(mobile, "guildid") == who.guildid)
            continue;
         else
            if (who.npctemplate)
               if (mobile.alignment == who.alignment)
                  continue;
               endif
            elseif (mobile.alignment == 2)
               continue;
            endif
         endif
      elseif (mobile in party || mobile == who)
         continue;
      endif

      newarray.append(mobile);
   endforeach

   return newarray;
endfunction

function GetCoordsInCircle(x, y, radius)
   var circle_points := array{ struct{ x := x, y := y } };

   var radius2 := radius * radius;

   var i;
   for (i := 0; i < radius; i+=1)
      var j := 0;
      while (i*i + j*j < radius2 )
      sleepms(2);
         if (i != 0 || j != 0)
            if (j != 0) circle_points.append(struct{ x := x+i, y := y+j }); endif
            if (j != 0 && i != 0) circle_points.append(struct{ x := x-i, y := y-j }); endif
            if (j != 0 || i != 0) circle_points.append(struct{ x := x+i, y := y-j }); endif
            if (i != 0) circle_points.append(struct{ x := x-i, y := y+j }); endif
         endif
         j += 1;
      endwhile
   endfor

   return circle_points;
endfunction

function GetItemCFGInfo(mobile, wpn)
	if ( mobile.IsA(POLCLASS_NPC) )
		var to_return := GetNpcRandomAttack(mobile);
      if (wpn.objtype != 0x1F020)
         var wpn_elem := GetItemDescriptor(cint(wpn.objtype));
         wpn_elem.hitbonus     := to_return.hitbonus;
         wpn_elem.dicedmg      := to_return.dicedmg;
         wpn_elem.AttackDamage := to_return.dicedmg;
         return wpn_elem;
      endif
      
      return to_return;
	endif
   
	var item_cfg2	:= ReadConfigFile(":*:itemdesc");
	return item_cfg2[wpn.objtype];
endfunction

function GetModByFacing(facing, mod := 0)
	var xmod := 0, ymod := 0;
	case ( facing ) // Should find a more sleek mathish way to do this!
		0: xmod := 0+mod;  ymod := -1; break;
		1: xmod := +1+mod; ymod := -1+mod; break;
		2: xmod := +1; ymod := 0+mod;  break;
		3: xmod := +1+mod; ymod := +1-mod; break;
		4: xmod := 0+mod;  ymod := +1; break;
		5: xmod := -1-mod; ymod := +1-mod; break;
		6: xmod := -1; ymod := 0+mod;  break;
		7: xmod := -1-mod; ymod := -1+mod; break;
		default:
			return error{"errortext":="Invalid facing."};
			break;
	endcase

   return array{ xmod, ymod };
endfunction

function ParryCheck(attacker, defender, byref rawdamage)

	var item_cfg := ReadConfigFile(":*:itemdesc");

	if ( GetCooldown(attacker, "parrydelay") > 0 )
		return 0;
	endif

	var usingWeapon := 0;
	var parryBonus := 0;
	var shield := defender.shield;

	var blocker;
	//verifica se tem a hab esudo do martir
	var wall := defender.party.getProp("BarreiraDeEscudos");
	var wallChance := 0;
	if ( wall )
		blocker := SystemFindObjectBySerial(wall.blocker);
		if (GetCooldown( blocker, "BarreiraDeEscudos") > 0)
			if (Distance(defender, blocker) < 2)
				wallChance := cint(wall.chance);
			endif
		else
			defender.party.eraseProp("BarreiraDeEscudos");
			blocker := 0;
		endif
	endif

	//checa de eh arma, se for diminui a chance
	if ( !shield.IsA(POLCLASS_ARMOR) )
		usingweapon := 1;
		shield := defender.weapon;
		if (shield.objtype == 0xF020 || shield.Attribute == RANGEDCOMBAT)
			return 0;
		endif
	endif

	//pega todos os bonus possiveis total = prop no mob + prop no item + itemdesc
	parryBonus := Cint(GetObjProperty(defender, "parrybonus")) + Cint(GetObjProperty(shield, "parrybonus")) + Cint((item_cfg[shield.objtype]).parrybonus);
	var tempParryBonus := cint(getObjProperty(defender, "#tempparrybonus"));
	if (tempParryBonus);
		parryBonus := parryBonus + tempParryBonus;
		EraseObjProperty(defender, "#tempparrybonus");
	endif

	//pega a skill e calcula a chance final e delay
	var skill := CDbl(AP_GetSkill(defender, PARRY));
	var parryDelay := 8 - cint(( AP_GetSkill(defender, PARRY)/20 )) +  Cint((item_cfg[shield.objtype]).parrydelay);
	if ( TemHabilidade(defender.party.leader, "Inspirar Defesas"))
		parryDelay := parryDelay - 1;
	endif
	var parry_chance := skill / 3;
	if (usingweapon)
		parry_chance := parry_chance /2;
	endif
	parry_chance := parry_chance + parryBonus;
	if (wallChance > parry_chance)
		parry_chance := wallChance;
	endif
	if (attacker.weapon.graphic == 0x26C4) // foices
		parry_chance := parry_chance - parry_chance*0.3; //diminui 30% da chance
	endif

	if (GetCooldown(defender, "barreiraindestrutivel"))
		parry_chance := 100;
		parryDelay := 2;
	endif

	//testa se defendeu ou nao
	var defendeu := 0;
	if ( RandomInt(100) > parry_chance ) // Failed
		return 0;
	else
		//defendeu, verifica as habilidades especiais
		defendeu := 1;
		if (GetCooldown(defender, "defesasgloriosas"))
			var basedamage := CalculateRawDamage(defender, defender.weapon);
			var dmgList := dictionary;
			dmgList := getNewDamageList(defender);
			var total := CalculateTotalDamage(attacker, basedamage, defender, dmgList);
			total:= total + 15;
			ApplyRawDamage(attacker, total);
			MostraDano(defender, attacker, total);
			MakeBleed(attacker);
			PlaySoundEffect(attacker, 0x51D);
			TS_StartTimer(attacker, "paralysis", 8);
			PrintTextAbove(defender, "*Bate com o escudo em "+attacker.name+"*");
			PrintTextAbove(attacker, "*Atordoado*");
			SendSysMessageEx(attacker, "Voce foi acertado violentamente com um escudo!", SSM_FAIL);
			SetCooldown(defender, "defesasgloriosas", 0);
		endif

		//pike
		if ( (attacker.weapon.graphic == 0x26C8) && ( usingWeapon ))
			var defweapon := defender.weapon;
			if (defweapon.objtype != 0xF020)
				var x := defender.x + RandomInt(2)+1;
				var y := defender.y + RandomInt(2)+1;
				MoveObjectToLocation(defweapon, x, y,  GetWorldHeight(x, y), attacker.realm, MOVEOBJECT_FORCELOCATION);
			endif
		endif

		if (GetCooldown(defender, "defesarevigorante"))
			healdamage(defender, rawdamage);
		endif

	endif

	//sobe a skill
	if (attacker.npctemplate && defender.connected)
		skill := cint(skill) + 15;
		var skillAttacker := cint(AP_GetSkill(attacker, attacker.weapon.attribute));
		if ( skill < skillAttacker )
			//printtextabove(attacker, "return " + skillDefender);
		else
			if (defendeu)
				SkillCheck(defender, PARRY, -1);
			else
				if ( RandomInt(5) == 1 )
					SkillCheck(defender, PARRY, -1);
				endif
			endif
		endif
	endif



	//PerformAction(defender, ANIM_TWIST_DODGE);
	if (blocker)
		printtext(blocker, "*defende o golpe pelo aliado*");
		SetCooldown(attacker, "parrydelay", parryDelay*2);
	else
		PrintText(defender, "*defende o golpe*");
		SetCooldown(attacker, "parrydelay", parryDelay);
	endif

	rawdamage := 0;

	//causa dano no shield
	SendSysMessage(defender, shield.desc+" levou algum dano.");
	var shieldhp := shield.hp - 1;
	if ( shieldhp <= 0 )
		SendSysMessage(defender, shield.desc+" foi destruido.");
		MoveObjectToLocation(shield, 1, 1, 1, defender.realm, MOVEOBJECT_FORCELOCATION);
		SetObjProperty(shield, "broken", 1);
		SetObjProperty(shield, "realdesc", shield.desc);
		SetName(shield, shield.desc+" [danificado]");
		MoveItemToContainer(shield, defender.backpack);
		//DestroyItem(shield);
	elseif ( shield.hp > shield.maxhp )
		shield.hp := shield.maxhp;
	else
		shield.hp := shieldhp;
	endif

	return 1;
endfunction

/*
function GetNpcRandomAttack(mobile)
	var attacks := GetObjProperty(mobile, "customAttacks");
	if (!attacks)
		attacks := GetNpcAttacks(mobile.npctemplate);
	endif
	
	if (!attacks || !attacks[1])
		return 0;
	endif

	var result := attacks.randomEntry();

	return result;
endfunction
*/

/*
function GetNpcAttacks(npctemplate)
	npctemplate := NPC_GetNPCConfig(npctemplate);
	var attacks := GetConfigStringArray(npctemplate, "Attack");
	if (!attacks || !attacks[1])
		return array{};
	endif

	var result := array{};
	foreach attack in attacks
		attack := SplitWords(attack, " ");

		var attack_s := struct{
			"name"    := attack[1],
			"dicedmg"    := attack[2],
			"DamageType" := attack[3],
			"AttackAnim" := cint(attack[4]),
         "AttackDamage" := cint(npctemplate.AttackDamage),
			"MaxRange"   := npctemplate.MaxRange,
			"MinRange"   := npctemplate.MinRange
		};
		result.append(attack_s);
		sleepms(2);
	endforeach

	return result;
endfunction
*/

function PushBack(who, dist, delay)

	var x := 0;
	var y := 0;

	if (who.facing == 0)
		y :=  1;
	elseif (who.facing == 1)
		y :=  1;
		x := - 1;
	elseif (who.facing == 2)
		x :=  - 1;
	elseif (who.facing == 3)
		x :=  - 1;
		y :=  - 1;
	elseif (who.facing == 4)
		y :=  - 1;
	elseif (who.facing == 5)
		x :=   1;
		y :=  - 1;
	elseif (who.facing == 6)
		x :=   1;
	elseif (who.facing == 7)
		x :=  1;
		y :=  1;
	endif

	var i := 0;
	for( i := 1; i <= dist; i := i + 1)
		var finalx := (who.x + x);
		var finaly := (who.y + y);
		var err := MoveObjectToLocation( who, finalx, finaly, who.z, who.realm );
		sleepms(delay);
		if (err == error)
			PrintText(who, "*bate no obstaculo*");
			break;
		endif
	endfor

endfunction

function PushBackEX(who, dist, delay, invert := 0)
   var mod := GetModByFacing(who.facing);
	var x := mod[1];
	var y := mod[2];

	if (!invert)
		x := x * -1;
		y := y * -1;
	endif
   
   for i:= 1 to dist
		var finalx := (who.x + x);
		var finaly := (who.y + y);
		if (!CheckLosAt(who, finalx, finaly, who.z))
			return 0;
		endif
		var err := MoveObjectToLocation( who, finalx, finaly, who.z, who.realm, MOVEITEM_IGNOREMOVABLE );
		sleepms(delay);
		if (err == error)
			PrintText(who, "*bate no obstaculo*");
			return 0;
		endif
	endfor

   return 1;
endfunction

function checkGender(object)
	var gender := 0;
	if (object.graphic)
		case (object.graphic)
			400:
			605: // elf
				gender := 1;
			401:
			606: // elf
				gender := 2;
		endcase
	elseif (object.corpsetype)
		case (object.graphic)
			400:
			605: // elf
			403:
			607: // elf
				gender := 1;
			401:
			606: // elf
			404:
			608: // elf
				gender := 2;
		endcase
	elseif (object.gender)
		if (gender == 0)
			gender := 1;
		else
			gender := 2;
		endif
	endif

	return gender;
endfunction

function PlayHurtSound(defender)
	if ( defender.IsA(POLCLASS_NPC) )
      var damaged_sound := NPC_GetNPCConfig(defender).HurtSound;
		if ( damaged_sound && RandomInt(2) == 2)
			PlaySoundEffect(defender, damaged_sound);
		endif
	endif
endfunction

function CheckifIsNotEnemy(who, mobiles)
   var spotserial := GetObjProperty(me, "spawnspot");
   var hostiles := ListHostiles(who);
   var party := who.party.members;
   var newarray := array{};
   foreach mobile in mobiles
      sleepms(2);
      if (mobile in hostiles)
         continue;
      elseif (mobile.npctemplate)
         if (mobile.master == who || mobile.master in party)
            newarray.append(mobile);
         elseif (GetObjProperty(mobile, "guildid") == who.guildid)
            newarray.append(mobile);
         elseif ((spotserial && GetObjProperty(mobile, "spawnspot") == spotserial))
            newarray.append(mobile);
         else
            if (who.npctemplate)
               if (mobile.alignment == who.alignment)
                  newarray.append(mobile);
               endif
            elseif (mobile.alignment == 2)
               newarray.append(mobile);
            endif
         endif
      elseif (mobile in party || mobile == who)
         newarray.append(mobile);
      endif
   endforeach

   return newarray;
endfunction
use cfgfile;
use math;

include "include/damage";
include ":tn:tngumps";

const  EXCEP := 1;
const  NORMAL := 2;
const  POOR := 3;
const STR_ERRORCHANCE := "ERROR 666";
const STR_NULLCHANCE := "nula";
const STR_VERYLOWCHANCE := "muito baixa";
const STR_LOWCHANCE := "baixa";
const STR_INTERMEDIATECHANCE := "media";
const STR_GOODCHANCE := "alta";
const STR_VERYHIGHCHANCE := "muito alta";

//funcao que pergunta por fechadura, caso o item seja lockable
function CraftLockable(who, theitem)
		if (theitem.isDoor())
			SetObjProperty(theitem, "pedreiro", who.serial);
			SetObjProperty(theitem, "criador", who.acctname);
			SetObjProperty(theitem, "validade", CInt(ReadGameClock()+360));
			MoveObjectToLocation(theitem, who.x, who.y, who.z, who.realm, MOVEOBJECT_NORMAL);
			theitem.movable := 0;
		endif

		if (KP_IsLockable(theitem ))
			var fechadura := YesNo(who, "Voce deseja por uma fechadura nesse item?");
			if (fechadura)
				SendSysMessageEx(who, "Escolha a fechadura para por neste item.", SSM_REQUEST);
				var targ := Target(who);
				if (targ)
					if (targ.objtype == 4001)
						if (!theitem.isDoor())
							var lvl := Cint(GetObjProperty(targ, "level"));
							if (!lvl)
								lvl := 1;
							endif
							SetObjProperty(theitem, "level", lvl);
						endif
						var lockid := CInt(GetObjProperty(targ, "LockId"));
						SetObjProperty(theitem, "LockId", lockid);
						DestroyItem(targ);
						SendSysMessageEx(who, "Voce instalou a fechadura com sucesso.", SSM_INFO);
					else
						SendSysMessageEx(who, "Isto nao parece uma fechadura.", SSM_FAIL);
					endif
				endif
			endif
		endif
endfunction

function GetMaterialName(obj);

  if ( (obj == "wood") || (obj == "leather") || (obj == "cloth") || (obj == "metal") || (obj == "glass") || (obj == "stone") || obj == "arrowhead")
    return obj;
  else
    var items := ReadconfigFile("::itemdesc");
    if (items[obj])
      return items[obj].desc;
    endif
  endif


endfunction

//Reference: http://uo.stratics.com/content/aos/baseproperties.shtml#exceptional
// as stratics says, exceptional weapons add +35 dmg increase

function AddQualityDamageMod(who, craft_skill, byref theitem, quality )

	//Qualidade acima de certo valor adiciona também dano físico extra
	var num_bonus := 0.0;
	if(quality >= 3.6) //dois bonus completos
		num_bonus := 2.0;
		theitem.damage_physical_mod := theitem.damage_physical_mod + 15;
	elseif(quality >= 3.2) //um bonus completo mais meio bonus
		num_bonus := 1.5;
		theitem.damage_physical_mod := theitem.damage_physical_mod + 10;
	elseif(quality >= 2.8)
		num_bonus := 1.0;
		theitem.damage_physical_mod := theitem.damage_physical_mod + 5;
	elseif(quality >= 2.0)
		num_bonus := 0.5;
	else
		num_bonus := 0.0;
	endif

	var bonus_principal := (Randomint(64937)+1)%2;
	//SendSysMessage(who,"Rolagem: "+ bonus_principal);
	var itemdesc := ReadConfigFile(":combat:itemdesc");

	var i;
	for(i:= num_bonus; i >= 0.5; i:=i-1.0)
		//Calculo de bonus para velocidade
		if(bonus_principal == 0)
			var base_speed := Cint(itemdesc[theitem.objtype].Speed);
			var bonus_speed := Cdbl(base_speed*(quality-1.0)/6.0);
			if (i >= 1.0)
				//SendSysMessage(who,"bonus velocidade cheia");
				SetObjProperty(theitem,"attackspeedbonus", Ceil(bonus_speed));
			elseif(i >= 0.5)
				//SendSysMessage(who,"bonus velocidade metade");
				SetObjProperty(theitem,"attackspeedbonus", Ceil(bonus_speed/2.0));
			endif
			bonus_principal:=1;
		//Calculo de bonus para danos
		elseif(bonus_principal == 1)
			var bonus_damage := (quality-1.0)*8.0;
			if (i >= 1.0)
				//mantém o bonus cheio
				//SendSysMessage(who,"bonus dano cheio");
			elseif(i >= 0.5)
				//muda o bonus para metade
				bonus_damage := bonus_damage/2.0;
				//SendSysMessage(who,"bonus dano metade");
			endif
			//Aplica os bonus com base nas propriedades da arma
			var cont := 1;
			var sorteio;
			//Só aplica bonus em tipos de dano que arma já tenha, seja pelo item base, ou seja pelo material
			while (cont <= CInt(bonus_damage))
				sorteio := RandomInt(5) + 1;
				case(sorteio)
					1:
						if (theitem.damage_physical)
							theitem.damage_physical_mod := theitem.damage_physical_mod +1;
							cont:=cont+1;
						endif
						break;
					2:
						if (theitem.damage_fire)
							theitem.damage_fire_mod := theitem.damage_fire_mod +1;
							cont:=cont+1;
						endif
						break;
					3:
						if (theitem.damage_cold)
							theitem.damage_cold_mod := theitem.damage_cold_mod +1;
							cont:=cont+1;
						endif
						break;
					4:
						if (theitem.damage_energy)
							theitem.damage_energy_mod := theitem.damage_energy_mod +1;
							cont:=cont+1;
						endif
						break;
					5:
						if (theitem.damage_poison)
							theitem.damage_poison_mod := theitem.damage_poison_mod +1;
							cont:=cont+1;
						endif
						break;
				endcase
				sleepms(7);
			endwhile
			bonus_principal:=0;
		endif
	endfor

/* //Hab foi desativada porque estava desbalanceada
	if(craft_skill == "MetalWorking")
		if(TemHabilidade(who,"Armas Velozes"))
			//SendSysMessage(who,"bonus velocidade" + (quality-1)/2 + "qualidade: "+ quality);
			SetObjProperty(theitem,"attackspeedbonus", CInt(elem.Speed * (0.1 + (quality-1)/4)));
			//theitem.speed_mod := CInt(elem.Speed * (0.1 + (quality-1)/4));
		endif
		//Acplica dano extra pra arma
		if(TemHabilidade(who,"Amolador de Laminas") && (elem.TipoAtaque == "Cortante"))
			//SendSysMessage(who,"bonus dano" + theitem.quality-1);
			theitem.dmg_mod := CInt(elem.MaxDam * (0.1 + (quality-1)/4));
		endif
		if(TemHabilidade(who,"Ponta Temperada") && (elem.TipoAtaque == "Perfurante"))
			theitem.dmg_mod := CInt(elem.MaxDam * (0.1 + (quality-1)/4));
		endif
		if(TemHabilidade(who,"Densidade Esmagadora") && (elem.TipoAtaque == "Contusivo"))
			theitem.dmg_mod := CInt(elem.MaxDam * (0.1 + (quality-1)/4));
		endif
	elseif(craft_skill == "WoodWorking")
		if(TemHabilidade(who,"Entalhe para Combate"))
			theitem.speed_mod := CInt(elem.Speed * (0.1 + (quality-1)/4));
		endif
		if(TemHabilidade(who,"Flexao da Madeira")) //&& (Cint(elem.maxrange) > 2))
			theitem.dmg_mod := CInt(elem.MaxDam * (0.1 + (quality-1)/4));
		endif
	endif
*/
	return;
endfunction

function AddQualityResistMod(who, craft_skill, byref theitem, quality)
//Define quantos bonus Opcionais serão incluídos nas armaduras
	var num_bonus := 0.0;
	if(quality >= 3.6) //dois bonus completos
		num_bonus := 2.0;
		theitem.resist_physical_mod := theitem.resist_physical_mod + 2;
	elseif(quality >= 3.2) //um bonus completo mais meio bonus
		num_bonus := 1.5;
		theitem.resist_physical_mod := theitem.resist_physical_mod + 1;
	elseif(quality >= 2.8)
		num_bonus := 1.0;
		theitem.resist_physical_mod := theitem.resist_physical_mod + 1;
	elseif(quality >= 2.0)
		num_bonus := 0.5;
	else
		num_bonus := 0.0;
	endif

	var bonus_principal := (Randomint(64937)+1)%2;
	var itemdesc := ReadConfigFile(":combat:itemdesc");

	// Check if this is leather armor
	var isLeather := 0;
	if (craft_skill == "Tailoring")
		var leatherTypes := array(0x13C7, 0x13C5, 0x13CD, 0x13C6, 0x1DB9, 0x13CB, 0x13CC, 0x13D3, 0x1C06, 0x1C08, 0x1C0A, 0x1C00);
		foreach type in leatherTypes
			if (theitem.objtype == type)
				isLeather := 1;
				break;
			endif
		endforeach
	endif

	var i;
	for(i:= num_bonus; i >= 0.5; i:=i-1.0)
		//Calculo de bonus na Penalidade de Destreza
		if(bonus_principal == 0)
			var base_DexPenalty := Cint(itemdesc[theitem.objtype].DexPenalty);
			var bonus_DexPenalty := Cdbl(base_DexPenalty*(quality-1.0)/3.2);
			if (i >= 1.0)
				if((base_DexPenalty == 0) && (quality > 1.0))
					theitem.weight_multiplier_mod := Max(0.2, 1-((quality-1.0)/3.2));
				else
					SetObjProperty(theitem,"penaltybonus", Floor(bonus_DexPenalty));
				endif
			elseif(i >= 0.5)
				if((base_DexPenalty == 0) && (quality > 1.0))
					theitem.weight_multiplier_mod:= Max(0.2, 1-((quality-1.0)/6.4));
				else
					SetObjProperty(theitem,"penaltybonus", Floor(bonus_DexPenalty/2.0));
				endif
			endif
			bonus_principal:=1;
		//Calculo de bonus para resistencias
		elseif(bonus_principal == 1)
			var bonus_prot := (quality-1.0)*2.0;
			if (i >= 1.0)
				// Full bonus
			elseif(i >= 0.5)
				bonus_prot := bonus_prot/2.0;
			endif

			// Apply physical resistance bonus for leather armor
			if (isLeather)
				var leather_bonus := bonus_prot * 1.5; // Leather gets 50% more physical resistance bonus
				theitem.resist_physical_mod := theitem.resist_physical_mod + Floor(leather_bonus);
			endif

			// Apply elemental resistances
			var cont := 1;
			var sorteio;
			while (cont <= CInt(bonus_prot))
				sorteio := RandomInt(5) + 1;
				case(sorteio)
					1:
						if (theitem.resist_physical)
							theitem.resist_physical_mod := theitem.resist_physical_mod + 1;
							cont:=cont+1;
						endif
						break;
					2:
						if (theitem.resist_fire)
							theitem.resist_fire_mod := theitem.resist_fire_mod + 1;
							cont:=cont+1;
						endif
						break;
					3:
						if (theitem.resist_cold)
							theitem.resist_cold_mod := theitem.resist_cold_mod + 1;
							cont:=cont+1;
						endif
						break;
					4:
						if (theitem.resist_poison)
							theitem.resist_poison_mod := theitem.resist_poison_mod + 1;
							cont:=cont+1;
						endif
						break;
					5:
						if (theitem.resist_energy)
							theitem.resist_energy_mod := theitem.resist_energy_mod + 1;
							cont:=cont+1;
						endif
						break;
				endcase
				sleepms(7);
			endwhile
			bonus_principal:=0;
		endif
	endfor
	return;
endfunction

 function AddItemDescription(who, item)
	var repeating := 1;
	var str;
	SendSysMessage(who, "Utilize apenas 3 linhas do gump.");
	while (repeating)
		str := SmallRequestGump(who, "Descreva o item", 1);
		if (len(str) < 84)
			break;
		endif
		SendSysMessage(who, "Voce excedeu o numero maximo de caracteres! Utilize apenas 3 linhas do gump");
	endwhile
	SetObjProperty(item, "description", str);
endfunction

function GetChanceString(value)
	if (TypeOf(value) == "String")
		return STR_ERRORCHANCE;
	elseif (value < 1)
		return STR_NULLCHANCE;
	elseif (value <= 20)
		return STR_VERYLOWCHANCE;
	elseif (value <= 40)
		return STR_LOWCHANCE;
	elseif (value <= 60)
		return STR_INTERMEDIATECHANCE;
	elseif (value <= 80)
		return STR_GOODCHANCE;
	elseif (value > 80)
		return STR_VERYHIGHCHANCE;
	endif
endfunction
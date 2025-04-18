/* $Id: client.inc 1343 2006-05-12 07:56:34Z austin $
 *
 * Purpose
 * This include file is designed to hold all the client lowlevel constants.
 * Not all constants might be usable as of now, but in the future, they
 * might be.
 *
 */


//TODO: not complete
enum GUMPID_LIST
    GUMPID_TNGUMPS     := 0x000015
endenum

enum HUMAN_ANIMATIONS
	ANIM_WALK_UNARMED         :=  0x0, // Walk unarmed.
	ANIM_WALK_ARMED           :=  0x1, // Walk armed.
	ANIM_RUN_UNARMED          :=  0x2, // Run unarmed.
	ANIM_RUN_ARMED            :=  0x3, // Run armed.
	ANIM_STAND                :=  0x4, // Stand.
	ANIM_FIDGET_1             :=  0x5, // Twist a little (look around?).
	ANIM_FIDGET_2             :=  0x6, // Look down.
	ANIM_STAND_1HAND          :=  0x7, // Stand 1 handed attack.
	ANIM_STAND_2HAND          :=  0x8, // Stand 2 handed attack.
	ANIM_ATTACK_1HAND_WIDE    :=  0x9, // Attack 1 handed wide (sword/any).
	ANIM_ATTACK_1HAND_JAB     :=  0xA, // Attack 1 handed jab (fencing).
	ANIM_ATTACK_1HAND_DOWN    :=  0xB, // Attack 1 handed down (mace).
	ANIM_ATTACK_2HAND_DOWN    :=  0xC, // Attack 2 handed jab (mace).
	ANIM_ATTACK_2HAND_WIDE    :=  0xD, // Attack 2 handed wide.
	ANIM_ATTACK_2HAND_JAB     :=  0xE, // Attack 2 handed jab (spear).
	ANIM_ATTACK_BOW           :=  0x12, // Attack bow.
	ANIM_ATTACK_XBOW          :=  0x13, // Attack xbow.
	ANIM_WALK_WARMODE         :=  0xF, // Walk in warmode.
	ANIM_CAST_DIR             :=  0x10, // Spellcasting direction.
	ANIM_CAST_AREA            :=  0x11, // Spellcasting area.
	ANIM_HIT                  :=  0x14, // Get hit.
	ANIM_DIE_BACK             :=  0x15, // Die backward.
	ANIM_DIE_FOR              :=  0x16, // Die forward.
	ANIM_BOW                  :=  0x20, // Bow.
	ANIM_SALUTE               :=  0x21, // Salute.
	ANIM_EAT                  :=  0x22  // Eat.
endenum


enum LAYERS
	LAYER_NONE        :=  0x0,   // Not used?
	LAYER_HAND1       :=  0x1,   // One handed weapon.
	LAYER_HAND2       :=  0x2,   // Two handed weapon, shield or misc.
	LAYER_SHOES       :=  0x3,   // Shoes.
	LAYER_PANTS       :=  0x4,   // Pants.
	LAYER_SHIRT       :=  0x5,   // Shirt.
	LAYER_HELM        :=  0x6,   // Helm or Hat.
	LAYER_GLOVES      :=  0x7,   // Gloves.
	LAYER_RING        :=  0x8,   // Ring.
	LAYER_UNUSED9     :=  0x9,   // Unused.
	LAYER_NECK        :=  0xA,   // Neck.
	LAYER_HAIR        :=  0xB,   // Hair
	LAYER_WAIST       :=  0xC,   // Waist (half apron).
	LAYER_CHEST       :=  0xD,   // Torso (inner) (chest armor).
	LAYER_WRIST       :=  0xE,   // Bracelet.
	LAYER_PACK2       :=  0xF,   // Unused (backpack, but ord. bp is 0x15).
	LAYER_BEARD       :=  0x10,   // Facial hair.
	LAYER_TUNIC       :=  0x11,   // Torso (middle) (tunic, sash etc.).
	LAYER_EARS        :=  0x12,   // Earrings.
	LAYER_ARMS        :=  0x13,   // Arms.
	LAYER_CAPE        :=  0x14,   // Back (cloak).
	LAYER_PACK        :=  0x15,   // Backpack
	LAYER_ROBE        :=  0x16,   // Torso (outer) (robe)
	LAYER_SKIRT       :=  0x17,   // Legs (outer) (skirt/robe).
	LAYER_LEGS        :=  0x18,   // Legs (inner) (leg armor).
	LAYER_MOUNT       :=  0x19,   // Mount (horse, ostard etc.).
	LAYER_VENDORSTOCK :=  0x1A,   // This vendor will sell and restock.
	LAYER_VENDOREXTRA :=  0x1B,   // This vendor will resell to players but not restock.
	LAYER_VENDORBUY   :=  0x1C,   // This vendor can buy from players but does not stock.
	LAYER_BANKBOX     :=  0x1D,   // Contents of bankbox
	LAYER_TRADE       :=  0x1E    // Can be multiple of these, do not use directly.
endenum


enum SPEECH_TYPES
	SPEECH_REGULAR      := 0x0,
	SPEECH_BROADCAST    := 0x1,
	SPEECH_EMOTE        := 0x2,
	SPEECH_LABEL        := 0x6,
	SPEECH_EMPHASIS     := 0x7,
	SPEECH_WHISPER      := 0x8,
	SPEECH_YELL         := 0x9,
	SPEECH_SPELL        := 0xA
endenum


enum FONTS
	FONT_MAX          :=  0x9,   // Highest font.
	FONT_MIN          :=  0x0,   // Lowest font.

	FONT_BOLD         :=  0x0,   // Bold text.
	FONT_SHADOW       :=  0x1,   // Text with shadow.
	FONT_BOLD_SHAD    :=  0x2,   // Bold text with shadow.
	FONT_NORMAL       :=  0x3,   // Normal (default).
	FONT_GOTHIC       :=  0x4,   // Gothic.
	FONT_ITALIC       :=  0x5,   // Italic
	FONT_SMALL_DARK   :=  0x6,   // Small dark letters.
	FONT_COLOR        :=  0x7,   // Colorful font.
	FONT_RUNE         :=  0x8,   // Rune font (only capitals).
	FONT_SMALL_LIGHT  :=  0x9    // Small light letters.
endenum

// * Colors *
// This is not supposed to be a "complete" colorchart, but a
// quick way to access normal colors.
const COLOR_DEFAULT         := 0x0000;
const COLOR_BLACK           := 0x0001;
const COLOR_INDIGO_DARK     := 0x0007;
const COLOR_INDIGO          := 0x0008;
const COLOR_INDIGO_LIGHT    := 0x0009;

const COLOR_VIOLET_DARK     := 0x000c;
const COLOR_VIOLET          := 0x000d;
const COLOR_VIOLET_LIGHT    := 0x000e;
const COLOR_MAGENTA_DARK    := 0x0011;
const COLOR_MAGENTA         := 0x0012;
const COLOR_MAGENTA_LIGHT   := 0x0013;
const COLOR_RED_DARK        := 0x0020;
const COLOR_RED             := 0x0022;
const COLOR_RED_LIGHT       := 0x0023;
const COLOR_ORANGE_DARK     := 0x002a;
const COLOR_ORANGE          := 0x002b;
const COLOR_ORANGE_LIGHT    := 0x002c;
const COLOR_YELLOW_DARK     := 0x0034;
const COLOR_YELLOW          := 0x0035;
const COLOR_YELLOW_LIGHT    := 0x0036;
const COLOR_GREEN_DARK      := 0x003e;
const COLOR_GREEN           := 0x003f;
const COLOR_GREEN_LIGHT     := 0x0040;
const COLOR_CYAN_DARK       := 0x0057;
const COLOR_CYAN            := 0x0058;
const COLOR_CYAN_LIGHT      := 0x0059;
const COLOR_BLUE_DARK       := 0x0061;
const COLOR_BLUE            := 0x0062;
const COLOR_BLUE_LIGHT      := 0x0063;
const COLOR_GRAY_DARK       := 0x0386;
const COLOR_GRAY            := 0x0387;
const COLOR_GRAY_LIGHT      := 0x0388;
const COLOR_BLACKMETAL      := 0x044e;
const COLOR_GOLDMETAL       := 0x046e;
const COLOR_BSHIMMER        := 0x0480;	// T2A Only : Waterelemental color.
const COLOR_WHITE           := 0x0481;	// T2A Only : True white.
const COLOR_STONE           := 0x0482;	// T2A Only : Rock ( stoneharpy?)
const COLOR_SPECTRAL        := 0x4631;  // Spectral
const COLOR_TRANSLUCENT     := 0x7cf8;  // Translucent

enum DIRECTIONS
	MIN_DIR	:=  0x0,
	MAX_DIR :=  0x7,

	DIR_N	:=  0x0,   // North.
	DIR_NE	:=  0x1,   // Northeast.
	DIR_E	:=  0x2,   // East.
	DIR_SE	:=  0x3,   // Southeast.
	DIR_S	:=  0x4,   // South.
	DIR_SW	:=  0x5,   // Southwest.
	DIR_W	:=  0x6,   // West.
	DIR_NW	:=  0x7    // Northwest.
endenum

enum GRAPHIC_EFFECTS
	GFX_DEATH_VORTEX_BIRTH	:= 0x3789,
	GFX_DEATH_VORTEX	:= 0x37CC,
	GFX_FIELD_OF_BLADES	:= 0x37A0,
	GFX_GLOW		:= 0x37B9,
	GFX_BLACK_GLOW		:= 0x37C3,
	GFX_SMOKE		:= 0x3728,
	GFX_FIZZLE		:= 0x3735
endenum

enum GRAPHIC_FIELDS
	GFX_POISON_GAS		:= 0x113A,
	GFX_POISON_FIELD_EW	:= 0x3915,
	GFX_POISON_FIELD_NS	:= 0x3920,
	GFX_ENERGY_FIELD_EW	:= 0x3946,
	GFX_ENERGY_FIELD_NS	:= 0x3956,
	GFX_PARALYSIS_FIELD_EW	:= 0x3967,
	GFX_PARALYSIS_FIELD_NS	:= 0x3979,
	GFX_FIRE_FIELD_EW	:= 0x398C,
	GFX_FIRE_FIELD_NS	:= 0x3996,
	GFX_ENERGY		:= 0x3818
endenum

enum GRAPHIC_SPARKLES
	GFX_MAGICAL_SPARKLES_EW	:= 0x1153,
	GFX_MAGICAL_SPARKLES_NS	:= 0x1159,
	GFX_BLUE_SPARKLES	:= 0x373A,
	GFX_RED_SPARKLES	:= 0x374A,
	GFX_BLUE_SPARKLE_SWIRL	:= 0x375A,
	GFX_BLUE_SPARKLE_BORDER	:= 0x376A,
	GFX_BLUE_SPARKLE_GROUND	:= 0x3779
endenum

enum GRAPHIC_FIRE
	GFX_SMALL_FIREBALL	:= 0x36E4,
	GFX_LARGE_FIREBALL	:= 0x36D4,
	GFX_WAVY_FIREBALL	:= 0x36F4,
	GFX_EXPLOSION_BALL	:= 0x36FE,
	GFX_FIRE_COLUMN		:= 0x3709,
	GFX_EXPLODE_1		:= 0x36CB,
	GFX_EXPLODE_2		:= 0x36BD,
	GFX_EXPLODE_3		:= 0x36B0
endenum
// Copyright (c) Athena Dev Teams - Licensed under GNU GPL
// For more information, see LICENCE in the main folder

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <limits.h>

#include "../common/timer.h"
#include "../common/nullpo.h"
#include "../common/malloc.h"
#include "../common/showmsg.h"
#include "../common/grfio.h"
#include "../common/ers.h"

#include "skill.h"
#include "map.h"
#include "clif.h"
#include "pc.h"
#include "status.h"
#include "pet.h"
#include "mercenary.h"	//[orn]
#include "mob.h"
#include "battle.h"
#include "party.h"
#include "itemdb.h"
#include "script.h"
#include "intif.h"
#include "log.h"
#include "chrif.h"
#include "guild.h"
#include "date.h"
#include "unit.h"

#define SKILLUNITTIMER_INVERVAL	100
//Guild Skills are shifted to these to make them stick into the skill array.
#define GD_SKILLRANGEMIN 900
#define GD_SKILLRANGEMAX GD_SKILLRANGEMIN+MAX_GUILDSKILL
#define HM_SKILLRANGEMIN 800
#define HM_SKILLRANGEMAX HM_SKILLRANGEMIN+MAX_HOMUNSKILL

int skill_names_id[MAX_SKILL_DB];
const struct skill_name_db skill_names[] = {
 { AC_CHARGEARROW, "AC_CHARGEARROW", "Arrow_Repel" } ,
 { AC_CONCENTRATION, "AC_CONCENTRATION", "Improve_Concentration" } ,
 { AC_DOUBLE, "AC_DOUBLE", "Double_Strafe" } ,
 { AC_MAKINGARROW, "AC_MAKINGARROW", "Arrow_Crafting" } ,
 { AC_OWL, "AC_OWL", "Owl's_Eye" } ,
 { AC_SHOWER, "AC_SHOWER", "Arrow_Shower" } ,
 { AC_VULTURE, "AC_VULTURE", "Vulture's_Eye" } ,
 { ALL_RESURRECTION, "ALL_RESURRECTION", "Resurrection" } ,
 { AL_ANGELUS, "AL_ANGELUS", "Angelus" } ,
 { AL_BLESSING, "AL_BLESSING", "Blessing" } ,
 { AL_CRUCIS, "AL_CRUCIS", "Signum_Crusis" } ,
 { AL_CURE, "AL_CURE", "Cure" } ,
 { AL_DECAGI, "AL_DECAGI", "Decrease_AGI" } ,
 { AL_DEMONBANE, "AL_DEMONBANE", "Demon_Bane" } ,
 { AL_DP, "AL_DP", "Divine_Protection" } ,
 { AL_HEAL, "AL_HEAL", "Heal" } ,
 { AL_HOLYLIGHT, "AL_HOLYLIGHT", "Holy_Light" } ,
 { AL_HOLYWATER, "AL_HOLYWATER", "Aqua_Benedicta" } ,
 { AL_INCAGI, "AL_INCAGI", "Increase_AGI" } ,
 { AL_PNEUMA, "AL_PNEUMA", "Pneuma" } ,
 { AL_RUWACH, "AL_RUWACH", "Ruwach" } ,
 { AL_TELEPORT, "AL_TELEPORT", "Teleport" } ,
 { AL_WARP, "AL_WARP", "Warp_Portal" } ,
 { AM_ACIDTERROR, "AM_ACIDTERROR", "Acid_Terror" } ,
 { AM_AXEMASTERY, "AM_AXEMASTERY", "Axe_Mastery" } ,
 { AM_BERSERKPITCHER, "AM_BERSERKPITCHER", "Aid_Berserk_Potion" } ,
 { AM_BIOETHICS, "AM_BIOETHICS", "Bioethics" } ,
 { AM_CALLHOMUN, "AM_CALLHOMUN", "Call_Homunculus" } ,
 { AM_CANNIBALIZE, "AM_CANNIBALIZE", "Summon_Flora" } ,
 { AM_CP_ARMOR, "AM_CP_ARMOR", "Synthetic_Armor" } ,
 { AM_CP_HELM, "AM_CP_HELM", "Biochemical_Helm" } ,
 { AM_CP_SHIELD, "AM_CP_SHIELD", "Synthetized_Shield" } ,
 { AM_CP_WEAPON, "AM_CP_WEAPON", "Alchemical_Weapon" } ,
 { AM_CULTIVATION, "AM_CULTIVATION", "Cultivation" } ,
 { AM_DEMONSTRATION, "AM_DEMONSTRATION", "Bomb" } ,
 { AM_LEARNINGPOTION, "AM_LEARNINGPOTION", "Potion_Research" } ,
 { AM_PHARMACY, "AM_PHARMACY", "Prepare_Potion" } ,
 { AM_POTIONPITCHER, "AM_POTIONPITCHER", "Aid_Potion" } ,
 { AM_REST, "AM_REST", "Vaporize" } ,
 { AM_RESURRECTHOMUN, "AM_RESURRECTHOMUN", "Homunculus_Resurrection" } ,
 { AM_SPHEREMINE, "AM_SPHEREMINE", "Summon_Marine_Sphere" } ,
 { AM_TWILIGHT1, "AM_TWILIGHT1", "Twilight_Pharmacy_1" } ,
 { AM_TWILIGHT2, "AM_TWILIGHT2", "Twilight_Pharmacy_2" } ,
 { AM_TWILIGHT3, "AM_TWILIGHT3", "Twilight_Pharmacy_3" } ,
 { ASC_BREAKER, "ASC_BREAKER", "Soul_Destroyer" } ,
 { ASC_CDP, "ASC_CDP", "Create_Deadly_Poison" } ,
 { ASC_EDP, "ASC_EDP", "Enchant_Deadly_Poison" } ,
 { ASC_KATAR, "ASC_KATAR", "Advanced_Katar_Mastery" } ,
 { ASC_METEORASSAULT, "ASC_METEORASSAULT", "Meteor_Assault" } ,
 { AS_CLOAKING, "AS_CLOAKING", "Cloaking" } ,
 { AS_ENCHANTPOISON, "AS_ENCHANTPOISON", "Enchant_Poison" } ,
 { AS_GRIMTOOTH, "AS_GRIMTOOTH", "Grimtooth" } ,
 { AS_KATAR, "AS_KATAR", "Katar_Mastery" } ,
 { AS_LEFT, "AS_LEFT", "Lefthand_Mastery" } ,
 { AS_POISONREACT, "AS_POISONREACT", "Poison_React" } ,
 { AS_RIGHT, "AS_RIGHT", "Righthand_Mastery" } ,
 { AS_SONICACCEL, "AS_SONICACCEL", "Sonic_Acceleration" } ,
 { AS_SONICBLOW, "AS_SONICBLOW", "Sonic_Blow" } ,
 { AS_SPLASHER, "AS_SPLASHER", "Venom_Splasher" } ,
 { AS_VENOMDUST, "AS_VENOMDUST", "Venom_Dust" } ,
 { AS_VENOMKNIFE, "AS_VENOMKNIFE", "Throw_Venom_Knife" } ,
 { BA_APPLEIDUN, "BA_APPLEIDUN", "Song_of_Lutie" } ,
 { BA_ASSASSINCROSS, "BA_ASSASSINCROSS", "Impressive_Riff" } ,
 { BA_DISSONANCE, "BA_DISSONANCE", "Unchained_Serenade" } ,
 { BA_FROSTJOKE, "BA_FROSTJOKE", "Unbarring_Octave" } ,
 { BA_MUSICALLESSON, "BA_MUSICALLESSON", "Music_Lessons" } ,
 { BA_MUSICALSTRIKE, "BA_MUSICALSTRIKE", "Melody_Strike" } ,
 { BA_PANGVOICE, "BA_PANGVOICE", "Pang_Voice" } ,
 { BA_POEMBRAGI, "BA_POEMBRAGI", "Magic_Strings" } ,
 { BA_WHISTLE, "BA_WHISTLE", "Perfect_Tablature" } ,
 { BD_ADAPTATION, "BD_ADAPTATION", "Amp" } ,
 { BD_DRUMBATTLEFIELD, "BD_DRUMBATTLEFIELD", "Battle_Theme" } ,
 { BD_ENCORE, "BD_ENCORE", "Encore" } ,	
 { BD_ETERNALCHAOS, "BD_ETERNALCHAOS", "Down_Tempo" } ,
 { BD_INTOABYSS, "BD_INTOABYSS", "Power_Cord" } ,
 { BD_LULLABY, "BD_LULLABY", "Lullaby" } ,
 { BD_RICHMANKIM, "BD_RICHMANKIM", "Mental_Sensing" } ,
 { BD_RINGNIBELUNGEN, "BD_RINGNIBELUNGEN", "Harmonic_Lick" } ,
 { BD_ROKISWEIL, "BD_ROKISWEIL", "Classical_Pluck" } ,
 { BD_SIEGFRIED, "BD_SIEGFRIED", "Acoustic_Rhythm" } ,
 { BS_ADRENALINE, "BS_ADRENALINE", "Adrenaline_Rush" } ,
 { BS_ADRENALINE2, "BS_ADRENALINE2", "Advanced_Adrenaline_Rush" } ,
 { BS_AXE, "BS_AXE", "Smith_Axe" } ,
 { BS_DAGGER, "BS_DAGGER", "Smith_Dagger" } ,
 { BS_ENCHANTEDSTONE, "BS_ENCHANTEDSTONE", "Enchantedstone_Craft" } ,
 { BS_FINDINGORE, "BS_FINDINGORE", "Ore_Discovery" } ,
 { BS_GREED, "BS_GREED", "Greed" } ,
 { BS_HAMMERFALL, "BS_HAMMERFALL", "Hammer_Fall" } ,
 { BS_HILTBINDING, "BS_HILTBINDING", "Hilt_Binding" } ,
 { BS_IRON, "BS_IRON", "Iron_Tempering" } ,
 { BS_KNUCKLE, "BS_KNUCKLE", "Smith_Knucklebrace" } ,
 { BS_MACE, "BS_MACE", "Smith_Mace" } ,
 { BS_MAXIMIZE, "BS_MAXIMIZE", "Power_Maximize" } ,
 { BS_ORIDEOCON, "BS_ORIDEOCON", "Oridecon_Research" } ,
 { BS_OVERTHRUST, "BS_OVERTHRUST", "Power-Thrust" } ,
 { BS_REPAIRWEAPON, "BS_REPAIRWEAPON", "Weapon_Repair" } ,
 { BS_SKINTEMPER, "BS_SKINTEMPER", "Skin_Tempering" } ,
 { BS_SPEAR, "BS_SPEAR", "Smith_Spear" } ,
 { BS_STEEL, "BS_STEEL", "Steel_Tempering" } ,
 { BS_SWORD, "BS_SWORD", "Smith_Sword" } ,
 { BS_TWOHANDSWORD, "BS_TWOHANDSWORD", "Smith_Two-handed_Sword" } ,
 { BS_UNFAIRLYTRICK, "BS_UNFAIRLYTRICK", "Unfair_Trick" } ,
 { BS_WEAPONPERFECT, "BS_WEAPONPERFECT", "Weapon_Perfection" } ,
 { BS_WEAPONRESEARCH, "BS_WEAPONRESEARCH", "Weaponry_Research" } ,
 { CG_ARROWVULCAN, "CG_ARROWVULCAN", "Vulcan_Arrow" } ,
 { CG_HERMODE, "CG_HERMODE", "Wand_of_Hermode" } ,
 { CG_LONGINGFREEDOM, "CG_LONGINGFREEDOM", "Longing_for_Freedom" } ,
 { CG_MARIONETTE, "CG_MARIONETTE", "Marionette_Control" } ,
 { CG_MOONLIT, "CG_MOONLIT", "Sheltering_Bliss" } ,
 { CG_TAROTCARD, "CG_TAROTCARD", "Tarot_Card_of_Fate" } ,
 { CH_CHAINCRUSH, "CH_CHAINCRUSH", "Chain_Crush_Combo" } ,
 { CH_PALMSTRIKE, "CH_PALMSTRIKE", "Raging_Palm_Strike" } ,
 { CH_SOULCOLLECT, "CH_SOULCOLLECT", "Zen" } ,
 { CH_TIGERFIST, "CH_TIGERFIST", "Glacier_Fist" } ,
 { CR_ACIDDEMONSTRATION, "CR_ACIDDEMONSTRATION", "Acid_Demonstration" } ,
 { CR_ALCHEMY, "CR_ALCHEMY", "Alchemy" } ,
 { CR_AUTOGUARD, "CR_AUTOGUARD", "Guard" } ,
 { CR_CULTIVATION, "CR_CULTIVATION", "Plant_Cultivation" } ,
 { CR_DEFENDER, "CR_DEFENDER", "Defending_Aura" } ,
 { CR_DEVOTION, "CR_DEVOTION", "Sacrifice" } ,
 { CR_FULLPROTECTION, "CR_FULLPROTECTION", "Full_Protection" } ,
 { CR_GRANDCROSS, "CR_GRANDCROSS", "Grand_Cross" } ,
 { CR_HOLYCROSS, "CR_HOLYCROSS", "Holy_Cross" } ,
 { CR_PROVIDENCE, "CR_PROVIDENCE", "Resistant_Souls" } ,
 { CR_REFLECTSHIELD, "CR_REFLECTSHIELD", "Shield_Reflect" } ,
 { CR_SHIELDBOOMERANG, "CR_SHIELDBOOMERANG", "Shield_Boomerang" } ,
 { CR_SHIELDCHARGE, "CR_SHIELDCHARGE", "Smite" } ,
 { CR_SHRINK, "CR_SHRINK", "Shrink" } ,
 { CR_SLIMPITCHER, "CR_SLIMPITCHER", "Slim_Pitcher" } ,
 { CR_SPEARQUICKEN, "CR_SPEARQUICKEN", "Spear_Quicken" } ,
 { CR_SYNTHESISPOTION, "CR_SYNTHESISPOTION", "Potion_Synthesis" } ,
 { CR_TRUST, "CR_TRUST", "Faith" } ,
 { DC_DANCINGLESSON, "DC_DANCINGLESSON", "Dance_Lessons" } ,
 { DC_DONTFORGETME, "DC_DONTFORGETME", "Slow_Grace" } ,
 { DC_FORTUNEKISS, "DC_FORTUNEKISS", "Lady_Luck" } ,
 { DC_HUMMING, "DC_HUMMING", "Focus_Ballet" } ,
 { DC_SCREAM, "DC_SCREAM", "Dazzler" } ,
 { DC_SERVICEFORYOU, "DC_SERVICEFORYOU", "Gypsy's_Kiss" } ,
 { DC_THROWARROW, "DC_THROWARROW", "Slinging_Arrow" } ,
 { DC_UGLYDANCE, "DC_UGLYDANCE", "Hip_Shaker" } ,
 { DC_WINKCHARM, "DC_WINKCHARM", "Sexy_Wink" } ,
 { GD_APPROVAL, "GD_APPROVAL", "Official_Guild_Approval" } ,
 { GD_BATTLEORDER, "GD_BATTLEORDER", "Battle_Command" } ,
 { GD_DEVELOPMENT, "GD_DEVELOPMENT", "Permanent_Development" } ,
 { GD_EMERGENCYCALL, "GD_EMERGENCYCALL", "Urgent_Call" } ,
 { GD_EXTENSION, "GD_EXTENSION", "Guild_Extension" } ,
 { GD_GLORYGUILD, "GD_GLORYGUILD", "Glory_of_Guild" } ,
 { GD_GLORYWOUNDS, "GD_GLORYWOUNDS", "Glorious_Wounds" } ,
 { GD_GUARDUP, "GD_GUARDUP", "Strengthen_Guardian" } ,
 { GD_HAWKEYES, "GD_HAWKEYES", "Sharp_Gaze" } ,
 { GD_KAFRACONTRACT, "GD_KAFRACONTRACT", "Contract_with_Kafra" } ,
 { GD_LEADERSHIP, "GD_LEADERSHIP", "Great_Leadership" } ,
 { GD_REGENERATION, "GD_REGENERATION", "Regeneration" } ,
 { GD_RESTORE, "GD_RESTORE", "Restoration" } ,
 { GD_SOULCOLD, "GD_SOULCOLD", "Cold_Heart" } ,
 { GS_ADJUSTMENT, "GS_ADJUSTMENT", "Adjustment" } ,
 { GS_BULLSEYE, "GS_BULLSEYE", "Bulls_Eye" } ,
 { GS_CHAINACTION, "GS_CHAINACTION", "Chain_Action" } ,
 { GS_CRACKER, "GS_CRACKER", "Cracker" } ,
 { GS_DESPERADO, "GS_DESPERADO", "Desperado" } ,
 { GS_DISARM, "GS_DISARM", "Disarm" } ,
 { GS_DUST, "GS_DUST", "Dust" } ,
 { GS_FLING, "GS_FLING", "Fling" } ,
 { GS_FULLBUSTER, "GS_FULLBUSTER", "Full_Buster" } ,
 { GS_GATLINGFEVER, "GS_GATLINGFEVER", "Gatling_Fever" } ,
 { GS_GLITTERING, "GS_GLITTERING", "Flip_the_Coin" } ,
 { GS_GROUNDDRIFT, "GS_GROUNDDRIFT", "Ground_Drift" } ,
 { GS_INCREASING, "GS_INCREASING", "Increasing_Accuracy" } ,
 { GS_MADNESSCANCEL, "GS_MADNESSCANCEL", "Madness_Canceler" } ,
 { GS_MAGICALBULLET, "GS_MAGICALBULLET", "Magical_Bullet" } ,
 { GS_PIERCINGSHOT, "GS_PIERCINGSHOT", "Piercing_Shot" } ,
 { GS_RAPIDSHOWER, "GS_RAPIDSHOWER", "Rapid_Shower" } ,
 { GS_SINGLEACTION, "GS_SINGLEACTION", "Single_Action" } ,
 { GS_SNAKEEYE, "GS_SNAKEEYE", "Snake_Eye" } ,
 { GS_SPREADATTACK, "GS_SPREADATTACK", "Spread_Attack" } ,
 { GS_TRACKING, "GS_TRACKING", "Tracking" } ,
 { GS_TRIPLEACTION, "GS_TRIPLEACTION", "Triple_Action" } ,
 { HP_ASSUMPTIO, "HP_ASSUMPTIO", "Assumptio" } ,
 { HP_BASILICA, "HP_BASILICA", "Basilica" } ,
 { HP_MANARECHARGE, "HP_MANARECHARGE", "Mana_Recharge" } ,
 { HP_MEDITATIO, "HP_MEDITATIO", "Meditatio" } ,
 { HT_ANKLESNARE, "HT_ANKLESNARE", "Ankle_Snare" } ,
 { HT_BEASTBANE, "HT_BEASTBANE", "Beast_Bane" } ,
 { HT_BLASTMINE, "HT_BLASTMINE", "Blast_Mine" } ,
 { HT_BLITZBEAT, "HT_BLITZBEAT", "Blitz_Beat" } ,
 { HT_CLAYMORETRAP, "HT_CLAYMORETRAP", "Claymore_Trap" } ,
 { HT_DETECTING, "HT_DETECTING", "Detect" } ,
 { HT_FALCON, "HT_FALCON", "Falconry_Mastery" } ,
 { HT_FLASHER, "HT_FLASHER", "Flasher" } ,
 { HT_FREEZINGTRAP, "HT_FREEZINGTRAP", "Freezing_Trap" } ,
 { HT_LANDMINE, "HT_LANDMINE", "Land_Mine" } ,
 { HT_PHANTASMIC, "HT_PHANTASMIC", "Phantasmic_Arrow" } ,
 { HT_POWER, "HT_POWER", "Beast_Strafing" } ,
 { HT_REMOVETRAP, "HT_REMOVETRAP", "Remove_Trap" } ,
 { HT_SANDMAN, "HT_SANDMAN", "Sandman" } ,
 { HT_SHOCKWAVE, "HT_SHOCKWAVE", "Shockwave_Trap" } ,
 { HT_SKIDTRAP, "HT_SKIDTRAP", "Skid_Trap" } ,
 { HT_SPRINGTRAP, "HT_SPRINGTRAP", "Spring_Trap" } ,
 { HT_STEELCROW, "HT_STEELCROW", "Steel_Crow" } ,
 { HT_TALKIEBOX, "HT_TALKIEBOX", "Talkie_Box" } ,
 { HW_GANBANTEIN, "HW_GANBANTEIN", "Ganbantein" } ,
 { HW_GRAVITATION, "HW_GRAVITATION", "Gravitation_Field" } ,
 { HW_MAGICCRASHER, "HW_MAGICCRASHER", "Stave_Crasher" } ,
 { HW_MAGICPOWER, "HW_MAGICPOWER", "Mystical_Amplification" } ,
 { HW_NAPALMVULCAN, "HW_NAPALMVULCAN", "Napalm_Vulcan" } ,
 { HW_SOULDRAIN, "HW_SOULDRAIN", "Soul_Drain" } ,
 { ITM_TOMAHAWK, "ITM_TOMAHAWK", "Tomahawk_Throwing" } ,
 { KN_AUTOCOUNTER, "KN_AUTOCOUNTER", "Counter_Attack" } ,
 { KN_BOWLINGBASH, "KN_BOWLINGBASH", "Bowling_Bash" } ,
 { KN_BRANDISHSPEAR, "KN_BRANDISHSPEAR", "Brandish_Spear" } ,
 { KN_CAVALIERMASTERY, "KN_CAVALIERMASTERY", "Cavalier_Mastery" } ,
 { KN_CHARGEATK, "KN_CHARGEATK", "Charge_Attack" } ,
 { KN_ONEHAND, "KN_ONEHAND", "Onehand_Quicken" } ,
 { KN_PIERCE, "KN_PIERCE", "Pierce" } ,
 { KN_RIDING, "KN_RIDING", "Peco_Peco_Ride" } ,
 { KN_SPEARBOOMERANG, "KN_SPEARBOOMERANG", "Spear_Boomerang" } ,
 { KN_SPEARMASTERY, "KN_SPEARMASTERY", "Spear_Mastery" } ,
 { KN_SPEARSTAB, "KN_SPEARSTAB", "Spear_Stab" } ,
 { KN_TWOHANDQUICKEN, "KN_TWOHANDQUICKEN", "Twohand_Quicken" } ,
 { LK_AURABLADE, "LK_AURABLADE", "Aura_Blade" } ,
 { LK_BERSERK, "LK_BERSERK", "Frenzy" } ,
 { LK_CONCENTRATION, "LK_CONCENTRATION", "Spear_Dynamo" } ,
 { LK_HEADCRUSH, "LK_HEADCRUSH", "Traumatic_Blow" } ,
 { LK_JOINTBEAT, "LK_JOINTBEAT", "Vital_Strike" } ,
 { LK_PARRYING, "LK_PARRYING", "Parry" } ,
 { LK_SPIRALPIERCE, "LK_SPIRALPIERCE", "Clashing_Spiral" } ,
 { LK_TENSIONRELAX, "LK_TENSIONRELAX", "Relax" } ,
 { MC_CARTREVOLUTION, "MC_CARTREVOLUTION", "Cart_Revolution" } ,
 { MC_CHANGECART, "MC_CHANGECART", "Change_Cart" } ,
 { MC_DISCOUNT, "MC_DISCOUNT", "Discount" } ,
 { MC_IDENTIFY, "MC_IDENTIFY", "Item_Appraisal" } ,
 { MC_INCCARRY, "MC_INCCARRY", "Enlarge_Weight_Limit" } ,
 { MC_LOUD, "MC_LOUD", "Crazy_Uproar" } ,
 { MC_MAMMONITE, "MC_MAMMONITE", "Mammonite" } ,
 { MC_OVERCHARGE, "MC_OVERCHARGE", "Overcharge" } ,
 { MC_PUSHCART, "MC_PUSHCART", "Pushcart" } ,
 { MC_VENDING, "MC_VENDING", "Vending" } ,
 { MG_COLDBOLT, "MG_COLDBOLT", "Cold_Bolt" } ,
 { MG_ENERGYCOAT, "MG_ENERGYCOAT", "Energy_Coat" } ,
 { MG_FIREBALL, "MG_FIREBALL", "Fire_Ball" } ,
 { MG_FIREBOLT, "MG_FIREBOLT", "Fire_Bolt" } ,
 { MG_FIREWALL, "MG_FIREWALL", "Fire_Wall" } ,
 { MG_FROSTDIVER, "MG_FROSTDIVER", "Frost_Diver" } ,
 { MG_LIGHTNINGBOLT, "MG_LIGHTNINGBOLT", "Lightening_Bolt" } ,
 { MG_NAPALMBEAT, "MG_NAPALMBEAT", "Napalm_Beat" } ,
 { MG_SAFETYWALL, "MG_SAFETYWALL", "Safety_Wall" } ,
 { MG_SIGHT, "MG_SIGHT", "Sight" } ,
 { MG_SOULSTRIKE, "MG_SOULSTRIKE", "Soul_Strike" } ,
 { MG_SRECOVERY, "MG_SRECOVERY", "Increase_SP_Recovery" } ,
 { MG_STONECURSE, "MG_STONECURSE", "Stone_Curse" } ,
 { MG_THUNDERSTORM, "MG_THUNDERSTORM", "Thunderstorm" } ,
 { MO_ABSORBSPIRITS, "MO_ABSORBSPIRITS", "Spiritual_Sphere_Absorption" } ,
 { MO_BALKYOUNG, "MO_BALKYOUNG", "Ki_Explosion" } ,
 { MO_BLADESTOP, "MO_BLADESTOP", "Root" } ,
 { MO_BODYRELOCATION, "MO_BODYRELOCATION", "Snap" } ,
 { MO_CALLSPIRITS, "MO_CALLSPIRITS", "Summon_Spirit_Sphere" } ,
 { MO_CHAINCOMBO, "MO_CHAINCOMBO", "Raging_Quadruple_Blow" } ,
 { MO_COMBOFINISH, "MO_COMBOFINISH", "Raging_Thrust" } ,
 { MO_DODGE, "MO_DODGE", "Flee" } ,
 { MO_EXPLOSIONSPIRITS, "MO_EXPLOSIONSPIRITS", "Fury" } ,
 { MO_EXTREMITYFIST, "MO_EXTREMITYFIST", "Guillotine_Fist" } ,
 { MO_FINGEROFFENSIVE, "MO_FINGEROFFENSIVE", "Throw_Spirit_Sphere" } ,
 { MO_INVESTIGATE, "MO_INVESTIGATE", "Occult_Impaction" } ,
 { MO_IRONHAND, "MO_IRONHAND", "Iron_Fists" } ,
 { MO_KITRANSLATION, "MO_KITRANSLATION", "Ki_Translation" } ,
 { MO_SPIRITSRECOVERY, "MO_SPIRITSRECOVERY", "Spiritual_Cadence" } ,
 { MO_STEELBODY, "MO_STEELBODY", "Mental_Strength" } ,
 { MO_TRIPLEATTACK, "MO_TRIPLEATTACK", "Raging_Trifecta_Blow" } ,
 { NJ_BAKUENRYU, "NJ_BAKUENRYU", "NJ_BAKUENRYU" } ,
 { NJ_BUNSINJYUTSU, "NJ_BUNSINJYUTSU", "NJ_BUNSINJYUTSU" } ,
 { NJ_HUUJIN, "NJ_HUUJIN", "NJ_HUUJIN" } ,
 { NJ_HUUMA, "NJ_HUUMA", "NJ_HUUMA" } ,
 { NJ_HYOUSENSOU, "NJ_HYOUSENSOU", "NJ_HYOUSENSOU" } ,
 { NJ_HYOUSYOURAKU, "NJ_HYOUSYOURAKU", "NJ_HYOUSYOURAKU" } ,
 { NJ_ISSEN, "NJ_ISSEN", "NJ_ISSEN" } ,
 { NJ_KAENSIN, "NJ_KAENSIN", "NJ_KAENSIN" } ,
 { NJ_KAMAITACHI, "NJ_KAMAITACHI", "NJ_KAMAITACHI" } ,
 { NJ_KASUMIKIRI, "NJ_KASUMIKIRI", "NJ_KASUMIKIRI" } ,
 { NJ_KIRIKAGE, "NJ_KIRIKAGE", "NJ_KIRIKAGE" } ,
 { NJ_KOUENKA, "NJ_KOUENKA", "NJ_KOUENKA" } ,
 { NJ_KUNAI, "NJ_KUNAI", "NJ_KUNAI" } ,
 { NJ_NEN, "NJ_NEN", "NJ_NEN" } ,
 { NJ_NINPOU, "NJ_NINPOU", "NJ_NINPOU" } ,
 { NJ_RAIGEKISAI, "NJ_RAIGEKISAI", "NJ_RAIGEKISAI" } ,
 { NJ_SHADOWJUMP, "NJ_SHADOWJUMP", "NJ_SHADOWJUMP" } ,
 { NJ_SUITON, "NJ_SUITON", "NJ_SUITON" } ,
 { NJ_SYURIKEN, "NJ_SYURIKEN", "NJ_SYURIKEN" } ,
 { NJ_TATAMIGAESHI, "NJ_TATAMIGAESHI", "NJ_TATAMIGAESHI" } ,
 { NJ_TOBIDOUGU, "NJ_TOBIDOUGU", "NJ_TOBIDOUGU" } ,
 { NJ_UTSUSEMI, "NJ_UTSUSEMI", "NJ_UTSUSEMI" } ,
 { NJ_ZENYNAGE, "NJ_ZENYNAGE", "NJ_ZENYNAGE" } ,
 { NPC_AGIUP, "NPC_AGIUP", "NPC_AGIUP" } ,
 { NPC_ATTRICHANGE, "NPC_ATTRICHANGE", "NPC_ATTRICHANGE" } ,
 { NPC_BARRIER, "NPC_BARRIER", "NPC_BARRIER" } ,
 { NPC_BLINDATTACK, "NPC_BLINDATTACK", "NPC_BLINDATTACK" } ,
 { NPC_BLOODDRAIN, "NPC_BLOODDRAIN", "NPC_BLOODDRAIN" } ,
 { NPC_BREAKARMOR, "NPC_BREAKARMOR", "NPC_BREAKARMOR" } ,
 { NPC_BREAKHELM, "NPC_BREAKHELM", "NPC_BREAKHELM" } ,
 { NPC_BREAKSHIELD, "NPC_BREAKSHIELD", "NPC_BREAKSHIELD" } ,
 { NPC_BREAKWEAPON, "NPC_BREAKWEAPON", "NPC_BREAKWEAPON" } ,
 { NPC_CALLSLAVE, "NPC_CALLSLAVE", "NPC_CALLSLAVE" } ,
 { NPC_CHANGEDARKNESS, "NPC_CHANGEDARKNESS", "NPC_CHANGEDARKNESS" } ,
 { NPC_CHANGEFIRE, "NPC_CHANGEFIRE", "NPC_CHANGEFIRE" } ,
 { NPC_CHANGEGROUND, "NPC_CHANGEGROUND", "NPC_CHANGEGROUND" } ,
 { NPC_CHANGEHOLY, "NPC_CHANGEHOLY", "NPC_CHANGEHOLY" } ,
 { NPC_CHANGEPOISON, "NPC_CHANGEPOISON", "NPC_CHANGEPOISON" } ,
 { NPC_CHANGETELEKINESIS, "NPC_CHANGETELEKINESIS", "NPC_CHANGETELEKINESIS" } ,
 { NPC_CHANGEUNDEAD, "NPC_CHANGEUNDEAD", "NPC_CHANGEUNDEAD" } ,
 { NPC_CHANGEWATER, "NPC_CHANGEWATER", "NPC_CHANGEWATER" } ,
 { NPC_CHANGEWIND, "NPC_CHANGEWIND", "NPC_CHANGEWIND" } ,
 { NPC_COMBOATTACK, "NPC_COMBOATTACK", "NPC_COMBOATTACK" } ,
 { NPC_CRITICALSLASH, "NPC_CRITICALSLASH", "NPC_CRITICALSLASH" } ,
 { NPC_CURSEATTACK, "NPC_CURSEATTACK", "NPC_CURSEATTACK" } ,
 { NPC_DARKBLESSING, "NPC_DARKBLESSING", "NPC_DARKBLESSING" } ,
 { NPC_DARKBREATH, "NPC_DARKBREATH", "NPC_DARKBREATH" } ,
 { NPC_DARKCROSS, "NPC_DARKCROSS", "NPC_DARKCROSS" } ,
 { NPC_DARKNESSATTACK, "NPC_DARKNESSATTACK", "NPC_DARKNESSATTACK" } ,
 { NPC_DARKSTRIKE, "NPC_DARKSTRIKE", "NPC_DARKSTRIKE" } ,
 { NPC_DARKTHUNDER, "NPC_DARKTHUNDER", "NPC_DARKTHUNDER" } ,
 { NPC_DEFENDER, "NPC_DEFENDER", "NPC_DEFENDER" } ,
 { NPC_EMOTION, "NPC_EMOTION", "NPC_EMOTION" } ,
 { NPC_EMOTION_ON, "NPC_EMOTION_ON", "NPC_EMOTION_ON" } ,
 { NPC_ENERGYDRAIN, "NPC_ENERGYDRAIN", "NPC_ENERGYDRAIN" } ,
 { NPC_FIREATTACK, "NPC_FIREATTACK", "NPC_FIREATTACK" } ,
 { NPC_GRANDDARKNESS, "NPC_GRANDDARKNESS", "NPC_GRANDDARKNESS" } ,
 { NPC_GROUNDATTACK, "NPC_GROUNDATTACK", "NPC_GROUNDATTACK" } ,
 { NPC_GUIDEDATTACK, "NPC_GUIDEDATTACK", "NPC_GUIDEDATTACK" } ,
 { NPC_HALLUCINATION, "NPC_HALLUCINATION", "NPC_HALLUCINATION" } ,
 { NPC_HOLYATTACK, "NPC_HOLYATTACK", "NPC_HOLYATTACK" } ,
 { NPC_INVISIBLE, "NPC_INVISIBLE", "NPC_INVISIBLE" } ,
 { NPC_KEEPING, "NPC_KEEPING", "NPC_KEEPING" } ,
 { NPC_LICK, "NPC_LICK", "NPC_LICK" } ,
 { NPC_MAGICALATTACK, "NPC_MAGICALATTACK", "NPC_MAGICALATTACK" } ,
 { NPC_MENTALBREAKER, "NPC_MENTALBREAKER", "NPC_MENTALBREAKER" } ,
 { NPC_METAMORPHOSIS, "NPC_METAMORPHOSIS", "NPC_METAMORPHOSIS" } ,
 { NPC_PETRIFYATTACK, "NPC_PETRIFYATTACK", "NPC_PETRIFYATTACK" } ,
 { NPC_PIERCINGATT, "NPC_PIERCINGATT", "NPC_PIERCINGATT" } ,
 { NPC_POISON, "NPC_POISON", "NPC_POISON" } ,
 { NPC_POISONATTACK, "NPC_POISONATTACK", "NPC_POISONATTACK" } ,
 { NPC_POWERUP, "NPC_POWERUP", "NPC_POWERUP" } ,
 { NPC_PROVOCATION, "NPC_PROVOCATION", "NPC_PROVOCATION" } ,
 { NPC_RANDOMATTACK, "NPC_RANDOMATTACK", "NPC_RANDOMATTACK" } ,
 { NPC_RANDOMMOVE, "NPC_RANDOMMOVE", "NPC_RANDOMMOVE" } ,
 { NPC_RANGEATTACK, "NPC_RANGEATTACK", "NPC_RANGEATTACK" } ,
 { NPC_REBIRTH, "NPC_REBIRTH", "NPC_REBIRTH" } ,
 { NPC_REVENGE, "NPC_REVENGE", "NPC_REVENGE" } ,
 { NPC_RUN, "NPC_RUN", "NPC_RUN" },
 { NPC_SELFDESTRUCTION, "NPC_SELFDESTRUCTION", "Kabooooom!" } ,
 { NPC_SIEGEMODE, "NPC_SIEGEMODE", "NPC_SIEGEMODE" } ,
 { NPC_SILENCEATTACK, "NPC_SILENCEATTACK", "NPC_SILENCEATTACK" } ,
 { NPC_SLEEPATTACK, "NPC_SLEEPATTACK", "NPC_SLEEPATTACK" } ,
 { NPC_SMOKING, "NPC_SMOKING", "NPC_SMOKING" } ,
 { NPC_SPEEDUP, "NPC_SPEEDUP", "NPC_SPEEDUP" } ,
 { NPC_SPLASHATTACK, "NPC_SPLASHATTACK", "NPC_SPLASHATTACK" } ,
 { NPC_STOP, "NPC_STOP", "NPC_STOP" } ,
 { NPC_STUNATTACK, "NPC_STUNATTACK", "NPC_STUNATTACK" } ,
 { NPC_SUICIDE, "NPC_SUICIDE", "NPC_SUICIDE" } ,
 { NPC_SUMMONMONSTER, "NPC_SUMMONMONSTER", "NPC_SUMMONMONSTER" } ,
 { NPC_SUMMONSLAVE, "NPC_SUMMONSLAVE", "NPC_SUMMONSLAVE" } ,
 { NPC_TELEKINESISATTACK, "NPC_TELEKINESISATTACK", "NPC_TELEKINESISATTACK" } ,
 { NPC_TRANSFORMATION, "NPC_TRANSFORMATION", "NPC_TRANSFORMATION" } ,
 { NPC_UNDEADATTACK, "NPC_UNDEADATTACK", "NPC_UNDEADATTACK" } ,
 { NPC_WATERATTACK, "NPC_WATERATTACK", "NPC_WATERATTACK" } ,
 { NPC_WINDATTACK, "NPC_WINDATTACK", "NPC_WINDATTACK" } ,
 { NV_BASIC, "NV_BASIC", "Basic_Skill" } ,
 { NV_FIRSTAID, "NV_FIRSTAID", "First Aid" } ,
 { NV_TRICKDEAD, "NV_TRICKDEAD", "Play_Dead" } ,
 { PA_GOSPEL, "PA_GOSPEL", "Battle_Chant" } ,
 { PA_PRESSURE, "PA_PRESSURE", "Gloria_Domini" } ,
 { PA_SACRIFICE, "PA_SACRIFICE", "Martyr's_Reckoning" } ,
 { PA_SHIELDCHAIN, "PA_SHIELDCHAIN", "Shield_Chain" } ,
 { PF_DOUBLECASTING, "PF_DOUBLECASTING", "Double_Casting" } ,
 { PF_FOGWALL, "PF_FOGWALL", "Blinding_Mist" } ,
 { PF_HPCONVERSION, "PF_HPCONVERSION", "Indulge" } ,
 { PF_MEMORIZE, "PF_MEMORIZE", "Foresight" } ,
 { PF_MINDBREAKER, "PF_MINDBREAKER", "Mind_Breaker" } ,
 { PF_SOULBURN, "PF_SOULBURN", "Soul_Siphon" } ,
 { PF_SOULCHANGE, "PF_SOULCHANGE", "Soul_Exhale" } ,
 { PF_SPIDERWEB, "PF_SPIDERWEB", "Fiber_Lock" } ,
 { PR_ASPERSIO, "PR_ASPERSIO", "Aspersio" } ,
 { PR_BENEDICTIO, "PR_BENEDICTIO", "B.S_Sacramenti" } ,
 { PR_GLORIA, "PR_GLORIA", "Gloria" } ,
 { PR_IMPOSITIO, "PR_IMPOSITIO", "Impositio_Manus" } ,
 { PR_KYRIE, "PR_KYRIE", "Kyrie_Eleison" } ,
 { PR_LEXAETERNA, "PR_LEXAETERNA", "Lex_Aeterna" } ,
 { PR_LEXDIVINA, "PR_LEXDIVINA", "Lex_Divina" } ,
 { PR_MACEMASTERY, "PR_MACEMASTERY", "Mace_Mastery" } ,
 { PR_MAGNIFICAT, "PR_MAGNIFICAT", "Magnificat" } ,
 { PR_MAGNUS, "PR_MAGNUS", "Magnus_Exorcismus" } ,
 { PR_REDEMPTIO, "PR_REDEMPTIO", "Redemptio" } ,
 { PR_SANCTUARY, "PR_SANCTUARY", "Sanctuary" } ,
 { PR_SLOWPOISON, "PR_SLOWPOISON", "Slow_Poison" } ,
 { PR_STRECOVERY, "PR_STRECOVERY", "Status_Recovery" } ,
 { PR_SUFFRAGIUM, "PR_SUFFRAGIUM", "Suffragium" } ,
 { PR_TURNUNDEAD, "PR_TURNUNDEAD", "Turn_Undead" } ,
 { RG_BACKSTAP, "RG_BACKSTAP", "Back_Stab" } ,
 { RG_CLEANER, "RG_CLEANER", "Remover" } ,
 { RG_CLOSECONFINE, "RG_CLOSECONFINE", "Close_Confine"} ,
 { RG_COMPULSION, "RG_COMPULSION", "Haggle" } ,
 { RG_FLAGGRAFFITI, "RG_FLAGGRAFFITI", "Piece" } ,
 { RG_GANGSTER, "RG_GANGSTER", "Slyness" } ,
 { RG_GRAFFITI, "RG_GRAFFITI", "Scribble" } ,
 { RG_INTIMIDATE, "RG_INTIMIDATE", "Snatch" } ,
 { RG_PLAGIARISM, "RG_PLAGIARISM", "Intimidate" } ,
 { RG_RAID, "RG_RAID", "Sightless_Mind" } ,
 { RG_SNATCHER, "RG_SNATCHER", "Gank" } ,
 { RG_STEALCOIN, "RG_STEALCOIN", "Mug" } ,
 { RG_STRIPARMOR, "RG_STRIPARMOR", "Divest_Armor" } ,
 { RG_STRIPHELM, "RG_STRIPHELM", "Divest_Helm" } ,
 { RG_STRIPSHIELD, "RG_STRIPSHIELD", "Divest_Shield" } ,
 { RG_STRIPWEAPON, "RG_STRIPWEAPON", "Divest_Weapon" } ,
 { RG_TUNNELDRIVE, "RG_TUNNELDRIVE", "Stalk" } ,
 { SA_ABRACADABRA, "SA_ABRACADABRA", "Hocus-pocus" } ,
 { SA_ADVANCEDBOOK, "SA_ADVANCEDBOOK", "Advanced_Book" } ,
 { SA_AUTOSPELL, "SA_AUTOSPELL", "Hindsight" } ,
 { SA_CASTCANCEL, "SA_CASTCANCEL", "Cast_Cancel" } ,
 { SA_CLASSCHANGE, "SA_CLASSCHANGE", "Class_Change" } ,
 { SA_COMA, "SA_COMA", "Coma" } ,
 { SA_CREATECON, "SA_CREATECON", "Create_Elemental_Converter" } ,
 { SA_DEATH, "SA_DEATH", "Grim_Reaper" } ,
 { SA_DELUGE, "SA_DELUGE", "Deluge" } ,
 { SA_DISPELL, "SA_DISPELL", "Dispell" } ,
 { SA_DRAGONOLOGY, "SA_DRAGONOLOGY", "Dragonology" } ,
 { SA_ELEMENTFIRE, "SA_ELEMENTFIRE", "Elemental_Change_Fire" } ,
 { SA_ELEMENTGROUND, "SA_ELEMENTGROUND", "Elemental_Change_Earth" } ,
 { SA_ELEMENTWATER, "SA_ELEMENTWATER", "Elemental_Change_Water" } ,
 { SA_ELEMENTWIND, "SA_ELEMENTWIND", "Elemental_Change_Wind" } ,
 { SA_FLAMELAUNCHER, "SA_FLAMELAUNCHER", "Endow_Blaze" } ,
 { SA_FORTUNE, "SA_FORTUNE", "Gold_Digger" } ,
 { SA_FREECAST, "SA_FREECAST", "Free_Cast" } ,
 { SA_FROSTWEAPON, "SA_FROSTWEAPON", "Endow_Tsunami" } ,
 { SA_FULLRECOVERY, "SA_FULLRECOVERY", "Rejuvenation" } ,
 { SA_GRAVITY, "SA_GRAVITY", "Gravity" } ,
 { SA_INSTANTDEATH, "SA_INSTANTDEATH", "Suicide" } ,
 { SA_LANDPROTECTOR, "SA_LANDPROTECTOR", "Magnetic_Earth" } ,
 { SA_LEVELUP, "SA_LEVELUP", "Leveling" } ,
 { SA_LIGHTNINGLOADER, "SA_LIGHTNINGLOADER", "Endow_Tornado" } ,
 { SA_MAGICROD, "SA_MAGICROD", "Magic_Rod" } ,
 { SA_MONOCELL, "SA_MONOCELL", "Mono_Cell" } ,
 { SA_QUESTION, "SA_QUESTION", "Questioning" } ,
 { SA_REVERSEORCISH, "SA_REVERSEORCISH", "Grampus_Morph" } ,
 { SA_SEISMICWEAPON, "SA_SEISMICWEAPON", "Endow_Quake" } ,
 { SA_SPELLBREAKER, "SA_SPELLBREAKER", "Spell_Breaker" } ,
 { SA_SUMMONMONSTER, "SA_SUMMONMONSTER", "Monster_Chant" } ,
 { SA_TAMINGMONSTER, "SA_TAMINGMONSTER", "Beastly_Hypnosis" } ,
 { SA_VIOLENTGALE, "SA_VIOLENTGALE", "Whirlwind" } ,
 { SA_VOLCANO, "SA_VOLCANO", "Volcano" } ,
 { SG_DEVIL, "SG_DEVIL", "Devil_of_the_Sun,_Moon_and_Stars" } ,
 { SG_FEEL, "SG_FEEL", "Feeling_of_the_Sun,_Moon_and_Star" } ,
 { SG_FRIEND, "SG_FRIEND", "Companion_of_the_Sun_and_Moon" } ,
 { SG_FUSION, "SG_FUSION", "Union_of_the_Sun,_Moon_and_Stars" } ,
 { SG_HATE, "SG_HATE", "Hatred_of_the_Sun,_Moon_and_Stars" } ,
 { SG_KNOWLEDGE, "SG_KNOWLEDGE", "Knowledge_of_the_Sun,_Moon_and_Stars" } ,
 { SG_MOON_ANGER, "SG_MOON_ANGER", "Fury_of_the_Moon" } ,
 { SG_MOON_BLESS, "SG_MOON_BLESS", "Bless_of_the_Moon" } ,
 { SG_MOON_COMFORT, "SG_MOON_COMFORT", "Comfort_of_the_Moon" } ,
 { SG_MOON_WARM, "SG_MOON_WARM", "Warmth_of_the_Moon" } ,
 { SG_STAR_ANGER, "SG_STAR_ANGER", "Fury_of_the_Stars" } ,
 { SG_STAR_BLESS, "SG_STAR_BLESS", "Bless_of_the_Stars" } ,
 { SG_STAR_COMFORT, "SG_STAR_COMFORT", "Comfort_of_the_Stars" } ,
 { SG_STAR_WARM, "SG_STAR_WARM", "Warmth_of_the_Stars" } ,
 { SG_SUN_ANGER, "SG_SUN_ANGER", "Fury_of_the_Sun" } ,
 { SG_SUN_BLESS, "SG_SUN_BLESS", "Bless_of_the_Sun" } ,
 { SG_SUN_COMFORT, "SG_SUN_COMFORT", "Comfort_of_the_Sun" } ,
 { SG_SUN_WARM, "SG_SUN_WARM", "Warmth_of_the_Sun" } ,
 { SL_ALCHEMIST, "SL_ALCHEMIST", "Spirit_of_Alchemist" } ,
 { SL_ASSASIN, "SL_ASSASIN", "Spirit_of_Assassin" } ,
 { SL_BARDDANCER, "SL_BARDDANCER", "Spirit_of_Bard_and_Dancer" } ,
 { SL_BLACKSMITH, "SL_BLACKSMITH", "Spirit_of_Blacksmith" } ,
 { SL_CRUSADER, "SL_CRUSADER", "Spirit_of_Crusader" } ,
 { SL_HIGH, "SL_HIGH", "Spirit_of_Advanced_1st_Class" } ,
 { SL_HUNTER, "SL_HUNTER", "Spirit_of_Hunter" } ,
 { SL_KAAHI, "SL_KAAHI", "Kaahi" } ,
 { SL_KAINA, "SL_KAINA", "Kaina" } ,
 { SL_KAITE, "SL_KAITE", "Kaite" } ,
 { SL_KAIZEL, "SL_KAIZEL", "Kaizel" } ,
 { SL_KAUPE, "SL_KAUPE", "Kaupe" } ,
 { SL_KNIGHT, "SL_KNIGHT", "Spirit_of_Knight" } ,
 { SL_MONK, "SL_MONK", "Spirit_of_Monk" } ,
 { SL_PRIEST, "SL_PRIEST", "Spirit_of_Priest" } ,
 { SL_ROGUE, "SL_ROGUE", "Spirit_of_Rogue" } ,
 { SL_SAGE, "SL_SAGE", "Spirit_of_Sage" } ,
 { SL_SKA, "SL_SKA", "Eska" } ,
 { SL_SKE, "SL_SKE", "Eske" } ,
 { SL_SMA, "SL_SMA", "Esma" } ,
 { SL_SOULLINKER, "SL_SOULLINKER", "Spirit_of_Soul_Linker" } ,
 { SL_STAR, "SL_STAR", "Spirit_of_Stars" } ,
 { SL_STIN, "SL_STIN", "Estin" } ,
 { SL_STUN, "SL_STUN", "Estun" } ,
 { SL_SUPERNOVICE, "SL_SUPERNOVICE", "Spirit_of_Super_Novice" } ,
 { SL_SWOO, "SL_SWOO", "Eswoo" } ,
 { SL_WIZARD, "SL_WIZARD", "Spirit_of_Wizard" } ,
 { SM_AUTOBERSERK, "SM_AUTOBERSERK", "Berserk" } ,
 { SM_BASH, "SM_BASH", "Bash" } ,
 { SM_ENDURE, "SM_ENDURE", "Endure" } ,
 { SM_FATALBLOW, "SM_FATALBLOW", "Fatal_Blow" } ,
 { SM_MAGNUM, "SM_MAGNUM", "Magnum_Break" } ,
 { SM_MOVINGRECOVERY, "SM_MOVINGRECOVERY", "HP_Recovery_While_Moving" } ,
 { SM_PROVOKE, "SM_PROVOKE", "Provoke" } ,
 { SM_RECOVERY, "SM_RECOVERY", "Increase_HP_Recovery" } ,
 { SM_SWORD, "SM_SWORD", "Sword_Mastery" } ,
 { SM_TWOHAND, "SM_TWOHAND", "Two-Handed_Sword_Mastery" } ,
 { SN_FALCONASSAULT, "SN_FALCONASSAULT", "Falcon_Assault" } ,
 { SN_SHARPSHOOTING, "SN_SHARPSHOOTING", "Focused_Arrow_Strike" } ,
 { SN_SIGHT, "SN_SIGHT", "Falcon_Eyes" } ,
 { SN_WINDWALK, "SN_WINDWALK", "Wind_Walker" } ,
 { ST_CHASEWALK, "ST_CHASEWALK", "Stealth" } ,
 { ST_FULLSTRIP, "ST_FULLSTRIP", "Full_Divestment" } ,
 { ST_PRESERVE, "ST_PRESERVE", "Preserve" } ,
 { ST_REJECTSWORD, "ST_REJECTSWORD", "Counter_Instinct" } ,
 { TF_BACKSLIDING, "TF_BACKSLIDING", "Back_Slide" } ,
 { TF_DETOXIFY, "TF_DETOXIFY", "Detoxify" } ,
 { TF_DOUBLE, "TF_DOUBLE", "Double_Attack" } ,
 { TF_HIDING, "TF_HIDING", "Hiding" } ,
 { TF_MISS, "TF_MISS", "Improve_Dodge" } ,
 { TF_PICKSTONE, "TF_PICKSTONE", "Find_Stone" } ,
 { TF_POISON, "TF_POISON", "Envenom" } ,
 { TF_SPRINKLESAND, "TF_SPRINKLESAND", "Sand_Attack" } ,
 { TF_STEAL, "TF_STEAL", "Steal" } ,
 { TF_THROWSTONE, "TF_THROWSTONE", "Stone_Fling" } ,
 { TK_COUNTER, "TK_COUNTER", "Spin_Kick" } ,
 { TK_DODGE, "TK_DODGE", "Sprint" } ,
 { TK_DOWNKICK, "TK_DOWNKICK", "Heel_Drop" } ,
 { TK_HIGHJUMP, "TK_HIGHJUMP", "Taekwon_Jump" } ,
 { TK_HPTIME, "TK_HPTIME", "Peaceful_Break" } ,
 { TK_JUMPKICK, "TK_JUMPKICK", "Flying_Kick" } ,
 { TK_MISSION,  "TK_MISSION", "Mission" } ,
 { TK_POWER, "TK_POWER", "Kihop" } ,
 { TK_READYCOUNTER, "TK_READYCOUNTER", "Spin_Kick_Stance" } ,
 { TK_READYDOWN, "TK_READYDOWN", "Heel_Drop_Stance" } ,
 { TK_READYSTORM, "TK_READYSTORM", "Tornado_Stance" } ,
 { TK_READYTURN, "TK_READYTURN", "Roundhouse_Stance" } ,
 { TK_RUN, "TK_RUN", "Sprint" } ,
 { TK_SEVENWIND, "TK_SEVENWIND", "Mild_Wind" } ,
 { TK_SPTIME, "TK_SPTIME", "Happy_Break" } ,
 { TK_STORMKICK, "TK_STORMKICK", "Storm_Kick" } ,
 { TK_TURNKICK, "TK_TURNKICK", "Turn_Kick" } ,
 { WE_BABY, "WE_BABY", "Mom,_Dad,_I_love_you!" } ,
 { WE_CALLBABY, "WE_CALLBABY", "Come_to_me,_honey~" } ,
 { WE_CALLPARENT, "WE_CALLPARENT", "Mom,_Dad,_I_miss_you!" } ,
 { WE_CALLPARTNER, "WE_CALLPARTNER", "Romantic_Rendezvous" } ,
 { WE_FEMALE, "WE_FEMALE", "Loving_Touch" } ,
 { WE_MALE, "WE_MALE", "Undying_Love" } ,
 { WS_CARTBOOST, "WS_CARTBOOST", "Cart_Boost" } ,
 { WS_CARTTERMINATION, "WS_CARTTERMINATION", "Cart_Termination" } ,
 { WS_CREATECOIN, "WS_CREATECOIN", "Coin_Craft" } ,
 { WS_CREATENUGGET, "WS_CREATENUGGET", "Nugget_Craft" } ,
 { WS_MELTDOWN, "WS_MELTDOWN", "Shattering_Strike" } ,
 { WS_OVERTHRUSTMAX, "WS_OVERTHRUSTMAX", "Max_Power-Thust" } ,
 { WS_SYSTEMCREATE, "WS_SYSTEMCREATE", "Auto_Attacking_Machine_Craft" } ,
 { WS_WEAPONREFINE, "WS_WEAPONREFINE", "Weapon_Refine" } ,
 { WZ_EARTHSPIKE, "WZ_EARTHSPIKE", "Earth_Spike" } ,
 { WZ_ESTIMATION, "WZ_ESTIMATION", "Sense" } ,
 { WZ_FIREPILLAR, "WZ_FIREPILLAR", "Fire_Pillar" } ,
 { WZ_FROSTNOVA, "WZ_FROSTNOVA", "Frost_Nova" } ,
 { WZ_HEAVENDRIVE, "WZ_HEAVENDRIVE", "Heaven's_Drive" } ,
 { WZ_ICEWALL, "WZ_ICEWALL", "Ice_Wall" } ,
 { WZ_JUPITEL, "WZ_JUPITEL", "Jupitel_Thunder" } ,
 { WZ_METEOR, "WZ_METEOR", "Meteor_Storm" } ,
 { WZ_QUAGMIRE, "WZ_QUAGMIRE", "Quagmire" } ,
 { WZ_SIGHTBLASTER, "WZ_SIGHTBLASTER", "Sight_Blaster" } ,
 { WZ_SIGHTRASHER, "WZ_SIGHTRASHER", "Sightrasher" } ,
 { WZ_STORMGUST, "WZ_STORMGUST", "Storm_Gust" } ,
 { WZ_VERMILION, "WZ_VERMILION", "Lord_of_Vermilion" } ,
 { WZ_WATERBALL, "WZ_WATERBALL", "Water_Ball" } ,
 //[blackhole89]
 { HLIF_HEAL, "HLIF_HEAL", "Healing_Touch" },
 { HLIF_AVOID, "HLIF_AVOID", "Avoid" },
 { HLIF_BRAIN, "HLIF_BRAIN", "Brain_Surgery" },
 { HLIF_CHANGE, "HLIF_CHANGE", "Change" },
 { HAMI_CASTLE, "HAMI_CASTLE", "Castling" },
 { HAMI_DEFENCE, "HAMI_DEFENCE", "Defense" },
 { HAMI_SKIN, "HAMI_SKIN", "Adamantium_Skin" },
 { HAMI_BLOODLUST, "HAMI_BLOODLUST", "Bloodlust" },
 { HFLI_MOON, "HFLI_MOON", "Moonlight" },
 { HFLI_FLEET, "HFLI_FLEET", "Fleeting_Move" },
 { HFLI_SPEED, "HFLI_SPEED", "Speed" },
 { HFLI_SBR44, "HFLI_SBR44", "S.B.R.44" },
 { HVAN_CAPRICE, "HVAN_CAPRICE", "Caprice" },
 { HVAN_CHAOTIC, "HVAN_CHAOTIC", "Benediction_of_Chaos" },
 { HVAN_INSTRUCT, "HVAN_INSTRUCT", "Instruct" },
 { HVAN_EXPLOSION, "HVAN_EXPLOSION", "Bio_Explosion" },
 { 0, "UNKNOWN_SKILL", "Unknown_Skill" }
};

static struct eri *skill_unit_ers = NULL; //For handling skill_unit's [Skotlex]
static struct eri *skill_timer_ers = NULL; //For handling skill_timerskills [Skotlex]

struct skill_db skill_db[MAX_SKILL_DB];
struct skill_produce_db skill_produce_db[MAX_SKILL_PRODUCE_DB];
struct skill_arrow_db skill_arrow_db[MAX_SKILL_ARROW_DB];
struct skill_abra_db skill_abra_db[MAX_SKILL_ABRA_DB];

// macros to check for out of bounds errors [celest]
// i: Skill ID, l: Skill Level, var: Value to return after checking
// for values that don't require level just put a one (putting 0 will trigger return 0; instead
// for values that might need to use a different function just skill_chk would suffice.
#define skill_chk(i, l) \
	if (i >= GD_SKILLRANGEMIN && i <= GD_SKILLRANGEMAX) { return 0; } \
	if (i >= HM_SKILLRANGEMIN && i <= HM_SKILLRANGEMAX) { return 0; } \
	if (i >= GD_SKILLBASE) {i = GD_SKILLRANGEMIN + i - GD_SKILLBASE;} \
	if (i >= HM_SKILLBASE) {i = HM_SKILLRANGEMIN + i - HM_SKILLBASE;} \
	if (i < 1 || i >= MAX_SKILL_DB) {return 0;} \
	if (l <= 0 || l > MAX_SKILL_LEVEL) {return 0;}
#define skill_get(var, i, l) \
	{ skill_chk(i, l); return var; }

// Skill DB
int	skill_get_hit( int id ){ skill_get (skill_db[id].hit, id, 1); }
int	skill_get_inf( int id ){ skill_get (skill_db[id].inf, id, 1); }
int	skill_get_pl( int id ){ skill_get (skill_db[id].pl, id, 1); }
int	skill_get_nk( int id ){ skill_get (skill_db[id].nk, id, 1); }
int	skill_get_max( int id ){ skill_get (skill_db[id].max, id, 1); }
int	skill_get_range( int id , int lv ){ skill_get(skill_db[id].range[lv-1], id, lv); }
int	skill_get_splash( int id , int lv ){ skill_chk (id, lv); return (skill_db[id].splash[lv-1]>=0?skill_db[id].splash[lv-1]:AREA_SIZE); }
int	skill_get_hp( int id ,int lv ){ skill_get (skill_db[id].hp[lv-1], id, lv); }
int	skill_get_sp( int id ,int lv ){ skill_get (skill_db[id].sp[lv-1], id, lv); }
int	skill_get_hp_rate(int id, int lv ){ skill_get (skill_db[id].hp_rate[lv-1], id, lv); }
int	skill_get_sp_rate(int id, int lv ){ skill_get (skill_db[id].sp_rate[lv-1], id, lv); }
int	skill_get_state(int id) { skill_get (skill_db[id].state, id, 1); }
int	skill_get_spiritball(int id, int lv) { skill_get (skill_db[id].spiritball[lv-1], id, lv); }
int	skill_get_itemid(int id, int idx) { skill_get (skill_db[id].itemid[idx], id, 1); }
int	skill_get_itemqty(int id, int idx) { skill_get (skill_db[id].amount[idx], id, 1); }
int	skill_get_zeny( int id ,int lv ){ skill_get (skill_db[id].zeny[lv-1], id, lv); }
int	skill_get_num( int id ,int lv ){ skill_get (skill_db[id].num[lv-1], id, lv); }
int	skill_get_cast( int id ,int lv ){ skill_get (skill_db[id].cast[lv-1], id, lv); }
int	skill_get_delay( int id ,int lv ){ skill_get (skill_db[id].delay[lv-1], id, lv); }
int	skill_get_walkdelay( int id ,int lv ){ skill_get (skill_db[id].walkdelay[lv-1], id, lv); }
int	skill_get_time( int id ,int lv ){ skill_get (skill_db[id].upkeep_time[lv-1], id, lv); }
int	skill_get_time2( int id ,int lv ){ skill_get (skill_db[id].upkeep_time2[lv-1], id, lv); }
int	skill_get_castdef( int id ){ skill_get (skill_db[id].cast_def_rate, id, 1); }
int	skill_get_weapontype( int id ){ skill_get (skill_db[id].weapon, id, 1); }
int	skill_get_ammotype( int id ){ skill_get (skill_db[id].ammo, id, 1); }
int	skill_get_ammo_qty( int id, int lv ){ skill_get (skill_db[id].ammo_qty[lv-1], id, lv); }
int	skill_get_inf2( int id ){ skill_get (skill_db[id].inf2, id, 1); }
int	skill_get_castcancel( int id ){ skill_get (skill_db[id].castcancel, id, 1); }
int	skill_get_maxcount( int id ){ skill_get (skill_db[id].maxcount, id, 1); }
int	skill_get_blewcount( int id ,int lv ){ skill_get (skill_db[id].blewcount[lv-1], id, lv); }
int	skill_get_mhp( int id ,int lv ){ skill_get (skill_db[id].mhp[lv-1], id, lv); }
int	skill_get_castnodex( int id ,int lv ){ skill_get (skill_db[id].castnodex[lv-1], id, lv); }
int	skill_get_delaynodex( int id ,int lv ){ skill_get (skill_db[id].delaynodex[lv-1], id, lv); }
int	skill_get_nocast ( int id ){ skill_get (skill_db[id].nocast, id, 1); }
int	skill_get_type( int id ){ skill_get (skill_db[id].skill_type, id, 1); }
int	skill_get_unit_id ( int id, int flag ){ skill_get (skill_db[id].unit_id[flag], id, 1); }
int	skill_get_unit_layout_type( int id ,int lv ){ skill_get (skill_db[id].unit_layout_type[lv-1], id, lv); }
int	skill_get_unit_interval( int id ){ skill_get (skill_db[id].unit_interval, id, 1); }
int	skill_get_unit_range( int id, int lv ){ skill_get (skill_db[id].unit_range[lv-1], id, lv); }
int	skill_get_unit_target( int id ){ skill_get ((skill_db[id].unit_target&BCT_ALL), id, 1); }
int	skill_get_unit_bl_target( int id ){ skill_get ((skill_db[id].unit_target&BL_ALL), id, 1); }
int	skill_get_unit_flag( int id ){ skill_get (skill_db[id].unit_flag, id, 1); }
const char*	skill_get_name( int id ){
	if (id >= GD_SKILLRANGEMIN && id <= GD_SKILLRANGEMAX)
		return "UNKNOWN_SKILL";
	if (id >= GD_SKILLBASE)
		id = GD_SKILLRANGEMIN + id - GD_SKILLBASE;
	if (id >= HM_SKILLBASE)	//[orn]
		id = HM_SKILLRANGEMIN + id - HM_SKILLBASE;
	if (id < 1 || id > MAX_SKILL_DB || skill_db[id].name==NULL)
		return "UNKNOWN_SKILL"; //Can't use skill_chk because we return a string.
	return skill_db[id].name; 
}

int skill_tree_get_max(int id, int b_class){
	int i, skillid;
	for(i=0;(skillid=skill_tree[b_class][i].id)>0;i++)
		if (id == skillid) return skill_tree[b_class][i].max;
	return skill_get_max (id);
}

int skill_castend_damage_id( struct block_list* src, struct block_list *bl,int skillid,int skilllv,unsigned int tick,int flag );
int skill_frostjoke_scream(struct block_list *bl,va_list ap);
int status_change_timer_sub(struct block_list *bl, va_list ap);
int skill_attack_area(struct block_list *bl,va_list ap);
struct skill_unit_group *skill_locate_element_field(struct block_list *bl); // [Skotlex]
int skill_graffitiremover(struct block_list *bl, va_list ap); // [Valaris]
int skill_greed(struct block_list *bl, va_list ap);
int skill_landprotector(struct block_list *bl, va_list ap);
int skill_ganbatein(struct block_list *bl, va_list ap);
int skill_trap_splash(struct block_list *bl, va_list ap);
int skill_count_target(struct block_list *bl, va_list ap);
struct skill_unit_group_tickset *skill_unitgrouptickset_search(struct block_list *bl,struct skill_unit_group *sg,int tick);
static int skill_unit_onplace(struct skill_unit *src,struct block_list *bl,unsigned int tick);
static int skill_unit_onleft(int skill_id, struct block_list *bl,unsigned int tick);
int skill_unit_effect(struct block_list *bl,va_list ap);
static void skill_moonlit(struct block_list* src, struct block_list* partner, int skilllv);

int enchant_eff[5] = { 10, 14, 17, 19, 20 };
int deluge_eff[5] = { 5, 9, 12, 14, 15 };

int skill_get_casttype (int id)
{
	int inf = skill_get_inf(id);
	if (inf&(INF_GROUND_SKILL))
		return CAST_GROUND;
	if (inf&INF_SUPPORT_SKILL)
		return CAST_NODAMAGE;
	if (inf&INF_SELF_SKILL) {
		if(skill_get_inf2(id)&INF2_NO_TARGET_SELF)
			return CAST_DAMAGE; //Combo skill.
		return CAST_NODAMAGE;
	}
	if (skill_get_nk(id)&NK_NO_DAMAGE)
		return CAST_NODAMAGE;
	return CAST_DAMAGE;
};

//Returns actual skill range taking into account attack range and AC_OWL [Skotlex]
int skill_get_range2 (struct block_list *bl, int id, int lv)
{
	int range = skill_get_range(id, lv);
	if(range < 0) {
		if (battle_config.use_weapon_skill_range&bl->type)
			return status_get_range(bl);
		range *=-1;
	}

	//Use attack range.
	if(!range && !(skill_get_inf(id)&INF_SELF_SKILL))
		return status_get_range(bl);

	//TODO: Find a way better than hardcoding the list of skills affected by AC_VULTURE
	switch (id) {
	case AC_SHOWER:
	case AC_DOUBLE:
	case HT_BLITZBEAT:
	case AC_CHARGEARROW:
	case SN_FALCONASSAULT:
	case SN_SHARPSHOOTING:
	case HT_POWER:
		if (bl->type == BL_PC)
			range += pc_checkskill((TBL_PC*)bl, AC_VULTURE);
		else
			range += 10; //Assume level 10?
		break;
	// added to allow GS skills to be effected by the range of Snake Eyes [Reddozen]
	case GS_RAPIDSHOWER:
	case GS_TRACKING:
	case GS_PIERCINGSHOT:
	case GS_FULLBUSTER:
	case GS_SPREADATTACK:
	case GS_GROUNDDRIFT:
		if (bl->type == BL_PC)
			range += pc_checkskill((TBL_PC*)bl, GS_SNAKEEYE);
		else
			range += 10; //Assume level 10?
		break;
	case NJ_KIRIKAGE:
		if (bl->type == BL_PC)
			range = skill_get_range(NJ_SHADOWJUMP,pc_checkskill((TBL_PC*)bl,NJ_SHADOWJUMP));
		break;
	}

	if(!range && bl->type != BL_PC)
		return 9; // Enable non players to use self skills on others. [Skotlex]
	return range;
}

int skill_calc_heal (struct block_list *bl, int skill_lv)
{
	int skill, heal;

	if (skill_lv >= battle_config.max_heal_lv)
		return battle_config.max_heal;

	heal = ( status_get_lv(bl)+status_get_int(bl) )/8 *(4+ skill_lv*8);
	if(bl->type == BL_PC && (skill = pc_checkskill((TBL_PC*)bl, HP_MEDITATIO)) > 0)
		heal += heal * skill * 2 / 100;

	if(bl->type == BL_HOM && (skill = merc_hom_checkskill( ((TBL_HOM*)bl)->master, HLIF_BRAIN)) > 0)	//[orn]
		heal += heal * skill * 2 / 100;
	return heal;
}

// Making plagiarize check its own function [Aru]
int can_copy (struct map_session_data *sd, int skillid)
{
	// Never copy NPC/Wedding Skills
	if (skill_get_inf2(skillid)&(INF2_NPC_SKILL|INF2_WEDDING_SKILL))
		return 0;

	// High-class skills
	if((skillid >= LK_AURABLADE && skillid <= ASC_CDP) || (skillid >= ST_PRESERVE && skillid <= CR_CULTIVATION))
	{
		if(battle_config.copyskill_restrict == 2)
			return 0;
		else if(battle_config.copyskill_restrict)
			return (sd->status.class_ == JOB_STALKER);
	}

	return 1;
}

// [MouseJstr] - skill ok to cast? and when?
int skillnotok (int skillid, struct map_session_data *sd)
{	
	int i = skillid,m;
	nullpo_retr (1, sd);
	m = sd->bl.m;

	if (skillid >= GD_SKILLRANGEMIN && skillid <= GD_SKILLRANGEMAX)
		return 1;

	if (i >= GD_SKILLBASE)
		i = GD_SKILLRANGEMIN + i - GD_SKILLBASE;
	if (i >= HM_SKILLBASE)	//[orn]
		i = HM_SKILLRANGEMIN + i - HM_SKILLBASE;
	
	if (i > MAX_SKILL || i < 0)
		return 1;
	
	if (sd->blockskill[i] > 0)
		return 1;

	if (battle_config.gm_skilluncond && pc_isGM(sd) >= battle_config.gm_skilluncond)
		return 0;  // gm's can do anything damn thing they want

	// Check skill restrictions [Celest]
	if(!map_flag_vs(m) && skill_get_nocast (skillid) & 1)
		return 1;
	if(map[m].flag.pvp && skill_get_nocast (skillid) & 2)
		return 1;
	if(map_flag_gvg(m) && skill_get_nocast (skillid) & 4)
		return 1;
	if(agit_flag && skill_get_nocast (skillid) & 8)
		return 1;
	if(map[m].flag.restricted && map[m].zone && skill_get_nocast (skillid) & (8*map[m].zone))
		return 1;

	switch (skillid) {
		case AL_WARP:
			if(map[m].flag.nowarp) {
				clif_skill_teleportmessage(sd,0);
				return 1;
			}
			return 0;
		break;
		case AL_TELEPORT:
			if(map[m].flag.noteleport) {
				clif_skill_teleportmessage(sd,0);
				return 1;
			}
			return 0;
		case TK_HIGHJUMP:
			if(map[m].flag.noteleport && !map_flag_vs(m))
		  	{	//Can't be used on noteleport maps, except for vs maps [Skotlex]
				clif_skill_fail(sd,skillid,0,0);
				return 1;
			}
			break;
		case WE_CALLPARTNER:
		case WE_CALLPARENT:
		case WE_CALLBABY:
			if (map[m].flag.nomemo) {
				clif_skill_teleportmessage(sd,1);
				return 1;
			}
			break;	
		case MC_VENDING:
		case MC_IDENTIFY:
			return 0; // always allowed
		case WZ_ICEWALL:
			// noicewall flag [Valaris]
			if (map[m].flag.noicewall) {
				clif_skill_fail(sd,skillid,0,0);
				return 1;
			}
			break;
		case GD_EMERGENCYCALL:
			if ( //No use map_flag_gvg since config already takes that into account
				!(battle_config.emergency_call&(agit_flag?2:1)) ||
				!(battle_config.emergency_call&
					(map[m].flag.gvg || map[m].flag.gvg_castle?8:4)) ||
				(battle_config.emergency_call&16 && map[m].flag.nowarpto)
			)	{
				clif_skill_fail(sd,skillid,0,0);
				return 1;
			}
			break;
	}
	return (map[m].flag.noskill);
}

// [orn] - skill ok to cast? and when?	//homunculus
int skillnotok_hom (int skillid, struct homun_data *hd)
{	
	int i = skillid;
	nullpo_retr (1, hd);
	//if (sd == 0)
		//return 0; 
		//return 1;
	// I think it was meant to be "no skills allowed when not a valid sd"
	
	if (skillid >= GD_SKILLRANGEMIN && skillid <= GD_SKILLRANGEMAX)
		return 1;

	if (i >= GD_SKILLBASE)
		i = GD_SKILLRANGEMIN + i - GD_SKILLBASE;
	if (i >= HM_SKILLBASE)	//[orn]
		i = HM_SKILLRANGEMIN + i - HM_SKILLBASE;
	
	if (i > MAX_SKILL || i < 0)
		return 1;
	
	if (hd->blockskill[i] > 0)
		return 1;

	// Check skill restrictions [Celest]
	if(!map_flag_vs(hd->bl.m) && skill_get_nocast (skillid) & 1)
		return 1;
	if(map[hd->bl.m].flag.pvp) {
		if(!battle_config.pk_mode && skill_get_nocast (skillid) & 2)
			return 1;
		if(battle_config.pk_mode && skill_get_nocast (skillid) & 16)
			return 1;
	}
	if(map_flag_gvg(hd->bl.m) && skill_get_nocast (skillid) & 4)
		return 1;
	if(agit_flag && skill_get_nocast (skillid) & 8)
		return 1;
	if(map[hd->bl.m].flag.restricted && map[hd->bl.m].zone && skill_get_nocast (skillid) & (8*map[hd->bl.m].zone))
		return 1;

	return (map[hd->bl.m].flag.noskill);
}

struct skill_unit_layout skill_unit_layout[MAX_SKILL_UNIT_LAYOUT];
int firewall_unit_pos;
int icewall_unit_pos;

struct skill_unit_layout *skill_get_unit_layout (int skillid, int skilllv, struct block_list *src, int x, int y)
{	
	int pos = skill_get_unit_layout_type(skillid,skilllv);
	int dir;

	if (pos != -1)
		return &skill_unit_layout[pos];

	if (src->x == x && src->y == y)
		dir = 2;
	else
		dir = map_calc_dir(src,x,y);

	if (skillid == MG_FIREWALL)
		return &skill_unit_layout [firewall_unit_pos + dir];
	else if (skillid == WZ_ICEWALL)
		return &skill_unit_layout [icewall_unit_pos + dir];

	ShowError("Unknown unit layout for skill %d, %d\n",skillid,skilllv);
	return &skill_unit_layout[0];
}

/*==========================================
 * 
 *------------------------------------------
 */
int skill_additional_effect (struct block_list* src, struct block_list *bl, int skillid, int skilllv, int attack_type, unsigned int tick)
{
	struct map_session_data *sd=NULL, *dstsd=NULL;
	struct mob_data *md=NULL, *dstmd=NULL;
	struct status_data *sstatus, *tstatus;
	struct status_change *sc, *tsc;

	int skill;
	int rate;

	nullpo_retr(0, src);
	nullpo_retr(0, bl);

	if(skillid < 0) 
	{	// remove the debug print when this case is finished
		ShowDebug("skill_additional_effect: skillid=%i\ncall: %p %p %i %i %i %i",skillid,
						src, bl,skillid,skilllv,attack_type,tick);
		return 0;
	}
	if(skillid > 0 && skilllv <= 0) return 0;	// don't forget auto attacks! - celest

	switch (src->type) {
	case BL_PC:
		sd = (struct map_session_data *)src;
		break;
	case BL_MOB:
		md = (struct mob_data *)src;
		break;
	}
	
	switch (bl->type) {
	case BL_PC:
		dstsd=(struct map_session_data *)bl;
		break;
	case BL_MOB:
		dstmd=(struct mob_data *)bl;
		break;
	}
	sc = status_get_sc(src);
	tsc = status_get_sc(bl);
	sstatus = status_get_status_data(src);
	tstatus = status_get_status_data(bl);
	if (!tsc) //skill additional effect is about adding effects to the target...
		//So if the target can't be inflicted with statuses, this is pointless.
		return 0;	

	switch(skillid){
	case 0: // Normal attacks (no skill used)
	{
		if(sd) {
			// Automatic trigger of Blitz Beat
			if (pc_isfalcon(sd) && sd->status.weapon == W_BOW && (skill=pc_checkskill(sd,HT_BLITZBEAT))>0 &&
				rand()%1000 <= sstatus->luk*10/3+1 ) {
				int lv=(sd->status.job_level+9)/10;
				skill_castend_damage_id(src,bl,HT_BLITZBEAT,(skill<lv)?skill:lv,tick,0xf00000);
			}
			// Gank
			if(dstmd && dstmd->state.steal_flag<battle_config.skill_steal_max_tries && sd->status.weapon != W_BOW &&
				(skill=pc_checkskill(sd,RG_SNATCHER)) > 0 &&
				(skill*15 + 55) + pc_checkskill(sd,TF_STEAL)*10 > rand()%1000) {
				if(pc_steal_item(sd,bl))
					clif_skill_nodamage(src,bl,TF_STEAL,skill,1);
				else
					clif_skill_fail(sd,RG_SNATCHER,0,0);
			}
			// Chance to trigger Taekwon kicks [Dralnu]
			if(sd->sc.count && sd->sc.data[SC_COMBO].timer == -1) {
				if(sd->sc.data[SC_READYSTORM].timer != -1 &&
					sc_start(src,SC_COMBO, 15, TK_STORMKICK,
						(2000 - 4*sstatus->agi - 2*sstatus->dex)))
					; //Stance triggered
				else if(sd->sc.data[SC_READYDOWN].timer != -1 &&
					sc_start(src,SC_COMBO, 15, TK_DOWNKICK,
						(2000 - 4*sstatus->agi - 2*sstatus->dex)))
					; //Stance triggered
				else if(sd->sc.data[SC_READYTURN].timer != -1 && 
					sc_start(src,SC_COMBO, 15, TK_TURNKICK,
						(2000 - 4*sstatus->agi - 2*sstatus->dex)))
					; //Stance triggered
				else if(sd->sc.data[SC_READYCOUNTER].timer != -1)
				{	//additional chance from SG_FRIEND [Komurka]
					rate = 20;
					if (sd->sc.data[SC_SKILLRATE_UP].timer != -1 && sd->sc.data[SC_SKILLRATE_UP].val1 == TK_COUNTER) {
						rate += rate*sd->sc.data[SC_SKILLRATE_UP].val2/100;
						status_change_end(src,SC_SKILLRATE_UP,-1);
					} 
					sc_start4(src,SC_COMBO, rate, TK_COUNTER, bl->id,0,0,
						(2000 - 4*sstatus->agi - 2*sstatus->dex));
				}
			}
		}
	
		if (sc && sc->count) {	
		// Enchant Poison gives a chance to poison attacked enemies
			if(sc->data[SC_ENCPOISON].timer != -1) //Don't use sc_start since chance comes in 1/10000 rate.
				status_change_start(bl,SC_POISON,sc->data[SC_ENCPOISON].val2,
					sc->data[SC_ENCPOISON].val1,0,0,0,
					skill_get_time2(AS_ENCHANTPOISON,sc->data[SC_ENCPOISON].val1),0);
			// Enchant Deadly Poison gives a chance to deadly poison attacked enemies
			if(sc->data[SC_EDP].timer != -1)
				sc_start4(bl,SC_DPOISON,sc->data[SC_EDP].val2,
					sc->data[SC_EDP].val1,0,0,0,skill_get_time2(ASC_EDP,sc->data[SC_EDP].val1));
		}
		if (tsc->count) {
			if (tsc->data[SC_SPLASHER].timer != -1)
				sc_start4(bl,SC_POISON,2*tsc->data[SC_SPLASHER].val1+10,
					tsc->data[SC_SPLASHER].val1,0,0,0,
					skill_get_time2(tsc->data[SC_SPLASHER].val2,tsc->data[SC_SPLASHER].val1));
		}
	}
	break;

	case SM_BASH:
		if( sd && skilllv > 5 && pc_checkskill(sd,SM_FATALBLOW)>0 ){
			//TODO: How much % per base level it actually is?
			sc_start(bl,SC_STUN,(5*(skilllv-5)+(int)sd->status.base_level/10),
				skilllv,skill_get_time2(SM_FATALBLOW,skilllv));
		}
		break;

	case AS_VENOMKNIFE:
		if (sd) //Poison chance must be that of Envenom. [Skotlex]
			skilllv = pc_checkskill(sd, TF_POISON);
	case TF_POISON:
	case AS_SPLASHER:
		if(!sc_start(bl,SC_POISON,(2*skilllv+10),skilllv,skill_get_time2(skillid,skilllv))
			&&	sd && skillid==TF_POISON
		)
			clif_skill_fail(sd,skillid,0,0);
		break;

	case AS_SONICBLOW:
		sc_start(bl,SC_STUN,(2*skilllv+10),skilllv,skill_get_time2(skillid,skilllv));
		break;

	case AS_GRIMTOOTH:
		{
			int type = dstsd?SC_SLOWDOWN:SC_STOP;
			if (tsc->data[type].timer == -1)
				sc_start(bl,type,100,skilllv,skill_get_time2(skillid, skilllv));
			break;
		}
	case MG_FROSTDIVER:
	case WZ_FROSTNOVA:
		sc_start(bl,SC_FREEZE,skilllv*3+35,skilllv,skill_get_time2(skillid,skilllv));
		break;

	case WZ_STORMGUST:
		tsc->data[SC_FREEZE].val3++;
		if(tsc->data[SC_FREEZE].val3 >= 3) //Tharis pointed out that this is normal freeze chance with a base of 300%
			sc_start(bl,SC_FREEZE,300,skilllv,skill_get_time2(skillid,skilllv));
		break;

	case WZ_METEOR:
		sc_start(bl,SC_STUN,3*skilllv,skilllv,skill_get_time2(skillid,skilllv));
		break;

	case WZ_VERMILION:
		sc_start(bl,SC_BLIND,4*skilllv,skilllv,skill_get_time2(skillid,skilllv));
		break;
		
	case HT_FREEZINGTRAP:
		sc_start(bl,SC_FREEZE,(3*skilllv+35),skilllv,skill_get_time2(skillid,skilllv));
		break;

	 case HT_FLASHER:
		sc_start(bl,SC_BLIND,(10*skilllv+30),skilllv,skill_get_time2(skillid,skilllv));
		break;

	case HT_LANDMINE:
		sc_start(bl,SC_STUN,(5*skilllv+30),skilllv,skill_get_time2(skillid,skilllv));
		break;

	case HT_SHOCKWAVE:
		status_percent_damage(src, bl, 0, 15*skilllv+5);
		break;

	case HT_SANDMAN:
		sc_start(bl,SC_SLEEP,(10*skilllv+40),skilllv,skill_get_time2(skillid,skilllv));
		break;

	case TF_SPRINKLESAND:
		sc_start(bl,SC_BLIND,20,skilllv,skill_get_time2(skillid,skilllv));
		break;

	case TF_THROWSTONE:
		sc_start(bl,SC_STUN,3,skilllv,skill_get_time(skillid,skilllv));
		sc_start(bl,SC_BLIND,3,skilllv,skill_get_time2(skillid,skilllv));
		break;

	case NPC_DARKCROSS:
	case CR_HOLYCROSS:
		sc_start(bl,SC_BLIND,3*skilllv,skilllv,skill_get_time2(skillid,skilllv));
		break;

	case CR_GRANDCROSS:
	case NPC_GRANDDARKNESS:
		if(battle_check_undead(tstatus->race,tstatus->def_ele) || tstatus->race == RC_DEMON)
			sc_start(bl,SC_BLIND,100,skilllv,skill_get_time2(skillid,skilllv));
		break;

	case AM_ACIDTERROR:
		sc_start(bl,SC_BLEEDING,(skilllv*3),skilllv,skill_get_time2(skillid,skilllv));
		if (skill_break_equip(bl, EQP_ARMOR, 100*skill_get_time(skillid,skilllv), BCT_ENEMY))
			clif_emotion(bl,23);
		break;

	case AM_DEMONSTRATION:
		skill_break_equip(bl, EQP_WEAPON, 100*skilllv, BCT_ENEMY);
		break;
		
	case CR_SHIELDCHARGE:
		sc_start(bl,SC_STUN,(15+skilllv*5),skilllv,skill_get_time2(skillid,skilllv));
		break;

	case PA_PRESSURE:
		status_percent_damage(src, bl, 0, 15+5*skilllv);
		break;

	case RG_RAID:
		sc_start(bl,SC_STUN,(10+3*skilllv),skilllv,skill_get_time(skillid,skilllv));
		sc_start(bl,SC_BLIND,(10+3*skilllv),skilllv,skill_get_time2(skillid,skilllv));
		break;

	case BA_FROSTJOKE:
		sc_start(bl,SC_FREEZE,(15+5*skilllv),skilllv,skill_get_time2(skillid,skilllv));
		break;

	case DC_SCREAM:
		sc_start(bl,SC_STUN,(25+5*skilllv),skilllv,skill_get_time2(skillid,skilllv));
		break;

	case BD_LULLABY:
		sc_start(bl,SC_SLEEP,15,skilllv,skill_get_time2(skillid,skilllv));
		break;

	case DC_UGLYDANCE:
		rate = 5+5*skilllv;
		if(sd && (skill=pc_checkskill(sd,DC_DANCINGLESSON)))
		    rate += 5+skill;
		status_zap(bl, 0, rate);
  		break;
	case SL_STUN:
		if (tstatus->size==1) //Only stuns mid-sized mobs.
			sc_start(bl,SC_STUN,(30+10*skilllv),skilllv,skill_get_time(skillid,skilllv));
		break;
			
	case NPC_PETRIFYATTACK:
	case NPC_CURSEATTACK:
	case NPC_SLEEPATTACK:
	case NPC_BLINDATTACK:
	case NPC_POISON:
	case NPC_SILENCEATTACK:
	case NPC_STUNATTACK:
		sc_start(bl,SkillStatusChangeTable(skillid),50+10*skilllv,skilllv,src->type==BL_PET?skilllv*1000:skill_get_time2(skillid,skilllv));
		break;

	case NPC_MENTALBREAKER:
		status_percent_damage(src, bl, 0, -(10+skilllv));
		break;
	// Equipment breaking monster skills [Celest]
	case NPC_BREAKWEAPON:
		skill_break_equip(bl, EQP_WEAPON, 150*skilllv, BCT_ENEMY);
		break;
	case NPC_BREAKARMOR:
		skill_break_equip(bl, EQP_ARMOR, 150*skilllv, BCT_ENEMY);
		break;
	case NPC_BREAKHELM:
		skill_break_equip(bl, EQP_HELM, 150*skilllv, BCT_ENEMY);
		break;
	case NPC_BREAKSHIELD:
		skill_break_equip(bl, EQP_SHIELD, 150*skilllv, BCT_ENEMY);
		break;

	case CH_TIGERFIST:
		sc_start(bl,SC_STOP,(10+skilllv*10),0,skill_get_time2(skillid,skilllv));
		break;

	case LK_SPIRALPIERCE:
		sc_start(bl,SC_STOP,(15+skilllv*5),0,skill_get_time2(skillid,skilllv));
		break;

	case ST_REJECTSWORD:
		sc_start(bl,SC_AUTOCOUNTER,(skilllv*15),skilllv,skill_get_time2(skillid,skilllv));
		break;

	case PF_FOGWALL:
		if (src != bl && tsc->data[SC_DELUGE].timer == -1)
			status_change_start(bl,SC_BLIND,10000,skilllv,0,0,0,skill_get_time2(skillid,skilllv),8);
		break;

	case LK_HEADCRUSH:				/* ヘッドクラッシュ */
		if (!(battle_check_undead(tstatus->race, tstatus->def_ele) || tstatus->race == RC_DEMON))
			sc_start(bl, SC_BLEEDING,50, skilllv, skill_get_time2(skillid,skilllv));
		break;

	case LK_JOINTBEAT:
		sc_start(bl,SkillStatusChangeTable(skillid),(5*skilllv+5),skilllv,skill_get_time2(skillid,skilllv));
		break;

	case ASC_METEORASSAULT:
		//Any enemies hit by this skill will receive Stun, Darkness, or external bleeding status ailment with a 5%+5*SkillLV% chance.
		switch(rand()%3) {
			case 0:
				sc_start(bl,SC_BLIND,(5+skilllv*5),skilllv,skill_get_time2(skillid,1));
				break;
			case 1:
				sc_start(bl,SC_STUN,(5+skilllv*5),skilllv,skill_get_time2(skillid,2));
				break;
			default:
				sc_start(bl,SC_BLEEDING,(5+skilllv*5),skilllv,skill_get_time2(skillid,3));
  		}
		break;

	case HW_NAPALMVULCAN:
		sc_start(bl,SC_CURSE,5*skilllv,skilllv,skill_get_time2(skillid,skilllv));
		break;

	case WS_CARTTERMINATION:	// Cart termination
		sc_start(bl,SC_STUN,5*skilllv,skilllv,skill_get_time2(skillid,skilllv));
		break;

	case CR_ACIDDEMONSTRATION:
		skill_break_equip(bl, EQP_WEAPON|EQP_ARMOR, 100*skilllv, BCT_ENEMY);
		break;

	case TK_DOWNKICK:
		sc_start(bl,SC_STUN,100,skilllv,skill_get_time2(skillid,skilllv));
		break;
			
	case TK_JUMPKICK:
		//Cancel out Soul Linker status of the target. [Skotlex]
		if (tsc->count) {
			if (tsc->data[SC_PRESERVE].timer != -1) //preserve blocks the cleaning
				break;	
			//Remove pitched potions effect.
			if (tsc->data[SC_ASPDPOTION0].timer != -1 && tsc->data[SC_ASPDPOTION0].val4)
				status_change_end(bl, SC_ASPDPOTION0, -1);
			if (tsc->data[SC_ASPDPOTION1].timer != -1 && tsc->data[SC_ASPDPOTION1].val4)
				status_change_end(bl, SC_ASPDPOTION1, -1);
			if (tsc->data[SC_ASPDPOTION2].timer != -1 && tsc->data[SC_ASPDPOTION2].val4)
				status_change_end(bl, SC_ASPDPOTION2, -1);
			if (tsc->data[SC_ASPDPOTION3].timer != -1 && tsc->data[SC_ASPDPOTION3].val4)
				status_change_end(bl, SC_ASPDPOTION3, -1);
			if (tsc->data[SC_SPIRIT].timer != -1)
				status_change_end(bl, SC_SPIRIT, -1);
			if (tsc->data[SC_ONEHAND].timer != -1)
				status_change_end(bl, SC_ONEHAND, -1);
			if (tsc->data[SC_ADRENALINE2].timer != -1)
				status_change_end(bl, SC_ADRENALINE2, -1);
		}		
		break;
	case TK_TURNKICK:
	case MO_BALKYOUNG: //Note: attack_type is passed as BF_WEAPON for the actual target, BF_MISC for the splash-affected mobs.
		if(attack_type == BF_MISC) //70% base stun chance...
			sc_start(bl,SC_STUN,70,skilllv,skill_get_time2(skillid,skilllv));
		break;
	case GS_BULLSEYE: //0.1% coma rate.
		status_change_start(bl,SC_COMA,10,skilllv,0,0,0,0,0);
		break;
	case GS_CRACKER:
		if (!dstsd)	// according to latest patch, should not work on players [Reddozen]
			sc_start(bl,SC_STUN,(100 - 10*distance_bl(src, bl)),skilllv,skill_get_time2(skillid,skilllv)); //Temp stun rate 
		break;
	case GS_PIERCINGSHOT:
		sc_start(bl,SC_BLEEDING,(skilllv*3),skilllv,skill_get_time2(skillid,skilllv));
		break;
	case NJ_HYOUSYOURAKU:
		sc_start(bl,SC_FREEZE,(10+10*skilllv),skilllv,skill_get_time2(skillid,skilllv));
		break;
	case GS_FLING:
		sc_start(bl,SC_FLING,100, sd?sd->spiritball_old:5,skill_get_time(skillid,skilllv));
		break;
	}

	if(sd && attack_type&BF_WEAPON &&
		skillid != MC_CARTREVOLUTION &&
		skillid != AM_DEMONSTRATION &&
		skillid != CR_REFLECTSHIELD
	){	//Trigger status effects
		int i, type;
		for(i=0; i < MAX_PC_BONUS && sd->addeff[i].flag; i++)
		{
			rate = sd->addeff[i].rate;
			type = sd->state.arrow_atk; //Ranged?
			if (type)
				rate += sd->addeff[i].arrow_rate;
			if (!rate) continue;
			
			if (!(sd->addeff[i].flag&ATF_LONG && sd->addeff[i].flag&ATF_SHORT))
			{	//Trigger has range consideration.
				if ((sd->addeff[i].flag&ATF_LONG && !type) ||
					(sd->addeff[i].flag&ATF_SHORT && type))
					continue; //Range Failed.
			}
			type =  sd->addeff[i].id;
			skill = skill_get_time2(StatusSkillChangeTable[type],7);
			
			if (sd->addeff[i].flag&ATF_TARGET)
				status_change_start(bl,type,rate,7,0,0,0,skill,0);
			
			if (sd->addeff[i].flag&ATF_SELF)
				status_change_start(src,type,rate,7,0,0,0,skill,0);
		}
	}

	if (md && battle_config.summons_trigger_autospells && md->master_id && md->special_state.ai)
	{	//Pass heritage to Master for status causing effects. [Skotlex]
		sd = map_id2sd(md->master_id);
		src = sd?&sd->bl:src;
	}

	//Reports say that autospell effects get triggered on skills and pretty much everything including splash attacks. [Skotlex]
	if(sd && !status_isdead(bl) && src != bl &&
		!(skillid && skill_get_nk(skillid)&NK_NO_DAMAGE)) {
		struct block_list *tbl;
		struct unit_data *ud;
		int i, skilllv;
		for (i = 0; i < MAX_PC_BONUS && sd->autospell[i].id; i++) {

			skill = (sd->autospell[i].id > 0) ? sd->autospell[i].id : -sd->autospell[i].id;
			
			if (skillnotok(skill, sd))
				continue;

			skilllv = (sd->autospell[i].lv > 0) ? sd->autospell[i].lv : 1;
			rate = (!sd->state.arrow_atk) ? sd->autospell[i].rate : sd->autospell[i].rate / 2;

			if (rand()%1000 > rate)
				continue;
			if (sd->autospell[i].id < 0)
				tbl = src;
			else
				tbl = bl;
			
			if (tbl != src && !battle_check_range(src, tbl, skill_get_range2(src, skill, skilllv)))
				continue; //Autoskills DO check for target-src range. [Skotlex]
			rate = skill_get_inf(skill);
			switch (skill_get_casttype(skill)) {
				case CAST_GROUND:
					skill_castend_pos2(src, tbl->x, tbl->y, skill, skilllv, tick, 0);
					break;
				case CAST_NODAMAGE:
					skill_castend_nodamage_id(src, tbl, skill, skilllv, tick, 0);
					break;
				case CAST_DAMAGE:
					skill_castend_damage_id(src, tbl, skill, skilllv, tick, 0);
					break;
			}
			//Set canact delay. [Skotlex]
			ud = unit_bl2ud(src);
			if (ud) {
				rate = skill_delayfix(src, skill, skilllv)/2;
				if (DIFF_TICK(ud->canact_tick, tick + rate) < 0)
					ud->canact_tick = tick+rate;
			}
			break; //Only one auto skill comes off at a time.
		}
	}

	//Polymorph
	if(sd && sd->classchange && attack_type&BF_WEAPON &&
		dstmd && !(tstatus->mode&MD_BOSS) && !dstmd->guardian_data &&
	  	(dstmd->class_ < 1324 || dstmd->class_ > 1363) && //Treasure boxes
	  	!mob_is_clone(dstmd->class_) && 
	  	(rand()%10000 < sd->classchange)) 
	{
		struct mob_db *mob;
		int class_;
		skill = 0;
		do {
			do {
				class_ = rand() % MAX_MOB_DB;
			} while (!mobdb_checkid(class_));
			
			rate = rand() % 1000000;
			mob = mob_db(class_);
		} while (
			(mob->status.mode&(MD_BOSS|MD_PLANT) || mob->summonper[0] <= rate) &&
		  	(skill++) < 2000);
		if (skill < 2000)
			mob_class_change(dstmd,class_);
	}
	

	return 0;
}

/* Splitted off from skill_additional_effect, which is never called when the
 * attack skill kills the enemy. Place in this function counter status effects 
 * when using skills (eg: Asura's sp regen penalty, or counter-status effects 
 * from cards) that will take effect on the source, not the target. [Skotlex]
 * Note: Currently this function only applies to Extremity Fist and BF_WEAPON 
 * type of skills, so not every instance of skill_additional_effect needs a call
 * to this one.
 */
int skill_counter_additional_effect (struct block_list* src, struct block_list *bl, int skillid, int skilllv, int attack_type, unsigned int tick)
{
	int rate;
	struct map_session_data *sd=NULL;
	struct map_session_data *dstsd=NULL;
	struct status_change *tsc;
	
	nullpo_retr(0, src);
	nullpo_retr(0, bl);

	if(skillid < 0) 
	{	// remove the debug print when this case is finished
		ShowDebug("skill_counter_additional_effect: skillid=%i\ncall: %p %p %i %i %i %i",skillid,
						src, bl,skillid,skilllv,attack_type,tick);
		return 0;
	}
	if(skillid > 0 && skilllv <= 0) return 0;	// don't forget auto attacks! - celest

	tsc = status_get_sc(bl);
	if (tsc && !tsc->count)
		tsc = NULL;
	
	BL_CAST(BL_PC, src, sd);
	BL_CAST(BL_PC, bl, dstsd);

	switch(skillid){
	case 0: //Normal Attack
		if(tsc && tsc->data[SC_KAAHI].timer != -1 && tsc->data[SC_KAAHI].val4 == -1)
			tsc->data[SC_KAAHI].val4 = add_timer(
				tick+skill_get_time2(SL_KAAHI,tsc->data[SC_KAAHI].val1), 
				kaahi_heal_timer, bl->id, SC_KAAHI); //Activate heal.
		break;
	case MO_EXTREMITYFIST:
		sc_start(src,SkillStatusChangeTable(skillid),100,skilllv,skill_get_time2(skillid,skilllv));
		break;
	case GS_FULLBUSTER:
		sc_start(src,SC_BLIND,(2*skilllv),skilllv,skill_get_time2(skillid,skilllv));
		break;
	}

	if(sd && skillid && attack_type&BF_MAGIC && status_isdead(bl) &&
	 	skill_get_inf(skillid)!=INF_GROUND_SKILL &&
		(rate=pc_checkskill(sd,HW_SOULDRAIN))>0
	){	//Soul Drain should only work on targetted spells [Skotlex]
		if (pc_issit(sd)) pc_setstand(sd); //Character stuck in attacking animation while 'sitting' fix. [Skotlex]
		clif_skill_nodamage(src,bl,HW_SOULDRAIN,rate,1);
		status_heal(src, 0, status_get_lv(bl)*(95+15*rate)/100, 2);
	}

	if(dstsd && attack_type&BF_WEAPON)
	{	//Counter effects.
		int i, type, time;
		for(i=0; i < MAX_PC_BONUS && dstsd->addeff2[i].flag; i++)
		{
			rate = dstsd->addeff2[i].rate;
			type = (sd && sd->state.arrow_atk) || (status_get_range(src)>2);
			if (type)
				rate+=dstsd->addeff2[i].arrow_rate;
			if (!rate) continue;
			
			if (!(dstsd->addeff2[i].flag&ATF_LONG && dstsd->addeff2[i].flag&ATF_SHORT))
			{	//Trigger has range consideration.
				if ((dstsd->addeff2[i].flag&ATF_LONG && !type) ||
					(dstsd->addeff2[i].flag&ATF_SHORT && type))
					continue; //Range Failed.
			}
			type =  dstsd->addeff2[i].id;
			time = skill_get_time2(StatusSkillChangeTable[type],7);
			
			if (dstsd->addeff2[i].flag&ATF_TARGET)
				status_change_start(src,type,rate,7,0,0,0,time,0);
			
			if (dstsd->addeff2[i].flag&ATF_SELF && !status_isdead(bl))
				status_change_start(bl,type,rate,7,0,0,0,time,0);
		}
	}
	
	//Trigger counter-spells to retaliate against damage causing skills. [Skotlex]
	if(dstsd && !status_isdead(bl) && src != bl && !(skillid && skill_get_nk(skillid)&NK_NO_DAMAGE)) 
	{
		struct block_list *tbl;
		struct unit_data *ud;
		int i, skillid, skilllv, rate;

		for (i = 0; i < MAX_PC_BONUS; i++) {
			if (dstsd->autospell2[i].id == 0)
				break;

			skillid = (dstsd->autospell2[i].id > 0) ? dstsd->autospell2[i].id : -dstsd->autospell2[i].id;
			skilllv = (dstsd->autospell2[i].lv > 0) ? dstsd->autospell2[i].lv : 1;
			rate = ((sd && !sd->state.arrow_atk) || (status_get_range(src)<=2)) ?
				dstsd->autospell2[i].rate : dstsd->autospell2[i].rate / 2;
			
			if (skillnotok(skillid, dstsd))
				continue;
			if (rand()%1000 > rate)
				continue;
			if (dstsd->autospell2[i].id < 0)
				tbl = bl;
			else
				tbl = src;
			
			if (tbl != bl && !battle_check_range(bl, tbl, skill_get_range2(bl, skillid, skilllv)))
				continue; //Autoskills DO check for target-src range. [Skotlex]
			
			switch (skill_get_casttype(skillid)) {
				case CAST_GROUND:
					skill_castend_pos2(bl, tbl->x, tbl->y, skillid, skilllv, tick, 0);
					break;
				case CAST_NODAMAGE:
					skill_castend_nodamage_id(bl, tbl, skillid, skilllv, tick, 0);
					break;
				case CAST_DAMAGE:
					skill_castend_damage_id(bl, tbl, skillid, skilllv, tick, 0);
					break;
			}
			//Set canact delay. [Skotlex]
			ud = unit_bl2ud(bl);
			if (ud) {
				rate = skill_delayfix(bl, skillid, skilllv)/2;
				if (DIFF_TICK(ud->canact_tick, tick + rate) < 0)
					ud->canact_tick = tick+rate;
			}
			break; //trigger only one auto-spell per hit.
		}
	}
	return 0;
}
/*=========================================================================
 Breaks equipment. On-non players causes the corresponding strip effect.
 - rate goes from 0 to 10000 (100.00%)
 - flag is a BCT_ flag to indicate which type of adjustment should be used
   (BCT_ENEMY/BCT_PARTY/BCT_SELF) are the valid values.
--------------------------------------------------------------------------*/
int skill_break_equip (struct block_list *bl, unsigned short where, int rate, int flag) 
{
	static int where_list[4] = {EQP_WEAPON, EQP_ARMOR, EQP_SHIELD, EQP_HELM};
	static int scatk[4] = {SC_STRIPWEAPON, SC_STRIPARMOR, SC_STRIPSHIELD, SC_STRIPHELM};
	static int scdef[4] = {SC_CP_WEAPON, SC_CP_ARMOR, SC_CP_SHIELD, SC_CP_HELM};
	struct status_change *sc = status_get_sc(bl);
	int i,j;
	TBL_PC *sd;
	BL_CAST(BL_PC, bl, sd);
	if (sc && !sc->count)
		sc = NULL;
	
	if (sd) {
		if (sd->unbreakable_equip)
			where &= ~sd->unbreakable_equip;
		if (sd->unbreakable)
			rate -= rate*sd->unbreakable/100;
		if (where&EQP_WEAPON) {
			switch (sd->status.weapon) {
				case W_FIST:	//Bare fists should not break :P
				case W_1HAXE:
				case W_2HAXE:
				case W_MACE: // Axes and Maces can't be broken [DracoRPG]
				case W_STAFF:
				case W_BOOK: //Rods and Books can't be broken [Skotlex]
					where &= ~EQP_WEAPON;
			}
		}
	}
	if (flag&BCT_ENEMY) {
		if (battle_config.equip_skill_break_rate != 100)
			rate = rate*battle_config.equip_skill_break_rate/100;
	} else if (flag&(BCT_PARTY|BCT_SELF)) {
		if (battle_config.equip_self_break_rate != 100)
			rate = rate*battle_config.equip_self_break_rate/100;
	}

	for (i = 0; i < 4; i++) {
		if (where&where_list[i]) {
			if (sc && sc->count && sc->data[scdef[i]].timer != -1)
				where&=~where_list[i];
			else if (rand()%10000 >= rate)
				where&=~where_list[i];
			else if (!sd) //Cause Strip effect.
				sc_start(bl,scatk[i],100,0,skill_get_time(StatusSkillChangeTable[scatk[i]],1));
		}
	}
	if (!where) //Nothing to break.
		return 0;
	if (sd) {
		for (i = 0; i < EQI_MAX; i++) {
			j = sd->equip_index[i];
			if (j <= 0 || sd->status.inventory[j].attribute == 1 || !sd->inventory_data[j])
				continue;
			flag = 0;
			switch(i) {
				case EQI_HEAD_TOP: //Upper Head
					flag = (where&EQP_HELM);
					break;
				case EQI_ARMOR: //Body
					flag = (where&EQP_ARMOR);
					break;
				case EQI_HAND_R: //Left/Right hands
				case EQI_HAND_L:
					flag = (
						(where&EQP_WEAPON && sd->inventory_data[j]->type == IT_WEAPON) ||
						(where&EQP_SHIELD && sd->inventory_data[j]->type == IT_ARMOR));
					break;
				default:
					continue;
			}
			if (flag) {
				sd->status.inventory[j].attribute = 1;
				pc_unequipitem(sd, j, 3);
			}
		}
		clif_equiplist(sd);
	}

	return where; //Return list of pieces broken.
}
/*=========================================================================
 Used to knock back players, monsters, traps, etc
 If count&0xf00000, the direction is send in the 6th byte.
 If count&0x10000, the direction is to the back of the target, otherwise is away from the src.
 If count&0x20000, position update packets must not be sent.
 IF count&0X40000, direction is random.
--------------------------------------------------------------------------*/
int skill_blown (struct block_list *src, struct block_list *target, int count)
{
	int dx=0,dy=0,nx,ny;
	int x=target->x,y=target->y;
	int dir,ret;
	struct skill_unit *su=NULL;

	nullpo_retr(0, src);

	if (src != target && map_flag_gvg(target->m))
		return 0; //No knocking back in WoE
	if (!count&0xffff)
		return 0; //Actual knockback distance is 0.
	
	switch (target->type) {
		case BL_MOB:
			if (((TBL_MOB*)target)->class_ == MOBID_EMPERIUM)
				return 0;
			if(src != target && is_boss(target)) //Bosses can't be knocked-back
				return 0;
			break;
		case BL_SKILL:
			su=(struct skill_unit *)target;
			break;
	}

	if (count&0xf00000)
		dir = (count>>20)&0xf;
	else if (count&0x10000 || (target->x==src->x && target->y==src->y))
		dir = unit_getdir(target);
	else if (count&0x40000) //Flag for random pushing.
		dir = rand()%8;
	else
		dir = map_calc_dir(target,src->x,src->y);
	if (dir>=0 && dir<8){
		dx = -dirx[dir];
		dy = -diry[dir];
	}

	ret=path_blownpos(target->m,x,y,dx,dy,count&0xffff);
	nx=ret>>16;
	ny=ret&0xffff;

	if (!su)
		unit_stop_walking(target,0); 

	dx = nx - x;
	dy = ny - y;

	if (!dx && !dy) //Could not knockback.
		return 0;
	
	map_foreachinmovearea(clif_outsight,target->m,
		x-AREA_SIZE,y-AREA_SIZE,x+AREA_SIZE,y+AREA_SIZE,
		dx,dy,target->type==BL_PC?BL_ALL:BL_PC,target);
		
	if(su)
		skill_unit_move_unit_group(su->group,target->m,dx,dy);
	else
		map_moveblock(target, nx, ny, gettick());

	map_foreachinmovearea(clif_insight,target->m,
		nx-AREA_SIZE,ny-AREA_SIZE,nx+AREA_SIZE,ny+AREA_SIZE,
		-dx,-dy,target->type==BL_PC?BL_ALL:BL_PC,target);
	
	if(!(count&0x20000)) 
		clif_blown(target);

	return (count&0xFFFF); //Return amount of knocked back cells.
}

/*
 * =========================================================================
 * �X�L��?U??��?�?�܂Ƃ�
 * flag��?��?B16?i?
 *	00XRTTff
 *  ff	= magic�Ōv�Z�ɓn�����?j
 *	TT	= �p�P�b�g��type����(0�Ńf�t�H���g?j
 *  X   = �p�P�b�g�̃X�L��Lv
 *  R	= �\��?iskill_area_sub�Ŏg�p����?j
 *-------------------------------------------------------------------------
 */

int skill_attack (int attack_type, struct block_list* src, struct block_list *dsrc, struct block_list *bl, int skillid, int skilllv, unsigned int tick, int flag)
{
	struct Damage dmg;
	struct status_data *sstatus, *tstatus;
	struct status_change *sc;
	struct map_session_data *sd, *tsd;
	int type,lv,damage,rdamage=0;

	if(skillid > 0 && skilllv <= 0) return 0;

	nullpo_retr(0, src);	//Source is the master behind the attack (player/mob/pet)
	nullpo_retr(0, dsrc); //dsrc is the actual originator of the damage, can be the same as src, or a skill casted by src.
	nullpo_retr(0, bl); //Target to be attacked.

	if (src != dsrc) {
		//When caster is not the src of attack, this is a ground skill, and as such, do the relevant target checking. [Skotlex]
		if (!status_check_skilluse(battle_config.skill_caster_check?src:NULL, bl, skillid, 2))
			return 0;
	} else if (flag && skill_get_nk(skillid)&NK_SPLASH) {
		//Note that splash attacks often only check versus the targetted mob, those around the splash area normally don't get checked for being hidden/cloaked/etc. [Skotlex]
		if (!status_check_skilluse(dsrc, bl, skillid, 2))
			return 0;
	}

	BL_CAST(BL_PC, dsrc, sd);
	BL_CAST(BL_PC, bl, tsd);

	sstatus = status_get_status_data(dsrc);
	tstatus = status_get_status_data(bl);
// Is this check really needed? FrostNova won't hurt you if you step right where the caster is?
	if(skillid == WZ_FROSTNOVA && dsrc->x == bl->x && dsrc->y == bl->y)
		return 0;

	type=-1;
	lv=(flag>>20)&0xf;
	dmg=battle_calc_attack(attack_type,src,bl,skillid,skilllv,flag&0xff );

	//Skotlex: Adjusted to the new system
	if(src->type==BL_PET && (struct pet_data *)src)
	{ // [Valaris]
		struct pet_data *pd = (struct pet_data *)src;
		if (pd->a_skill && pd->a_skill->div_ && pd->a_skill->id == skillid)
		{
			int element = skill_get_pl(skillid);
			if (skillid == -1)
				element = sstatus->rhw.ele;
			dmg.damage=battle_attr_fix(src, bl, skilllv, element, tstatus->def_ele, tstatus->ele_lv);
			dmg.damage2=0;
			dmg.div_= pd->a_skill->div_;
		}
	}

	sc= status_get_sc(bl);
	if (sc && !sc->count) sc = NULL; //Don't need it.

	if (attack_type&BF_MAGIC) {
	 	if(sc && sc->data[SC_KAITE].timer != -1 && (dmg.damage || dmg.damage2)
			&& !(sstatus->mode&MD_BOSS) && (sd || status_get_lv(src) <= 80) )
		{	//Works on players or mobs with level under 80.
			clif_specialeffect(bl, 438, AREA);
			if (--sc->data[SC_KAITE].val2 <= 0)
				status_change_end(bl, SC_KAITE, -1);
			bl = src; //Just make the skill attack yourself @.@
			sc = status_get_sc(bl);
			tsd = (bl->type == BL_PC)?(TBL_PC*)bl:NULL;
			if (sc && !sc->count)
				sc = NULL; //Don't need it.
			//Spirit of Wizard blocks bounced back spells.
			if (sc && sc->data[SC_SPIRIT].timer != -1 && sc->data[SC_SPIRIT].val2 == SL_WIZARD
				&& !(tsd && (type = pc_search_inventory (tsd, 7321)) < 0))
			{
				if (tsd) pc_delitem(tsd, type, 1, 0);
				dmg.damage = dmg.damage2 = 0;
				dmg.dmg_lv = ATK_FLEE;
			}
		}
	
		if(sc && sc->data[SC_MAGICROD].timer != -1 && src == dsrc) {
			//struct unit_data *ud;
			int sp = skill_get_sp(skillid,skilllv);
			dmg.damage = dmg.damage2 = 0;
			dmg.dmg_lv = ATK_FLEE; //This will prevent skill additional effect from taking effect. [Skotlex]
			sp = sp * sc->data[SC_MAGICROD].val2 / 100;
			if(skillid == WZ_WATERBALL && skilllv > 1)
				sp = sp/((skilllv|1)*(skilllv|1)); //Estimate SP cost of a single water-ball
			status_heal(bl, 0, sp, 2);
			clif_skill_nodamage(bl,bl,SA_MAGICROD,sc->data[SC_MAGICROD].val1,1);
			/* It was reported you don't get an act delay once it triggers.
			ud = unit_bl2ud(bl);
			if (ud) ud->canact_tick = tick
				+ skill_delayfix(bl, SA_MAGICROD, sc->data[SC_MAGICROD].val1);
			*/	
		}
	}

	damage = dmg.damage + dmg.damage2;

	if (damage > 0 && src != bl && src == dsrc)
		rdamage = battle_calc_return_damage(bl, &damage, dmg.flag);
		
	if(lv==15)
		lv=-1;

	if( flag&0xff00 )
		type=(flag&0xff00)>>8;

	if((damage <= 0 || damage < dmg.div_)
			&& skillid != CH_PALMSTRIKE) //Palm Strike is the only skill that will knockback even if it misses. [Skotlex]
		dmg.blewcount = 0;

	if(skillid == CR_GRANDCROSS||skillid == NPC_GRANDDARKNESS) {
		if(battle_config.gx_disptype) dsrc = src;
		if(src == bl) type = 4;
	}

	if(sd) {
		//Sorry for removing the Japanese comments, but they were actually distracting 
		//from the actual code and I couldn't understand a thing anyway >.< [Skotlex]
		if (sd->sc.data[SC_COMBO].timer!=-1)
		{	//End combo state after skill is invoked. [Skotlex]
			switch (skillid) {
			case TK_TURNKICK:
			case TK_STORMKICK:
			case TK_DOWNKICK:
			case TK_COUNTER:
				if (pc_famerank(sd->char_id,MAPID_TAEKWON))
			  	{	//Extend combo time.
					sd->skillid_old = skillid; //Set as previous so you can't repeat
					sd->skilllv_old = skilllv;
					sd->sc.data[SC_COMBO].val1 = skillid; //Update combo-skill
					delete_timer(sd->sc.data[SC_COMBO].timer, status_change_timer);
					sd->sc.data[SC_COMBO].timer = add_timer(
						tick+sd->sc.data[SC_COMBO].val4,
					  	status_change_timer, src->id, SC_COMBO);
					break;
				}
			default:
				status_change_end(src,SC_COMBO,-1);
			}
		}
		switch(skillid)
		{
			case MO_TRIPLEATTACK:
			{
				int delay = 1000 - 4*sstatus->agi - 2*sstatus->dex;
				if (pc_checkskill(sd, MO_CHAINCOMBO) > 0)
					delay += 300 * battle_config.combo_delay_rate / 100;
				sc_start(src,SC_COMBO,100,MO_TRIPLEATTACK,delay);
				clif_combo_delay(src, delay);
				
				if (sd->status.party_id>0) //bonus from SG_FRIEND [Komurka]
					party_skill_check(sd, sd->status.party_id, MO_TRIPLEATTACK, skilllv);
				break;
			}
			case MO_CHAINCOMBO:
			{
				int delay = 1000 - 4*sstatus->agi - 2*sstatus->dex;
				if(pc_checkskill(sd, MO_COMBOFINISH) > 0 && sd->spiritball > 0)
					delay += 300 * battle_config.combo_delay_rate /100;
				sc_start(src,SC_COMBO,100,MO_CHAINCOMBO,delay);
				clif_combo_delay(src,delay);
				break;
			}
			case MO_COMBOFINISH:
			{
				int delay = 700 - 4*sstatus->agi - 2*sstatus->dex;
				if (
					(pc_checkskill(sd, MO_EXTREMITYFIST) > 0 && sd->spiritball >= 4 && sd->sc.data[SC_EXPLOSIONSPIRITS].timer != -1) ||
					(pc_checkskill(sd, CH_TIGERFIST) > 0 && sd->spiritball > 0) ||
					(pc_checkskill(sd, CH_CHAINCRUSH) > 0 && sd->spiritball > 1)
				)
					delay += 300 * battle_config.combo_delay_rate /100;
				sc_start(src,SC_COMBO,100,MO_COMBOFINISH,delay);
				clif_combo_delay(src,delay);
				break;
			}
			case CH_TIGERFIST:
			{	//Tigerfist is now a combo-only skill. [Skotlex]
				int delay = 1000 - 4*sstatus->agi - 2*sstatus->dex;
				if(
					(pc_checkskill(sd, MO_EXTREMITYFIST) > 0 && sd->spiritball >= 3 && sd->sc.data[SC_EXPLOSIONSPIRITS].timer != -1) ||
					(pc_checkskill(sd, CH_CHAINCRUSH) > 0)
				)
					delay += 300 * battle_config.combo_delay_rate /100;
				sc_start(src,SC_COMBO,100,CH_TIGERFIST,delay);
				clif_combo_delay(src,delay);
				break;
			}
			case CH_CHAINCRUSH:
			{
				int delay = 1000 - 4*sstatus->agi - 2*sstatus->dex;
				if(pc_checkskill(sd, MO_EXTREMITYFIST) > 0 &&
					sd->spiritball >= 1 &&
					sd->sc.data[SC_EXPLOSIONSPIRITS].timer != -1)
					delay += 300 * battle_config.combo_delay_rate /100;
				sc_start(src,SC_COMBO,100,CH_CHAINCRUSH,delay);
				clif_combo_delay(src,delay);
				break;
			}
			case AC_DOUBLE:
				if((tstatus->race == RC_BRUTE || tstatus->race == RC_INSECT) &&
					pc_checkskill(sd, HT_POWER)) {
					//TODO: This code was taken from Triple Blows, is this even how it should be? [Skotlex]
					sc_start4(src,SC_COMBO,100,HT_POWER,bl->id,0,0,2000);
					clif_combo_delay(src,2000);
				}
				break;
			case TK_COUNTER:
			{	//bonus from SG_FRIEND [Komurka]
				int level;
				if(sd->status.party_id>0 && (level = pc_checkskill(sd,SG_FRIEND)))
					party_skill_check(sd, sd->status.party_id, TK_COUNTER,level);
			}
				break;
			case SL_STIN:
			case SL_STUN:
				if (skilllv >= 7 && sd->sc.data[SC_SMA].timer == -1)
					sc_start(src,SC_SMA,100,skilllv,skill_get_time(SL_SMA, skilllv));
				break;
			case GS_FULLBUSTER:
				//Can't attack nor use items until skill's delay expires. [Skotlex]
				sd->ud.attackabletime = sd->canuseitem_tick = sd->ud.canact_tick;
				break;
		}	//Switch End
	}

	//Display damage.
	switch(skillid){
	//Skills who's damage should't show any skill-animation.
	case SM_MAGNUM:
	case AS_SPLASHER:
	case ASC_METEORASSAULT:
//	case GS_SPREADATTACK: <- as it is, it shows no animation at all.
		dmg.dmotion = clif_skill_damage(dsrc,bl,tick,dmg.amotion,dmg.dmotion, damage, dmg.div_, skillid, -1, 5);
		break;
	case KN_BRANDISHSPEAR:
	case NJ_KAMAITACHI:
	case NJ_HUUMA:
		{	//Only display skill animation for skill's target.
			struct unit_data *ud = unit_bl2ud(src);
			if (ud && ud->skilltarget == bl->id)
				dmg.dmotion = clif_skill_damage(dsrc,bl,tick,dmg.amotion,dmg.dmotion, damage, dmg.div_, skillid, (lv!=0)?lv:skilllv, type);
			else
				dmg.dmotion = clif_damage(src,bl,tick,dmg.amotion,dmg.dmotion,damage,dmg.div_,8,dmg.damage2); // can't know why 8, but it works for all skills...
			break;
		}
	case PA_GOSPEL: //Should look like Holy Cross [Skotlex]
		dmg.dmotion = clif_skill_damage(dsrc,bl,tick,dmg.amotion,dmg.dmotion, damage, dmg.div_, CR_HOLYCROSS, -1, 5);
		break;

	case NPC_SELFDESTRUCTION:
		if(src->type==BL_PC)
			dmg.blewcount = 10;
		dmg.amotion = 0; //Disable delay or attack will do no damage since source is dead by the time it takes effect. [Skotlex]
	case KN_AUTOCOUNTER: //Skills that need be passed as a normal attack for the client to display correctly.
	case TF_DOUBLE:
	case GS_CHAINACTION:
	case SN_SHARPSHOOTING:
		dmg.dmotion = clif_damage(src,bl,tick,dmg.amotion,dmg.dmotion,damage,dmg.div_,dmg.type,dmg.damage2);
		break;
	case CR_GRANDCROSS:
	case NPC_GRANDDARKNESS:
		//Only show animation when hitting yourself. [Skotlex]
		if (src!=bl) {
			dmg.dmotion = clif_skill_damage(dsrc,bl,tick,dmg.amotion,dmg.dmotion, damage, dmg.div_, skillid, -1, 5);
			break;
		}
	default:
		dmg.dmotion = clif_skill_damage(dsrc,bl,tick,dmg.amotion,dmg.dmotion, damage, dmg.div_, skillid, (lv!=0)?lv:skilllv, (skillid==0)? 5:type );
	}
	
	map_freeblock_lock();

	if(damage > 0 && dmg.flag&BF_SKILL && tsd
		&& pc_checkskill(tsd,RG_PLAGIARISM)
	  	&& (!sc || sc->data[SC_PRESERVE].timer == -1) 
		&& damage < tsd->status.hp)
	{	//Updated to not be able to copy skills if the blow will kill you. [Skotlex]
		if ((!tsd->status.skill[skillid].id || tsd->status.skill[skillid].flag >= 13) &&
			can_copy(tsd,skillid))	// Split all the check into their own function [Aru]
		{
			if (tsd->cloneskill_id && tsd->status.skill[tsd->cloneskill_id].flag == 13){
				tsd->status.skill[tsd->cloneskill_id].id = 0;
				tsd->status.skill[tsd->cloneskill_id].lv = 0;
				tsd->status.skill[tsd->cloneskill_id].flag = 0;
			}
			tsd->cloneskill_id = skillid;
			tsd->status.skill[skillid].id = skillid;
			tsd->status.skill[skillid].lv = skilllv;
			if ((lv = pc_checkskill(tsd,RG_PLAGIARISM)) < skilllv)
				tsd->status.skill[skillid].lv = lv;
			tsd->status.skill[skillid].flag = 13;//cloneskill flag
			pc_setglobalreg(tsd, "CLONE_SKILL", tsd->cloneskill_id);
			pc_setglobalreg(tsd, "CLONE_SKILL_LV", tsd->status.skill[skillid].lv);
			clif_skillinfoblock(tsd);
		}
	}
	if (skillid != WZ_HEAVENDRIVE && bl->type == BL_SKILL && damage > 0) {
		struct skill_unit* su = (struct skill_unit*)bl;
		if (su->group && skill_get_inf2(su->group->skill_id)&INF2_TRAP)
			damage = 0; //Only Heaven's drive may damage traps. [Skotlex]
	}
	if (!dmg.amotion) {
		status_fix_damage(src,bl,damage,dmg.dmotion); //Deal damage before knockback to allow stuff like firewall+storm gust combo.
		if (dmg.dmg_lv == ATK_DEF || damage > 0) {
			if (!status_isdead(bl))
				skill_additional_effect(src,bl,skillid,skilllv,attack_type,tick);
			//Counter status effects [Skotlex] 
			skill_counter_additional_effect(dsrc,bl,skillid,skilllv,attack_type,tick);
		}
	}

	//Only knockback if it's still alive, otherwise a "ghost" is left behind. [Skotlex]
	if (dmg.blewcount > 0 && !status_isdead(bl))
		skill_blown(skillid==NJ_TATAMIGAESHI?src:dsrc,bl,dmg.blewcount);
	
	//Delayed damage must be dealt after the knockback (it needs to know actual position of target)
	if (dmg.amotion)
		battle_delay_damage(tick+dmg.amotion,src,bl,attack_type,skillid,skilllv,damage,dmg.dmg_lv,dmg.dmotion);

	if(skillid == RG_INTIMIDATE && damage > 0 && !(tstatus->mode&MD_BOSS)) {
		int rate = 50 + skilllv * 5;
		rate = rate + (status_get_lv(src) - status_get_lv(bl));
		if(rand()%100 < rate)
			skill_addtimerskill(src,tick + 800,bl->id,0,0,skillid,skilllv,0,flag);
	}

	if(sd && dmg.flag&BF_WEAPON && src != bl && src == dsrc && damage > 0) {
		if (battle_config.left_cardfix_to_right)
			battle_drain(sd, bl, dmg.damage, dmg.damage, tstatus->race, tstatus->mode&MD_BOSS);
		else
			battle_drain(sd, bl, dmg.damage, dmg.damage2, tstatus->race, tstatus->mode&MD_BOSS);
	}

	if (rdamage>0) {
		if (dmg.amotion)
			battle_delay_damage(tick+dmg.amotion,bl,src,0,0,0,rdamage,ATK_DEF,0);
		else
			status_fix_damage(bl,src,rdamage,0);
		clif_damage(src,src,tick, dmg.amotion,0,rdamage,1,4,0);
		//Use Reflect Shield to signal this kind of skill trigger. [Skotlex]
		if (tsd && src != bl)
			battle_drain(tsd, src, rdamage, rdamage, sstatus->race, is_boss(src));
		skill_additional_effect(bl,src,CR_REFLECTSHIELD, 1,BF_WEAPON,tick);
	}

	if (!(flag & 1) &&
		(	
			skillid == MG_COLDBOLT || skillid == MG_FIREBOLT || skillid == MG_LIGHTNINGBOLT
		) &&
		(sc = status_get_sc(src)) &&
		sc->count && sc->data[SC_DOUBLECAST].timer != -1 &&
		rand() % 100 < 40+10*sc->data[SC_DOUBLECAST].val1)
	{
//		skill_addtimerskill(src, tick + dmg.div_*dmg.amotion, bl->id, 0, 0, skillid, skilllv, BF_MAGIC, flag|1);
		skill_addtimerskill(src, tick + dmg.amotion, bl->id, 0, 0, skillid, skilllv, BF_MAGIC, flag|1);
	}

	map_freeblock_unlock();

	return damage;
}

/*==========================================
 * �X�L����??U?�p(map_foreachinarea����Ă΂��)
 * flag�ɂ���?F16?i?���m�F
 * MSB <- 00fTffff ->LSB
 *	T	=�^?�Q�b�g�I?�p(BCT_*)
 *  ffff=���R�Ɏg�p�\
 *  0	=�\��?B0�ɌŒ�
 *------------------------------------------
 */
static int skill_area_temp[8];
static int skill_unit_temp[8];	/* For storing skill_unit ids as players move in/out of them. [Skotlex] */
static int skill_unit_index=0;	//Well, yeah... am too lazy to pass pointers around :X
typedef int (*SkillFunc)(struct block_list *, struct block_list *, int, int, unsigned int, int);
int skill_area_sub (struct block_list *bl, va_list ap)
{
	struct block_list *src;
	int skill_id,skill_lv,flag;
	unsigned int tick;
	SkillFunc func;

	nullpo_retr(0, bl);
	nullpo_retr(0, ap);

	src=va_arg(ap,struct block_list *);
	skill_id=va_arg(ap,int);
	skill_lv=va_arg(ap,int);
	tick=va_arg(ap,unsigned int);
	flag=va_arg(ap,int);
	func=va_arg(ap,SkillFunc);

	if(battle_check_target(src,bl,flag) > 0)
		func(src,bl,skill_id,skill_lv,tick,flag);
	return 0;
}

static int skill_check_unit_range_sub (struct block_list *bl, va_list ap)
{
	struct skill_unit *unit;
	int skillid,g_skillid;

	unit = (struct skill_unit *)bl;

	if(bl->prev == NULL || bl->type != BL_SKILL)
		return 0;

	if(!unit->alive)
		return 0;

	skillid = va_arg(ap,int);
	g_skillid = unit->group->skill_id;

	switch (skillid)
	{
		case MG_SAFETYWALL:
		case AL_PNEUMA:
			if(g_skillid != MG_SAFETYWALL && g_skillid != AL_PNEUMA)
				return 0;
			break;
		case AL_WARP:
		case HT_SKIDTRAP:
		case HT_LANDMINE:
		case HT_ANKLESNARE:
		case HT_SHOCKWAVE:
		case HT_SANDMAN:
		case HT_FLASHER:
		case HT_FREEZINGTRAP:
		case HT_BLASTMINE:
		case HT_CLAYMORETRAP:
		case HT_TALKIEBOX:
		case HP_BASILICA:
			//Non stackable on themselves and traps (including venom dust which does not has the trap inf2 set)
			if (skillid != g_skillid && !(skill_get_inf2(g_skillid)&INF2_TRAP) && g_skillid != AS_VENOMDUST)
				return 0;
			break;
		default: //Avoid stacking with same kind of trap. [Skotlex]
			if (g_skillid != skillid)
				return 0;
			break;
	}

	return 1;
}

static int skill_check_unit_range (struct block_list *bl, int x, int y, int skillid, int skilllv)
{
	//Non players do not check for the skill's splash-trigger area.
	int range = bl->type==BL_PC?skill_get_unit_range(skillid, skilllv):0;
	int layout_type = skill_get_unit_layout_type(skillid,skilllv);
	if (layout_type==-1 || layout_type>MAX_SQUARE_LAYOUT) {
		ShowError("skill_check_unit_range: unsupported layout type %d for skill %d\n",layout_type,skillid);
		return 0;
	}

	range += layout_type;
	return map_foreachinarea(skill_check_unit_range_sub,bl->m,
			x-range,y-range,x+range,y+range,BL_SKILL,skillid);
}

static int skill_check_unit_range2_sub (struct block_list *bl, va_list ap)
{
	int skillid;

	if(bl->prev == NULL)
		return 0;

	if(status_isdead(bl))
		return 0;

	skillid = va_arg(ap,int);
	if (skillid==HP_BASILICA && bl->type==BL_PC)
		return 0;

	if (skillid==AM_DEMONSTRATION && bl->type==BL_MOB && ((TBL_MOB*)bl)->class_ == MOBID_EMPERIUM)
		return 0; //Allow casting Bomb/Demonstration Right under emperium [Skotlex]
	return 1;
}

static int skill_check_unit_range2 (struct block_list *bl, int x, int y, int skillid, int skilllv)
{
	int range, type;

	switch (skillid) {	// to be expanded later
	case WZ_ICEWALL:
		range = 2;
		break;
	default:
		{
			int layout_type = skill_get_unit_layout_type(skillid,skilllv);
			if (layout_type==-1 || layout_type>MAX_SQUARE_LAYOUT) {
				ShowError("skill_check_unit_range2: unsupported layout type %d for skill %d\n",layout_type,skillid);
				return 0;
			}
			range = skill_get_unit_range(skillid,skilllv) + layout_type;
		}
		break;
	}

	// if the caster is a monster/NPC, only check for players
	// otherwise just check characters
	if (bl->type == BL_PC)
		type = BL_CHAR;
	else
		type = BL_PC;

	return map_foreachinarea(skill_check_unit_range2_sub, bl->m,
		x - range, y - range, x + range, y + range,
		type, skillid);
}

int skill_guildaura_sub (struct block_list *bl, va_list ap)
{
	struct map_session_data *sd;
	int gid, id, strvit, agidex;
	
	sd = (struct map_session_data *)bl;

	id = va_arg(ap,int);
	gid = va_arg(ap,int);
	if (sd->status.guild_id != gid)
		return 0;
	strvit = va_arg(ap,int);
	agidex = va_arg(ap,int);

	if (sd->sc.count && sd->sc.data[SC_GUILDAURA].timer != -1) {
		if (sd->sc.data[SC_GUILDAURA].val3 != strvit ||
			sd->sc.data[SC_GUILDAURA].val4 != agidex) {
			sd->sc.data[SC_GUILDAURA].val3 = strvit;
			sd->sc.data[SC_GUILDAURA].val4 = agidex;
			status_calc_bl(&sd->bl, StatusChangeFlagTable[SC_GUILDAURA]);
		}
		return 0;
	}
	sc_start4(&sd->bl, SC_GUILDAURA,100, 1, id, strvit, agidex, 1000);
	return 1;
}

/*==========================================
 * [orn]
 * Checks that you have the requirements for casting a skill for homunculus.
 * Flag:
 * &1: finished casting the skill (invoke hp/sp/item consumption)
 * &2: picked menu entry (Warp Portal, Teleport and other menu based skills)
 *------------------------------------------
 */
static int skill_check_condition_hom (struct homun_data *hd, int skill, int lv, int type)
{
	struct status_data *status;
	struct status_change *sc;
	int j,hp,sp,hp_rate,sp_rate,state,mhp ;

	nullpo_retr(0, hd);

	if (lv <= 0) return 0;

	status = &hd->battle_status;
	sc = &hd->sc;
	if (!sc->count)
		sc = NULL;
	
	// for the guild skills [celest]
	if (skill >= HM_SKILLBASE)	//[orn]
		j = HM_SKILLRANGEMIN + skill - HM_SKILLBASE;
	else
		j = skill;
	if (j < 0 || j >= MAX_SKILL_DB)
  		return 0;
	//Code speedup, rather than using skill_get_* over and over again.
	if (lv < 1 || lv > MAX_SKILL_LEVEL)
		return 0;
	hp = skill_db[j].hp[lv-1];
	sp = skill_db[j].sp[lv-1];
	hp_rate = skill_db[j].hp_rate[lv-1];
	sp_rate = skill_db[j].sp_rate[lv-1];
	state = skill_db[j].state;
	mhp = skill_db[j].mhp[lv-1];
	if(mhp > 0)
		hp += (status->max_hp * mhp)/100;
	if(hp_rate > 0)
		hp += (status->hp * hp_rate)/100;
	else
		hp += (status->max_hp * (-hp_rate))/100;
	if(sp_rate > 0)
		sp += (status->sp * sp_rate)/100;
	else
		sp += (status->max_sp * (-sp_rate))/100;

	switch(skill) { // Check for cost reductions due to skills & SCs
		case HFLI_SBR44:
			if(hd->master->homunculus.intimacy < 200)
				return 0;
			break;
		case HVAN_EXPLOSION:
			if(hd->master->homunculus.intimacy < battle_config.hvan_explosion_intimate)
				return 0;
			break;
	}
	if(!(type&2)){
		if( hp>0 && status->hp <= (unsigned int)hp) {
			clif_skill_fail(hd->master,skill,2,0);
			return 0;
		}
		if( sp>0 && status->sp < (unsigned int)sp) {
			clif_skill_fail(hd->master,skill,1,0);
			return 0;
		}
	}

	switch(state) {
	case ST_MOVE_ENABLE:
		//Check only on begin casting. [Skotlex]
		if(!type && !unit_can_move(&hd->bl)) {
			clif_skill_fail(hd->master,skill,0,0);
			return 0;
		}
		break;
	}

	if(!(type&1))
		return 1;

	if(type&2)
		return 1;

	if(sp || hp)
		status_zap(&hd->bl, hp, sp);

	return 1;
}

/*=========================================================================
 *
 */
int skill_area_sub_count (struct block_list *src, struct block_list *target, int skillid, int skilllv, unsigned int tick, int flag)
{
	if(skill_area_temp[0] < 0xffff)
		skill_area_temp[0]++;
	return 1;
}

int skill_count_water (struct block_list *src, int range)
{
	int i,x,y,cnt = 0,size = range*2+1;
	struct skill_unit *unit;

	for (i=0;i<size*size;i++) {
		x = src->x+(i%size-range);
		y = src->y+(i/size-range);
		if (map_getcell(src->m,x,y,CELL_CHKWATER)) {
			cnt++;
			continue;
		}
		unit = map_find_skill_unit_oncell(src,x,y,SA_DELUGE,NULL);
		if (!unit)
		  unit = map_find_skill_unit_oncell(src,x,y,NJ_SUITON,NULL);
		if (unit) {
			cnt++;
			skill_delunit(unit);
		}
	}
	return cnt;
}

/*==========================================
 *
 *------------------------------------------
 */
static int skill_timerskill (int tid, unsigned int tick, int id, int data)
{
	struct block_list *src = map_id2bl(id),*target;
	struct unit_data *ud = unit_bl2ud(src);
	struct skill_timerskill *skl = NULL;
	int range;

	nullpo_retr(0, src);
	nullpo_retr(0, ud);
	skl = ud->skilltimerskill[data];
	nullpo_retr(0, skl);
	ud->skilltimerskill[data] = NULL;

	do {
		if(src->prev == NULL)
			break;
		if(skl->target_id) {
			target = map_id2bl(skl->target_id);
			if(!target && skl->skill_id == RG_INTIMIDATE)
				target = src; //Required since it has to warp.
			if(target == NULL)
				break;	
			if(target->prev == NULL)
				break;
			if(src->m != target->m)
				break;
			if(status_isdead(src))
				break;
			if(status_isdead(target) && skl->skill_id != RG_INTIMIDATE && skl->skill_id != WZ_WATERBALL)
				break;

			switch(skl->skill_id) {
				case RG_INTIMIDATE:
					if (unit_warp(src,-1,-1,-1,3) == 0) {
						short x,y;
						map_search_freecell(src, 0, &x, &y, 1, 1, 0);
						if (target != src && !status_isdead(target))
							unit_warp(target, -1, x, y, 3);
					}
					break;
				case BA_FROSTJOKE:
				case DC_SCREAM:
					range= skill_get_splash(skl->skill_id, skl->skill_lv);
					map_foreachinarea(skill_frostjoke_scream,skl->map,skl->x-range,skl->y-range,
						skl->x+range,skl->y+range,BL_CHAR,src,skl->skill_id,skl->skill_lv,tick);
					break;

				case WZ_WATERBALL:
					if (!status_isdead(target))
						skill_attack(BF_MAGIC,src,src,target,skl->skill_id,skl->skill_lv,tick,skl->flag);
					if (skl->type>1 && !status_isdead(target)) {
						skill_addtimerskill(src,tick+125,target->id,0,0,skl->skill_id,skl->skill_lv,skl->type-1,skl->flag);
					} else {
						struct status_change *sc = status_get_sc(src);
						if(sc && sc->data[SC_MAGICPOWER].timer != -1)
							status_change_end(src,SC_MAGICPOWER,-1);
					}
					break;
				default:
					skill_attack(skl->type,src,src,target,skl->skill_id,skl->skill_lv,tick,skl->flag);
					break;
			}
		}
		else {
			if(src->m != skl->map)
				break;
			switch(skl->skill_id) {
				case WZ_METEOR:
					if(skl->type >= 0) {
						skill_unitsetting(src,skl->skill_id,skl->skill_lv,skl->type>>16,skl->type&0xFFFF,skl->flag);
						clif_skill_poseffect(src,skl->skill_id,skl->skill_lv,skl->x,skl->y,tick);
					}
					else
						skill_unitsetting(src,skl->skill_id,skl->skill_lv,skl->x,skl->y,skl->flag);
					break;
			}
		}
	} while (0);
	//Free skl now that it is no longer needed.
	ers_free(skill_timer_ers, skl);
	return 0;
}

/*==========================================
 *
 *------------------------------------------
 */
int skill_addtimerskill (struct block_list *src, unsigned int tick, int target, int x,int y, int skill_id, int skill_lv, int type, int flag)
{
	int i;
	struct unit_data *ud;
	nullpo_retr(1, src);
	ud = unit_bl2ud(src);
	nullpo_retr(1, ud);
	
	for(i=0;i<MAX_SKILLTIMERSKILL && ud->skilltimerskill[i]; i++);
	if (i==MAX_SKILLTIMERSKILL) return 1;
	
	ud->skilltimerskill[i] = ers_alloc(skill_timer_ers, struct skill_timerskill);
	ud->skilltimerskill[i]->timer = add_timer(tick, skill_timerskill, src->id, i);
	ud->skilltimerskill[i]->src_id = src->id;
	ud->skilltimerskill[i]->target_id = target;
	ud->skilltimerskill[i]->skill_id = skill_id;
	ud->skilltimerskill[i]->skill_lv = skill_lv;
	ud->skilltimerskill[i]->map = src->m;
	ud->skilltimerskill[i]->x = x;
	ud->skilltimerskill[i]->y = y;
	ud->skilltimerskill[i]->type = type;
	ud->skilltimerskill[i]->flag = flag;
	return 0;
}

/*==========================================
 *
 *------------------------------------------
 */
int skill_cleartimerskill (struct block_list *src)
{
	int i;
	struct unit_data *ud;
	nullpo_retr(0, src);
	ud = unit_bl2ud(src);
	nullpo_retr(0, ud);
		
	for(i=0;i<MAX_SKILLTIMERSKILL;i++) {
		if(ud->skilltimerskill[i]) {
			delete_timer(ud->skilltimerskill[i]->timer, skill_timerskill);
			ers_free(skill_timer_ers, ud->skilltimerskill[i]);
			ud->skilltimerskill[i]=NULL;
		}
	}
	return 1;
}

static int skill_reveal_trap (struct block_list *bl, va_list ap)
{
	TBL_SKILL *su = (TBL_SKILL*)bl;
	if (su->alive && su->group && skill_get_inf2(su->group->skill_id)&INF2_TRAP)
	{	//Reveal trap.
		//Change look is not good enough, the client ignores it as an actual trap still. [Skotlex]
		//clif_changetraplook(bl, su->group->unit_id);
		clif_skill_setunit(su);
		return 1;
	}
	return 0;
}

/*==========================================
 *
 *
 *------------------------------------------
 */
int skill_castend_damage_id (struct block_list* src, struct block_list *bl, int skillid, int skilllv, unsigned int tick, int flag)
{
	struct map_session_data *sd = NULL, *tsd = NULL;
	struct status_data *tstatus;
	struct status_change *sc;

	if (skillid > 0 && skilllv <= 0) return 0;

	nullpo_retr(1, src);
	nullpo_retr(1, bl);	

	if (src->m != bl->m)
		return 1;

	if (bl->prev == NULL)
		return 1;
	
	if (src->type == BL_PC)
		sd = (struct map_session_data *)src;
	if (bl->type == BL_PC)
		tsd = (struct map_session_data *)bl;

	if (status_isdead(bl))
		return 1;

	if (skillid && skill_get_type(skillid) == BF_MAGIC && status_isimmune(bl))
	{	//GTB makes all targetted magic fail silently.
		if (sd) clif_skill_fail(sd,skillid,0,0);
		return 1;
	}
	
	sc = status_get_sc(src);	
	if (sc && !sc->count)
		sc = NULL; //Unneeded
	
	tstatus = status_get_status_data(bl);
	
	map_freeblock_lock();

	switch(skillid)
	{
	case SM_BASH:
	case MC_MAMMONITE:
	case TF_DOUBLE:
	case AC_DOUBLE:
	case AS_SONICBLOW:
	case KN_PIERCE:
	case KN_SPEARBOOMERANG:
	case KN_BRANDISHSPEAR:
	case TF_POISON:
	case TF_SPRINKLESAND:
	case AC_CHARGEARROW:
	case RG_RAID:
	case RG_INTIMIDATE:
	case AM_ACIDTERROR:
	case BA_MUSICALSTRIKE:
	case DC_THROWARROW:
	case BA_DISSONANCE:
	case CR_HOLYCROSS:
	case NPC_DARKCROSS:
	case CR_SHIELDCHARGE:
	case CR_SHIELDBOOMERANG:
	case NPC_PIERCINGATT:
	case NPC_MENTALBREAKER:
	case NPC_RANGEATTACK:
	case NPC_CRITICALSLASH:
	case NPC_COMBOATTACK:
	case NPC_GUIDEDATTACK:
	case NPC_POISON:
	case NPC_BLINDATTACK:
	case NPC_SILENCEATTACK:
	case NPC_STUNATTACK:
	case NPC_PETRIFYATTACK:
	case NPC_CURSEATTACK:
	case NPC_SLEEPATTACK:
	case NPC_RANDOMATTACK:
	case NPC_WATERATTACK:
	case NPC_GROUNDATTACK:
	case NPC_FIREATTACK:
	case NPC_WINDATTACK:
	case NPC_POISONATTACK:
	case NPC_HOLYATTACK:
	case NPC_DARKNESSATTACK:
	case NPC_TELEKINESISATTACK:
	case NPC_UNDEADATTACK:
	case NPC_BREAKARMOR:
	case NPC_BREAKWEAPON:
	case NPC_BREAKHELM:
	case NPC_BREAKSHIELD:
	case LK_AURABLADE:
	case LK_SPIRALPIERCE:
	case LK_HEADCRUSH:
	case LK_JOINTBEAT:
	case CG_ARROWVULCAN:
	case HW_MAGICCRASHER:
	case ASC_METEORASSAULT:
	case ITM_TOMAHAWK:
	case MO_TRIPLEATTACK:
	case CH_CHAINCRUSH:
	case CH_TIGERFIST:
	case PA_SHIELDCHAIN:	// Shield Chain
	case PA_SACRIFICE:
	case WS_CARTTERMINATION:	// Cart Termination
	case AS_VENOMKNIFE:
	case HT_PHANTASMIC:
	case HT_POWER:
	case TK_DOWNKICK:
	case TK_COUNTER:
	case GS_TRIPLEACTION:
	case GS_MAGICALBULLET:
	case GS_CRACKER:
	case GS_TRACKING:
	case GS_PIERCINGSHOT:
	case GS_RAPIDSHOWER:
	case GS_DUST:
	case GS_FULLBUSTER:
	case NJ_SYURIKEN:
	case NJ_KUNAI:
	case ASC_BREAKER:
	case HFLI_MOON:	//[orn]
	case HFLI_SBR44:	//[orn]
		skill_attack(BF_WEAPON,src,src,bl,skillid,skilllv,tick,flag);
		break;

	case MO_COMBOFINISH:
		if (!(flag&1) && sc && sc->data[SC_SPIRIT].timer != -1 && sc->data[SC_SPIRIT].val2 == SL_MONK)
		{	//Becomes a splash attack when Soul Linked.
			map_foreachinrange(skill_area_sub, bl,
				skill_get_splash(skillid, skilllv),BL_CHAR,
				src,skillid,skilllv,tick, flag|BCT_ENEMY|1,
				skill_castend_damage_id);
		} else
			skill_attack(BF_WEAPON,src,src,bl,skillid,skilllv,tick,flag);
		break;

	case TK_STORMKICK: // Taekwon kicks [Dralnu]
		clif_skill_nodamage(src,bl,skillid,skilllv,1);
		map_foreachinrange(skill_attack_area, src,
			skill_get_splash(skillid, skilllv), BL_CHAR,
			BF_WEAPON, src, src, skillid, skilllv, tick, flag, BCT_ENEMY);	
		break;

	case TK_JUMPKICK:
		skill_attack(BF_WEAPON,src,src,bl,skillid,skilllv,tick,flag);
		if (unit_movepos(src, bl->x, bl->y, 0, 0))
			clif_slide(src,bl->x,bl->y);
		break;
	
	case SN_SHARPSHOOTING:
	case NJ_KAMAITACHI:
		//It won't shoot through walls since on castend there has to be a direct
		//line of sight between caster and target.
		map_foreachinpath (skill_attack_area,src->m,src->x,src->y,bl->x,bl->y,
			skill_get_splash(skillid, skilllv),BL_CHAR,
			skill_get_type(skillid),src,src,skillid,skilllv,tick,flag,BCT_ENEMY);
		break;

	case MO_INVESTIGATE:
		skill_attack(BF_WEAPON,src,src,bl,skillid,skilllv,tick,flag);
		if (sc && sc->data[SC_BLADESTOP].timer != -1)
			status_change_end(src,SC_BLADESTOP,-1);
		break;

	case RG_BACKSTAP:
		{
			int dir = map_calc_dir(src, bl->x, bl->y), t_dir = unit_getdir(bl);
			if ((!check_distance_bl(src, bl, 0) && !map_check_dir(dir, t_dir)) || bl->type == BL_SKILL) {
				if (sc && sc->data[SC_HIDING].timer != -1)
					status_change_end(src, SC_HIDING, -1);
				skill_attack(BF_WEAPON, src, src, bl, skillid, skilllv, tick, flag);
				dir = dir < 4 ? dir+4 : dir-4; // change direction [Celest]
				unit_setdir(bl,dir);
			}
			else if (sd)
				clif_skill_fail(sd,skillid,0,0);
		}
		break;

	case MO_FINGEROFFENSIVE:
		{
			skill_attack(BF_WEAPON,src,src,bl,skillid,skilllv,tick,flag);
			if (battle_config.finger_offensive_type && sd) {
				int i;
				for (i = 1; i < sd->spiritball_old; i++)
					skill_addtimerskill(src, tick + i * 200, bl->id, 0, 0, skillid, skilllv, BF_WEAPON, flag);
//				sd->canmove_tick = tick + (sd->spiritball_old - 1) * 200; Should be handled by the canmove delay on skill_cast_db [Skotlex]
			}
			if (sc && sc->data[SC_BLADESTOP].timer != -1)
				status_change_end(src,SC_BLADESTOP,-1);
		}
		break;

	case MO_CHAINCOMBO:
		{
			skill_attack(BF_WEAPON,src,src,bl,skillid,skilllv,tick,flag);
			if (sc && sc->data[SC_BLADESTOP].timer != -1)
				status_change_end(src,SC_BLADESTOP,-1);
		}
		break;
	
	case MO_EXTREMITYFIST:
		if (sc && sc->count)
		{
			if (sc->data[SC_EXPLOSIONSPIRITS].timer != -1)
				status_change_end(src, SC_EXPLOSIONSPIRITS, -1);
			if (sc->data[SC_BLADESTOP].timer != -1)
				status_change_end(src,SC_BLADESTOP,-1);
		}
	case KN_CHARGEATK:
		if(!check_distance_bl(src, bl, 2)) { //Need to move to target.
			int dx,dy;

			dx = bl->x - src->x;
			dy = bl->y - src->y;
			if(dx > 0) dx++;
			else if(dx < 0) dx--;
			if (dy > 0) dy++;
			else if(dy < 0) dy--;

			if (skillid == KN_CHARGEATK) //Store distance in flag [Skotlex]
				flag = distance_bl(src, bl);
			
			if (!unit_movepos(src, src->x+dx, src->y+dy, 1, 1)) {
				if (sd) clif_skill_fail(sd,skillid,0,0);
				break;
			}
			clif_slide(src,src->x,src->y);
		} else //Assume minimum distance of 1 for Charge.
			flag = 1;
		skill_attack(BF_WEAPON,src,src,bl,skillid,skilllv,tick,flag);
		break;

	//Splash attack skills.
	case AS_SPLASHER:
		if ((flag&1) && bl->id == skill_area_temp[1])
		{	//Should do 100% damage on targetted character.
			skill_attack(skill_get_type(skillid), src, src, bl, skillid, skilllv, tick, 0);
			break;
		}
	case AS_GRIMTOOTH:
	case MC_CARTREVOLUTION:	
	case NPC_SPLASHATTACK:
	case AC_SHOWER:	
	case MG_NAPALMBEAT:
	case MG_FIREBALL:
	case HW_NAPALMVULCAN:
	case NJ_HUUMA:
	case NJ_BAKUENRYU:
		if (flag&1) //Invoked from map_foreachinarea, skill_area_temp[0] holds number of targets to divide damage by.
			skill_attack(skill_get_type(skillid), src, src, bl, skillid, skilllv, tick, skill_area_temp[0]);
		else {
			if ( skillid == NJ_BAKUENRYU ) clif_skill_nodamage(src,bl,skillid,skilllv,1);
			skill_area_temp[0] = 0;
			skill_area_temp[1] = bl->id;
			if (skill_get_nk(skillid)&NK_SPLASHSPLIT)
				map_foreachinrange(skill_area_sub, bl, 
					skill_get_splash(skillid, skilllv), BL_CHAR,
					src, skillid, skilllv, tick, BCT_ENEMY, skill_area_sub_count);
			else if (skillid==AS_SPLASHER) //Need split damage anyway.
				skill_area_temp[0] = 2;

			map_foreachinrange(skill_area_sub, bl,
				skill_get_splash(skillid, skilllv), BL_CHAR,
				src, skillid, skilllv, tick, flag|BCT_ENEMY|1,
				skill_castend_damage_id);
		}
		break;

	//Done apart because you can't mix the flag with BCT_ENEMY for auto-blitz.
	case HT_BLITZBEAT:
		if (flag&1) //Warning, 0x100000 is currently BCT_NEUTRAL, so don't mix it when asking for the enemy. [Skotlex]
			skill_attack(skill_get_type(skillid), src, src, bl, skillid, skilllv, tick, skill_area_temp[0]);
		else {
			skill_area_temp[0] = 0;
			skill_area_temp[1] = bl->id;
			//Warning, 0x100000 is currently BCT_NEUTRAL, so don't mix it when asking for the enemy. [Skotlex]
			if (skillid==HT_BLITZBEAT && flag&0xf00000)
				map_foreachinrange(skill_area_sub, bl, 
						skill_get_splash(skillid, skilllv), BL_CHAR,
						src, skillid, skilllv, tick, BCT_ENEMY, skill_area_sub_count);
			else
				skill_area_temp[0] = 1;
			map_foreachinrange(skill_area_sub, bl,
				skill_get_splash(skillid, skilllv), BL_CHAR,
				src, skillid, skilllv, tick, BCT_ENEMY|1,
				skill_castend_damage_id);
		}
		break;
	case SM_MAGNUM:
		if(flag&1)
			skill_attack(BF_WEAPON,src,src,bl,skillid,skilllv,tick,flag);
		else {
			//If we get here, someone changed magnum to be a enemy targetted skill,
			//so treat it as such.
			map_foreachinrange(skill_area_sub, bl,
				skill_get_splash(skillid, skilllv),BL_CHAR,
				src,skillid,skilllv,tick, flag|BCT_ENEMY|1,
				skill_castend_damage_id);
			//Initiate 10% of your damage becomes fire element.
			clif_skill_nodamage (src,bl,skillid,skilllv,1);
			sc_start4(src,SC_WATK_ELEMENT,100,3,20,0,0,skill_get_time2(skillid, skilllv));
			if (sd) skill_blockpc_start (sd, skillid, skill_get_time(skillid, skilllv));
		}
		break;

	case KN_BOWLINGBASH:
		if(flag&1){
			if(bl->id==skill_area_temp[1])
				break;
			//Splash damage is always two hits for 500%
			skill_attack(BF_WEAPON,src,src,bl,skillid,skilllv,tick,0x0500);
			skill_attack(BF_WEAPON,src,src,bl,skillid,skilllv,tick,0x0500);
		} else {
			int i,c;
			c = skill_get_blewcount(skillid,skilllv);
			for(i=0;i<c;i++){
				if (!skill_blown(src,bl,0x20000|1))
					break; //Can't knockback
				skill_area_temp[0]=0;
				map_foreachinrange(skill_area_sub,bl,
					skill_get_splash(skillid, skilllv),BL_CHAR,
					src,skillid,skilllv,tick, flag|BCT_ENEMY,
					skill_area_sub_count);
				if(skill_area_temp[0]>1) break;
			}
			clif_blown(bl); //Update target pos.
			if (i==c) { //No targets found. Single attack for 600%
				skill_attack(BF_WEAPON,src,src,bl,skillid,skilllv,tick,1);
			} else {
				skill_area_temp[1]=bl->id;
				map_foreachinrange(skill_area_sub,bl,
					skill_get_splash(skillid, skilllv),BL_CHAR,
					src,skillid,skilllv,tick, flag|BCT_ENEMY|1,
					skill_castend_damage_id);
				//Weirdo dual-hit property, two attacks for 500%
				skill_attack(BF_WEAPON,src,src,bl,skillid,skilllv,tick,0);
				skill_attack(BF_WEAPON,src,src,bl,skillid,skilllv,tick,0);
			} 
		}
		break;
	
	case KN_SPEARSTAB:
		if(flag&1){
			if (bl->id==skill_area_temp[1])
				break;
			if (skill_attack(BF_WEAPON,src,src,bl,skillid,skilllv,tick,0x0500))
				skill_blown(src,bl,skill_area_temp[2]);
		} else {
			int x=bl->x,y=bl->y,i,dir;
			dir = map_calc_dir(bl,src->x,src->y);
			skill_area_temp[1] = bl->id;
			skill_area_temp[2] = skill_get_blewcount(skillid,skilllv)|dir<<20;
			if (skill_attack(BF_WEAPON,src,src,bl,skillid,skilllv,tick,0))
				skill_blown(src,bl,skill_area_temp[2]);
			for (i=0;i<4;i++) {
				map_foreachincell(skill_area_sub,bl->m,x,y,BL_CHAR,
					src,skillid,skilllv,tick,flag|BCT_ENEMY|1,
					skill_castend_damage_id);
				x += dirx[dir];
				y += diry[dir];
			}
		}
		break;

	case TK_TURNKICK:
	case MO_BALKYOUNG: //Active part of the attack. Skill-attack [Skotlex]
	{
		skill_area_temp[1] = bl->id; //NOTE: This is used in skill_castend_nodamage_id to avoid affecting the target.
		if (skill_attack(BF_WEAPON,src,src,bl,skillid,skilllv,tick,0))
			map_foreachinrange(skill_area_sub,bl,
				skill_get_splash(skillid, skilllv),BL_CHAR,
				src,skillid,skilllv,tick,flag|BCT_ENEMY|1,
				skill_castend_nodamage_id);
	}
		break;	
	case CH_PALMSTRIKE: //	Palm Strike takes effect 1sec after casting. [Skotlex]
	//	clif_skill_nodamage(src,bl,skillid,skilllv,0); //Can't make this one display the correct attack animation delay :/
		clif_damage(src,bl,tick,status_get_amotion(src),0,0,1,4,0); //Displays MISS, but better than nothing :X
		skill_addtimerskill(src, tick + 1000, bl->id, 0, 0, skillid, skilllv, BF_WEAPON, flag);
		break;	

	case PR_TURNUNDEAD:
	case ALL_RESURRECTION:
		if (!battle_check_undead(tstatus->race, tstatus->def_ele))
			break;
		skill_attack(BF_MAGIC,src,src,bl,skillid,skilllv,tick,flag);
		break;

	case MG_SOULSTRIKE:
	case NPC_DARKSTRIKE:
	case MG_COLDBOLT:
	case MG_FIREBOLT:
	case MG_LIGHTNINGBOLT:
	case WZ_EARTHSPIKE:
	case AL_HEAL:
	case AL_HOLYLIGHT:
	case WZ_JUPITEL:
	case NPC_DARKTHUNDER:
	case NPC_MAGICALATTACK:
	case PR_ASPERSIO:
	case MG_FROSTDIVER:
	case WZ_SIGHTBLASTER:
	case WZ_SIGHTRASHER:
	case NJ_KOUENKA:
	case NJ_HYOUSENSOU:
	case NJ_HUUJIN:
		skill_attack(BF_MAGIC,src,src,bl,skillid,skilllv,tick,flag);
		break;

	case HVAN_CAPRICE: //[blackhole89]
		{
			int ran=rand()%4;
			int sid = 0;
			switch(ran)
			{
			case 0: sid=MG_COLDBOLT; break;
			case 1: sid=MG_FIREBOLT; break;
			case 2: sid=MG_LIGHTNINGBOLT; break;
			case 3: sid=WZ_EARTHSPIKE; break;
			}
			skill_attack(BF_MAGIC,src,src,bl,sid,skilllv,tick,flag);
		}
		break;
	case WZ_WATERBALL:			/* ウォーターボール */
		skill_attack(BF_MAGIC,src,src,bl,skillid,skilllv,tick,flag);
		if (skilllv>1) {
			int range = skilllv/2;
			int cnt;

		  	if (sd)
				cnt = skill_count_water(src,range);
			else {
				range = 2*range+1;
				cnt = range*range;
			}
			cnt--;
			if (cnt > 0)
				skill_addtimerskill(src,tick+125,bl->id,0,0,
					skillid,skilllv,cnt,flag);
		} else if (sd) //Eat up deluge tiles.
			skill_count_water(src,0);

		break;

	case PR_BENEDICTIO:
		//Should attack undead and demons. [Skotlex]
		if (battle_check_undead(tstatus->race, tstatus->def_ele) || tstatus->race == RC_DEMON)
			skill_attack(BF_MAGIC, src, src, bl, skillid, skilllv, tick, flag);
	break;

	case SL_STIN:
	case SL_STUN:
	case SL_SMA:
		if (sd && !battle_config.allow_es_magic_pc && bl->type != BL_MOB) {
			status_change_start(src,SC_STUN,10000,skilllv,0,0,0,500,10);
			clif_skill_fail(sd,skillid,0,0);
			break;
		}
		skill_attack(BF_MAGIC,src,src,bl,skillid,skilllv,tick,flag);
		break;

	case NPC_DARKBREATH:
		clif_emotion(src,7);
	case SN_FALCONASSAULT:
	case PA_PRESSURE:
	case CR_ACIDDEMONSTRATION:
	case TF_THROWSTONE:
	case NPC_SMOKING:
	case HVAN_EXPLOSION:	//[orn]
	case GS_FLING:
	case NJ_ZENYNAGE:
		skill_attack(BF_MISC,src,src,bl,skillid,skilllv,tick,flag);
		break;

	case NPC_SELFDESTRUCTION:
		if (src != bl)
			skill_attack(BF_MISC,src,src,bl,skillid,skilllv,tick,flag);
		break;

	// Celest
	case PF_SOULBURN:
		if (rand()%100 < (skilllv < 5 ? 30 + skilllv * 10 : 70)) {
			clif_skill_nodamage(src,bl,skillid,skilllv,1);
			if (skilllv == 5)
				skill_attack(BF_MAGIC,src,src,bl,skillid,skilllv,tick,0 );
			status_percent_damage(src, bl, 0, 100);
		} else {
			clif_skill_nodamage(src,src,skillid,skilllv,1);
			if (skilllv == 5)
				skill_attack(BF_MAGIC,src,src,src,skillid,skilllv,tick,0 );
			status_percent_damage(src, src, 0, 100);
		}		
		if (sd) skill_blockpc_start (sd, skillid, (skilllv < 5 ? 10000: 15000));
		break;

	case NPC_BLOODDRAIN:
	case NPC_ENERGYDRAIN:
		{
			int heal = skill_attack( (skillid == NPC_BLOODDRAIN) ? BF_WEAPON : BF_MAGIC,
					src, src, bl, skillid, skilllv, tick, flag);
			if (heal > 0){
				clif_skill_nodamage(NULL, src, AL_HEAL, heal, 1);
				status_heal(src, heal, 0, 0);
			}
		}
		break;

	case GS_BULLSEYE:
		if(tstatus->race == RC_BRUTE || tstatus->race == RC_DEMIHUMAN)
			skill_attack(BF_WEAPON,src,src,bl,skillid,skilllv,tick,flag);
		else if (sd)
			clif_skill_fail(sd,skillid,0,0);
		break;
	case GS_DESPERADO:
	case GS_SPREADATTACK:
		if(flag&1)
			skill_attack(BF_WEAPON,src,src,bl,skillid,skilllv,tick,flag);
		else {
			//If we get here, someone changed it to be a enemy targetted skill,
			//so treat it as such.
			map_foreachinrange(skill_area_sub, bl,
				skill_get_splash(skillid, skilllv),BL_CHAR,
				src,skillid,skilllv,tick, flag|BCT_ENEMY|1,
				skill_castend_damage_id);
		}
		break;

	case NJ_KASUMIKIRI:
		skill_attack(BF_WEAPON,src,src,bl,skillid,skilllv,tick,flag);
		sc_start(src,SC_HIDING,100,skilllv,skill_get_time(skillid,skilllv));
		break;
	case NJ_KIRIKAGE:
		{
			int dir = map_calc_dir(src,bl->x,bl->y);
			status_change_end(src, SC_HIDING, -1);
			skill_attack(BF_WEAPON,src,src,bl,skillid,skilllv,tick,flag);
			if (unit_movepos(src, bl->x - dirx[dir], bl->y - diry[dir], 0, 0))
				clif_slide(src,src->x,src->y);
		}
		break;
	case NJ_ISSEN:
		skill_attack(BF_WEAPON,src,src,bl,skillid,skilllv,tick,flag);
		if (sc && sc->data[SC_NEN].timer != -1)
			status_change_end(src,SC_NEN,-1);
		break;
	case 0:
		if(sd) {
			if (flag & 3){
				if (bl->id != skill_area_temp[1])
					skill_attack(BF_WEAPON, src, src, bl, skillid, skilllv, tick, 0x0500);
			} else {
				skill_area_temp[1] = bl->id;
				map_foreachinrange(skill_area_sub, bl,
					sd->splash_range, BL_CHAR,
					src, skillid, skilllv, tick, flag | BCT_ENEMY | 1,
					skill_castend_damage_id);
			}
		}
		break;

	default:
		ShowWarning("skill_castend_damage_id: Unknown skill used:%d\n",skillid);
		map_freeblock_unlock();
		return 1;
	}

	map_freeblock_unlock();	

	if (sd && !(flag&1) && sd->state.arrow_atk) //Consume arrow on last invocation to this skill.
		battle_consume_ammo(sd, skillid, skilllv);
	return 0;
}

/*==========================================
 * 
 *------------------------------------------
 */
int skill_castend_nodamage_id (struct block_list *src, struct block_list *bl, int skillid, int skilllv, unsigned int tick, int flag)
{
	struct map_session_data *sd = NULL;
	struct homun_data *hd = NULL;
	struct map_session_data *dstsd = NULL;
	struct status_data *sstatus, *tstatus;
	struct status_change *tsc;
	struct mob_data *md = NULL;
	struct mob_data *dstmd = NULL;
	int i,type=-1;
	
	if(skillid > 0 && skilllv <= 0) return 0;	// celest

	nullpo_retr(1, src);
	nullpo_retr(1, bl);

	if (src->m != bl->m)
		return 1;

	if (src->type == BL_PC) {
		sd = (struct map_session_data *)src;
	} else if (src->type == BL_HOM) {	//[orn]
		hd = (struct homun_data *)src;
	} else if (src->type == BL_MOB) {
		md = (struct mob_data *)src;
	}

	if (bl->type == BL_PC){
		dstsd = (struct map_session_data *)bl;
	} else if (bl->type == BL_MOB){
		dstmd = (struct mob_data *)bl;
	}

	if(bl->prev == NULL)
		return 1;
	if(status_isdead(src))
		return 1;
	if(src!=bl && status_isdead(bl) && skillid != ALL_RESURRECTION && skillid != PR_REDEMPTIO)
		return 1;

	tstatus = status_get_status_data(bl);
	sstatus = status_get_status_data(src);

	//Check for undead skills that convert a no-damage skill into a damage one. [Skotlex]
	switch (skillid) {
		case HLIF_HEAL:	//[orn]
			if (bl->type != BL_HOM) {
				if (sd) clif_skill_fail(sd,skillid,0,0) ;
	        break ;
			}
 		case AL_HEAL:
		case ALL_RESURRECTION:
		case PR_ASPERSIO:
			//Apparently only player casted skills can be offensive like this.
			if (sd && battle_check_undead(tstatus->race,tstatus->def_ele)) {
				if (battle_check_target(src, bl, BCT_ENEMY) < 1) {
				  	//Offensive heal does not works on non-enemies. [Skotlex]
					clif_skill_fail(sd,skillid,0,0);
					return 0;
				}
				return skill_castend_damage_id (src, bl, skillid, skilllv, tick, flag);
			}
			break;
		case NPC_SMOKING: //Since it is a self skill, this one ends here rather than in damage_id. [Skotlex]
			return skill_castend_damage_id (src, bl, skillid, skilllv, tick, flag);
		default:
			//Skill is actually ground placed.
			if ((src == bl || skillid == PF_SPIDERWEB) && skill_get_unit_id(skillid,0))
				return skill_castend_pos2(src,bl->x,bl->y,skillid,skilllv,tick,0);
	}

	if (skillid > 0)
		type = SkillStatusChangeTable(skillid);
	
	tsc = status_get_sc(bl);

	map_freeblock_lock();
	switch(skillid)
	{
	case HLIF_HEAL:	//[orn]
	case AL_HEAL:
		{
			int heal = skill_calc_heal(src, skilllv);
			int heal_get_jobexp;
	
			if (status_isimmune(bl) || (dstmd && dstmd->class_ == MOBID_EMPERIUM))
				heal=0;
			if (sd) {
				if (sd && dstsd && sd->status.partner_id == dstsd->status.char_id &&
					(sd->class_&MAPID_UPPERMASK) == MAPID_SUPER_NOVICE && sd->status.sex == 0)
					heal = heal*2;
			}

			if (tsc && tsc->count && tsc->data[SC_KAITE].timer != -1 
				&& !(sstatus->mode&MD_BOSS)
			) { //Bounce back heal
				if (--tsc->data[SC_KAITE].val2 <= 0)
					status_change_end(bl, SC_KAITE, -1);
				if (src == bl) heal=0; //When you try to heal yourself and you are under Kaite, the heal is voided.
				clif_skill_nodamage (src, src, skillid, heal, 1);
				heal_get_jobexp = status_heal(src,heal,0,0);
			} else {
				clif_skill_nodamage (src, bl, skillid, heal, 1);
				heal_get_jobexp = status_heal(bl,heal,0,0);
			}

			if(sd && dstsd && heal > 0 && sd != dstsd && battle_config.heal_exp > 0){
				heal_get_jobexp = heal_get_jobexp * battle_config.heal_exp / 100;
				if (heal_get_jobexp <= 0)
					heal_get_jobexp = 1;
				pc_gainexp (sd, bl, 0, heal_get_jobexp);
			}
		}
		break;

	case PR_REDEMPTIO:
		if (sd && !(flag&1)) {
			if (sd->status.party_id == 0) {
				clif_skill_fail(sd,skillid,0,0);
				break;
			}
			skill_area_temp[0] = 0;
			party_foreachsamemap(skill_area_sub,
				sd,skill_get_splash(skillid, skilllv),
				src,skillid,skilllv,tick, flag|BCT_PARTY|1,
				skill_castend_nodamage_id);
			if (skill_area_temp[0] == 0) {
				clif_skill_fail(sd,skillid,0,0);
				break;
			}
			skill_area_temp[0] = 5 - skill_area_temp[0]; // The actual penalty...
			if (skill_area_temp[0] > 0 && !map[src->m].flag.nopenalty) { //Apply penalty
				sd->status.base_exp -= pc_nextbaseexp(sd) * skill_area_temp[0] * 2/1000; //0.2% penalty per each.
				sd->status.job_exp -= pc_nextjobexp(sd) * skill_area_temp[0] * 2/1000;
				clif_updatestatus(sd,SP_BASEEXP);
				clif_updatestatus(sd,SP_JOBEXP);
			}
			status_set_hp(src, 1, 0);
			status_set_sp(src, 0, 0);
			break;
		} else if (status_isdead(bl) && flag&1) { //Revive
			skill_area_temp[0]++; //Count it in, then fall-through to the Resurrection code.
			skilllv = 3; //Resurrection level 3 is used
		} else //Invalid target, skip resurrection.
			break;
		
	case ALL_RESURRECTION:
		if(sd && map_flag_gvg(bl->m))
		{	//No reviving in WoE grounds!
			clif_skill_fail(sd,skillid,0,0);
			break;
		}
		if (!status_isdead(bl))
			break;
		{	
			int per = 0, sper = 0;
			if (map[bl->m].flag.pvp && dstsd && dstsd->pvp_point < 0)
				break;

			switch(skilllv){
			case 1: per=10; break;
			case 2: per=30; break;
			case 3: per=50; break;
			case 4: per=80; break;
			}
			if(dstsd && dstsd->special_state.restart_full_recover) 
				per = sper = 100;
			if (status_revive(bl, per, sper))
			{
				clif_skill_nodamage(src,bl,ALL_RESURRECTION,skilllv,1); //Both Redemption and Res show this skill-animation.
				if(sd && dstsd && battle_config.resurrection_exp > 0) 
				{
					int exp = 0,jexp = 0;
					int lv = dstsd->status.base_level - sd->status.base_level, jlv = dstsd->status.job_level - sd->status.job_level;
					if(lv > 0) {
						exp = (int)((double)dstsd->status.base_exp * (double)lv * (double)battle_config.resurrection_exp / 1000000.);
						if (exp < 1) exp = 1;
					}
					if(jlv > 0) {
						jexp = (int)((double)dstsd->status.job_exp * (double)lv * (double)battle_config.resurrection_exp / 1000000.);
						if (jexp < 1) jexp = 1;
					}
					if(exp > 0 || jexp > 0)
						pc_gainexp (sd, bl, exp, jexp);
				}
			}
		}
		break;

	case AL_DECAGI:
		clif_skill_nodamage (src, bl, skillid, skilllv,
			sc_start(bl, type,
				(40 + skilllv * 2 + (status_get_lv(src) + sstatus->int_)/5),
				skilllv, skill_get_time(skillid,skilllv)));
		break;

	case AL_CRUCIS:
		if (flag & 1) {
			if (battle_check_target (src, bl, BCT_ENEMY))
				sc_start(bl,type,
					23+skilllv*4 +status_get_lv(src) -status_get_lv(bl),
					skilllv,60000);
		} else {
			map_foreachinrange(skill_area_sub, src,
				skill_get_splash(skillid, skilllv), BL_CHAR,
				src, skillid, skilllv, tick, flag|BCT_ENEMY|1,
				skill_castend_nodamage_id);
			clif_skill_nodamage(src, bl, skillid, skilllv, 1);
		}
		break;

	case PR_LEXDIVINA:
		if (tsc && tsc->count && tsc->data[type].timer != -1) {
			status_change_end(bl,type, -1);
			clif_skill_nodamage (src, bl, skillid, skilllv, 1);
		} else 
			clif_skill_nodamage (src, bl, skillid, skilllv, 
		  		sc_start(bl,type,100,skilllv,skill_get_time(skillid,skilllv)));
		break;

	case SA_ABRACADABRA:
		{
			int abra_skillid = 0, abra_skilllv;
			do {
				abra_skillid = rand() % MAX_SKILL_ABRA_DB;
				if (
//Unneeded check, use the "per" field to know if the skill is valid.
//					skill_get_inf2(abra_skillid)&(INF2_NPC_SKILL|INF2_SONG_DANCE|INF2_ENSEMBLE_SKILL) || //NPC/Song/Dance skills are out
					!skill_get_inf(abra_skillid) || //Passive skills cannot be casted
					skill_abra_db[abra_skillid].req_lv > skilllv || //Required lv for it to appear
					rand()%10000 >= skill_abra_db[abra_skillid].per
				)
					abra_skillid = 0;	// reset to get a new id
			} while (abra_skillid == 0);
			abra_skilllv = skill_get_max(abra_skillid) >  skilllv ? skilllv : skill_get_max(abra_skillid);
			clif_skill_nodamage (src, bl, skillid, skilllv, 1);
			
			if (sd)
			{	//Crash-protection against Abracadabra casting pets
				sd->skillitem = abra_skillid;
				sd->skillitemlv = abra_skilllv;
				sd->state.abra_flag = 1;
				clif_item_skill (sd, abra_skillid, abra_skilllv, "Abracadabra");
			} else
			{	// [Skotlex]
				struct unit_data *ud = unit_bl2ud(src);
				int inf = skill_get_inf(abra_skillid);
				int target_id = 0;
				if (!ud) break;
				if (inf&INF_SELF_SKILL || inf&INF_SUPPORT_SKILL) { 
					if (src->type == BL_PET)
						bl = (struct block_list*)((TBL_PET*)src)->msd;
					if (!bl) bl = src;
					unit_skilluse_id(src, bl->id, abra_skillid, abra_skilllv); 
				} else {	//Assume offensive skills
					if (ud->target)
						target_id = ud->target;
					else switch (src->type) {
						case BL_MOB:
							target_id = ((TBL_MOB*)src)->target_id;
							break;
						case BL_PET:
							target_id = ((TBL_PET*)src)->target_id;
							break;
					}
					if (!target_id)
						break;
					if (skill_get_casttype(abra_skillid) == CAST_GROUND) {
						bl = map_id2bl(target_id);
						if (!bl) bl = src;
						unit_skilluse_pos(src, bl->x, bl->y, abra_skillid, abra_skilllv); 
					} else
						unit_skilluse_id(src, target_id, abra_skillid, abra_skilllv);
				}
			}
		}
		break;

	case SA_COMA:
		clif_skill_nodamage(src,bl,skillid,skilllv,
			sc_start(bl,type,100,skilllv,skill_get_time2(skillid,skilllv)));
		break;
	case SA_FULLRECOVERY:
		clif_skill_nodamage(src,bl,skillid,skilllv,1);
		if (status_isimmune(bl))
			break;
		status_percent_heal(bl, 100, 100);
		break;
	case SA_SUMMONMONSTER:
		clif_skill_nodamage(src,bl,skillid,skilllv,1);
		if (sd) mob_once_spawn(sd,map[src->m].name,src->x,src->y,"--ja--",-1,1,"");
		break;
	case SA_LEVELUP:
		clif_skill_nodamage(src,bl,skillid,skilllv,1);
		if (sd && pc_nextbaseexp(sd)) pc_gainexp(sd, NULL, pc_nextbaseexp(sd) * 10 / 100, 0);
		break;
	case SA_INSTANTDEATH:
		clif_skill_nodamage(src,bl,skillid,skilllv,1);
		status_set_hp(bl,1,0);
		break;
	case SA_QUESTION:
	case SA_GRAVITY:
		clif_skill_nodamage(src,bl,skillid,skilllv,1);
		break;
	case SA_CLASSCHANGE:
		{
			static int changeclass[]={1038,1039,1046,1059,1086,1087,1112,1115
				,1157,1159,1190,1272,1312,1373,1492};
			int class_ = mob_random_class (changeclass,sizeof(changeclass)/sizeof(changeclass[0]));
			clif_skill_nodamage(src,bl,skillid,skilllv,1);
			if(dstmd) mob_class_change(dstmd,class_);
		}
		break;
	case SA_MONOCELL:
		{
			static int poringclass[]={1002};
			int class_ = mob_random_class (poringclass,sizeof(poringclass)/sizeof(poringclass[0]));
			clif_skill_nodamage(src,bl,skillid,skilllv,1);
			if(dstmd) mob_class_change(dstmd,class_);
		}
		break;
	case SA_DEATH:
		clif_skill_nodamage(src,bl,skillid,skilllv,1);
		status_kill(bl);
		break;
	case SA_REVERSEORCISH:
		clif_skill_nodamage(src,bl,skillid,skilllv,
			sc_start(bl,type,100,skilllv,skill_get_time(skillid, skilllv)));
		break;
	case SA_FORTUNE:
		clif_skill_nodamage(src,bl,skillid,skilllv,1);
		if(sd) pc_getzeny(sd,status_get_lv(bl)*100);
		break;
	case SA_TAMINGMONSTER:
		clif_skill_nodamage(src,bl,skillid,skilllv,1);
		if (sd && dstmd) {
			for (i = 0; i < MAX_PET_DB; i++) {
				if (dstmd->class_ == pet_db[i].class_) {
					pet_catch_process1 (sd, dstmd->class_);
					break;
				}
			}
		}
		break;

	case AL_INCAGI:
	case AL_BLESSING:
	case PR_SLOWPOISON:
	case PR_IMPOSITIO:
	case PR_LEXAETERNA:
	case PR_SUFFRAGIUM:
	case PR_BENEDICTIO:
		clif_skill_nodamage(src,bl,skillid,skilllv,
			sc_start(bl,type,100,skilllv,skill_get_time(skillid,skilllv)));
		break;

	case CR_PROVIDENCE:
		if(sd && dstsd){ //Check they are not another crusader [Skotlex]
			if ((dstsd->class_&MAPID_UPPERMASK) == MAPID_CRUSADER) {	
				clif_skill_fail(sd,skillid,0,0);
				map_freeblock_unlock();
				return 1;
			}
		}
		clif_skill_nodamage(src,bl,skillid,skilllv,
			sc_start(bl,type,100,skilllv,skill_get_time(skillid,skilllv)));
		break;
		
	case CG_MARIONETTE:
		{
			struct status_change *sc= status_get_sc(src);
			int type2 = SC_MARIONETTE2;

			if(sc && tsc){
				if (sc->data[type].timer == -1 && tsc->data[type2].timer == -1) {
					sc_start(src,type,100,bl->id,skill_get_time(skillid,skilllv));
					sc_start(bl,type2,100,src->id,skill_get_time(skillid,skilllv));
					clif_marionette(src, bl);
					clif_skill_nodamage(src,bl,skillid,skilllv,1);
				}
				else if (sc->data[type].timer != -1 && tsc->data[type2].timer != -1 &&
					sc->data[type].val1 == bl->id && tsc->data[type2].val1 == src->id) {
					status_change_end(src, type, -1);
					status_change_end(bl, type2, -1);
				}
				else {
					if (sd) clif_skill_fail(sd,skillid,0,0);
					map_freeblock_unlock();
					return 1;
				}
			}
		}
		break;

	case RG_CLOSECONFINE:
		clif_skill_nodamage(src,bl,skillid,skilllv,
			sc_start4(bl,type,100,skilllv,src->id,0,0,skill_get_time(skillid,skilllv)));
		break;
	case SA_FLAMELAUNCHER:	// added failure chance and chance to break weapon if turned on [Valaris]
	case SA_FROSTWEAPON:
	case SA_LIGHTNINGLOADER:
	case SA_SEISMICWEAPON:
		if (dstsd) {
			if(dstsd->status.weapon == W_FIST ||
				(dstsd->sc.count && dstsd->sc.data[type].timer == -1 &&
				(	//Allow re-enchanting to lenghten time. [Skotlex]
					dstsd->sc.data[SC_FIREWEAPON].timer != -1 ||
					dstsd->sc.data[SC_WATERWEAPON].timer != -1 ||
					dstsd->sc.data[SC_WINDWEAPON].timer != -1 ||
					dstsd->sc.data[SC_EARTHWEAPON].timer != -1 ||
					dstsd->sc.data[SC_SHADOWWEAPON].timer != -1 ||
					dstsd->sc.data[SC_GHOSTWEAPON].timer != -1 ||
					dstsd->sc.data[SC_ENCPOISON].timer != -1
				))
				) {
				if (sd) clif_skill_fail(sd,skillid,0,0);
				clif_skill_nodamage(src,bl,skillid,skilllv,0);
				break;
			}
		}
		if (sd) {
			int i = pc_search_inventory (sd, skill_db[skillid].itemid[0]);
			if(i < 0 || sd->status.inventory[i].amount < skill_db[skillid].amount[0]) {
				clif_skill_fail(sd,skillid,0,0);
				break;
			}
			pc_delitem(sd, i, skill_db[skillid].amount[0], 0);
		}
		// 100% success rate at lv4 & 5, but lasts longer at lv5
		if(!clif_skill_nodamage(src,bl,skillid,skilllv, sc_start(bl,type,(60+skilllv*10),skilllv, skill_get_time(skillid,skilllv)))) {
			if (sd)
				clif_skill_fail(sd,skillid,0,0);
			if (skill_break_equip(bl, EQP_WEAPON, 10000, BCT_PARTY) && sd && sd != dstsd)
				clif_displaymessage(sd->fd,"You broke target's weapon");
		}
		break;

	case PR_ASPERSIO:
		if (sd && dstmd) {
			clif_skill_nodamage(src,bl,skillid,skilllv,0);
			break;
		}
		clif_skill_nodamage(src,bl,skillid,skilllv,
			sc_start(bl,type,100,skilllv,skill_get_time(skillid,skilllv)));
		break;

	case TK_SEVENWIND:
		switch(skilllv){
			case 1:
				type=SC_EARTHWEAPON;
				break;
			case 2:
				type=SC_WINDWEAPON;
				break;
			case 3:
				type=SC_WATERWEAPON;
				break;
			case 4:
				type=SC_FIREWEAPON;
				break;
			case 5:
				type=SC_GHOSTWEAPON;
				break;
			case 6:
				type=SC_SHADOWWEAPON;
				break;
			case 7:
				type=SC_ASPERSIO;
				break;
		}
		clif_skill_nodamage(src,bl,skillid,skilllv,
			sc_start(bl,type,100,skilllv,skill_get_time(skillid,skilllv)));
		break;

	case PR_KYRIE:
		clif_skill_nodamage(bl,bl,skillid,skilllv,
			sc_start(bl,type,100,skilllv,skill_get_time(skillid,skilllv)));
		break;
	//Passive Magnum, should had been casted on yourself.
	case SM_MAGNUM:
		map_foreachinrange(skill_area_sub, src,
			skill_get_splash(skillid, skilllv),BL_CHAR,
			src,skillid,skilllv,tick, flag|BCT_ENEMY|1,
			skill_castend_damage_id);
 //Initiate 10% of your damage becomes fire element.
		clif_skill_nodamage (src,src,skillid,skilllv,1);
		sc_start4(src,SC_WATK_ELEMENT,100,3,20,0,0,skill_get_time2(skillid, skilllv));
		if (sd) skill_blockpc_start (sd, skillid, skill_get_time(skillid, skilllv));
		break;
	case LK_BERSERK:
	case KN_AUTOCOUNTER:
	case KN_TWOHANDQUICKEN:	
	case KN_ONEHAND:
	case CR_SPEARQUICKEN:
	case CR_REFLECTSHIELD:
	case AS_POISONREACT:
	case MC_LOUD:
	case MG_ENERGYCOAT:
	case MG_SIGHT:
	case AL_RUWACH:
	case MO_EXPLOSIONSPIRITS:
	case MO_STEELBODY:
	case MO_BLADESTOP:
	case LK_AURABLADE:
	case LK_PARRYING:	
	case LK_CONCENTRATION:
	case WS_CARTBOOST:
	case SN_SIGHT:
	case WS_MELTDOWN:
	case WS_OVERTHRUSTMAX:	// Overthrust Max [Celest]
	case ST_REJECTSWORD:
	case HW_MAGICPOWER:
	case PF_MEMORIZE:
	case PA_SACRIFICE:
	case ASC_EDP:			// [Celest]
	case NPC_STOP:
	case WZ_SIGHTBLASTER:
	case SG_SUN_COMFORT:
	case SG_MOON_COMFORT:
	case SG_STAR_COMFORT:
	case NPC_HALLUCINATION:
	case HP_ASSUMPTIO:
	case GS_MADNESSCANCEL:
	case GS_ADJUSTMENT:
	case GS_INCREASING:
	case GS_CRACKER:
	case NJ_KASUMIKIRI:
	case NJ_UTSUSEMI:
	case NJ_NEN:
		clif_skill_nodamage(src,bl,skillid,skilllv,
			sc_start(bl,type,100,skilllv,skill_get_time(skillid,skilllv)));
		break;
	case HLIF_AVOID:	//[orn]
	case HAMI_DEFENCE:
		i = skill_get_time(skillid,skilllv);
		clif_skill_nodamage(src,bl,skillid,skilllv,
			sc_start(src,type,100,skilllv,i));
		if (bl != src)
			clif_skill_nodamage(src,bl,skillid,skilllv,
				sc_start(bl,type,100,skilllv,i));
		break;
	case NJ_BUNSINJYUTSU:
		clif_skill_nodamage(src,bl,skillid,skilllv,
			sc_start(bl,type,100,skilllv,skill_get_time(skillid,skilllv)));
		if (tsc && tsc->data[SC_NEN].timer != -1)
			status_change_end(bl,SC_NEN,-1);
		break;
	case CG_MOONLIT:
		clif_skill_nodamage(src,bl,skillid,skilllv,1);
		if (sd && battle_config.player_skill_partner_check &&
			(!battle_config.gm_skilluncond || pc_isGM(sd) < battle_config.gm_skilluncond)) {
			skill_check_pc_partner(sd, skillid, &skilllv, 1, 1);
		} else
			skill_moonlit(bl, NULL, skilllv); //The knockback must be invoked before starting the effect which places down the map cells. [Skotlex]
		
		break;
/* Was modified to only affect targetted char.	[Skotlex]
	case HP_ASSUMPTIO:
		if (flag&1)
			sc_start(bl,type,100,skilllv,skill_get_time(skillid,skilllv));
		else
		{
			map_foreachinrange(skill_area_sub, bl,
				skill_get_splash(skillid, skilllv), BL_PC,
				src, skillid, skilllv, tick, flag|BCT_ALL|1,
				skill_castend_nodamage_id);
			clif_skill_nodamage(src,bl,skillid,skilllv,1);
		}
		break;
*/
	case SM_ENDURE:
		clif_skill_nodamage(src,bl,skillid,skilllv,
			sc_start(bl,type,100,skilllv,skill_get_time(skillid,skilllv)));
		if (sd)
			skill_blockpc_start (sd, skillid, skill_get_time2(skillid,skilllv));
		break;

	case AS_ENCHANTPOISON: // Prevent spamming [Valaris]
		if (sd && dstsd && dstsd->sc.count) {
			if(dstsd->sc.data[SC_FIREWEAPON].timer != -1 ||
				dstsd->sc.data[SC_WATERWEAPON].timer != -1 ||
				dstsd->sc.data[SC_WINDWEAPON].timer != -1 ||
				dstsd->sc.data[SC_EARTHWEAPON].timer != -1 ||
				dstsd->sc.data[SC_SHADOWWEAPON].timer != -1 ||
				dstsd->sc.data[SC_GHOSTWEAPON].timer != -1
			//	dstsd->sc.data[SC_ENCPOISON].timer != -1 //People say you should be able to recast to lengthen the timer. [Skotlex]
			) {
					clif_skill_nodamage(src,bl,skillid,skilllv,0);
					clif_skill_fail(sd,skillid,0,0);
					break;
			}
		}
		clif_skill_nodamage(src,bl,skillid,skilllv,
			sc_start(bl,type,100,skilllv,skill_get_time(skillid,skilllv)));
		break;

	case LK_TENSIONRELAX:
		clif_skill_nodamage(src,bl,skillid,skilllv,
			sc_start4(bl,type,100,skilllv,0,0,skill_get_time2(skillid,skilllv),
				skill_get_time(skillid,skilllv)));
		break;

	case MC_CHANGECART:
		clif_skill_nodamage(src,bl,skillid,skilllv,1);
		break;

	case TK_MISSION:
		if (sd) {
			int id;
			if (sd->mission_mobid && (sd->mission_count || rand()%100)) { //Cannot change target when already have one
				clif_mission_mob(sd, sd->mission_mobid, sd->mission_count);
				clif_skill_fail(sd,skillid,0,0);
				break;
			}
			id = mob_get_random_id(0,0, sd->status.base_level);
			if (!id) {
				clif_skill_fail(sd,skillid,0,0);
				break;
			}
			sd->mission_mobid = id;
			sd->mission_count = 0;
			pc_setglobalreg(sd,"TK_MISSION_ID", id);
			clif_mission_mob(sd, id, 0);
			clif_skill_nodamage(src,bl,skillid,skilllv,1);
		}
		break;

	case AC_CONCENTRATION:
		{
			clif_skill_nodamage(src,bl,skillid,skilllv,
				sc_start(bl,type,100,skilllv,skill_get_time(skillid,skilllv)));
			map_foreachinrange( status_change_timer_sub, src,
				skill_get_splash(skillid, skilllv), BL_CHAR,
				src,status_get_sc(src),type,tick);
		}
		break;

	case SM_PROVOKE:
		if((tstatus->mode&MD_BOSS) || battle_check_undead(tstatus->race,tstatus->def_ele)) {
			map_freeblock_unlock();
			return 1;
		}
		//TODO: How much does base level affects? Dummy value of 1% per level difference used. [Skotlex]
		clif_skill_nodamage(src,bl,skillid,skilllv,
			(i=sc_start(bl,type,
				50 +3*skilllv +status_get_lv(src) -status_get_lv(bl),
				skilllv,skill_get_time(skillid,skilllv))));
		if (!i)
		{
			if (sd) 
				clif_skill_fail(sd,skillid,0,0);
			map_freeblock_unlock();
			return 0;
		}
		unit_skillcastcancel(bl, 2);

		if(tsc && tsc->count){
			if(tsc->data[SC_FREEZE].timer!=-1)
				status_change_end(bl,SC_FREEZE,-1);
			if(tsc->data[SC_STONE].timer!=-1 && tsc->data[SC_STONE].val2==0)
				status_change_end(bl,SC_STONE,-1);
			if(tsc->data[SC_SLEEP].timer!=-1)
				status_change_end(bl,SC_SLEEP,-1);
		}

		if(dstmd) {
			dstmd->state.provoke_flag = src->id;
			mob_target(dstmd,src,skill_get_range2(src,skillid,skilllv));
		}
		break;

	case CR_DEVOTION:
		if(sd && dstsd)
		{
			int lv = sd->status.base_level - dstsd->status.base_level;
			if (lv < 0) lv = -lv;
			if (lv > battle_config.devotion_level_difference ||
				(dstsd->sc.data[type].timer != -1 && dstsd->sc.data[type].val1 != src->id) || //Avoid overriding [Skotlex]
				(dstsd->class_&MAPID_UPPERMASK) == MAPID_CRUSADER) {
				clif_skill_fail(sd,skillid,0,0);
				map_freeblock_unlock();
				return 1;
			}
			//Look for an empty slot (or reuse in case you cast it twice in the same char. [Skotlex]
			for (i = 0; i < skilllv && i < 5 && sd->devotion[i]!=bl->id && sd->devotion[i]; i++);
			if (i == skilllv)
			{
				clif_skill_fail(sd,skillid,0,0);
				map_freeblock_unlock();
				return 1;
			}
			sd->devotion[i] = bl->id;
			clif_skill_nodamage(src,bl,skillid,skilllv,
				sc_start4(bl,type,100,src->id,i,skill_get_range2(src,skillid,skilllv),skill_get_time2(skillid, skilllv),1000));
			clif_devotion(sd);
		}
		else
			if (sd)
				clif_skill_fail(sd,skillid,0,0);
		break;

	case MO_CALLSPIRITS:
		if(sd) {
			clif_skill_nodamage(src,bl,skillid,skilllv,1);
			pc_addspiritball(sd,skill_get_time(skillid,skilllv),skilllv);
		}
		break;

	case CH_SOULCOLLECT:
		if(sd) {
			clif_skill_nodamage(src,bl,skillid,skilllv,1);
			for (i = 0; i < 5; i++)
				pc_addspiritball(sd,skill_get_time(skillid,skilllv),5);
		}
		break;

	case MO_KITRANSLATION:
		if(dstsd && (dstsd->class_&MAPID_BASEMASK)!=MAPID_GUNSLINGER) {
			pc_addspiritball(dstsd,skill_get_time(skillid,skilllv),5);
		}
		break;

	case TK_TURNKICK:
	case MO_BALKYOUNG: //Passive part of the attack. Splash knock-back+stun. [Skotlex]
		if (skill_area_temp[1] != bl->id) {
			skill_blown(src,bl,skill_get_blewcount(skillid,skilllv));
			skill_additional_effect(src,bl,skillid,skilllv,BF_MISC,tick); //Use Misc rather than weapon to signal passive pushback
		}
		break;	

	case MO_ABSORBSPIRITS:
		i = 0;
		if (dstsd && dstsd->spiritball && (sd == dstsd || map_flag_vs(src->m)) && (dstsd->class_&MAPID_BASEMASK)!=MAPID_GUNSLINGER)
		{	// split the if for readability, and included gunslingers in the check so that their coins cannot be removed [Reddozen]
			i = dstsd->spiritball * 10;
			pc_delspiritball(dstsd,dstsd->spiritball,0);
		} else if (dstmd && !(tstatus->mode&MD_BOSS) && rand() % 100 < 20)
		{	// check if target is a monster and not a Boss, for the 20% chance to absorb 2 SP per monster's level [Reddozen]
			i = 2 * dstmd->db->lv;
			mob_target(dstmd,src,0);
		}
		if (i) {
			status_heal(src, 0, i, 3);
			clif_skill_nodamage(src,bl,skillid,skilllv,0);
		}
		break;

	case AC_MAKINGARROW:
		if(sd) {
			clif_arrow_create_list(sd);
			clif_skill_nodamage(src,bl,skillid,skilllv,1);
		}
		break;

	case AM_PHARMACY:
		if(sd) {
			clif_skill_produce_mix_list(sd,22);
			clif_skill_nodamage(src,bl,skillid,skilllv,1);
		}
		break;

	case SA_CREATECON:
		if(sd) {
			clif_skill_produce_mix_list(sd,23);
			clif_skill_nodamage(src,bl,skillid,skilllv,1);
		}
		break;

	case BS_HAMMERFALL:
		clif_skill_nodamage(src,bl,skillid,skilllv,
			sc_start(bl,SC_STUN,(20 + 10 * skilllv),skilllv,skill_get_time2(skillid,skilllv)));
		break;
	case RG_RAID:
		clif_skill_nodamage(src,bl,skillid,skilllv,1);
		map_foreachinrange(skill_area_sub, bl,
			skill_get_splash(skillid, skilllv), BL_CHAR,
			src,skillid,skilllv,tick, flag|BCT_ENEMY|1,
			skill_castend_damage_id);
		status_change_end(src, SC_HIDING, -1);
		break;

	case ASC_METEORASSAULT:
	case GS_SPREADATTACK:
		clif_skill_nodamage(src,bl,skillid,skilllv,1);
		map_foreachinrange(skill_area_sub, src,
			skill_get_splash(skillid, skilllv), BL_CHAR,
			src,skillid,skilllv,tick, flag|BCT_ENEMY|1,
			skill_castend_damage_id);
		break;

	case KN_BRANDISHSPEAR:
		{
			int c,n=4;
			int dir = map_calc_dir(src,bl->x,bl->y);
			struct square tc;
			int x=bl->x,y=bl->y;
			skill_brandishspear_first(&tc,dir,x,y);
			skill_brandishspear_dir(&tc,dir,4);
			if(skilllv > 9){
				for(c=1;c<4;c++){
					map_foreachincell(skill_area_sub,
						bl->m,tc.val1[c],tc.val2[c],BL_CHAR,
						src,skillid,skilllv,tick, flag|BCT_ENEMY|n,
						skill_castend_damage_id);
				}
			}
			if(skilllv > 6){
				skill_brandishspear_dir(&tc,dir,-1);
				n--;
			}else{
				skill_brandishspear_dir(&tc,dir,-2);
				n-=2;
			}

			if(skilllv > 3){
				for(c=0;c<5;c++){
					map_foreachincell(skill_area_sub,
						bl->m,tc.val1[c],tc.val2[c],BL_CHAR,
						src,skillid,skilllv,tick, flag|BCT_ENEMY|n,
						skill_castend_damage_id);
					if(skilllv > 6 && n==3 && c==4){
						skill_brandishspear_dir(&tc,dir,-1);
						n--;c=-1;
					}
				}
			}
			for(c=0;c<10;c++){
				if(c==0||c==5) skill_brandishspear_dir(&tc,dir,-1);
				map_foreachincell(skill_area_sub,
					bl->m,tc.val1[c%5],tc.val2[c%5],BL_CHAR,
					src,skillid,skilllv,tick, flag|BCT_ENEMY|1,
					skill_castend_damage_id);
			}
		}
		break;
	
	case WZ_SIGHTRASHER:
		//Passive side of the attack.
		status_change_end(src,SC_SIGHT,-1);
		clif_skill_nodamage(src,bl,skillid,skilllv,1);
		map_foreachinrange(skill_area_sub,src,
			skill_get_splash(skillid, skilllv),BL_CHAR,
			src,skillid,skilllv,tick, flag|BCT_ENEMY|1,
			skill_castend_damage_id);
		break;

	case NJ_HYOUSYOURAKU:
	case NJ_RAIGEKISAI:
		clif_skill_nodamage(src,bl,skillid,skilllv,1);
	case WZ_FROSTNOVA:
		map_foreachinrange(skill_attack_area, src,
			skill_get_splash(skillid, skilllv), BL_CHAR,
			BF_MAGIC, src, src, skillid, skilllv, tick, flag, BCT_ENEMY);
		break;

	case HVAN_EXPLOSION:	//[orn]
		if(hd){
			hd->master->homunculus.intimacy = 200;
			clif_send_homdata(hd->master,0x100,hd->master->homunculus.intimacy/100);
		}
	case NPC_SELFDESTRUCTION:
		//Self Destruction hits everyone in range (allies+enemies)
		//Except for Summoned Marine spheres on non-versus maps, where it's just enemy.
		i = (md && md->special_state.ai == 2 && !map_flag_vs(src->m))?
			BCT_ENEMY:BCT_ALL;
		clif_skill_nodamage(src, src, skillid, -1, 1);
		map_foreachinrange(skill_area_sub, bl,
			skill_get_splash(skillid, skilllv), BL_CHAR,
			src, skillid, skilllv, tick, flag|i,
			skill_castend_damage_id);
		status_damage(src, src, sstatus->max_hp,0,0,1);
		break;

	case AL_ANGELUS:
	case PR_MAGNIFICAT:
	case PR_GLORIA:
	case SN_WINDWALK:
		if (sd == NULL || sd->status.party_id == 0 || (flag & 1)) {
			clif_skill_nodamage(bl,bl,skillid,skilllv,
				sc_start(bl,type,100,skilllv,skill_get_time(skillid,skilllv)));
		} else if (sd) {
			party_foreachsamemap (skill_area_sub,
				sd,skill_get_splash(skillid, skilllv),
				src,skillid,skilllv,tick, flag|BCT_PARTY|1,
				skill_castend_nodamage_id);
		}
		break;

	case BS_ADRENALINE:
	case BS_ADRENALINE2:
	case BS_WEAPONPERFECT:
	case BS_OVERTHRUST:
		if (sd == NULL || sd->status.party_id == 0 || (flag & 1)) {
			clif_skill_nodamage(bl,bl,skillid,skilllv,
				sc_start4(bl,type,100,skilllv,(src == bl)? 1:0,0,0,skill_get_time(skillid,skilllv)));
		} else if (sd) {
			party_foreachsamemap(skill_area_sub,
				sd,skill_get_splash(skillid, skilllv),
				src,skillid,skilllv,tick, flag|BCT_PARTY|1,
				skill_castend_nodamage_id);
		}
		break;

	case BS_MAXIMIZE:
	case NV_TRICKDEAD:
	case CR_DEFENDER:
	case CR_AUTOGUARD:
	case TK_READYSTORM:
	case TK_READYDOWN:
	case TK_READYTURN:
	case TK_READYCOUNTER:
	case TK_DODGE:
	case CR_SHRINK:
	case ST_PRESERVE:
	case SG_FUSION:
	case GS_GATLINGFEVER:
		if (tsc && tsc->data[type].timer != -1)
			i = status_change_end(bl, type, -1);
		else
			i = sc_start(bl,type,100,skilllv,skill_get_time(skillid,skilllv));
			clif_skill_nodamage(src,bl,skillid,skilllv,i);
		break;
	case SL_KAITE:
	case SL_KAAHI:
	case SL_KAIZEL:
	case SL_KAUPE:
		if (sd) {
			if (!dstsd || !(
				(sd->sc.data[SC_SPIRIT].timer != -1 && sd->sc.data[SC_SPIRIT].val2 == SL_SOULLINKER) ||
				(dstsd->class_&MAPID_UPPERMASK) == MAPID_SOUL_LINKER ||
				dstsd->char_id == sd->char_id ||
				dstsd->char_id == sd->status.partner_id ||
				dstsd->char_id == sd->status.child
			)) {
				status_change_start(src,SC_STUN,10000,skilllv,0,0,0,500,8);
				clif_skill_fail(sd,skillid,0,0);
				break;
			}
		}
		clif_skill_nodamage(src,bl,skillid,skilllv,
			sc_start(bl,type,100,skilllv,skill_get_time(skillid, skilllv)));
		break;
	case SM_AUTOBERSERK:	// Celest
		if (tsc && tsc->data[type].timer != -1)
			i = status_change_end(bl, type, -1);				
		else
			i = sc_start(bl,type,100,skilllv,60000);
		clif_skill_nodamage(src,bl,skillid,skilllv,i);
		break;
	case TF_HIDING:
	case ST_CHASEWALK:
		if (tsc && tsc->data[type].timer != -1)
			i = status_change_end(bl, type, -1);
		else
			i = sc_start(bl,type,100,skilllv,skill_get_time(skillid,skilllv));
		clif_skill_nodamage(src,bl,skillid,-1,i); //Hide skill-scream animation.
		break;
	case TK_RUN:
			if (tsc && tsc->data[type].timer != -1)
				i = status_change_end(bl, type, -1);
			else
				i = sc_start4(bl,type,100,skilllv,unit_getdir(bl),0,0,0);
//			If the client receives a skill-use packet inmediately before
//			a walkok packet, it will discard the walk packet! [Skotlex]
//			clif_skill_nodamage(src,bl,skillid,skilllv,i);
		break;
	case AS_CLOAKING:
		if(tsc && tsc->data[type].timer!=-1 )
			i = status_change_end(bl, type, -1);
		else
			i = sc_start(bl,type,100,skilllv,skill_get_time(skillid,skilllv));
		clif_skill_nodamage(src,bl,skillid,-1,i);
		if (!i && sd)
			clif_skill_fail(sd,skillid,0,0);
		break;

	case BD_ADAPTATION:
		if(tsc && tsc->data[SC_DANCING].timer!=-1){
			clif_skill_nodamage(src,bl,skillid,skilllv,1);
			skill_stop_dancing(bl);
		}
		break;

	case BA_FROSTJOKE:
	case DC_SCREAM:
		clif_skill_nodamage(src,bl,skillid,skilllv,1);
		skill_addtimerskill(src,tick+2000,bl->id,src->x,src->y,skillid,skilllv,0,flag);
		if (md) {
			char temp[128];
			if (strlen(md->name) + strlen(skill_db[skillid].desc) > 120)
				break; //Message won't fit on buffer. [Skotlex]
			sprintf(temp,"%s : %s !!",md->name,skill_db[skillid].desc);
			clif_message(&md->bl,temp);
		}
		break;


	case BA_PANGVOICE:
		clif_skill_nodamage(src,bl,skillid,skilllv,
			sc_start(bl,SC_CONFUSION,50,7,skill_get_time(skillid,skilllv)));
		break;

	case DC_WINKCHARM:
		if(dstsd){
			clif_skill_nodamage(src,bl,skillid,skilllv,
				sc_start(bl,SC_CONFUSION,30,7,skill_get_time2(skillid,skilllv)));
		}else if(dstmd)
		{
			if(status_get_lv(src)>status_get_lv(bl) && (tstatus->race == RC_DEMON || tstatus->race == RC_DEMIHUMAN || tstatus->race == RC_ANGEL)) {
				clif_skill_nodamage(src,bl,skillid,skilllv,
					sc_start(bl,type,70,skilllv,skill_get_time(skillid,skilllv)));
			} else{
				clif_skill_nodamage(src,bl,skillid,skilllv,0);
				if(sd) clif_skill_fail(sd,skillid,0,0);
			}
		}
		break;

	case TF_STEAL:
		if(sd) {
			if(pc_steal_item(sd,bl))
				clif_skill_nodamage(src,bl,skillid,skilllv,1);
			else
				clif_skill_fail(sd,skillid,0x0a,0);
		}
		break;

	case RG_STEALCOIN:
		if(sd) {
			if(pc_steal_coin(sd,bl))
				clif_skill_nodamage(src,bl,skillid,skilllv,1);
			else
				clif_skill_fail(sd,skillid,0,0);
		}
		break;

	case MG_STONECURSE:
		{
			if (tstatus->mode&MD_BOSS) {
				if (sd) clif_skill_fail(sd,skillid,0,0);
				break;
			}
			if(status_isimmune(bl) || !tsc)
				break;
			if (dstmd)
				mob_target(dstmd,src,skill_get_range2(src,skillid,skilllv));
			
			if (tsc->data[SC_STONE].timer != -1) {
				status_change_end(bl,SC_STONE,-1);
				if (sd) clif_skill_fail(sd,skillid,0,0);
				break;
			}
			if (sc_start(bl,SC_STONE,(skilllv*4+20),skilllv,skill_get_time2(skillid,skilllv)))
					clif_skill_nodamage(src,bl,skillid,skilllv,1);
			else if(sd) {
				clif_skill_fail(sd,skillid,0,0);
				// Level 6-10 doesn't consume a red gem if it fails [celest]
				if (skilllv > 5) break;
			}
			if (sd) {
				if (sd->sc.data[SC_SPIRIT].timer != -1 && sd->sc.data[SC_SPIRIT].val2 == SL_WIZARD)
					break; //Do not delete the gemstone.
				if ((i=pc_search_inventory(sd, skill_db[skillid].itemid[0])) >= 0 )
					pc_delitem(sd, i, skill_db[skillid].amount[0], 0);
			}
		}
		break;

	case NV_FIRSTAID:
		clif_skill_nodamage(src,bl,skillid,5,1);
		status_heal(bl,5,0,0);
		break;

	case AL_CURE:
		if(status_isimmune(bl)) {
			clif_skill_nodamage(src,bl,skillid,skilllv,0);
			break;
		}
		status_change_end(bl, SC_SILENCE	, -1 );
		status_change_end(bl, SC_BLIND	, -1 );
		status_change_end(bl, SC_CONFUSION, -1 );
		if(battle_check_undead(tstatus->race,tstatus->def_ele))
			sc_start(bl, SC_CONFUSION,100,1,skill_get_time2(skillid, skilllv));
		clif_skill_nodamage(src,bl,skillid,skilllv,1);
		break;

	case TF_DETOXIFY:
		clif_skill_nodamage(src,bl,skillid,skilllv,1);
		status_change_end(bl, SC_POISON	, -1 );
		status_change_end(bl, SC_DPOISON	, -1 );
		break;

	case PR_STRECOVERY:
		if(status_isimmune(bl)) {
			clif_skill_nodamage(src,bl,skillid,skilllv,0);
			break;
		}
		status_change_end(bl, SC_FREEZE	, -1 );
		status_change_end(bl, SC_STONE	, -1 );
		status_change_end(bl, SC_SLEEP	, -1 );
		status_change_end(bl, SC_STUN	, -1 );
		//Is this equation really right? It looks so... special.
		if(battle_check_undead(tstatus->race,tstatus->def_ele) )
		{
			status_change_start(bl, SC_BLIND,
				100*(100-(tstatus->int_/2+tstatus->vit/3+tstatus->luk/10)),
				1,0,0,0,
				skill_get_time2(skillid, skilllv) * (100-(tstatus->int_+tstatus->vit)/2)/100,10);
		}
		clif_skill_nodamage(src,bl,skillid,skilllv,1);
		if(dstmd)
			mob_unlocktarget(dstmd,tick);
		break;

	case WZ_ESTIMATION:
		if(sd) {
			clif_skill_nodamage(src,bl,skillid,skilllv,1);
			clif_skill_estimation((struct map_session_data *)src,bl);
		}
		break;

	case BS_REPAIRWEAPON:
		if(sd && dstsd)
			clif_item_repair_list(sd,dstsd);
		break;

	case MC_IDENTIFY:
		if(sd)
			clif_item_identify_list(sd);
		break;

	// Weapon Refining [Celest]
	case WS_WEAPONREFINE:
		if(sd)
			clif_item_refine_list(sd);
		break;

	case MC_VENDING:
		if(sd)
		{	//Prevent vending of GMs with unnecessary Level to trade/drop. [Skotlex]
			if ( pc_can_give_items(pc_isGM(sd)) )
				clif_skill_fail(sd,skillid,0,0);
			else
				clif_openvendingreq(sd,2+skilllv);
		}
		break;

	case AL_TELEPORT:
		if(sd) {
			if (map[bl->m].flag.noteleport) {
				clif_skill_teleportmessage(sd,0);
				break;
			}
			if(!battle_config.duel_allow_teleport && sd->duel_group) { // duel restriction [LuzZza]
				clif_displaymessage(sd->fd, "Duel: Can't use teleport in duel.");
				break;
			}
			clif_skill_nodamage(src,bl,skillid,skilllv,1);
			if(skilllv == 1) {
				// possibility to skip menu [LuzZza]
				if(!battle_config.skip_teleport_lv1_menu &&
					sd->skillitem != AL_TELEPORT) //If skillid is not teleport, this was auto-casted! [Skotlex]
					clif_skill_warppoint(sd,skillid,skilllv,"Random","","","");
				else
					pc_randomwarp(sd,3);
			} else {
				if (sd->skillitem != AL_TELEPORT)
					clif_skill_warppoint(sd,skillid,skilllv,"Random",
						mapindex_id2name(sd->status.save_point.map),"","");
				else //Autocasted Teleport level 2??
					pc_setpos(sd,sd->status.save_point.map,
						sd->status.save_point.x,sd->status.save_point.y,3);
			}
		} else
			unit_warp(bl,-1,-1,-1,3);
		break;

	case AL_HOLYWATER:
		if(sd) {
			if (skill_produce_mix(sd, skillid, 523, 0, 0, 0, 1))
				clif_skill_nodamage(src,bl,skillid,skilllv,1);
			else
				clif_skill_fail(sd,skillid,0,0);
		}
		break;
		
	case TF_PICKSTONE:
		if(sd) {
			int eflag;
			struct item item_tmp;
			struct block_list tbl;
			clif_skill_nodamage(src,bl,skillid,skilllv,1);
			memset(&item_tmp,0,sizeof(item_tmp));
			memset(&tbl,0,sizeof(tbl)); // [MouseJstr]
			item_tmp.nameid = 7049;
			item_tmp.identify = 1;
			tbl.id = 0;
			clif_takeitem(&sd->bl,&tbl);
			eflag = pc_additem(sd,&item_tmp,1);
			if(eflag) {
				clif_additem(sd,0,0,eflag);
				map_addflooritem(&item_tmp,1,sd->bl.m,sd->bl.x,sd->bl.y,NULL,NULL,NULL,0);
			}
		}
		break;
	case ASC_CDP:
     	if(sd) {
			clif_skill_nodamage(src,bl,skillid,skilllv,1);
			skill_produce_mix(sd, skillid, 678, 0, 0, 0, 1); //Produce a Deadly Poison Bottle.
		}
		break;

	case RG_STRIPWEAPON:
	case RG_STRIPSHIELD:
	case RG_STRIPARMOR:
	case RG_STRIPHELM:
	case ST_FULLSTRIP:			// Rewritten most of the code [DracoRPG]
	case GS_DISARM:				// Added disarm. [Reddozen]
		{
		int strip_fix, equip = 0;
		int sclist[4] = {0,0,0,0};

		switch (skillid) {
		case RG_STRIPWEAPON:
		case GS_DISARM:
		   equip = EQP_WEAPON;
			break;
		case RG_STRIPSHIELD:
		   equip = EQP_SHIELD;
			break;
		case RG_STRIPARMOR:
		   equip = EQP_ARMOR;
			break;
		case RG_STRIPHELM:
		   equip = EQP_HELM;
			break;
		case ST_FULLSTRIP:
		   equip = EQP_WEAPON|EQP_SHIELD|EQP_ARMOR|EQP_HELM;
			break;
		}

		strip_fix = sstatus->dex - tstatus->dex;
		if(strip_fix < 0)
			strip_fix=0;
		if (rand()%100 >= 5+2*skilllv+strip_fix/5)
		{
			if (sd)
				clif_skill_fail(sd,skillid,0,0);
			break;
		}
		if (dstsd) {
			for (i=0;i<EQI_MAX;i++) {
				if (dstsd->equip_index[i]<0 || !dstsd->inventory_data[dstsd->equip_index[i]])
					continue;
				switch (i) {
				case EQI_HAND_L: //Shield / left-hand weapon
					if(dstsd->inventory_data[dstsd->equip_index[i]]->type == IT_ARMOR)
					{ //Shield
						if (equip&EQP_SHIELD &&
							!(dstsd->unstripable_equip&EQP_SHIELD) &&
						  	!(tsc && tsc->data[SC_CP_SHIELD].timer != -1)
						){
							sclist[1] = SC_STRIPSHIELD; // Okay, we found a shield to strip - It is really a shield, not a two-handed weapon or a left-hand weapon
							pc_unequipitem(dstsd,dstsd->equip_index[i],3);
						}
						continue;
					}
					//Continue to weapon
				case EQI_HAND_R:
					if (equip&EQP_WEAPON &&
						!(dstsd->unstripable_equip&EQP_WEAPON) &&
				  		!(tsc && tsc->data[SC_CP_WEAPON].timer != -1)
					) {
						sclist[0] = SC_STRIPWEAPON; // Okay, we found a weapon to strip - It can be a right-hand, left-hand or two-handed weapon
						pc_unequipitem(dstsd,dstsd->equip_index[i],3);
					}
					break;
				case EQI_ARMOR: //Armor
					if (equip &EQP_ARMOR && 
						!(dstsd->unstripable_equip &EQP_ARMOR) &&
					  	!(tsc && tsc->data[SC_CP_ARMOR].timer != -1)
					) {
						sclist[2] = SC_STRIPARMOR; // Okay, we found an armor to strip
						pc_unequipitem(dstsd,dstsd->equip_index[i],3);
					}
					break;
				case EQI_HEAD_TOP: //Helm  
					if (equip &EQP_HELM &&
						!(dstsd->unstripable_equip &EQP_HELM) &&
						!(tsc && tsc->data[SC_CP_HELM].timer != -1)
					 ) {
						sclist[3] = SC_STRIPHELM; // Okay, we found a helm to strip
						pc_unequipitem(dstsd,dstsd->equip_index[i],3);
					}
					break;
				}
			}
		} else if (!(tstatus->mode&MD_BOSS)) {
			if (equip&EQP_WEAPON && !(tsc && tsc->data[SC_CP_WEAPON].timer != -1))
				sclist[0] = SC_STRIPWEAPON;
			if (equip&EQP_SHIELD && !(tsc && tsc->data[SC_CP_SHIELD].timer != -1))
				sclist[1] = SC_STRIPSHIELD;
			if (equip&EQP_ARMOR && !(tsc && tsc->data[SC_CP_ARMOR].timer != -1))
				sclist[2] = SC_STRIPARMOR;
			if (equip&EQP_HELM && !(tsc && tsc->data[SC_CP_HELM].timer != -1))
				sclist[3] = SC_STRIPHELM;
		}
		equip = 0; //Reuse equip to hold how many stats are invoked.
		for (i=0;i<4;i++) {
			if (sclist[i]) // Start the SC only if an equipment was stripped from this location
			equip+=sc_start(bl,sclist[i],100,skilllv,skill_get_time(skillid,skilllv)+strip_fix/2);
		}
		if (equip)
			clif_skill_nodamage(src,bl,skillid,skilllv,1);
		else if (sd) //Nothing stripped.
			clif_skill_fail(sd,skillid,0,0);
		break;
		}

	/* PotionPitcher */
	case AM_BERSERKPITCHER:
	case AM_POTIONPITCHER:
		{
			int i,x,hp = 0,sp = 0,bonus=100;
			if(sd) {
				x = skilllv%11 - 1;
				i = pc_search_inventory(sd,skill_db[skillid].itemid[x]);
				if(i < 0 || skill_db[skillid].itemid[x] <= 0) {
					clif_skill_fail(sd,skillid,0,0);
					map_freeblock_unlock();
					return 1;
				}
				if(sd->inventory_data[i] == NULL || sd->status.inventory[i].amount < skill_db[skillid].amount[x]) {
					clif_skill_fail(sd,skillid,0,0);
					map_freeblock_unlock();
					return 1;
				}
				if(skillid == AM_BERSERKPITCHER) { //Does not override use-level, and cannot be used on bows.
					if (dstsd && (dstsd->status.base_level<(unsigned int)sd->inventory_data[i]->elv || dstsd->weapontype1 == W_BOW)) {
						clif_skill_fail(sd,skillid,0,0);
						map_freeblock_unlock();
						return 1;
					 }
				}
				potion_flag = 1;
				potion_hp = potion_sp = potion_per_hp = potion_per_sp = 0;
				potion_target = bl->id;
				run_script(sd->inventory_data[i]->script,0,sd->bl.id,0);
				pc_delitem(sd,i,skill_db[skillid].amount[x],0);
				potion_flag = potion_target = 0;
				if (sd->sc.data[SC_SPIRIT].timer != -1 && sd->sc.data[SC_SPIRIT].val2 == SL_ALCHEMIST)
					bonus += sd->status.base_level;
				if(potion_per_hp > 0 || potion_per_sp > 0) {
					hp = tstatus->max_hp * potion_per_hp / 100;
					hp = hp * (100 + pc_checkskill(sd,AM_POTIONPITCHER)*10 + pc_checkskill(sd,AM_LEARNINGPOTION)*5)*bonus/10000;
					if(dstsd) {
						sp = dstsd->status.max_sp * potion_per_sp / 100;
						sp = sp * (100 + pc_checkskill(sd,AM_POTIONPITCHER)*10 + pc_checkskill(sd,AM_LEARNINGPOTION)*5)*bonus/10000;
					}
				}
				else {
					if(potion_hp > 0) {
						hp = potion_hp * (100 + pc_checkskill(sd,AM_POTIONPITCHER)*10 + pc_checkskill(sd,AM_LEARNINGPOTION)*5)*bonus/10000;
						hp = hp * (100 + (tstatus->vit<<1)) / 100;
						if(dstsd)
							hp = hp * (100 + pc_checkskill(dstsd,SM_RECOVERY)*10) / 100;
					}
					if(potion_sp > 0) {
						sp = potion_sp * (100 + pc_checkskill(sd,AM_POTIONPITCHER)*10 + pc_checkskill(sd,AM_LEARNINGPOTION)*5)*bonus/10000;
						sp = sp * (100 + (tstatus->int_<<1)) / 100;
						if(dstsd)
							sp = sp * (100 + pc_checkskill(dstsd,MG_SRECOVERY)*10) / 100;
					}
				}
			}
			else {
				hp = (1 + rand()%400) * (100 + skilllv*10) / 100;
				hp = hp * (100 + (tstatus->vit<<1)) / 100;
				if(dstsd)
					hp = hp * (100 + pc_checkskill(dstsd,SM_RECOVERY)*10) / 100;
			}
			clif_skill_nodamage(src,bl,skillid,skilllv,1);
			if(hp > 0 || (skillid == AM_POTIONPITCHER && hp <= 0 && sp <= 0))
				clif_skill_nodamage(NULL,bl,AL_HEAL,hp,1);
			if(sp > 0)
				clif_skill_nodamage(NULL,bl,MG_SRECOVERY,sp,1);
			status_heal(bl,hp,sp,0);
		}
		break;
	case AM_CP_WEAPON:
	case AM_CP_SHIELD:
	case AM_CP_ARMOR:
	case AM_CP_HELM:
		{
			int scid = SC_STRIPWEAPON + (skillid - AM_CP_WEAPON);
			if(tsc && tsc->data[scid].timer != -1)
				status_change_end(bl, scid, -1 );
			clif_skill_nodamage(src,bl,skillid,skilllv,
				sc_start(bl,type,100,skilllv,skill_get_time(skillid,skilllv)));
		}
		break;
	case AM_TWILIGHT1:
		if (sd) {
			clif_skill_nodamage(src,bl,skillid,skilllv,1);
			//Prepare 200 White Potions.
			if (!skill_produce_mix(sd, skillid, 504, 0, 0, 0, 200))
				clif_skill_fail(sd,skillid,0,0);
		}
		break;
	case AM_TWILIGHT2:
		if (sd) {
			clif_skill_nodamage(src,bl,skillid,skilllv,1);
			//Prepare 200 Slim White Potions.
			if (!skill_produce_mix(sd, skillid, 547, 0, 0, 0, 200))
				clif_skill_fail(sd,skillid,0,0);
		}
		break;
	case AM_TWILIGHT3:
		if (sd) {
			//check if you can produce all three, if not, then fail:
			if (!skill_can_produce_mix(sd,970,-1, 100) //100 Alcohol
				|| !skill_can_produce_mix(sd,7136,-1, 50) //50 Acid Bottle
				|| !skill_can_produce_mix(sd,7135,-1, 50) //50 Flame Bottle
			) {
				clif_skill_fail(sd,skillid,0,0);
				break;
			}
			clif_skill_nodamage(src,bl,skillid,skilllv,1);
			skill_produce_mix(sd, skillid, 970, 0, 0, 0, 100);
			skill_produce_mix(sd, skillid, 7136, 0, 0, 0, 50);
			skill_produce_mix(sd, skillid, 7135, 0, 0, 0, 50);
		}
		break;
	case SA_DISPELL:
		{
			int i;
			clif_skill_nodamage(src,bl,skillid,skilllv,1);
			i = status_get_sc_def_mdef(bl);
			if (i >= 10000 ||
				tsc == NULL || (tsc->data[SC_SPIRIT].timer != -1 && tsc->data[SC_SPIRIT].val2 == SL_ROGUE) || //Rogue's spirit defends againt dispel.
			//Fixed & changed to use a proportionnal reduction (no info, but seems far more logical) [DracoRPG]
				rand()%10000 >= (10000-i)*(50+10*skilllv)/100)
			{
				if (sd)
					clif_skill_fail(sd,skillid,0,0);
				break;
			}
			if(status_isimmune(bl) || !tsc->count)
				break;
			for(i=0;i<SC_MAX;i++){
				if (tsc->data[i].timer == -1)
					continue;
				if(i==SC_HALLUCINATION || i==SC_WEIGHT50 || i==SC_WEIGHT90
					|| i==SC_STRIPWEAPON || i==SC_STRIPSHIELD || i==SC_STRIPARMOR || i==SC_STRIPHELM
					|| i==SC_CP_WEAPON || i==SC_CP_SHIELD || i==SC_CP_ARMOR || i==SC_CP_HELM
					|| i==SC_COMBO || i==SC_DANCING || i==SC_GUILDAURA || i==SC_EDP
					|| i==SC_AUTOBERSERK  || i==SC_CARTBOOST || i==SC_MELTDOWN || i==SC_MOONLIT
					|| i==SC_SAFETYWALL || i==SC_SMA || i==SC_SPEEDUP0
					)
					continue;
				if(i==SC_BERSERK) tsc->data[i].val2=0; //Mark a dispelled berserk to avoid setting hp to 100 by setting hp penalty to 0.
				status_change_end(bl,i,-1);
			}
		}
		break;

	case TF_BACKSLIDING: //This is the correct implementation as per packet logging information. [Skotlex]
		clif_skill_nodamage(src,bl,skillid,skilllv,1);
		skill_blown(src,bl,skill_get_blewcount(skillid,skilllv)|0x10000);
		break;

	case TK_HIGHJUMP:
		{
			int x,y, dir = unit_getdir(src);

			x = src->x + dirx[dir]*skilllv*2;
			y = src->y + diry[dir]*skilllv*2;
			
			clif_skill_nodamage(src,bl,TK_HIGHJUMP,skilllv,1);
			if(map_getcell(src->m,x,y,CELL_CHKPASS)) {
				unit_movepos(src, x, y, 1, 0);
				clif_slide(src,x,y);
			}
		}
		break;

	case SA_CASTCANCEL:
		clif_skill_nodamage(src,bl,skillid,skilllv,1);
		unit_skillcastcancel(src,1);
		if(sd) {
			int sp = skill_get_sp(sd->skillid_old,sd->skilllv_old);
			sp = sp * (90 - (skilllv-1)*20) / 100;
			if(sp < 0) sp = 0;
			status_zap(src, 0, sp);
		}
		break;
	case SA_SPELLBREAKER:
		{
			int sp;
			if(tsc && tsc->data[SC_MAGICROD].timer != -1) {
				sp = skill_get_sp(skillid,skilllv);
				sp = sp * tsc->data[SC_MAGICROD].val2 / 100;
				if(sp < 1) sp = 1;
				status_heal(bl,0,sp,2);
				clif_skill_nodamage(bl,bl,SA_MAGICROD,tsc->data[SC_MAGICROD].val1,1);
				status_percent_damage(bl, src, 0, -20); //20% max SP damage.
			} else {
				struct unit_data *ud = unit_bl2ud(bl);
				int bl_skillid=0,bl_skilllv=0,hp = 0;
				if (!ud || ud->skilltimer == -1) break; //Nothing to cancel.
				bl_skillid = ud->skillid;
				bl_skilllv = ud->skilllv;
				if (tstatus->mode & MD_BOSS)
				{	//Only 10% success chance against bosses. [Skotlex]
					if (rand()%100 < 90)
					{
						if (sd) clif_skill_fail(sd,skillid,0,0);
						break;
					}
				} else if (!dstsd || map_flag_vs(bl->m)) //HP damage only on pvp-maps when against players.
					hp = tstatus->max_hp/50; //Recover 2% HP [Skotlex]
				
				clif_skill_nodamage(src,bl,skillid,skilllv,1);
				unit_skillcastcancel(bl,0);
				sp = skill_get_sp(bl_skillid,bl_skilllv);
				status_zap(bl, hp, sp);

				if (hp && skilllv >= 5)
					hp>>=1;	//Recover half damaged HP at level 5 [Skotlex]
				else
					hp = 0;
				
				if (skilllv > 1 && sp) //Recover some of the SP used
					sp = sp*(25*(skilllv-1))/100;
				else
					sp = 0;

				if(hp || sp)
					status_heal(src, hp, sp, 2);
			}
		}
		break;
	case SA_MAGICROD:
		sc_start(bl,type,100,skilllv,skill_get_time(skillid,skilllv));
		break;
	case SA_AUTOSPELL:
		clif_skill_nodamage(src,bl,skillid,skilllv,1);
		if(sd)
			clif_autospell(sd,skilllv);
		else {
			int maxlv=1,spellid=0;
			static const int spellarray[3] = { MG_COLDBOLT,MG_FIREBOLT,MG_LIGHTNINGBOLT };
			if(skilllv >= 10) {
				spellid = MG_FROSTDIVER;
//				if (tsc && tsc->data[SC_SPIRIT].timer != -1 && tsc->data[SC_SPIRIT].val2 == SA_SAGE)
//					maxlv = 10;
//				else
					maxlv = skilllv - 9;
			}
			else if(skilllv >=8) {
				spellid = MG_FIREBALL;
				maxlv = skilllv - 7;
			}
			else if(skilllv >=5) {
				spellid = MG_SOULSTRIKE;
				maxlv = skilllv - 4;
			}
			else if(skilllv >=2) {
				int i = rand()%3;
				spellid = spellarray[i];
				maxlv = skilllv - 1;
			}
			else if(skilllv > 0) {
				spellid = MG_NAPALMBEAT;
				maxlv = 3;
			}
			if(spellid > 0)
				sc_start4(src,SC_AUTOSPELL,100,skilllv,spellid,maxlv,0,
					skill_get_time(SA_AUTOSPELL,skilllv));
		}
		break;

	case BS_GREED:
		if(sd){
			clif_skill_nodamage(src,bl,skillid,skilllv,1);
			map_foreachinrange(skill_greed,bl,
				skill_get_splash(skillid, skilllv),BL_ITEM,bl);
		}
		break;

	case SA_ELEMENTWATER:
	case SA_ELEMENTFIRE:
	case SA_ELEMENTGROUND:
	case SA_ELEMENTWIND:
		if(sd && !dstmd) //Only works on monsters.
			break;
		if(tstatus->mode&MD_BOSS)
			break;
	case NPC_ATTRICHANGE:
	case NPC_CHANGEWATER:
	case NPC_CHANGEGROUND:
	case NPC_CHANGEFIRE:
	case NPC_CHANGEWIND:
	case NPC_CHANGEPOISON:
	case NPC_CHANGEHOLY:
	case NPC_CHANGEDARKNESS:
	case NPC_CHANGETELEKINESIS:
	case NPC_CHANGEUNDEAD:
		clif_skill_nodamage(src,bl,skillid,skilllv,
			sc_start4(bl, type, 100, skilllv, skillid, skill_get_pl(skillid), 0, 
				skill_get_time(skillid, skilllv)));
		break;

	case NPC_PROVOCATION:
		clif_skill_nodamage(src,bl,skillid,skilllv,1);
		if(md && md->skillidx >= 0)
			clif_pet_performance(src,md->db->skill[md->skillidx].val[0]);
		break;

	case NPC_KEEPING:
	case NPC_BARRIER:
		{
			int skill_time = skill_get_time(skillid,skilllv);
			struct unit_data *ud = unit_bl2ud(bl);
			if (clif_skill_nodamage(src,bl,skillid,skilllv,
					sc_start(bl,type,100,skilllv,skill_time))
			&& ud) {	//Disable attacking/acting/moving for skill's duration.
				ud->attackabletime = 
				ud->canact_tick =
				ud->canmove_tick = tick + skill_time;
			}
		}
		break;

	case NPC_REBIRTH:
		//New rebirth System uses Kaizel lv1. [Skotlex]
		sc_start(bl,type,100,1,skill_get_time(SL_KAIZEL,skilllv));
		break;

	case NPC_DARKBLESSING:
		clif_skill_nodamage(src,bl,skillid,skilllv,
			sc_start(bl,type,(50+skilllv*5),skilllv,skill_get_time2(skillid,skilllv)));
		break;

	case NPC_LICK:
		status_zap(bl, 0, 100);
		clif_skill_nodamage(src,bl,skillid,skilllv,
			sc_start(bl,type,(skilllv*5),skilllv,skill_get_time2(skillid,skilllv)));
		break;

	case NPC_SUICIDE:
		clif_skill_nodamage(src,bl,skillid,skilllv,1);
		status_kill(src); //When suiciding, neither exp nor drops is given.
		break;

	case NPC_SUMMONSLAVE:
	case NPC_SUMMONMONSTER:
		if(md && md->skillidx >= 0)
			mob_summonslave(md,md->db->skill[md->skillidx].val,skilllv,skillid);
		break;

	case NPC_CALLSLAVE:
		mob_warpslave(src,MOB_SLAVEDISTANCE);
		break;

	case NPC_RANDOMMOVE:
		if (md) {
			md->next_walktime = tick - 1;
			mob_randomwalk(md,tick);
		}
		break;
	
	case NPC_SPEEDUP:
		{
			// or does it increase casting rate? just a guess xD
			int i = SC_ASPDPOTION0 + skilllv - 1;
			if (i > SC_ASPDPOTION3)
				i = SC_ASPDPOTION3;
			clif_skill_nodamage(src,bl,skillid,skilllv,
				sc_start(bl,i,100,skilllv,skilllv * 60000));
		}
		break;

	case NPC_REVENGE:
		// not really needed... but adding here anyway ^^
		if (md && md->master_id > 0) {
			struct block_list *mbl, *tbl;
			if ((mbl = map_id2bl(md->master_id)) == NULL ||
				(tbl = battle_gettargeted(mbl)) == NULL)
				break;
			md->state.provoke_flag = tbl->id;
			mob_target(md, tbl, sstatus->rhw.range);
		}
		break;

	case NPC_RUN:
		{
			const int mask[8][2] = {{0,-1},{1,-1},{1,0},{1,1},{0,1},{-1,1},{-1,0},{-1,-1}};
			int dir = (bl == src)?unit_getdir(src):map_calc_dir(src,bl->x,bl->y); //If cast on self, run forward, else run away.
			unit_stop_attack(src);
			//Run skillv tiles overriding the can-move check.
			if (unit_walktoxy(src, src->x + skilllv * mask[dir][0], src->y + skilllv * mask[dir][1], 2) && md)
				md->state.skillstate = MSS_WALK; //Otherwise it isn't updated in the ai.
		}
		break;

	case NPC_TRANSFORMATION:
	case NPC_METAMORPHOSIS:
		if(md && md->skillidx >= 0) {
			int class_ = mob_random_class (md->db->skill[md->skillidx].val,0);
			if (skilllv > 1) //Multiply the rest of mobs. [Skotlex]
				mob_summonslave(md,md->db->skill[md->skillidx].val,skilllv-1,skillid);
			if (class_) mob_class_change(md, class_);
		}
		break;

	case NPC_EMOTION_ON:
	case NPC_EMOTION:
		if(md && md->skillidx >= 0)
		{
			clif_emotion(&md->bl,md->db->skill[md->skillidx].val[0]);
			if(!md->special_state.ai &&
				(md->db->skill[md->skillidx].val[1] || md->db->skill[md->skillidx].val[2]))
			//NPC_EMOTION & NPC_EMOTION_ON can change a mob's mode 'permanently' [Skotlex]
				//val[1] 'sets' the mode, val[2] can add/remove from the current mode based on skill used:
				//NPC_EMOTION_ON adds a mode / NPC_EMOTION removes it.
				sc_start4(src, type, 100, skilllv,
					md->db->skill[md->skillidx].val[1],
					skillid==NPC_EMOTION_ON?md->db->skill[md->skillidx].val[2]:0,
					skillid==NPC_EMOTION   ?md->db->skill[md->skillidx].val[2]:0,
					skill_get_time(skillid, skilllv));
		}
		break;

	case NPC_DEFENDER:
		clif_skill_nodamage(src,bl,skillid,skilllv,1);
		break;

	case NPC_POWERUP:
		sc_start(bl,SC_INCATKRATE,100,40*skilllv,skill_get_time(skillid, skilllv));
		clif_skill_nodamage(src,bl,skillid,skilllv,
			sc_start(bl,type,100,40*skilllv,skill_get_time(skillid, skilllv)));
		break;
		
	case NPC_AGIUP:
		sc_start(bl,SC_SPEEDUP1,100,skilllv,skill_get_time(skillid, skilllv));
		clif_skill_nodamage(src,bl,skillid,skilllv,
			sc_start(bl,type,100,40*skilllv,skill_get_time(skillid, skilllv)));
		break;

	case NPC_INVISIBLE:
		//On level 1, use level 10 cloaking (no speed penalty) 
		//with val4 passed as 1 is for "infinite cloak".
		clif_skill_nodamage(src,bl,skillid,skilllv,
			sc_start4(bl,type,100,9+skilllv,0,0,1,skill_get_time(skillid,skilllv)));
		break;
		
	case NPC_SIEGEMODE:
		// not sure what it does
		clif_skill_nodamage(src,bl,skillid,skilllv,1);
		break;

	case WE_MALE:
		{
			int hp_rate=(skilllv <= 0)? 0:skill_db[skillid].hp_rate[skilllv-1];
			int gain_hp= tstatus->max_hp*abs(hp_rate)/100; // The earned is the same % of the target HP than it costed the caster. [Skotlex]
			clif_skill_nodamage(src,bl,skillid,status_heal(bl, gain_hp, 0, 0),1);
		}
		break;
	case WE_FEMALE:
		{
			int sp_rate=(skilllv <= 0)? 0:skill_db[skillid].sp_rate[skilllv-1];
			int gain_sp=tstatus->max_sp*abs(sp_rate)/100;// The earned is the same % of the target SP than it costed the caster. [Skotlex]
			clif_skill_nodamage(src,bl,skillid,status_heal(bl, 0, gain_sp, 0),1);
		}
		break;

// parent-baby skills
	case WE_BABY:
		if(sd){
			struct map_session_data *f_sd = pc_get_father(sd);
			struct map_session_data *m_sd = pc_get_mother(sd);
			// if neither was found
			if(!f_sd && !m_sd){
				clif_skill_fail(sd,skillid,0,0);
				map_freeblock_unlock();
				return 0;
			}
			status_change_start(bl,SC_STUN,10000,skilllv,0,0,0,skill_get_time2(skillid,skilllv),8);
			if (f_sd) sc_start(&f_sd->bl,type,100,skilllv,skill_get_time(skillid,skilllv));
			if (m_sd) sc_start(&m_sd->bl,type,100,skilllv,skill_get_time(skillid,skilllv));
		}
		break;

	case PF_HPCONVERSION:
		{
			int hp, sp;
			hp = sstatus->max_hp/10;
			sp = hp * 10 * skilllv / 100;
			if (!status_charge(src,hp,0)) {
				if (sd) clif_skill_fail(sd,skillid,0,0);
				break;
			}
			clif_skill_nodamage(src, bl, skillid, skilllv, 1);
			status_heal(bl,0,sp,2);
		}
		break;
	case HT_REMOVETRAP:
		clif_skill_nodamage(src,bl,skillid,skilllv,1);
		{
			struct skill_unit *su=NULL;
			struct item item_tmp;
			int flag;
			if((bl->type==BL_SKILL) &&
			   (su=(struct skill_unit *)bl) &&
			   (su->group->src_id == src->id || map_flag_vs(bl->m)) &&
				(skill_get_inf2(su->group->skill_id) & INF2_TRAP))
			{	
				if(sd && !su->group->state.into_abyss)
				{	//Avoid collecting traps when it does not costs to place them down. [Skotlex]
					if(battle_config.skill_removetrap_type){
						for(i=0;i<10;i++) {
							if(skill_db[su->group->skill_id].itemid[i] > 0){
								memset(&item_tmp,0,sizeof(item_tmp));
								item_tmp.nameid = skill_db[su->group->skill_id].itemid[i];
								item_tmp.identify = 1;
								if(item_tmp.nameid && (flag=pc_additem(sd,&item_tmp,skill_db[su->group->skill_id].amount[i]))){
									clif_additem(sd,0,0,flag);
									map_addflooritem(&item_tmp,skill_db[su->group->skill_id].amount[i],sd->bl.m,sd->bl.x,sd->bl.y,NULL,NULL,NULL,0);
								}
							}
						}
					}else{
						memset(&item_tmp,0,sizeof(item_tmp));
						item_tmp.nameid = 1065;
						item_tmp.identify = 1;
						if(item_tmp.nameid && (flag=pc_additem(sd,&item_tmp,1))){
							clif_additem(sd,0,0,flag);
							map_addflooritem(&item_tmp,1,sd->bl.m,sd->bl.x,sd->bl.y,NULL,NULL,NULL,0);
						}
					}
				}
				skill_delunit(su);
			}
		}
		break;
	case HT_SPRINGTRAP:
		clif_skill_nodamage(src,bl,skillid,skilllv,1);
		{
			struct skill_unit *su=NULL;
			if((bl->type==BL_SKILL) && (su=(struct skill_unit *)bl) && (su->group) ){
				switch(su->group->unit_id){
					case UNT_ANKLESNARE:	// ankle snare
						if (su->group->val2 != 0)
							// if it is already trapping something don't spring it,
							// remove trap should be used instead
							break;
						// otherwise fallthrough to below
					case UNT_BLASTMINE:
					case UNT_SKIDTRAP:
					case UNT_LANDMINE:
					case UNT_SHOCKWAVE:
					case UNT_SANDMAN:
					case UNT_FLASHER:
					case UNT_FREEZINGTRAP:
					case UNT_CLAYMORETRAP:
					case UNT_TALKIEBOX:
						su->group->unit_id = UNT_USED_TRAPS;
						clif_changetraplook(bl, UNT_USED_TRAPS);
						su->group->limit=DIFF_TICK(tick+1500,su->group->tick);
						su->limit=DIFF_TICK(tick+1500,su->group->tick);
				}
			}
		}
		break;
	case BD_ENCORE:
		clif_skill_nodamage(src,bl,skillid,skilllv,1);
		if(sd)
			unit_skilluse_id(src,src->id,sd->skillid_dance,sd->skilllv_dance);
		break;

	case AS_SPLASHER:
		if(tstatus->max_hp*3/4 < tstatus->hp) {
			map_freeblock_unlock();
			return 1;
		}
		clif_skill_nodamage(src,bl,skillid,skilllv,
			sc_start4(bl,type,100,
				skilllv,skillid,src->id,skill_get_time(skillid,skilllv),1000));
		break;

	case PF_MINDBREAKER:
		{
			if(tstatus->mode&MD_BOSS || battle_check_undead(tstatus->race,tstatus->def_ele))
			{
				map_freeblock_unlock();
				return 1;
			}
	
			if (tsc && tsc->data[type].timer != -1)
			{	//HelloKitty2 (?) explained that this silently fails when target is
				//already inflicted. [Skotlex]
				map_freeblock_unlock();
				return 1;
			}

			//Has a 55% + skilllv*5% success chance.
			if (!clif_skill_nodamage(src,bl,skillid,skilllv,
				sc_start(bl,type,55+5*skilllv,skilllv,skill_get_time(skillid,skilllv))))
			{	
				if (sd) clif_skill_fail(sd,skillid,0,0);
				map_freeblock_unlock();
				return 0;
			}

			unit_skillcastcancel(bl,0);

			if(tsc && tsc->count){
				if(tsc->data[SC_FREEZE].timer!=-1)
					status_change_end(bl,SC_FREEZE,-1);
				if(tsc->data[SC_STONE].timer!=-1 && tsc->data[SC_STONE].val2==0)
					status_change_end(bl,SC_STONE,-1);
				if(tsc->data[SC_SLEEP].timer!=-1)
					status_change_end(bl,SC_SLEEP,-1);
			}

			if(dstmd)
				mob_target(dstmd,src,skill_get_range2(src,skillid,skilllv));
		}
		break;

	case PF_SOULCHANGE:
		{
			unsigned int sp1 = 0, sp2 = 0;
			if (dstmd) {
				if (dstmd->state.soul_change_flag) {
					if(sd) clif_skill_fail(sd,skillid,0,0);
					break;
				}
				dstmd->state.soul_change_flag = 1;
				sp2 = sstatus->max_sp * 3 /100;
				status_heal(src, 0, sp2, 2);
				clif_skill_nodamage(src,bl,skillid,skilllv,1);
				break;
			}
			sp1 = sstatus->sp;
			sp2 = tstatus->sp;
			status_set_sp(src, sp2, 3);
			status_set_sp(bl, sp1, 3);
			clif_skill_nodamage(src,bl,skillid,skilllv,1);
		}
		break;

	// Slim Pitcher (normally Condensed Potion doesn't give SP (Heals party members))
	case CR_SLIMPITCHER:
		if (potion_hp || potion_sp) {
			int hp = potion_hp, sp = potion_sp;
			hp = hp * (100 + (tstatus->vit<<1))/100;
			sp = sp * (100 + (tstatus->int_<<1))/100;

			if (dstsd) {
				if (hp)
					hp = hp * (100 + pc_checkskill(dstsd,SM_RECOVERY)*10)/100;
				if (sp)
					sp = sp * (100 + pc_checkskill(dstsd,MG_SRECOVERY)*10)/100;
			}
			if(hp > 0)
				clif_skill_nodamage(NULL,bl,AL_HEAL,hp,1);
			if(sp > 0)
				clif_skill_nodamage(NULL,bl,MG_SRECOVERY,sp,1);
			status_heal(bl,hp,sp,0);
		}
		break;
	// Full Chemical Protection
	case CR_FULLPROTECTION:
		{
			int i, skilltime;
			skilltime = skill_get_time(skillid,skilllv);
			if (!tsc) {
				clif_skill_nodamage(src,bl,skillid,skilllv,0);
				break;
			}
			for (i=0; i<4; i++) {
				if(tsc->data[SC_STRIPWEAPON + i].timer != -1)
					status_change_end(bl, SC_STRIPWEAPON + i, -1 );
				sc_start(bl,SC_CP_WEAPON + i,100,skilllv,skilltime);
			}
			clif_skill_nodamage(src,bl,skillid,skilllv,1);
		}
		break;

	case RG_CLEANER:	//AppleGirl
		clif_skill_nodamage(src,bl,skillid,skilllv,1);
		break;


	case PF_DOUBLECASTING:
		if (!clif_skill_nodamage(src,bl,skillid,skilllv,
			sc_start(bl,type,30+ 10*skilllv,skilllv,skill_get_time(skillid,skilllv))))
			if (sd) clif_skill_fail(sd,skillid,0,0);
		break;

	case CG_LONGINGFREEDOM:
		{
			if (tsc && tsc->data[SC_LONGING].timer == -1 && tsc->data[SC_DANCING].timer != -1 && tsc->data[SC_DANCING].val4
				&& tsc->data[SC_DANCING].val1 != CG_MOONLIT) //Can't use Longing for Freedom while under Moonlight Petals. [Skotlex]
			{
				clif_skill_nodamage(src,bl,skillid,skilllv,
					sc_start(bl,type,100,skilllv,skill_get_time(skillid,skilllv)));
			}
		}
		break;

	case CG_TAROTCARD:
		{
			int eff, count = -1;
			if (rand() % 100 > skilllv * 8) {
				if (sd) clif_skill_fail(sd,skillid,0,0);
				map_freeblock_unlock();
				return 0;
			}
			do {
				eff = rand() % 14;
				clif_specialeffect(bl, 523 + eff, AREA);
				switch (eff)
				{
				case 0:	// heals SP to 0
					status_percent_damage(src, bl, 0, 100);
					break;
				case 1:	// matk halved
					sc_start(bl,SC_INCMATKRATE,100,-50,skill_get_time2(skillid,skilllv));
					break;
				case 2:	// all buffs removed
					status_change_clear_buffs(bl,1);
					break;
				case 3:	// 1000 damage, random armor destroyed
					{
						int where[] = { EQP_ARMOR, EQP_SHIELD, EQP_HELM };
						status_fix_damage(src, bl, 1000, 0);
						clif_damage(src,bl,tick,0,0,1000,0,0,0);
						skill_break_equip(bl, where[rand()%3], 10000, BCT_ENEMY);
					}
					break;
				case 4:	// atk halved
					sc_start(bl,SC_INCATKRATE,100,-50,skill_get_time2(skillid,skilllv));
					break;
				case 5:	// 2000HP heal, random teleported
					status_heal(src, 2000, 0, 0);
					unit_warp(src, -1,-1,-1, 3);
					break;
				case 6:	// random 2 other effects
					if (count == -1)
						count = 3;
					else
						count++; //Should not retrigger this one.
					break;
				case 7:	// stop freeze or stoned
					{
						int sc[] = { SC_STOP, SC_FREEZE, SC_STONE };
						sc_start(bl,sc[rand()%3],100,skilllv,skill_get_time2(skillid,skilllv));
					}
					break;
				case 8:	// curse coma and poison
					sc_start(bl,SC_COMA,100,skilllv,skill_get_time2(skillid,skilllv));
					sc_start(bl,SC_CURSE,100,skilllv,skill_get_time2(skillid,skilllv));
					sc_start(bl,SC_POISON,100,skilllv,skill_get_time2(skillid,skilllv));
					break;
				case 9:	// confusion
					sc_start(bl,SC_CONFUSION,100,skilllv,skill_get_time2(skillid,skilllv));
					break;
				case 10:	// 6666 damage, atk matk halved, cursed
					status_fix_damage(src, bl, 6666, 0);
					clif_damage(src,bl,tick,0,0,6666,0,0,0);
					sc_start(bl,SC_INCATKRATE,100,-50,skill_get_time2(skillid,skilllv));
					sc_start(bl,SC_INCMATKRATE,100,-50,skill_get_time2(skillid,skilllv));
					sc_start(bl,SC_CURSE,skilllv,100,skill_get_time2(skillid,skilllv));
					break;
				case 11:	// 4444 damage
					status_fix_damage(src, bl, 4444, 0);
					clif_damage(src,bl,tick,0,0,4444,0,0,0);
					break;
				case 12:	// stun
					sc_start(bl,SC_STUN,100,skilllv,5000);
					break;
				case 13:	// atk,matk,hit,flee,def reduced
					sc_start(bl,SC_INCATKRATE,100,-20,skill_get_time2(skillid,skilllv));
					sc_start(bl,SC_INCMATKRATE,100,-20,skill_get_time2(skillid,skilllv));
					sc_start(bl,SC_INCHITRATE,100,-20,skill_get_time2(skillid,skilllv));
					sc_start(bl,SC_INCFLEERATE,100,-20,skill_get_time2(skillid,skilllv));
					sc_start(bl,SC_INCDEFRATE,100,-20,skill_get_time2(skillid,skilllv));
					break;
				default:
					break;			
				}			
			} while ((--count) > 0);
			clif_skill_nodamage(src,bl,skillid,skilllv,1);
		}
		break;

	case SL_ALCHEMIST:
	case SL_ASSASIN:
	case SL_BARDDANCER:
	case SL_BLACKSMITH:
	case SL_CRUSADER:
	case SL_HUNTER:
	case SL_KNIGHT:
	case SL_MONK:
	case SL_PRIEST:
	case SL_ROGUE:
	case SL_SAGE:
	case SL_SOULLINKER:
	case SL_STAR:
	case SL_SUPERNOVICE:
	case SL_WIZARD:
		if (sd && !(dstsd && (dstsd->class_&MAPID_UPPERMASK) == type)) {
			clif_skill_fail(sd,skillid,0,0);
			break;
		}
		clif_skill_nodamage(src,bl,skillid,skilllv,
			sc_start4(bl,SC_SPIRIT,100,skilllv,skillid,0,0,skill_get_time(skillid,skilllv)));
		sc_start(src,SC_SMA,100,skilllv,skill_get_time(SL_SMA,skilllv));
		break;
	case SL_HIGH:
		if (sd && !(dstsd && (dstsd->class_&JOBL_UPPER) && !(dstsd->class_&JOBL_2) && dstsd->status.base_level < 70)) {
			clif_skill_fail(sd,skillid,0,0);
			break;
		}
		clif_skill_nodamage(src,bl,skillid,skilllv,
			sc_start4(bl,type,100,skilllv,skillid,0,0,skill_get_time(skillid,skilllv)));
		sc_start(src,SC_SMA,100,skilllv,skill_get_time(SL_SMA,skilllv));
		break;

	case SL_SWOO:
		if (tsc && tsc->data[type].timer != -1) {
			sc_start(src,SC_STUN,100,skilllv,10000);
			break;
		}
	case SL_SKA: // [marquis007]
	case SL_SKE:
		if (sd && !battle_config.allow_es_magic_pc && bl->type != BL_MOB) {
			clif_skill_fail(sd,skillid,0,0);
			status_change_start(src,SC_STUN,10000,skilllv,0,0,0,500,10);
		} else
			clif_skill_nodamage(src,bl,skillid,skilllv,
				sc_start(bl,type,100,skilllv,skill_get_time(skillid,skilllv)));
		
		if (skillid == SL_SKE)
			sc_start(src,SC_SMA,100,skilllv,skill_get_time(SL_SMA,skilllv));

		break;
		
	// New guild skills [Celest]
	case GD_BATTLEORDER:
		if(flag&1) {
			if (status_get_guild_id(src) == status_get_guild_id(bl))
				sc_start(bl,type,100,skilllv,skill_get_time(skillid, skilllv));
		} else if (status_get_guild_id(src)) {
			clif_skill_nodamage(src,bl,skillid,skilllv,1);
			map_foreachinrange(skill_area_sub, src,
				skill_get_splash(skillid, skilllv), BL_CHAR,
				src,skillid,skilllv,tick, flag|BCT_GUILD|1,
				skill_castend_nodamage_id);
			if (sd)
				guild_block_skill(sd,skill_get_time2(skillid,skilllv));
		}
		break;
	case GD_REGENERATION:
		if(flag&1) {
			if (status_get_guild_id(src) == status_get_guild_id(bl))
				sc_start(bl,type,100,skilllv,skill_get_time(skillid, skilllv));
		} else if (status_get_guild_id(src)) {
			clif_skill_nodamage(src,bl,skillid,skilllv,1);
			map_foreachinrange(skill_area_sub, src,
				skill_get_splash(skillid, skilllv), BL_CHAR,
				src,skillid,skilllv,tick, flag|BCT_GUILD|1,
				skill_castend_nodamage_id);
			if (sd)
				guild_block_skill(sd,skill_get_time2(skillid,skilllv));
		}
		break;
	case GD_RESTORE:
		if(flag&1) {
			if (status_get_guild_id(src) == status_get_guild_id(bl))
				clif_skill_nodamage(src,bl,AL_HEAL,status_percent_heal(bl,90,90),1);
		} else if (status_get_guild_id(src)) {
			clif_skill_nodamage(src,bl,skillid,skilllv,1);
			map_foreachinrange(skill_area_sub, src,
				skill_get_splash(skillid, skilllv), BL_CHAR,
				src,skillid,skilllv,tick, flag|BCT_GUILD|1,
				skill_castend_nodamage_id);
			if (sd)
				guild_block_skill(sd,skill_get_time2(skillid,skilllv));
		}
		break;
	case GD_EMERGENCYCALL:
		{
			int dx[9]={-1, 1, 0, 0,-1, 1,-1, 1, 0};
			int dy[9]={ 0, 0, 1,-1, 1,-1,-1, 1, 0};
			int j = 0;
			struct guild *g = NULL;
			// i don't know if it actually summons in a circle, but oh well. ;P
			g = sd?sd->state.gmaster_flag:guild_search(status_get_guild_id(src));
			if (!g)
				break;
			for(i = 0; i < g->max_member; i++, j++) {
				if (j>8) j=0;
				if ((dstsd = g->member[i].sd) != NULL && sd != dstsd) {
					 if (map[dstsd->bl.m].flag.nowarp && !map_flag_gvg(dstsd->bl.m))
						 continue;
					clif_skill_nodamage(src,bl,skillid,skilllv,1);
					if(map_getcell(src->m,src->x+dx[j],src->y+dy[j],CELL_CHKNOREACH))
						dx[j] = dy[j] = 0;
					pc_setpos(dstsd, map[src->m].index, src->x+dx[j], src->y+dy[j], 2);
				}
			}
			if (sd)
				guild_block_skill(sd,skill_get_time2(skillid,skilllv));
		}
		break;

	case SG_FEEL:
		if (sd) {
			if(!sd->feel_map[skilllv-1].index) {
				//AuronX reported you CAN memorize the same map as all three. [Skotlex]
				clif_skill_nodamage(src,bl,skillid,skilllv,1);
				clif_parse_ReqFeel(sd->fd,sd, skilllv);
			}
			else
				clif_feel_info(sd, skilllv-1);
		}
		break;	

	case SG_HATE:
		if (sd) {
			clif_skill_nodamage(src,bl,skillid,skilllv,1);
			if(dstsd)  //PC
			{
				sd->hate_mob[skilllv-1] = dstsd->status.class_;
				pc_setglobalreg(sd,"PC_HATE_MOB_STAR",sd->hate_mob[skilllv-1]+1);
				clif_hate_mob(sd,skilllv,sd->hate_mob[skilllv-1]);
			}
			else if(dstmd) // mob
			{ 
				switch(skilllv)
				{
				case 1:
					if (tstatus->size==0)
					{
						sd->hate_mob[0] = dstmd->class_;
						pc_setglobalreg(sd,"PC_HATE_MOB_SUN",sd->hate_mob[0]+1);
						clif_hate_mob(sd,skilllv,sd->hate_mob[skilllv-1]);
					} else clif_skill_fail(sd,skillid,0,0);
					break;
				case 2:
					if (tstatus->size==1 && tstatus->max_hp>=6000)
					{
						sd->hate_mob[1] = dstmd->class_;
						pc_setglobalreg(sd,"PC_HATE_MOB_MOON",sd->hate_mob[1]+1);
						clif_hate_mob(sd,skilllv,sd->hate_mob[skilllv-1]);
					} else clif_skill_fail(sd,skillid,0,0);
					break;
				case 3:
					if (tstatus->size==2 && tstatus->max_hp>=20000)
					{
						sd->hate_mob[2] = dstmd->class_;
						pc_setglobalreg(sd,"PC_HATE_MOB_STAR",sd->hate_mob[2]+1);
						clif_hate_mob(sd,skilllv,sd->hate_mob[skilllv-1]);
					} else clif_skill_fail(sd,skillid,0,0);
					break;
				default:
					clif_skill_fail(sd,skillid,0,0);
					break;
				}
			}
		}
		break;

	case GS_GLITTERING:
		if(sd) {
			clif_skill_nodamage(src,bl,skillid,skilllv,1);
			if(rand()%100 < (50+10*skilllv))
				pc_addspiritball(sd,skill_get_time(skillid,skilllv),10);
			else if(sd->spiritball > 0)
				pc_delspiritball(sd,1,0);
		}
		break;
	
	case AM_REST:	//[orn]
		if (sd && !merc_hom_vaporize(sd,1))
			clif_skill_fail(sd,skillid,0,0);
		break;

	case HAMI_CASTLE:	//[orn]
		if(rand()%100 > 20*skilllv || src == bl)
			break; //Failed.
		{
			int x,y;
			x = src->x;
			y = src->y;

			if (unit_movepos(src,bl->x,bl->y,0,0)) {
				clif_skill_nodamage(src,bl,skillid,skilllv,1);
				clif_fixpos(src) ;
				if (unit_movepos(bl,x,y,0,0))
					clif_fixpos(bl) ;

				//TODO: Shouldn't also players and the like switch targets?
				map_foreachinrange(skill_chastle_mob_changetarget,src,
					AREA_SIZE, BL_MOB, bl, src);
			}
		}
		break;
	case HVAN_CHAOTIC:	//[orn]
		{
			if(hd){
				//HOM,PC,MOB
				struct block_list* heal_target=NULL;
				int heal = skill_calc_heal( src, 1+rand()%skilllv );
				static const int per[10][2]={{20,50},{50,60},{25,75},{60,64},{34,67},
											 {34,67},{34,67},{34,67},{34,67},{34,67}};
				int rnd = rand()%100;
				if(rnd<per[skilllv-1][0])
				{
					heal_target = &hd->bl;
				}else if(rnd<per[skilllv-1][1])
				{
					if(!status_isdead(&hd->master->bl))
						heal_target = &hd->master->bl;
					else
						heal_target = &hd->bl;
				}else{//MOB
					heal_target = map_id2bl(hd->target_id);
					if(heal_target==NULL)
						heal_target = &hd->bl;
				}
				clif_skill_nodamage(src,heal_target,AL_HEAL,heal,1);
				clif_skill_nodamage(src,heal_target,skillid,heal,1);
				status_heal(heal_target, heal, 0, 0);
				skill_blockmerc_start(hd, skillid, skill_get_time2(skillid,skilllv)) ;
			}
		}
		break;
	case HAMI_BLOODLUST:	//[orn]
	case HFLI_FLEET:	//[orn]
	case HFLI_SPEED:	//[orn]
		if ( hd ) {
			clif_skill_nodamage(src,bl,skillid,skilllv,
				sc_start(&hd->bl,type,100,skilllv,skill_get_time(skillid,skilllv))) ;
			skill_blockmerc_start(hd, skillid, skill_get_time2(skillid,skilllv)) ;
		}
		else
				clif_skill_fail(hd->master,skillid,0,0);
		break;
	case HLIF_CHANGE:	//[orn]
		if ( hd ) {
			clif_skill_nodamage(src,bl,skillid,skilllv,
				sc_start(&hd->bl,type,100,skilllv,skill_get_time(skillid,skilllv))) ;
			status_heal(&hd->bl, hd->master->homunculus.max_hp, 0, 0);
			skill_blockmerc_start(hd, skillid, skill_get_time2(skillid,skilllv)) ;
		}
		else
				clif_skill_fail(hd->master,skillid,0,0);
		break;

	default:
		ShowWarning("skill_castend_nodamage_id: Unknown skill used:%d\n",skillid);
		map_freeblock_unlock();
		return 1;
	}

	if (dstmd) //Mob skill event for no damage skills (damage ones are handled in battle_calc_damage) [Skotlex]
		mobskill_event(dstmd, src, tick, MSC_SKILLUSED|(skillid<<16));
	
	if (sd && !(flag&1) && sd->state.arrow_atk) //Consume arrow on last invocation to this skill.
		battle_consume_ammo(sd, skillid, skilllv);

	map_freeblock_unlock();
	return 0;
}

/*==========================================
 *
 *------------------------------------------
 */
int skill_castend_id (int tid, unsigned int tick, int id, int data)
{
	struct block_list *target, *src = map_id2bl(id);
	struct map_session_data* sd = NULL;
	struct homun_data* hd = NULL;	//[orn]
	struct mob_data* md = NULL;
	struct unit_data* ud = unit_bl2ud(src);
	struct status_change *sc = NULL;
	int inf,inf2;

	nullpo_retr(0, ud);

	BL_CAST( BL_PC,  src, sd);
	BL_CAST( BL_HOM,  src, hd);	//[orn]
	BL_CAST( BL_MOB, src, md);

	if( src->prev == NULL ) {
		ud->skilltimer = -1;
		return 0;
	}

	switch (ud->skillid) {
		//These should become skill_castend_pos
		case WE_CALLPARTNER:
		case WE_CALLPARENT:
		case WE_CALLBABY:
		case AM_CALLHOMUN:	
		case AM_RESURRECTHOMUN:
			//Find a random spot to place the skill. [Skotlex]
			inf2 = skill_get_splash(ud->skillid, ud->skilllv);
			ud->skillx = src->x + inf2;
			ud->skilly = src->y + inf2;
			if (!map_random_dir(src, &ud->skillx, &ud->skilly)) {
				ud->skillx = src->x;
				ud->skilly = src->y;
			}
			return skill_castend_pos(tid,tick,id,data);
	}

	if(ud->skillid != SA_CASTCANCEL ) {
		if( ud->skilltimer != tid ) {
			ShowError("skill_castend_id: Timer mismatch %d!=%d!\n", ud->skilltimer, tid);
			ud->skilltimer = -1;
			return 0;
		}
		if( sd && ud->skilltimer != -1 && pc_checkskill(sd,SA_FREECAST))
			status_freecast_switch(sd);
		ud->skilltimer=-1;
	}

	if (ud->skilltarget == id)
		target = src;
	else
		target = map_id2bl(ud->skilltarget);

	// Use a do so that you can break out of it when the skill fails.
	do {
		if(!target || target->prev==NULL) break;

		if(src->m != target->m || status_isdead(src)) break;

		if(ud->skillid == RG_BACKSTAP) {
			int dir = map_calc_dir(src,target->x,target->y),t_dir = unit_getdir(target);
			if(check_distance_bl(src, target, 0) || map_check_dir(dir,t_dir)) {
				break;
			}
		}
		if (ud->skillid == PR_LEXDIVINA)
		{
			sc = status_get_sc(target);
			if (battle_check_target(src,target, BCT_ENEMY)<=0 &&
				(!sc || sc->data[SC_SILENCE].timer == -1))
			{	//If it's not an enemy, and not silenced, you can't use the skill on them. [Skotlex]
				clif_skill_nodamage (src, target, ud->skillid, ud->skilllv, 0);
				break;
			}
		} else {
			//Check target validity.
			inf = skill_get_inf(ud->skillid);
			inf2 = skill_get_inf2(ud->skillid);

			if(inf&INF_ATTACK_SKILL ||
				(inf&INF_SELF_SKILL && inf2&INF2_NO_TARGET_SELF)) //Combo skills
				inf = BCT_ENEMY; //Offensive skill.
			else
				inf = 0;

			if(inf2 & (INF2_PARTY_ONLY|INF2_GUILD_ONLY) && src != target)
				inf |= 	
					(inf2&INF2_PARTY_ONLY?BCT_PARTY:0)|
					(inf2&INF2_GUILD_ONLY?BCT_GUILD:0)|
					(inf2&INF2_ALLOW_ENEMY?BCT_ENEMY:0);

			if (inf && battle_check_target(src, target, inf) <= 0)
				break;

			if(inf&BCT_ENEMY && (sc = status_get_sc(target)) &&
				sc->count && sc->data[SC_FOGWALL].timer != -1 &&
				rand()%100 < 75)
		  	{	//Fogwall makes all offensive-type targetted skills fail at 75%
				if (sd) clif_skill_fail(sd,ud->skillid,0,0);
				break;
			}
		}
		//Avoid doing double checks for instant-cast skills.
		if (tid != -1 && !status_check_skilluse(src, target, ud->skillid, 1))
			break;

		if(md) {
			if(ud->skillid != NPC_EMOTION)//Set afterskill delay.
				md->last_thinktime=tick + (tid==-1?md->status.adelay:md->status.amotion);
			if(md->skillidx >= 0) {
				md->skilldelay[md->skillidx]=tick;
				if (md->db->skill[md->skillidx].emotion >= 0)
					clif_emotion(src, md->db->skill[md->skillidx].emotion);
			}
		}

		if(src != target && battle_config.skill_add_range &&
			!check_distance_bl(src, target, skill_get_range2(src,ud->skillid,ud->skilllv)+battle_config.skill_add_range))
		{
			if (sd) {
				clif_skill_fail(sd,ud->skillid,0,0);
				if(battle_config.skill_out_range_consume) //Consume items anyway. [Skotlex]
					skill_check_condition(sd,ud->skillid, ud->skilllv,1);
			}
			break;
		}

		if(sd && !skill_check_condition(sd,ud->skillid, ud->skilllv,1))
			break;
			
		if(hd && !skill_check_condition_hom(hd,ud->skillid, ud->skilllv,1))	//[orn]
			break;
			
		if (ud->walktimer != -1 && ud->skillid != TK_RUN)
			unit_stop_walking(src,1);
		
		if (ud->skillid == SA_MAGICROD)
			ud->canact_tick = tick;
		else
			ud->canact_tick = tick + skill_delayfix(src, ud->skillid, ud->skilllv);
	
		if (skill_get_state(ud->skillid) != ST_MOVE_ENABLE)
			unit_set_walkdelay(src, tick, battle_config.default_skill_delay+skill_get_walkdelay(ud->skillid, ud->skilllv), 1);
		
		if(battle_config.skill_log && battle_config.skill_log&src->type)
			ShowInfo("Type %d, ID %d skill castend id [id =%d, lv=%d, target ID %d)\n",
				src->type, src->id, ud->skillid, ud->skilllv, target->id);
		if (skill_get_casttype(ud->skillid) == CAST_NODAMAGE)
			skill_castend_nodamage_id(src,target,ud->skillid,ud->skilllv,tick,0);
		else
			skill_castend_damage_id(src,target,ud->skillid,ud->skilllv,tick,0);

		sc = status_get_sc(src);
		if(sc && sc->count && sc->data[SC_MAGICPOWER].timer != -1 && ud->skillid != HW_MAGICPOWER && ud->skillid != WZ_WATERBALL)
			status_change_end(src,SC_MAGICPOWER,-1);		

		if (ud->skilltimer == -1) {
			if(md) md->skillidx = -1;
			else ud->skillid = 0; //mobs can't clear this one as it is used for skill condition 'afterskill'
			ud->skilllv = ud->skilltarget = 0;
		}
		return 1;
	} while(0);
	//Skill failed.
	if (ud->skillid == MO_EXTREMITYFIST && sd &&
		!(sc && sc->count && sc->data[SC_FOGWALL].timer != -1))
  	{	//When Asura fails... (except when it fails from Fog of Wall)
		//Consume SP/spheres
		skill_check_condition(sd,ud->skillid, ud->skilllv,1);
		status_set_sp(src, 0, 0);
		sc = &sd->sc;
		if (sc->count)
		{	//End states
			if (sc->data[SC_EXPLOSIONSPIRITS].timer != -1)
				status_change_end(src, SC_EXPLOSIONSPIRITS, -1);
			if (sc->data[SC_BLADESTOP].timer != -1)
				status_change_end(src,SC_BLADESTOP,-1);
		}
		if (target && target->m == src->m)
		{	//Move character to target anyway.
			int dx,dy;
			dx = target->x - src->x;
			dy = target->y - src->y;
			if(dx > 0) dx++;
			else if(dx < 0) dx--;
			if (dy > 0) dy++;
			else if(dy < 0) dy--;
			
			if (unit_movepos(src, src->x+dx, src->y+dy, 1, 1))
			{	//Display movement + animation.
				clif_slide(src,src->x,src->y);
				clif_skill_damage(src,target,tick,sd->battle_status.amotion,0,0,1,ud->skillid, ud->skilllv, 5);
			}
			clif_skill_fail(sd,ud->skillid,0,0);
		}
	}
	ud->skillid = ud->skilllv = ud->skilltarget = 0;
	ud->canact_tick = tick;
	if(sd) sd->skillitem = sd->skillitemlv = -1;
	if(md) md->skillidx  = -1;
	return 0;
}

/*==========================================
 * 
 *------------------------------------------
 */
int skill_castend_pos (int tid, unsigned int tick, int id, int data)
{
	struct block_list* src = map_id2bl(id);
	int maxcount;
	struct map_session_data *sd = NULL;
	struct homun_data *hd = NULL;	//[orn]
	struct unit_data *ud = unit_bl2ud(src);
	struct mob_data *md = NULL;

	nullpo_retr(0, ud);

	BL_CAST( BL_PC , src, sd);
	BL_CAST( BL_HOM , src, hd);	//[orn]
	BL_CAST( BL_MOB, src, md);

	if( src->prev == NULL ) {
		ud->skilltimer = -1;
		return 0;
	}
	
	if( ud->skilltimer != tid )
	{
		ShowError("skill_castend_pos: Timer mismatch %d!=%d\n", ud->skilltimer, tid);
		ud->skilltimer = -1;
		return 0;
	}

	if(sd && ud->skilltimer != -1 && pc_checkskill(sd,SA_FREECAST))
		status_freecast_switch(sd);

	ud->skilltimer=-1;
	do {
		if(status_isdead(src)) break;

		if (!(battle_config.skill_reiteration && src->type&battle_config.skill_reiteration) &&
			skill_get_unit_flag(ud->skillid)&UF_NOREITERATION &&
			skill_check_unit_range(src,ud->skillx,ud->skilly,ud->skillid,ud->skilllv)
		)
			break;

		if (battle_config.skill_nofootset && src->type&battle_config.skill_nofootset &&
			skill_get_unit_flag(ud->skillid)&UF_NOFOOTSET &&
			skill_check_unit_range2(src,ud->skillx,ud->skilly,ud->skillid,ud->skilllv)
		)
			break;
		
		if(battle_config.land_skill_limit && src->type&battle_config.land_skill_limit &&
			(maxcount = skill_get_maxcount(ud->skillid)) > 0
		  ) {
			int i;
			for(i=0;i<MAX_SKILLUNITGROUP && ud->skillunit[i] && maxcount;i++) {
				if(ud->skillunit[i]->skill_id == ud->skillid)
					maxcount--;
			}
			if(!maxcount)
				break;
		}

		if(tid != -1)
		{	//Avoid double checks on instant cast skills. [Skotlex]
			if (!status_check_skilluse(src, NULL, ud->skillid, 1))
				break;
			if(battle_config.skill_add_range &&
				!check_distance_blxy(src, ud->skillx, ud->skilly, skill_get_range2(src,ud->skillid,ud->skilllv)+battle_config.skill_add_range)) {
				if (sd && battle_config.skill_out_range_consume) //Consume items anyway.
					skill_check_condition(sd,ud->skillid, ud->skilllv,1);
				break;
			}
		}
			
		if(sd && !skill_check_condition(sd,ud->skillid, ud->skilllv, 1))
			break;

		if(hd && !skill_check_condition_hom(hd,ud->skillid, ud->skilllv, 1))	//[orn]
			break;

		if(md) {
			md->last_thinktime=tick + (tid==-1?md->status.adelay:md->status.amotion);
			if(md->skillidx >= 0) {
				md->skilldelay[md->skillidx]=tick;
				if (md->db->skill[md->skillidx].emotion >= 0)
					clif_emotion(src, md->db->skill[md->skillidx].emotion);
			}
		}

		if(battle_config.skill_log && battle_config.skill_log&src->type)
			ShowInfo("Type %d, ID %d skill castend pos [id =%d, lv=%d, (%d,%d)]\n",
				src->type, src->id, ud->skillid, ud->skilllv, ud->skillx, ud->skilly);
		unit_stop_walking(src,1);
		ud->canact_tick = tick + skill_delayfix(src, ud->skillid, ud->skilllv);
		unit_set_walkdelay(src, tick, battle_config.default_skill_delay+skill_get_walkdelay(ud->skillid, ud->skilllv), 1);
		skill_castend_pos2(src,ud->skillx,ud->skilly,ud->skillid,ud->skilllv,tick,0);

		if (ud->skilltimer == -1) {
			if (md) md->skillidx = -1;
			else ud->skillid = 0; //Non mobs can't clear this one as it is used for skill condition 'afterskill'
			ud->skilllv = ud->skillx = ud->skilly = 0;
		}
		return 1;
	} while(0);

	ud->canact_tick = tick;
	ud->skillid = ud->skilllv = 0;
	if(sd) {
		clif_skill_fail(sd,ud->skillid,0,0);
		sd->skillitem = sd->skillitemlv = -1;
	}
	if(md) md->skillidx  = -1;
	return 0;

}

/*==========================================
 *
 *------------------------------------------
 */
int skill_castend_pos2 (struct block_list *src, int x, int y, int skillid, int skilllv, unsigned int tick, int flag)
{
	struct map_session_data *sd=NULL;
	struct status_change *sc;
	struct skill_unit_group *sg;
	int i,type;

	//if(skilllv <= 0) return 0;
	if(skillid > 0 && skilllv <= 0) return 0;	// celest

	nullpo_retr(0, src);

	if(status_isdead(src))
		return 0;

	if(src->type==BL_PC)
		sd=(struct map_session_data *)src;

	sc = status_get_sc(src);
	if (sc && !sc->count)
		sc = NULL; //Unneeded.
	type = SkillStatusChangeTable(skillid);

	switch (skillid) { //Skill effect.
		case WZ_METEOR:
		case MO_BODYRELOCATION:
		case CR_CULTIVATION:
		case HW_GANBANTEIN:
			break; //Effect is displayed on respective switch case.
		default:
			if(skill_get_inf(skillid)&INF_SELF_SKILL)
				clif_skill_nodamage(src,src,skillid,skilllv,1);
			else
				clif_skill_poseffect(src,skillid,skilllv,x,y,tick);
	}
	switch(skillid)
	{
	case PR_BENEDICTIO:
		skill_area_temp[1] = src->id;
		i = skill_get_splash(skillid, skilllv);
		map_foreachinarea(skill_area_sub, 
			src->m, x-i, y-i, x+i, y+i, BL_PC,
			src, skillid, skilllv, tick, flag|BCT_ALL|1,
			skill_castend_nodamage_id);
		map_foreachinarea(skill_area_sub,
			src->m, x-i, y-i, x+i, y+i, BL_CHAR,
			src, skillid, skilllv, tick, flag|BCT_ENEMY|1,
			skill_castend_damage_id);
		break;

	case BS_HAMMERFALL:
		i = skill_get_splash(skillid, skilllv);
		map_foreachinarea (skill_area_sub,
			src->m, x-i, y-i, x+i, y+i, BL_CHAR,
			src, skillid, skilllv, tick, flag|BCT_ENEMY|2,
			skill_castend_nodamage_id);
		break;

	case HT_DETECTING:
		i = skill_get_splash(skillid, skilllv);
		map_foreachinarea( status_change_timer_sub,
			src->m, x-i, y-i, x+i,y+i,BL_CHAR,
			src,status_get_sc(src),SC_SIGHT,tick);
		if(battle_config.traps_setting&1)
		map_foreachinarea( skill_reveal_trap,
			src->m, x-i, y-i, x+i,y+i,BL_SKILL);
		break;

	case MG_SAFETYWALL:
	case MG_FIREWALL:	
	case MG_THUNDERSTORM:
	case AL_PNEUMA:
	case WZ_ICEWALL:
	case WZ_FIREPILLAR:
	case WZ_QUAGMIRE:
	case WZ_VERMILION:
	case WZ_STORMGUST:
	case WZ_HEAVENDRIVE:
	case PR_SANCTUARY:
	case PR_MAGNUS:
	case CR_GRANDCROSS:
	case NPC_GRANDDARKNESS:
	case HT_SKIDTRAP:
	case HT_LANDMINE:
	case HT_ANKLESNARE:
	case HT_SHOCKWAVE:
	case HT_SANDMAN:
	case HT_FLASHER:
	case HT_FREEZINGTRAP:
	case HT_BLASTMINE:
	case HT_CLAYMORETRAP:
	case AS_VENOMDUST:
	case AM_DEMONSTRATION:
	case PF_FOGWALL:
	case PF_SPIDERWEB:
	case HT_TALKIEBOX:
	case WE_CALLPARTNER:
	case WE_CALLPARENT:
	case WE_CALLBABY:
	case AC_SHOWER:	//Ground-placed skill implementation.
	case SA_VOLCANO:
	case SA_DELUGE:
	case SA_VIOLENTGALE:
	case SA_LANDPROTECTOR:
	case BD_LULLABY:
	case BD_RICHMANKIM:
	case BD_ETERNALCHAOS:
	case BD_DRUMBATTLEFIELD:
	case BD_RINGNIBELUNGEN:
	case BD_ROKISWEIL:
	case BD_INTOABYSS:
	case BD_SIEGFRIED:
	case BA_DISSONANCE:
	case BA_POEMBRAGI:
	case BA_WHISTLE:
	case BA_ASSASSINCROSS:
	case BA_APPLEIDUN:
	case DC_UGLYDANCE:
	case DC_HUMMING:
	case DC_DONTFORGETME:
	case DC_FORTUNEKISS:
	case DC_SERVICEFORYOU:
	case GS_DESPERADO:
	case NJ_KAENSIN:
	case NJ_BAKUENRYU:
	case NJ_SUITON:
	case NJ_HYOUSYOURAKU:
	case NJ_RAIGEKISAI:
	case NJ_KAMAITACHI:
		flag|=1;//Set flag to 1 to prevent deleting ammo (it will be deleted on group-delete).
	case GS_GROUNDDRIFT: //Ammo should be deleted right away.
		skill_unitsetting(src,skillid,skilllv,x,y,0);
		break;
	case RG_GRAFFITI:			/* Graffiti [Valaris] */
		skill_clear_unitgroup(src);
		skill_unitsetting(src,skillid,skilllv,x,y,0);
		flag|=1;
		break;
	case HP_BASILICA:
		skill_clear_unitgroup(src);
		sg = skill_unitsetting(src,skillid,skilllv,x,y,0);
		sc_start(src,type,100,skilllv,skill_get_time(skillid,skilllv));
		flag|=1;
		break;
	case CG_HERMODE:
		skill_clear_unitgroup(src);
		sg = skill_unitsetting(src,skillid,skilllv,x,y,0);
		sc_start4(src,SC_DANCING,100,
			skillid,0,0,sg->group_id,skill_get_time(skillid,skilllv));
		flag|=1;
		break;
	case RG_CLEANER: // [Valaris]
		i = skill_get_splash(skillid, skilllv);
		map_foreachinarea(skill_graffitiremover,src->m,x-i,y-i,x+i,y+i,BL_SKILL);
		break;

	case WZ_METEOR:
		{
			int flag=0, area = skill_get_splash(skillid, skilllv);
			short tmpx, tmpy, x1 = 0, y1 = 0;
			if (sc && sc->data[SC_MAGICPOWER].timer != -1)
				flag = flag|2; //Store the magic power flag for future use. [Skotlex]
			for(i=0;i<2+(skilllv>>1);i++) {
				tmpx = x;
				tmpy = y;
				if (!map_search_freecell(NULL, src->m, &tmpx, &tmpy, area, area, 1))
					continue;
				if(!(flag&1)){
					clif_skill_poseffect(src,skillid,skilllv,tmpx,tmpy,tick);
					flag=flag|1;
				}
				if(i > 0)
					skill_addtimerskill(src,tick+i*1000,0,tmpx,tmpy,skillid,skilllv,(x1<<16)|y1,flag&2); //Only pass the Magic Power flag
				x1 = tmpx;
				y1 = tmpy;
			}
			skill_addtimerskill(src,tick+i*1000,0,tmpx,tmpy,skillid,skilllv,-1,flag&2); //Only pass the Magic Power flag
		}
		break;

	case AL_WARP:
		if(sd) {
			clif_skill_warppoint(sd,skillid,skilllv,mapindex_id2name(sd->status.save_point.map),
				(skilllv>1 && sd->status.memo_point[0].map)?mapindex_id2name(sd->status.memo_point[0].map):"",
				(skilllv>2 && sd->status.memo_point[1].map)?mapindex_id2name(sd->status.memo_point[1].map):"",
				(skilllv>3 && sd->status.memo_point[2].map)?mapindex_id2name(sd->status.memo_point[2].map):"");
		}
		break;

	case MO_BODYRELOCATION:
		if (unit_movepos(src, x, y, 1, 1)) {
			clif_skill_poseffect(src,skillid,skilllv,src->x,src->y,tick);
//			clif_slide(src, src->x, src->y); //Poseffect is the one that makes the char snap on the client...
			if (sd) skill_blockpc_start (sd, MO_EXTREMITYFIST, 2000);
		}
		break;
	case NJ_SHADOWJUMP:
	{
		unit_movepos(src, x, y, 1, 0);
		unit_setdir(src, (unit_getdir(src) + 4)%8);
		clif_slide(src,x,y);

		if (sc && sc->data[SC_HIDING].timer != -1)
			status_change_end(src, SC_HIDING, -1);
	}
		break;
	case AM_SPHEREMINE:
	case AM_CANNIBALIZE:
		if(sd) {
			int summons[5] = { 1020, 1068, 1118, 1500, 1368 };
			int class_ = skillid==AM_SPHEREMINE?1142:summons[skilllv-1];
			struct mob_data *md;

			// Correct info, don't change any of this! [celest]
			md = mob_once_spawn_sub(src, src->m, x, y, sd->status.name,class_,"");
			if (md) {
				md->master_id = src->id;
				md->special_state.ai = skillid==AM_SPHEREMINE?2:3;
				md->deletetimer = add_timer (gettick() + skill_get_time(skillid,skilllv), mob_timer_delete, md->bl.id, 0);
				mob_spawn (md); //Now it is ready for spawning.
			}
		}
		break;

	// Slim Pitcher [Celest] (normally Condensed Potion doesn't give SP (Heals party members))
	case CR_SLIMPITCHER:
		if (sd) {
			int i = skilllv%11 - 1;
			int j = pc_search_inventory(sd,skill_db[skillid].itemid[i]);
			if(j < 0 || skill_db[skillid].itemid[i] <= 0 || sd->inventory_data[j] == NULL ||
				sd->status.inventory[j].amount < skill_db[skillid].amount[i]) {
				clif_skill_fail(sd,skillid,0,0);
				return 1;
			}
			potion_flag = 1;
			potion_hp = 0;
			potion_sp = 0;
			run_script(sd->inventory_data[j]->script,0,sd->bl.id,0);
			pc_delitem(sd,j,skill_db[skillid].amount[i],0);
			potion_flag = 0;
			//Apply skill bonuses
			i = pc_checkskill(sd,CR_SLIMPITCHER)*10
				+ pc_checkskill(sd,AM_POTIONPITCHER)*10
				+ pc_checkskill(sd,AM_LEARNINGPOTION)*5;

			potion_hp = potion_hp * (100+i)/100;
			potion_sp = potion_sp * (100+i)/100;
			
			if(potion_hp > 0 || potion_sp > 0) {
				i = skill_get_splash(skillid, skilllv);
				map_foreachinarea(skill_area_sub,
					src->m,x-i,y-i,x+i,y+i,BL_CHAR,
					src,skillid,skilllv,tick,flag|BCT_PARTY|BCT_GUILD|1,
					skill_castend_nodamage_id);
			}
		} else {
			int i = skilllv%11 - 1;
			struct item_data *item = itemdb_search(i);
			i = skill_db[skillid].itemid[i];
			item = itemdb_search(i);
			potion_flag = 1;
			potion_hp = 0;
			potion_sp = 0;
			run_script(item->script,0,src->id,0);
			potion_flag = 0;
			i = skill_get_max(CR_SLIMPITCHER)*10;

			potion_hp = potion_hp * (100+i)/100;
			potion_sp = potion_sp * (100+i)/100;
			
			if(potion_hp > 0 || potion_sp > 0) {
				i = skill_get_splash(skillid, skilllv);
				map_foreachinarea(skill_area_sub,
					src->m,x-i,y-i,x+i,y+i,BL_CHAR,
					src,skillid,skilllv,tick,flag|BCT_PARTY|BCT_GUILD|1,
						skill_castend_nodamage_id);
			}
		}
		break;

	case HW_GANBANTEIN:
		if (rand()%100 < 80) {
			clif_skill_poseffect(src,skillid,skilllv,x,y,tick);
			i = skill_get_splash(skillid, skilllv);
			map_foreachinarea (skill_ganbatein, src->m, x-i, y-i, x+i, y+i, BL_SKILL);
		} else {
			if (sd) clif_skill_fail(sd,skillid,0,0);
			return 1;
		}
		break;
	
	case HW_GRAVITATION:
		sg = skill_unitsetting(src,skillid,skilllv,x,y,0);	
		sc_start4(src,type,100,skilllv,0,BCT_SELF,(int)sg,skill_get_time(skillid,skilllv));
		flag|=1;
		break;

	// Plant Cultivation [Celest]
	case CR_CULTIVATION:
		if (sd) {
			int i = skilllv - 1;
			int j = pc_search_inventory(sd,skill_db[skillid].itemid[i]);
			if(j < 0 || skill_db[skillid].itemid[i] <= 0 || sd->inventory_data[j] == NULL ||
				sd->status.inventory[j].amount < skill_db[skillid].amount[i]) {
				clif_skill_fail(sd,skillid,0,0);
				return 1;
			}
			pc_delitem(sd,j,skill_db[skillid].amount[i],0);
			clif_skill_poseffect(src,skillid,skilllv,x,y,tick);
			if (rand()%100 < 50)
				mob_once_spawn(sd, "this", x, y, "--ja--",(skilllv < 2 ? 1084+rand()%2 : 1078+rand()%6), 1, "");
			else
				clif_skill_fail(sd,skillid,0,0);
		}
		break;

	case SG_SUN_WARM:
	case SG_MOON_WARM:
	case SG_STAR_WARM:
		sg = skill_unitsetting(src,skillid,skilllv,src->x,src->y,0);
		sc_start4(src,type,100,skilllv,0,0,(int)sg,skill_get_time(skillid,skilllv));
		flag|=1;
		break;

	case PA_GOSPEL:
		if (sc && sc->data[type].timer != -1 && sc->data[type].val4 == BCT_SELF)
			status_change_end(src,SC_GOSPEL,-1);
		else
	  	{
			sg = skill_unitsetting(src,skillid,skilllv,src->x,src->y,0);
			if (sc && sc->data[type].timer != -1)
				status_change_end(src,type,-1); //Was under someone else's Gospel. [Skotlex]
			sc_start4(src,type,100,skilllv,0,(int)sg,BCT_SELF,skill_get_time(skillid,skilllv));
		}
		break;
	case NJ_TATAMIGAESHI:
		sc_start(src,type,100,skilllv,skill_get_time2(skillid,skilllv));
		skill_unitsetting(src,skillid,skilllv,src->x,src->y,0);
		break;

	case AM_CALLHOMUN:	//[orn]
		if (sd && !merc_call_homunculus(sd, x, y))
			clif_skill_fail(sd,skillid,0,0);
		break;

	case AM_RESURRECTHOMUN:	//[orn]
		if (sd)
		{
			if (map_flag_gvg(src->m) || //No reviving in WoE grounds!
				!merc_revive_homunculus(sd, 10*skilllv, x, y))
			{
				clif_skill_fail(sd,skillid,0,0);
				break;
			}
		}
		break;

	default:
		ShowWarning("skill_castend_pos2: Unknown skill used:%d\n",skillid);
		return 1;
	}

	if (sc && sc->data[SC_MAGICPOWER].timer != -1)
		status_change_end(&sd->bl,SC_MAGICPOWER,-1);

	if (sd && !(flag&1) && sd->state.arrow_atk) //Consume arrow if a ground skill was not invoked. [Skotlex]
		battle_consume_ammo(sd, skillid, skilllv);
		
	return 0;
}

/*==========================================
 * 
 *------------------------------------------
 */
int skill_castend_map (struct map_session_data *sd, int skill_num, const char *map)
{
	int x=0,y=0;

	nullpo_retr(0, sd);

//Simplify skill_failed code.
#define skill_failed(sd) { sd->menuskill_id = sd->menuskill_lv = 0; }

	if( sd->bl.prev == NULL || pc_isdead(sd) )
		return 0;

	if(sd->sc.opt1 || sd->sc.option&OPTION_HIDE ) {
		skill_failed(sd);
		return 0;
	}
	if(sd->sc.count && (
		sd->sc.data[SC_SILENCE].timer!=-1 ||
		sd->sc.data[SC_ROKISWEIL].timer!=-1 ||
		sd->sc.data[SC_AUTOCOUNTER].timer != -1 ||
		sd->sc.data[SC_STEELBODY].timer != -1 ||
		sd->sc.data[SC_DANCING].timer!=-1 ||
		sd->sc.data[SC_BERSERK].timer != -1 ||
		sd->sc.data[SC_MARIONETTE].timer != -1
	 ))
		return 0;

	if( skill_num != sd->menuskill_id) 
		return 0;

	if (strlen(map) > MAP_NAME_LENGTH-1)
	{	//Map_length check, as it is sent by the client and we shouldn't trust it [Skotlex]
		if (battle_config.error_log)
			ShowError("skill_castend_map: Received map name '%s' too long!\n", map);
		skill_failed(sd);
		return 0;
	}

	pc_stop_attack(sd);
	pc_stop_walking(sd,0);

	if(battle_config.skill_log && battle_config.skill_log&BL_PC)
		ShowInfo("PC %d skill castend skill =%d map=%s\n",sd->bl.id,skill_num,map);

	if(strcmp(map,"cancel")==0) {
		skill_failed(sd);
		return 0;
	}
	
	switch(skill_num){
	case AL_TELEPORT:
		if(strcmp(map,"Random")==0)
			pc_randomwarp(sd,3);
		else if (sd->menuskill_lv > 1) //Need lv2 to be able to warp here.
			pc_setpos(sd,sd->status.save_point.map,
				sd->status.save_point.x,sd->status.save_point.y,3);
		break;

	case AL_WARP:
		{
			const struct point *p[4];
			struct skill_unit_group *group;
			int i, lv, wx, wy;
			int maxcount=0;
			unsigned short mapindex;
			mapindex  = mapindex_name2id((char*)map);
			if(!mapindex) { //Given map not found?
				clif_skill_fail(sd,skill_num,0,0);
				skill_failed(sd);
				return 0;
			}
			p[0] = &sd->status.save_point;
			p[1] = &sd->status.memo_point[0];
			p[2] = &sd->status.memo_point[1];
			p[3] = &sd->status.memo_point[2];

			if((maxcount = skill_get_maxcount(skill_num)) > 0) {
				for(i=0;i<MAX_SKILLUNITGROUP && sd->ud.skillunit[i] && maxcount;i++) {
					if(sd->ud.skillunit[i]->skill_id == skill_num)
						maxcount--;
				}
				if(!maxcount) {
					clif_skill_fail(sd,skill_num,0,0);
					skill_failed(sd);
					return 0;
				}
			}
		
			//When it's an item-used warp-portal, the skill-lv used is lost.. assume max level.
			lv = sd->skillitem==skill_num?skill_get_max(skill_num):pc_checkskill(sd,skill_num);
			wx = sd->menuskill_lv>>16;
			wy = sd->menuskill_lv&0xffff;
			
			if(lv <= 0) return 0;
			for(i=0;i<lv;i++){
				if(mapindex == p[i]->map){
					x=p[i]->x;
					y=p[i]->y;
					break;
				}
			}
			if(x==0 || y==0) {
				skill_failed(sd);
				return 0;
			}

			if(!skill_check_condition(sd, sd->menuskill_id, lv,3)) //This checks versus skillid/skilllv...
			{
				skill_failed(sd);
				return 0;
			}
			
			if(skill_check_unit_range2(&sd->bl,wx,wy,skill_num,lv) > 0) {
				clif_skill_fail(sd,0,0,0);
				skill_failed(sd);
				return 0;
			}
			if((group=skill_unitsetting(&sd->bl,skill_num,lv,wx,wy,0))==NULL) {
				skill_failed(sd);
				return 0;
			}
			//Now that there's a mapindex, use that in val3 rather than a string. [Skotlex]
			group->val2=(x<<16)|y;
			group->val3 = mapindex;
		}
		break;
	}

	sd->menuskill_id = sd->menuskill_lv = 0;
	return 0;
#undef skill_failed
}

static int skill_dance_overlap_sub(struct block_list *bl, va_list ap)
{
	struct skill_unit *target = (struct skill_unit*)bl,
		*src = va_arg(ap, struct skill_unit*);
	int flag = va_arg(ap, int);
	if (src == target)
		return 0;
	if (!target->group || !(target->group->state.song_dance&0x1))
		return 0;
	if (!(target->val2 & src->val2 & ~UF_ENSEMBLE)) //They don't match (song + dance) is valid.
		return 0;
	if (flag) { //Set dissonance
		target->val1 = src->val1 = target->val2&UF_SONG?BA_DISSONANCE:DC_UGLYDANCE;
		target->val2 |= UF_ENSEMBLE; //Add ensemble to signal this unit is overlapping.
	} else { //Remove dissonance
		target->val1 = target->group->skill_id; //Restore skill id
		target->val2 &= ~UF_ENSEMBLE;
	}
	clif_skill_setunit(target); //Update look of affected cell.
	return 1;
}

//Does the song/dance overlapping -> dissonance check. [Skotlex]
//When flag is 0, this unit is about to be removed, cancel the dissonance effect
//When 1, this unit has been positioned, so start the cancel effect.
int skill_dance_overlap(struct skill_unit *unit, int flag)
{
	if (!unit || !unit->group || !(unit->group->state.song_dance&0x1))
		return 0;
	if (!flag && !(unit->val2&UF_ENSEMBLE))
		return 0; //Nothing to remove, this unit is not overlapped.
	if (unit->val1 != unit->group->skill_id) 
	{	//Reset state
		unit->val1 = unit->group->skill_id;
		unit->val2 &= ~UF_ENSEMBLE;
	}
	return map_foreachincell(skill_dance_overlap_sub,
		unit->bl.m,unit->bl.x,unit->bl.y,BL_SKILL,unit,flag);
}

/*==========================================
 * Initializes and sets a ground skill.
 * flag&1 is used to determine when the skill 'morphs' (Warp portal becomes active, or Fire Pillar becomes active)
 * flag&2 is used to determine if this skill was casted with Magic Power active.
 *------------------------------------------
 */
struct skill_unit_group *skill_unitsetting (struct block_list *src, int skillid, int skilllv, int x, int y, int flag)
{
	struct skill_unit_group *group;
	int i,limit,val1=0,val2=0,val3=0;
	int count=0;
	int target,interval,range,unit_flag;
	struct skill_unit_layout *layout;
	struct map_session_data *sd;
	struct status_data *status;
	struct status_change *sc;
	int active_flag=1;

	nullpo_retr(0, src);

	limit = skill_get_time(skillid,skilllv);
	range = skill_get_unit_range(skillid,skilllv);
	interval = skill_get_unit_interval(skillid);
	target = skill_get_unit_target(skillid);
	unit_flag = skill_get_unit_flag(skillid);
	layout = skill_get_unit_layout(skillid,skilllv,src,x,y);

	if (skillid == AL_WARP && flag && src->type == BL_SKILL)
	{	//Warp Portal morphing to active mode, extract relevant data from src. [Skotlex]
		group= ((TBL_SKILL*)src)->group;
		src = map_id2bl(group->src_id);
		if (!src) return NULL;
		val2=group->val2; //Copy the (x,y) position you warp to
		val3=group->val3; //as well as the mapindex to warp to.
	}
	
	BL_CAST(BL_PC, src, sd);
	status = status_get_status_data(src);
	sc= status_get_sc(src);	// for traps, firewall and fogwall - celest
	if (sc && !sc->count)
		sc = NULL;

	switch(skillid){

	case MG_SAFETYWALL:
		val2=skilllv+1;
		break;
	case MG_FIREWALL:
		if(sc && sc->data[SC_VIOLENTGALE].timer!=-1)
			limit = limit*3/2;
		val2=4+skilllv;
		break;

	case AL_WARP:
		val1=skilllv+6;
		if(!(flag&1))
			limit=2000;
		break;

	case PR_SANCTUARY:
		val1=(skilllv+3)*2;
		val2=(skilllv>6)?777:skilllv*100;
		break;

	case WZ_FIREPILLAR:
		if((flag&1)!=0)
			limit=1000;
		val1=skilllv+2;
		break;
	case WZ_QUAGMIRE:	//The target changes to "all" if used in a gvg map. [Skotlex]
	case AM_DEMONSTRATION:
		if (map_flag_vs(src->m) && battle_config.vs_traps_bctall
			&& (src->type&battle_config.vs_traps_bctall))
			target = BCT_ALL;
		break;
	case NJ_SUITON:
		skill_clear_group(src,1);
		break;
	case HT_SHOCKWAVE:
		val1=skilllv*15+10;
	case HT_SANDMAN:
	case HT_CLAYMORETRAP:
	case HT_SKIDTRAP:
	case HT_LANDMINE:
	case HT_ANKLESNARE:
	case HT_FLASHER:
	case HT_FREEZINGTRAP:
	case HT_BLASTMINE:
		if (map_flag_gvg(src->m))
			limit *= 4; // longer trap times in WOE [celest]
		if (battle_config.vs_traps_bctall && map_flag_vs(src->m)
			&& (src->type&battle_config.vs_traps_bctall))
			target = BCT_ALL;
		break;

	case SA_LANDPROTECTOR:
		{
			int aoe_diameter;	// -- aoe_diameter (moonsoul) added for sage Area Of Effect skills
			val1=skilllv*15+10;
			aoe_diameter=skilllv+skilllv%2+5;
			count=aoe_diameter*aoe_diameter;	// -- this will not function if changed to ^2 (moonsoul)
		}
	//No break because we also have to check if we use gemstones. [Skotlex]
	case SA_VOLCANO:
	case SA_DELUGE:
	case SA_VIOLENTGALE:
	{
		struct skill_unit_group *old_sg;
		if ((old_sg = skill_locate_element_field(src)) != NULL)
		{
			if (old_sg->skill_id == skillid && old_sg->limit > 0)
			{	//Use the previous limit (minus the elapsed time) [Skotlex]
				limit = old_sg->limit - DIFF_TICK(gettick(), old_sg->tick);
				if (limit < 0)	//This can happen... 
					limit = skill_get_time(skillid,skilllv);
			}
			skill_clear_group(src,1);
		}
		break;
	}

	case BA_DISSONANCE:
	case DC_UGLYDANCE:
		val1 = 10;	//FIXME: This value is not used anywhere, what is it for? [Skotlex]
		break;
	case BA_WHISTLE:
		val1 = skilllv +status->agi/10; // Flee increase
		val2 = ((skilllv+1)/2)+status->luk/10; // Perfect dodge increase
		if(sd){
			val1 += pc_checkskill(sd,BA_MUSICALLESSON);
			val2 += pc_checkskill(sd,BA_MUSICALLESSON);
		}
		break;
	case DC_HUMMING:
        val1 = 2*skilllv+status->dex/10; // Hit increase
		if(sd)
			val1 += 2*pc_checkskill(sd,DC_DANCINGLESSON);
		break;
	case BA_POEMBRAGI:
		val1 = 3*skilllv+status->dex/10; // Casting time reduction
		//For some reason at level 10 the base delay reduction is 50%.
		val2 = (skilllv<10?3*skilllv:50)+status->int_/10; // After-cast delay reduction
		if(sd){
			val1 += pc_checkskill(sd,BA_MUSICALLESSON);
			val2 += pc_checkskill(sd,BA_MUSICALLESSON);
		}
		break;
	case DC_DONTFORGETME:
		val1 = 30*skilllv+status->dex; // ASPD decrease
		val2 = 100 -2*skilllv -status->agi/10; // Movement speed adjustment.
		if(sd){
			val1 += pc_checkskill(sd,DC_DANCINGLESSON);
			val2 -= pc_checkskill(sd,DC_DANCINGLESSON);
		}
		if (val2 < 1) val2 = 1;
		break;
	case BA_APPLEIDUN:
		val1 = 5+2*skilllv+status->vit/10; // MaxHP percent increase
		val2 = 30+5*skilllv+5*(status->vit/10); // HP recovery
		if(sd){
			val1 += pc_checkskill(sd,BA_MUSICALLESSON);
			val2 += 5*pc_checkskill(sd,BA_MUSICALLESSON);
		}
		break;
	case DC_SERVICEFORYOU:
		val1 = 10+skilllv+(status->int_/10); // MaxSP percent increase
		val2 = 10+3*skilllv+(status->int_/10); // SP cost reduction
		if(sd){
			val1 += pc_checkskill(sd,DC_DANCINGLESSON);
			val2 += pc_checkskill(sd,DC_DANCINGLESSON);
		}
		break;
	case BA_ASSASSINCROSS:
		val1 = 100+10*skilllv+status->agi; // ASPD increase
		if(sd)
			val1 += pc_checkskill(sd,BA_MUSICALLESSON);
		break;
	case DC_FORTUNEKISS:
		val1 = 10+skilllv+(status->luk/10); // Critical increase
		if(sd)
			val1 += pc_checkskill(sd,DC_DANCINGLESSON);
		val1*=10; //Because every 10 crit is an actual cri point.
		break;
	case BD_DRUMBATTLEFIELD:
		val1 = (skilllv+1)*25;	//Watk increase
		val2 = (skilllv+1)*2;	//Def increase
		break;
	case BD_RINGNIBELUNGEN:
		val1 = (skilllv+2)*25;	//Watk increase
		break;
	case BD_RICHMANKIM:
		val1 = 25 + 11*skilllv; //Exp increase bonus.
		break;
	case BD_SIEGFRIED:
		val1 = 55 + skilllv*5;	//Elemental Resistance
		val2 = skilllv*10;	//Status ailment resistance
		break;
	case PF_FOGWALL:
		if(sc && (
			sc->data[SC_DELUGE].timer!=-1 || sc->data[SC_SUITON].timer != -1
		)) limit *= 2;
		break;
	case RG_GRAFFITI:			/* Graffiti */
		count=1;	// Leave this at 1 [Valaris]
		break;
	case WE_CALLPARTNER:
		if (sd) val1 = sd->status.partner_id;
		break;
	case WE_CALLPARENT:
		if (sd) {
			val1 = sd->status.father;
		 	val2 = sd->status.mother;
		}
		break;
	case WE_CALLBABY:
		if (sd) val1 = sd->status.child;
		break;
	case NJ_KAENSIN:
		skill_clear_group(src, 1); //Delete previous Kaensins/Suitons
		val2 = (skilllv+1)/2 + 4;
		break;

	case GS_GROUNDDRIFT:
		{	//Take on the base element, not the elemental one.
			struct status_data *bstatus = status_get_base_status(src);
			val1 = bstatus?bstatus->rhw.ele:status->rhw.ele;
			if (sd) sd->state.arrow_atk = 0; //Disable consumption right away.
			else if (!val1) val1 = ELE_WATER+rand()%(ELE_WIND-ELE_WATER);
			break;
		}
	}

	nullpo_retr(NULL, group=skill_initunitgroup(src,(count > 0 ? count : layout->count),
		skillid,skilllv,skill_get_unit_id(skillid,flag&1), limit, interval));
	group->val1=val1;
	group->val2=val2;
	group->val3=val3;
	group->target_flag=target;
	group->bl_flag= skill_get_unit_bl_target(skillid);
	group->state.into_abyss = (sc && sc->data[SC_INTOABYSS].timer != -1); //Store into abyss state, to know it shouldn't give traps back. [Skotlex]
	group->state.magic_power = (flag&2 || (sc && sc->data[SC_MAGICPOWER].timer != -1)); //Store the magic power flag. [Skotlex]
	group->state.ammo_consume = (sd && sd->state.arrow_atk); //Store if this skill needs to consume ammo.
	group->state.song_dance = (unit_flag&(UF_DANCE|UF_SONG)?1:0)|(unit_flag&UF_ENSEMBLE?2:0); //Signals if this is a song/dance/duet

  	//if tick is greater than current, do not invoke onplace function just yet. [Skotlex]
	if (DIFF_TICK(group->tick, gettick()) > 100)
		active_flag = 0;

	if(skillid==HT_TALKIEBOX ||
	   skillid==RG_GRAFFITI){
		group->valstr=(char *) aMallocA(MESSAGE_SIZE*sizeof(char));
		if(group->valstr==NULL){
			ShowFatalError("skill_castend_map: out of memory !\n");
			exit(1);
		}
		memcpy(group->valstr,talkie_mes,MESSAGE_SIZE-1);
		group->valstr[MESSAGE_SIZE-1] = '\0';
	}

	//Why redefine local variables when the ones of the function can be reused? [Skotlex]
	val1=skilllv;
	val2=0;
	limit=group->limit;
	count=group->unit_count;
	for(i=0;i<count;i++){
		struct skill_unit *unit;
		int ux,uy,alive=1;
		ux = x + layout->dx[i];
		uy = y + layout->dy[i];
		switch (skillid) {
		case MG_FIREWALL:
		case NJ_KAENSIN:
			val2=group->val2;
			break;
		case WZ_ICEWALL:
			if(skilllv <= 1)
				val1 = 500;
			else
				val1 = 200 + 200*skilllv;
			break;
		case RG_GRAFFITI:	/* Graffiti [Valaris] */
			ux+=(i%5-2);
			uy+=(i/5-2);
			break;
		default:
			if (group->state.song_dance&0x1) {
				val1 = skillid; //Holds SKILL id to use.
				val2 = unit_flag&(UF_DANCE|UF_SONG); //Store whether this is a song/dance
			}
			break;
		}
		if(range<=0)
			map_foreachincell(skill_landprotector,src->m,ux,uy,BL_SKILL,skillid,&alive, src);
		
		if(alive && map_getcell(src->m,ux,uy,CELL_CHKWALL))
			alive = 0;
		
		if (alive && battle_config.skill_wall_check) {
			//Check if there's a path between cell and center of casting.
			if (!path_search_long(NULL,src->m,ux,uy,x,y))
				alive = 0;
		}
					
		if(alive && skillid == WZ_ICEWALL) {
			if(src->x == x && src->y==y) // Ice Wall not allowed on self [DracoRPG]
				alive=0;
			else {
				val2=map_getcell(src->m,ux,uy,CELL_GETTYPE);
				if(val2==5 || val2==1)
					alive=0;
				else
					clif_changemapcell(src->m,ux,uy,5,0);
			}
		}

		if(alive){
			nullpo_retr(NULL, unit=skill_initunit(group,i,ux,uy,val1,val2));
			unit->limit=limit;
			unit->range=range;
		
			if (range==0 && active_flag)
				map_foreachincell(skill_unit_effect,unit->bl.m,
					unit->bl.x,unit->bl.y,group->bl_flag,&unit->bl,gettick(),1);
		}
	}
	if (!group->alive_count)
	{	//No cells? Something that was blocked completely by Land Protector?
		skill_delunitgroup(src, group);
		return NULL;
	}
	
	return group;
}

/*==========================================
 * 
 *------------------------------------------
 */
int skill_unit_onplace (struct skill_unit *src, struct block_list *bl, unsigned int tick)
{
	struct skill_unit_group *sg;
	struct block_list *ss;
	struct status_change *sc;
	int type,skillid;

	nullpo_retr(0, src);
	nullpo_retr(0, bl);
	
	if(bl->prev==NULL || !src->alive || status_isdead(bl))
		return 0;

	nullpo_retr(0, sg=src->group);
	nullpo_retr(0, ss=map_id2bl(sg->src_id));

	if (skill_get_type(sg->skill_id) == BF_MAGIC &&
		map_getcell(bl->m, bl->x, bl->y, CELL_CHKLANDPROTECTOR))
		return 0; //AoE skills are ineffective. [Skotlex]
	
	sc = status_get_sc(bl);
	
	if (sc && sc->option&OPTION_HIDE && sg->skill_id != WZ_HEAVENDRIVE)
		return 0; //Hidden characters are inmune to AoE skills except Heaven's Drive. [Skotlex]
	
	type = SkillStatusChangeTable(sg->skill_id);
	skillid = sg->skill_id; //In case the group is deleted, we need to return the correct skill id, still.
	switch (sg->unit_id) {
	case UNT_SAFETYWALL:
		//TODO: Find a more reliable way to handle the link to sg, this could cause dangling pointers. [Skotlex]
		if (sc && sc->data[type].timer == -1)
			sc_start4(bl,type,100,sg->skill_lv,sg->group_id,(int)sg,0,sg->limit);
		break;

	case UNT_WARP_WAITING:
		if(bl->type==BL_PC){
			struct map_session_data *sd = (struct map_session_data *)bl;
			if((!sd->chatID || battle_config.chat_warpportal)
				&& sd->ud.to_x == src->bl.x && sd->ud.to_y == src->bl.y) {
				if (pc_setpos(sd,sg->val3,sg->val2>>16,sg->val2&0xffff,3) == 0) {
					if (--sg->val1<=0 || sg->src_id == bl->id)
						skill_delunitgroup(NULL, sg);
				}
			}
		} else
		if(bl->type == BL_MOB && battle_config.mob_warp&2)
		{
			int m = map_mapindex2mapid(sg->val3);
			if (m < 0) break; //Map not available on this map-server.
			unit_warp(bl,m,sg->val2>>16,sg->val2&0xffff,3);
		}
		break;

	case UNT_QUAGMIRE:
		if(sc && sc->data[type].timer==-1)
			sc_start4(bl,type,100,sg->skill_lv,sg->group_id,0,0,sg->limit);
		break;

	case UNT_VOLCANO:
	case UNT_DELUGE:
	case UNT_VIOLENTGALE:
	case UNT_SUITON:
		if(sc && sc->data[type].timer==-1)
			sc_start(bl,type,100,sg->skill_lv,sg->limit);
		break;

	case UNT_RICHMANKIM:
	case UNT_ETERNALCHAOS:
	case UNT_DRUMBATTLEFIELD:
	case UNT_RINGNIBELUNGEN:
	case UNT_ROKISWEIL:
	case UNT_INTOABYSS:
	case UNT_SIEGFRIED:
	case UNT_HERMODE:
		 //Needed to check when a dancer/bard leaves their ensemble area.
		if (sg->src_id==bl->id &&
			!(sc && sc->data[SC_SPIRIT].timer != -1 && sc->data[SC_SPIRIT].val2 == SL_BARDDANCER))
			return skillid;
		if (sc && sc->data[type].timer==-1)
			sc_start4(bl,type,100,sg->skill_lv,sg->val1,sg->val2,0,sg->limit);
		break;
	case UNT_WHISTLE:
	case UNT_ASSASSINCROSS:
	case UNT_POEMBRAGI:
	case UNT_APPLEIDUN:
	case UNT_HUMMING:
	case UNT_DONTFORGETME:
	case UNT_FORTUNEKISS:
	case UNT_SERVICEFORYOU:
		if (sg->src_id==bl->id && (!sc || sc->data[SC_SPIRIT].timer == -1 || sc->data[SC_SPIRIT].val2 != SL_BARDDANCER))
			return 0;
		if (!sc) return 0;
		if (sc->data[type].timer==-1)
			sc_start4(bl,type,100,sg->skill_lv,sg->val1,sg->val2,0,sg->limit);
		else if (sc->data[type].val4 == 1) {
			//Readjust timers since the effect will not last long.
			sc->data[type].val4 = 0;
			delete_timer(sc->data[type].timer, status_change_timer);
			sc->data[type].timer = add_timer(tick+sg->limit, status_change_timer, bl->id, type);
		}
		break;
	case UNT_FOGWALL:
		if (sc && sc->data[type].timer==-1)
		{
			sc_start4(bl, type, 100, sg->skill_lv, sg->val1, sg->val2, sg->group_id, sg->limit);
			if (battle_check_target(&src->bl,bl,BCT_ENEMY)>0)
				skill_additional_effect (ss, bl, sg->skill_id, sg->skill_lv, BF_MISC, tick);
		}
		break;

	case UNT_GRAVITATION:
		if (sc && sc->data[type].timer==-1)
			sc_start4(bl,type,100,sg->skill_lv,0,BCT_ENEMY,sg->group_id,sg->limit);
		break;
	
	case UNT_ICEWALL: //Destroy the cell. [Skotlex]
		src->val1 = 0;
		if(src->limit + sg->tick > tick + 700)
			src->limit = DIFF_TICK(tick+700,sg->tick);
		break;
	}	

	return skillid;
}

/*==========================================
 * 
 *------------------------------------------
 */
int skill_unit_onplace_timer (struct skill_unit *src, struct block_list *bl, unsigned int tick)
{
	struct skill_unit_group *sg;
	struct block_list *ss;
	struct map_session_data *sd;
	struct status_data *tstatus, *sstatus;
	struct status_change *tsc, *sc;
	struct skill_unit_group_tickset *ts;
	int matk_min = 0, matk_max = 0; //For Magic power...
	int type, skillid;
	int diff=0;

	nullpo_retr(0, src);
	nullpo_retr(0, bl);

	if (bl->prev==NULL || !src->alive || status_isdead(bl))
		return 0;

	nullpo_retr(0, sg=src->group);
	nullpo_retr(0, ss=map_id2bl(sg->src_id));
	BL_CAST(BL_PC, ss, sd);
	tsc = status_get_sc(bl);
	tstatus = status_get_status_data(bl);
	if (sg->state.magic_power)  //For magic power. 
	{
		sc = status_get_sc(ss);
		sstatus = status_get_status_data(ss);
	} else {
		sc = NULL;
		sstatus = NULL;
	}
	type = SkillStatusChangeTable(sg->skill_id);
	skillid = sg->skill_id;

	if (sg->interval == -1) {
		switch (sg->unit_id) {
			case UNT_ANKLESNARE: //These happen when a trap is splash-triggered by multiple targets on the same cell.
			case UNT_SPIDERWEB:
			case UNT_FIREPILLAR_ACTIVE:
				return 0;
			default:
				if (battle_config.error_log)
					ShowError("skill_unit_onplace_timer: interval error (unit id %x)\n", sg->unit_id);
				return 0;
		}
	}

	if ((ts = skill_unitgrouptickset_search(bl,sg,tick)))
	{	//Not all have it, eg: Traps don't have it even though they can be hit by Heaven's Drive [Skotlex]
		diff = DIFF_TICK(tick,ts->tick);
		if (diff < 0)
			return 0;
		ts->tick = tick+sg->interval;
		
		if ((skillid==CR_GRANDCROSS || skillid==NPC_GRANDDARKNESS) && !battle_config.gx_allhit)
			ts->tick += sg->interval*(map_count_oncell(bl->m,bl->x,bl->y,BL_CHAR)-1);
	}
	//Temporarily set magic power to have it take effect. [Skotlex]
	if (sg->state.magic_power && sc && sc->data[SC_MAGICPOWER].timer == -1)
	{	//Store previous values.
		matk_min = sstatus->matk_min;
		matk_max = sstatus->matk_max;
		//Note to NOT return from the function until this is unset!
		sstatus->matk_min = sc->data[SC_MAGICPOWER].val3;
		sstatus->matk_max = sc->data[SC_MAGICPOWER].val4;
	}

	switch (sg->unit_id)
	{
		case UNT_FIREWALL:
			{
				int count=0;
				if (tstatus->def_ele == ELE_FIRE || battle_check_undead(tstatus->race, tstatus->def_ele)) {
					//This is the best Aegis approximation we can do without 
					//changing the minimum skill unit interval. [Skotlex]
					while (count++ < battle_config.firewall_hits_on_undead && src->val2-- && !status_isdead(bl))
						skill_attack(BF_MAGIC,ss,&src->bl,bl,sg->skill_id,sg->skill_lv,tick+count*10,1);
				} else {
					skill_attack(BF_MAGIC,ss,&src->bl,bl,sg->skill_id,sg->skill_lv,tick,0);
					src->val2--;
				}
				if (src->val2<=0)
					skill_delunit(src);
			break;
			}

		case UNT_SANCTUARY:
			if (battle_check_undead(tstatus->race, tstatus->def_ele) || tstatus->race==RC_DEMON)
			{	//Only damage enemies with offensive Sanctuary. [Skotlex]
				if(battle_check_target(&src->bl,bl,BCT_ENEMY)>0 &&
					skill_attack(BF_MAGIC, ss, &src->bl, bl, sg->skill_id, sg->skill_lv, tick, 0))
					// reduce healing count if this was meant for damaging [hekate]
					sg->val1 -= 2;
			} else {
				int heal = sg->val2;
				if (tstatus->hp >= tstatus->max_hp)
					break;
				if (status_isimmune(bl))
					heal = 0;	/* 黄金蟲カード（ヒール量０） */
				clif_skill_nodamage(&src->bl, bl, AL_HEAL, heal, 1);
				status_heal(bl, heal, 0, 0);
				if (diff >= 500)
					sg->val1--;
			}
			if (sg->val1 <= 0)
				skill_delunitgroup(NULL,sg);
			break;

		case UNT_MAGNUS:
			if (!battle_check_undead(tstatus->race,tstatus->def_ele) && tstatus->race!=RC_DEMON)
				break;
			skill_attack(BF_MAGIC,ss,&src->bl,bl,sg->skill_id,sg->skill_lv,tick,0);
			break;

		case UNT_ATTACK_SKILLS:
			switch (sg->skill_id) 
			{
				case SG_SUN_WARM: //SG skills [Komurka]
				case SG_MOON_WARM:
				case SG_STAR_WARM:
					if(bl->type==BL_PC)
						//Only damage SP [Skotlex]
						status_zap(bl, 0, 60);
					else if(status_charge(bl, 0, 2))
						//Otherwise, Knockback attack.
						skill_attack(BF_WEAPON,ss,&src->bl,bl,sg->skill_id,sg->skill_lv,tick,0);
				break;

				default:
					skill_attack(skill_get_type(sg->skill_id),ss,&src->bl,bl,sg->skill_id,sg->skill_lv,tick,0);			
			}
			break;

		case UNT_FIREPILLAR_WAITING:
			skill_unitsetting(ss,sg->skill_id,sg->skill_lv,src->bl.x,src->bl.y,1);
			skill_delunit(src);
			break;

		case UNT_FIREPILLAR_ACTIVE:
			map_foreachinrange(skill_attack_area,bl,
				skill_get_splash(sg->skill_id, sg->skill_lv), sg->bl_flag,
				BF_MAGIC,ss,&src->bl,sg->skill_id,sg->skill_lv,tick,0,BCT_ENEMY);  // area damage [Celest]
			sg->interval = -1; //Mark it used up so others can't trigger it for massive splash damage. [Skotlex]
			sg->limit=DIFF_TICK(tick,sg->tick) + 1500;
			break;

		case UNT_SKIDTRAP:
			{
				skill_blown(&src->bl,bl,skill_get_blewcount(sg->skill_id,sg->skill_lv)|0x10000);
				sg->unit_id = UNT_USED_TRAPS;
				clif_changetraplook(&src->bl, UNT_USED_TRAPS);
				sg->limit=DIFF_TICK(tick,sg->tick)+1500;
				sg->state.into_abyss = 1; //Prevent Remove Trap from giving you the trap back. [Skotlex]
			}
			break;

		case UNT_SPIDERWEB:
		case UNT_ANKLESNARE:
			if(sg->val2==0 && tsc && tsc->data[type].timer==-1){
				int sec = skill_get_time2(sg->skill_id,sg->skill_lv);
				if (sc_start(bl,type,100,sg->skill_lv,sec))
				{
					struct TimerData* td = get_timer(tsc->data[type].timer); 
					if (td) sec = DIFF_TICK(td->tick, tick);
					map_moveblock(bl, src->bl.x, src->bl.y, tick);
					clif_fixpos(bl);
					sg->val2=bl->id;
				} else
					sec = 3000; //Couldn't trap it?
				//clif_01ac(&src->bl); //Removed? Check the openkore description of this packet: [Skotlex]
				// 01AC: long ID
				// Indicates that an object is trapped, but ID is not a
				// valid monster or player ID.
				sg->limit = DIFF_TICK(tick,sg->tick)+sec;
				sg->interval = -1;
				src->range = 0;
				sg->state.into_abyss = 1; //Prevent Remove Trap from giving you the trap back. [Skotlex]
			}
			break;

		case UNT_VENOMDUST:
			if(tsc && tsc->data[type].timer==-1 )
				status_change_start(bl,type,10000,sg->skill_lv,sg->group_id,0,0,skill_get_time2(sg->skill_id,sg->skill_lv),8);
			break;

		case UNT_LANDMINE:
			skill_attack(BF_MISC,ss,&src->bl,bl,sg->skill_id,sg->skill_lv,tick,0);
			sg->unit_id = UNT_USED_TRAPS;
			clif_changetraplook(&src->bl, UNT_FIREPILLAR_ACTIVE);
			sg->limit=DIFF_TICK(tick,sg->tick)+1500;
			sg->state.into_abyss = 1; //Prevent Remove Trap from giving you the trap back. [Skotlex]
			break;

		case UNT_CLAYMORETRAP:
		case UNT_BLASTMINE:
			//Hold number of targets (required for damage calculation)
			type = map_foreachinrange(skill_count_target,&src->bl,
				skill_get_splash(sg->skill_id, sg->skill_lv), sg->bl_flag, &src->bl);
		case UNT_SHOCKWAVE:
		case UNT_SANDMAN:
		case UNT_FLASHER:
		case UNT_FREEZINGTRAP:
			map_foreachinrange(skill_trap_splash,&src->bl,
				skill_get_splash(sg->skill_id, sg->skill_lv), sg->bl_flag,
				&src->bl,tick,type);
			sg->unit_id = UNT_USED_TRAPS;
			clif_changetraplook(&src->bl, UNT_USED_TRAPS);
			sg->limit=DIFF_TICK(tick,sg->tick)+1500;
			sg->state.into_abyss = 1; //Prevent Remove Trap from giving you the trap back. [Skotlex]
			break;

		case UNT_TALKIEBOX:
			if (sg->src_id == bl->id)
				break;
			if (sg->val2 == 0){
				clif_talkiebox(&src->bl, sg->valstr);
				sg->unit_id = UNT_USED_TRAPS;
				clif_changetraplook(&src->bl, UNT_USED_TRAPS);
				sg->limit = DIFF_TICK(tick, sg->tick) + 5000;
				sg->val2 = -1;
				sg->state.into_abyss = 1; //Prevent Remove Trap from giving you the trap back. [Skotlex]
			}
			break;

		case UNT_LULLABY:
			if (ss->id == bl->id)
				break;
			skill_additional_effect(ss, bl, sg->skill_id, sg->skill_lv, BF_LONG|BF_SKILL|BF_MISC, tick);
			break;

		case UNT_UGLYDANCE:	//Ugly Dance [Skotlex]
			if (ss->id != bl->id)
				skill_additional_effect(ss, bl, sg->skill_id, sg->skill_lv, BF_LONG|BF_SKILL|BF_MISC, tick);
			break;

		case UNT_DISSONANCE:
			skill_attack(BF_MISC, ss, &src->bl, bl, sg->skill_id, sg->skill_lv, tick, 0);
			break;

		case UNT_APPLEIDUN: //Apple of Idun [Skotlex]
		{
			int heal;
			if (sg->src_id == bl->id)
				break;
			heal = sg->val2;
			clif_skill_nodamage(&src->bl, bl, AL_HEAL, heal, 1);
			status_heal(bl, heal, 0, 0);
			break;	
		}

		case UNT_TATAMIGAESHI:
		case UNT_DEMONSTRATION:
			skill_attack(BF_WEAPON,ss,&src->bl,bl,sg->skill_id,sg->skill_lv,tick,0);
			break;

		case UNT_GOSPEL:
			if (rand()%100 > sg->skill_lv*10)
				break;
			if (ss != bl && battle_check_target(ss,bl,BCT_PARTY)>0) { // Support Effect only on party, not guild
				int i = rand()%13; // Positive buff count
				switch (i)
				{
					case 0: // Heal 1~9999 HP
						{
							int heal = rand() %9999+1;
							clif_skill_nodamage(ss,bl,AL_HEAL,heal,1);
							status_heal(bl,heal,0,0);
						}
						break;
					case 1: // End all negative status
						status_change_clear_buffs(bl,2);
						break;
					case 2: // Level 10 Blessing
						sc_start(bl,SC_BLESSING,100,10,skill_get_time2(sg->skill_id, sg->skill_lv));
						break;
					case 3: // Level 10 Increase AGI
						sc_start(bl,SC_INCREASEAGI,100,10,skill_get_time2(sg->skill_id, sg->skill_lv));
						break;
					case 4: // Enchant weapon with Holy element
						sc_start(bl,SC_ASPERSIO,100,1,skill_get_time2(sg->skill_id, sg->skill_lv));
						break;
					case 5: // Enchant armor with Holy element
						sc_start(bl,SC_BENEDICTIO,100,1,skill_get_time2(sg->skill_id, sg->skill_lv));
						break;
					case 6: // MaxHP +100%
						sc_start(bl,SC_INCMHPRATE,100,100,skill_get_time2(sg->skill_id, sg->skill_lv));
						break;
					case 7: // MaxSP +100%
						sc_start(bl,SC_INCMSPRATE,100,100,skill_get_time2(sg->skill_id, sg->skill_lv));
						break;
					case 8: // All stats +20
						sc_start(bl,SC_INCALLSTATUS,100,20,skill_get_time2(sg->skill_id, sg->skill_lv));
						break;
					case 9: // DEF +25%
						sc_start(bl,SC_INCDEFRATE,100,25,skill_get_time2(sg->skill_id, sg->skill_lv));
						break;
					case 10: // ATK +100%
						sc_start(bl,SC_INCATKRATE,100,100,skill_get_time2(sg->skill_id, sg->skill_lv));
						break;
					case 11: // HIT/Flee +50
						sc_start(bl,SC_INCHIT,100,50,skill_get_time2(sg->skill_id, sg->skill_lv));
						sc_start(bl,SC_INCFLEE,100,50,skill_get_time2(sg->skill_id, sg->skill_lv));
						break;
					case 12: // Immunity to all status
						sc_start(bl,SC_SCRESIST,100,100,skill_get_time2(sg->skill_id, sg->skill_lv));
						break;
				}
			}
			else if (battle_check_target(&src->bl,bl,BCT_ENEMY)>0) { // Offensive Effect
				int i = rand()%9; // Negative buff count
				switch (i)
				{
					case 0: // Deal 1~9999 damage
						skill_attack(BF_MISC,ss,&src->bl,bl,sg->skill_id,sg->skill_lv,tick,0);
						break;
					case 1: // Curse
						sc_start(bl,SC_CURSE,100,1,skill_get_time2(sg->skill_id, sg->skill_lv));
						break;
					case 2: // Blind
						sc_start(bl,SC_BLIND,100,1,skill_get_time2(sg->skill_id, sg->skill_lv));
						break;
					case 3: // Poison
						sc_start(bl,SC_POISON,100,1,skill_get_time2(sg->skill_id, sg->skill_lv));
						break;
					case 4: // Level 10 Provoke
						sc_start(bl,SC_PROVOKE,100,10,skill_get_time2(sg->skill_id, sg->skill_lv));
						break;
					case 5: // DEF -100%
						sc_start(bl,SC_INCDEFRATE,100,-100,skill_get_time2(sg->skill_id, sg->skill_lv));
						break;
					case 6: // ATK -100%
						sc_start(bl,SC_INCATKRATE,100,-100,skill_get_time2(sg->skill_id, sg->skill_lv));
						break;
					case 7: // Flee -100%
						sc_start(bl,SC_INCFLEERATE,100,-100,skill_get_time2(sg->skill_id, sg->skill_lv));
						break;
					case 8: // Speed/ASPD -25%
						sc_start4(bl,SC_GOSPEL,100,1,0,0,BCT_ENEMY,skill_get_time2(sg->skill_id, sg->skill_lv));
						break;
				}
			}
			break;

		case UNT_GRAVITATION:
			skill_attack(BF_MAGIC,ss,&src->bl,bl,sg->skill_id,sg->skill_lv,tick,0);
			break;

		case UNT_DESPERADO:
			if (!(rand()%10)) //Has a low chance of connecting. [Skotlex]
				skill_attack(BF_WEAPON,ss,&src->bl,bl,sg->skill_id,sg->skill_lv,tick,0);
			break;

		case UNT_GROUNDDRIFT:
			skill_attack(BF_WEAPON,ss,&src->bl,bl,sg->skill_id,sg->skill_lv,tick,sg->val1);
			break;

		case UNT_KAENSIN:
			skill_attack(BF_MAGIC,ss,&src->bl,bl,sg->skill_id,sg->skill_lv,tick,0);
			if (--src->val2 <= 0)
				skill_delunit(src);
			break;
	}

	if (sg->state.magic_power && sc && sc->data[SC_MAGICPOWER].timer == -1)
	{	//Unset magic power.
		sstatus->matk_min = matk_min;
		sstatus->matk_max = matk_max;
	}

	if (bl->type == BL_MOB && ss != bl)
		mobskill_event((TBL_MOB*)bl, ss, tick, MSC_SKILLUSED|(skillid<<16));

	return skillid;
}
/*==========================================
 *
 *------------------------------------------
 */
int skill_unit_onout (struct skill_unit *src, struct block_list *bl, unsigned int tick)
{
	struct skill_unit_group *sg;
	struct status_change *sc;
	int type;

	nullpo_retr(0, src);
	nullpo_retr(0, bl);
	nullpo_retr(0, sg=src->group);
	sc = status_get_sc(bl);
	if (sc && !sc->count)
		sc = NULL;
	
	type = SkillStatusChangeTable(sg->skill_id);

	if (bl->prev==NULL || !src->alive || //Need to delete the trap if the source died.
		(status_isdead(bl) && sg->unit_id != UNT_ANKLESNARE && sg->unit_id != UNT_SPIDERWEB))
		return 0;

	switch(sg->unit_id){
	case UNT_SAFETYWALL:
		if (sc && sc->data[type].timer!=-1)
			status_change_end(bl,type,-1);
		break;
	case UNT_HERMODE:	//Clear Hermode if the owner moved.
		if (sc && sc->data[type].timer!=-1 && sc->data[type].val3 == BCT_SELF && sc->data[type].val4 == sg->group_id)
			status_change_end(bl,type,-1);
		break;
		
	case UNT_SPIDERWEB:
		{
			struct block_list *target = map_id2bl(sg->val2);
			if (target && target==bl)
			{
				status_change_end(bl,SC_SPIDERWEB,-1);
				sg->limit = DIFF_TICK(tick,sg->tick)+1000;
			}
			break;
		}
	}
	return sg->skill_id;
}

/*==========================================
 * Triggered when a char steps out of a skill group [Skotlex]
 *------------------------------------------
 */
static int skill_unit_onleft (int skill_id, struct block_list *bl, unsigned int tick)
{
	struct status_change *sc;
	int type;

	sc = status_get_sc(bl);
	if (sc && !sc->count)
		sc = NULL;
	
	type = SkillStatusChangeTable(skill_id);

	switch (skill_id)
	{
		case WZ_QUAGMIRE:
			if (bl->type==BL_MOB)
				break;
			if (sc && sc->data[type].timer != -1)
				status_change_end(bl, type, -1);
			break;

		case BD_LULLABY:
		case BD_RICHMANKIM:
		case BD_ETERNALCHAOS:
		case BD_DRUMBATTLEFIELD:
		case BD_RINGNIBELUNGEN:	
		case BD_ROKISWEIL:
		case BD_INTOABYSS:
		case BD_SIEGFRIED:
			if(sc && sc->data[SC_DANCING].timer != -1 && sc->data[SC_DANCING].val1 == skill_id)
			{	//Check if you just stepped out of your ensemble skill to cancel dancing. [Skotlex]
				//We don't check for SC_LONGING because someone could always have knocked you back and out of the song/dance.
				//FIXME: This code is not perfect, it doesn't checks for the real ensemble's owner,
				//it only checks if you are doing the same ensemble. So if there's two chars doing an ensemble
				//which overlaps, by stepping outside of the other parther's ensemble will cause you to cancel 
				//your own. Let's pray that scenario is pretty unlikely and noone will complain too much about it.
				skill_stop_dancing(bl);
			}
		case MG_SAFETYWALL:
		case AL_PNEUMA:
		case SA_VOLCANO:
		case SA_DELUGE:
		case SA_VIOLENTGALE:
		case CG_HERMODE:
		case HW_GRAVITATION:
		case NJ_SUITON:
			if (sc && sc->data[type].timer != -1)
				status_change_end(bl, type, -1);
			break;
			
		case BA_POEMBRAGI:
		case BA_WHISTLE:
		case BA_ASSASSINCROSS:
		case BA_APPLEIDUN:
		case DC_HUMMING:
		case DC_DONTFORGETME:
		case DC_FORTUNEKISS:	
		case DC_SERVICEFORYOU:
			if (sc && sc->data[type].timer != -1)
			{
				delete_timer(sc->data[type].timer, status_change_timer);
				//NOTE: It'd be nice if we could get the skill_lv for a more accurate extra time, but alas...
				//not possible on our current implementation.
				sc->data[type].val4 = 1; //Store the fact that this is a "reduced" duration effect.
				sc->data[type].timer = add_timer(tick+skill_get_time2(skill_id,1), status_change_timer, bl->id, type);
			}
			break;
		case PF_FOGWALL:
			if (sc && sc->data[type].timer != -1)
			{
				status_change_end(bl,type,-1);
				if (sc->data[SC_BLIND].timer!=-1)
				{
					if (bl->type == BL_PC) //Players get blind ended inmediately, others have it still for 30 secs. [Skotlex]
						status_change_end(bl, SC_BLIND, -1);
					else {
						delete_timer(sc->data[SC_BLIND].timer, status_change_timer);
						sc->data[SC_BLIND].timer = add_timer(30000+tick, status_change_timer, bl->id, SC_BLIND);
					}
				}
			}
			break;
	case UNT_GOSPEL:
		if (sc && sc->data[type].timer != -1 && sc->data[type].val4 == BCT_ALL) //End item-no-use Gospel Effect. [Skotlex]
			status_change_end(bl, type, -1);
		break;

	}
	return skill_id;
}

/*==========================================
 * Invoked when a unit cell has been placed/removed/deleted.
 * flag values:
 * flag&1: Invoke onplace function (otherwise invoke onout)
 * flag&4: Invoke a onleft call (the unit might be scheduled for deletion)
 *------------------------------------------
 */
int skill_unit_effect (struct block_list *bl, va_list ap)
{
	struct skill_unit *unit;
	struct skill_unit_group *group;
	int flag,unit_id=0,skill_id;
	unsigned int tick;

	unit=va_arg(ap,struct skill_unit*);
	tick = va_arg(ap,unsigned int);
	flag = va_arg(ap,unsigned int);

	if (!unit->alive || bl->prev==NULL)
		return 0;

	nullpo_retr(0, group=unit->group);
  	
	//Necessary in case the group is deleted after calling on_place/on_out [Skotlex]
	skill_id = group->skill_id;

	//Target-type check.
	if(!(group->bl_flag&bl->type && battle_check_target(&unit->bl,bl,group->target_flag)>0))
	{
		if (flag&4 && group->src_id == bl->id && group->state.song_dance&0x2)
			skill_unit_onleft(skill_id, bl, tick);//Ensemble check to terminate it.
		return 0;
	}

	if (group->state.song_dance&0x1 && unit->val2&UF_ENSEMBLE)
  	{	//Treat this group as if it were BA_DISSONANCE/DC_UGLYDANCE.
		//Values will be restored on proper switch case.
		flag|=64; //Signal to remember to restore it.
		unit_id = group->unit_id;
		unit->val1 = group->skill_id;
		if (unit->val2&UF_SONG) {
			group->unit_id = UNT_DISSONANCE;
			group->skill_id = BA_DISSONANCE;
		} else {
			group->unit_id = UNT_UGLYDANCE;
			group->skill_id = DC_UGLYDANCE;
		}
	}

	if (flag&1)
		skill_unit_onplace(unit,bl,tick);
	else
		skill_unit_onout(unit,bl,tick);

	if (flag&64)
	{	//TODO: Normally, this is dangerous since the unit and group could be freed
		//inside the onout/onplace functions. Currently it is safe because we know song/dance
		//cells do not get deleted within them. [Skotlex]
		skill_id = group->skill_id;
		group->unit_id = unit_id;
		group->skill_id = unit->val1;
	}

	if (flag&4) skill_unit_onleft(skill_id, bl, tick);
	return 0;
}

/*==========================================
 * 
 *------------------------------------------
 */
int skill_unit_onlimit (struct skill_unit *src, unsigned int tick)
{
	struct skill_unit_group *sg;
	nullpo_retr(0, src);
	nullpo_retr(0, sg=src->group);

	switch(sg->unit_id){
	case UNT_WARP_ACTIVE:
		skill_unitsetting(&src->bl,sg->skill_id,sg->skill_lv,src->bl.x,src->bl.y,1);
		break;

	case UNT_ICEWALL:
		clif_changemapcell(src->bl.m,src->bl.x,src->bl.y,src->val2,1);
		break;
	case UNT_CALLFAMILY:
		{
			struct map_session_data *sd = NULL;
			if(sg->val1) {
			  	sd = map_charid2sd(sg->val1);
				sg->val1 = 0;
				if (sd && !map[sd->bl.m].flag.nowarp)
					pc_setpos(sd,map[src->bl.m].index,src->bl.x,src->bl.y,3);
			}
			if(sg->val2) {
				sd = map_charid2sd(sg->val2);
				sg->val2 = 0;
				if (sd && !map[sd->bl.m].flag.nowarp)
					pc_setpos(sd,map[src->bl.m].index,src->bl.x,src->bl.y,3);
			}
		}
		break;
	case UNT_ANKLESNARE:
		{
			struct block_list *target = map_id2bl(sg->val2);
			if(target)
				status_change_end(target,SC_ANKLE,-1);
		}
		break;
	}

	return 0;
}
/*==========================================
 *
 *------------------------------------------
 */
int skill_unit_ondamaged (struct skill_unit *src, struct block_list *bl, int damage, unsigned int tick)
{
	struct skill_unit_group *sg;

	nullpo_retr(0, src);
	nullpo_retr(0, sg=src->group);

	if (skill_get_inf2(sg->skill_id)&INF2_TRAP && damage > 0)
		skill_delunitgroup(NULL,sg);
	else 
	switch(sg->unit_id){
	case UNT_ICEWALL:
		src->val1-=damage;
		break;
	default:
		damage = 0;
		break;
	}
	return damage;
}

static int skill_moonlit_sub(struct block_list *bl, va_list ap) {
	struct block_list *src = va_arg(ap, struct block_list*);
	struct block_list *partner = va_arg(ap, struct block_list*);
	int blowcount = va_arg(ap, int);
	if (bl == src || bl == partner)
		return 0;
	skill_blown(src, bl, blowcount);
	return 1;
}

/*==========================================
 * Starts the moonlit effect by first knocking back all other characters in the vecinity.
 * partner may be null, but src cannot be.
 *------------------------------------------
 */
static void skill_moonlit (struct block_list* src, struct block_list* partner, int skilllv)
{
	int range = skill_get_splash(CG_MOONLIT, skilllv);
	int blowcount = range+1, time = skill_get_time(CG_MOONLIT,skilllv);
	
	map_foreachinrange(skill_moonlit_sub,src,
		range, BL_CHAR,src,partner,blowcount);
	if(partner)
		map_foreachinrange(skill_moonlit_sub,partner,
			range, BL_CHAR,src,partner,blowcount);
		
	sc_start4(src,SC_DANCING,100,CG_MOONLIT,0,0,partner?partner->id:BCT_SELF,time+1000);
	sc_start4(src,SkillStatusChangeTable(CG_MOONLIT),100,skilllv,0,0,0,time);
	
	if (partner) {
		sc_start4(partner,SC_DANCING,100,CG_MOONLIT,0,0,src->id,time+1000);
		sc_start4(partner,SkillStatusChangeTable(CG_MOONLIT),100,skilllv,0,0,0,time);
	}
	
}
/*==========================================
 *
 *------------------------------------------
 */

static int skill_check_condition_char_sub (struct block_list *bl, va_list ap)
{
	int *c, skillid;
	struct block_list *src;
	struct map_session_data *sd;
	struct map_session_data *tsd;
	int *p_sd;	//Contains the list of characters found.

	nullpo_retr(0, bl);
	nullpo_retr(0, ap);
	nullpo_retr(0, tsd=(struct map_session_data*)bl);
	nullpo_retr(0, src=va_arg(ap,struct block_list *));
	nullpo_retr(0, sd=(struct map_session_data*)src);

	c=va_arg(ap,int *);
	p_sd = va_arg(ap, int *);
	skillid = va_arg(ap,int);

	if ((skillid != PR_BENEDICTIO && *c >=1) || *c >=2)
		return 0; //Partner found for ensembles, or the two companions for Benedictio. [Skotlex]
	
	if (bl == src)
		return 0;

	if(pc_isdead(tsd))
		return 0;

	if (tsd->sc.count && (tsd->sc.data[SC_SILENCE].timer != -1 || tsd->sc.opt1))
		return 0;
	
	switch(skillid)
	{
		case PR_BENEDICTIO:
		{
			int dir = map_calc_dir(&sd->bl,tsd->bl.x,tsd->bl.y);
			dir = (unit_getdir(&sd->bl) + dir)%8; //This adjusts dir to account for the direction the sd is facing.
			if ((tsd->class_&MAPID_BASEMASK) == MAPID_ACOLYTE && (dir == 2 || dir == 6) //Must be standing to the left/right of Priest.
				&& sd->status.sp >= 10)
				p_sd[(*c)++]=tsd->bl.id;
			return 1;
		}
		default: //Warning: Assuming Ensemble Dance/Songs for code speed. [Skotlex]
			{
				int skilllv;
				if(pc_issit(tsd) || !unit_can_move(&tsd->bl))
					return 0;
				if (sd->status.sex != tsd->status.sex &&
						(tsd->class_&MAPID_UPPERMASK) == MAPID_BARDDANCER &&
						(skilllv = pc_checkskill(tsd, skillid)) > 0 &&
						(tsd->weapontype1==W_MUSICAL || tsd->weapontype1==W_WHIP) &&
						sd->status.party_id && tsd->status.party_id &&
						sd->status.party_id == tsd->status.party_id &&
						tsd->sc.data[SC_DANCING].timer == -1)
				{
					p_sd[(*c)++]=tsd->bl.id;
					return skilllv;
				} else {
					return 0;
				}
			}
			break;
	}
	return 0;
}

/*==========================================
 * Checks and stores partners for ensemble skills [Skotlex]
 *------------------------------------------
 */
int skill_check_pc_partner (struct map_session_data *sd, int skill_id, int* skill_lv, int range, int cast_flag)
{
	static int c=0;
	static int p_sd[2] = { 0, 0 };
	int i;
	if (cast_flag)
	{	//Execute the skill on the partners.
		struct map_session_data* tsd;
		switch (skill_id)
		{
			case PR_BENEDICTIO:
				for (i = 0; i < c; i++)
				{
					if ((tsd = map_id2sd(p_sd[i])) != NULL)
						status_charge(&tsd->bl, 0, 10);
				}
				return c;
			case CG_MOONLIT:
				if (c > 0 && (tsd = map_id2sd(p_sd[0])) != NULL)
				{
					clif_skill_nodamage(&tsd->bl, &sd->bl, skill_id, *skill_lv, 1);
					skill_moonlit(&sd->bl, &tsd->bl, *skill_lv);
					tsd->skillid_dance = skill_id;
					tsd->skilllv_dance = *skill_lv;
				}
				return c;
			default: //Warning: Assuming Ensemble skills here (for speed)
				if (c > 0 && (tsd = map_id2sd(p_sd[0])) != NULL)
				{
					sd->sc.data[SC_DANCING].val4= tsd->bl.id;
					sc_start4(&tsd->bl,SC_DANCING,100,skill_id,sd->sc.data[SC_DANCING].val2,0,sd->bl.id,skill_get_time(skill_id,*skill_lv)+1000);
					clif_skill_nodamage(&tsd->bl, &sd->bl, skill_id, *skill_lv, 1);
					tsd->skillid_dance = skill_id;
					tsd->skilllv_dance = *skill_lv;
				}
				return c;
		}
	}
	//Else: new search for partners.
	c = 0;
	memset (p_sd, 0, sizeof(p_sd));
	i = map_foreachinrange(skill_check_condition_char_sub, &sd->bl,
			range, BL_PC, &sd->bl, &c, &p_sd, skill_id);

	if (skill_id != PR_BENEDICTIO) //Apply the average lv to encore skills.
		*skill_lv = (i+(*skill_lv))/(c+1); //I know c should be one, but this shows how it could be used for the average of n partners.
	return c;
}

/*==========================================
 * 
 *------------------------------------------
 */

static int skill_check_condition_mob_master_sub (struct block_list *bl, va_list ap)
{
	int *c,src_id,mob_class,skill;
	struct mob_data *md;

	md=(struct mob_data*)bl;
	src_id=va_arg(ap,int);
	mob_class=va_arg(ap,int);
	skill=va_arg(ap,int);
	c=va_arg(ap,int *);

	if(md->master_id != src_id ||
		md->special_state.ai != (skill == AM_SPHEREMINE?2:3))
		return 0; //Non alchemist summoned mobs have nothing to do here.

	if(md->class_==mob_class)
		(*c)++;

	return 1;
}

static int skill_check_condition_hermod_sub(struct block_list *bl,va_list ap)
{
	struct npc_data *nd;
	nd=(struct npc_data*)bl;

	if (nd->bl.subtype == WARP)
		return 1;
	return 0;
}

/*==========================================
 * Determines if a given skill should be made to consume ammo 
 * when used by the player. [Skotlex]
 *------------------------------------------
 */
int skill_isammotype (TBL_PC *sd, int skill)
{
	return (
		(sd->status.weapon == W_BOW || (sd->status.weapon >= W_REVOLVER && sd->status.weapon <= W_GRENADE)) &&
		skill != HT_PHANTASMIC && skill != GS_MAGICALBULLET &&
		skill_get_type(skill) == BF_WEAPON && !(skill_get_nk(skill)&NK_NO_DAMAGE) 
	);
}

/*==========================================
 * Checks that you have the requirements for casting a skill.
 * Flag:
 * &1: finished casting the skill (invoke hp/sp/item consumption)
 * &2: picked menu entry (Warp Portal, Teleport and other menu based skills)
 *------------------------------------------
 */
int skill_check_condition (struct map_session_data *sd, int skill, int lv, int type)
{
	struct status_data *status;
	struct status_change *sc;
	int i,j,hp,sp,hp_rate,sp_rate,zeny,weapon,ammo,ammo_qty,state,spiritball,mhp;
	int index[10],itemid[10],amount[10];
	int delitem_flag = 1, checkitem_flag = 1;

	nullpo_retr(0, sd);

	if (lv <= 0) return 0;

	if( battle_config.gm_skilluncond &&
		pc_isGM(sd)>= battle_config.gm_skilluncond &&
		sd->skillitem != skill)
	{	//GMs don't override the skillItem check, otherwise they can use items without them being consumed! [Skotlex]
		sd->skillitem = sd->skillitemlv = -1;
		return 1;
	}

	status = &sd->battle_status;
	sc = &sd->sc;
	if (!sc->count)
		sc = NULL;
	
	if(pc_is90overweight(sd)) {
		clif_skill_fail(sd,skill,9,0);
		sd->skillitem = sd->skillitemlv = -1;
		return 0;
	}

	if (sd->state.abra_flag)
	{
		sd->skillitem = sd->skillitemlv = -1;
		if(type&1) sd->state.abra_flag = 0;
		return 1;
	}

	if (sd->menuskill_id == AM_PHARMACY &&
		(skill == AM_PHARMACY || skill == AC_MAKINGARROW || skill == BS_REPAIRWEAPON ||
		skill == AM_TWILIGHT1 || skill == AM_TWILIGHT2  || skill == AM_TWILIGHT3 
	)) {
		sd->skillitem = sd->skillitemlv = -1;
		return 0;
	}

	if(sd->skillitem == skill) {
		if(!type) //When a target was selected
		{	//Consume items that were skipped in pc_use_item [Skotlex]
			 if((i = sd->itemindex) == -1 ||
				sd->status.inventory[i].nameid != sd->itemid ||
				sd->inventory_data[i] == NULL ||
				!sd->inventory_data[i]->flag.delay_consume ||
				sd->status.inventory[i].amount < 1
				)
			{	//Something went wrong, item exploit?
				sd->itemid = sd->itemindex = -1;
				return 0;
			}
			//Consume
			sd->itemid = sd->itemindex = -1;
			if(skill == WZ_EARTHSPIKE && sc &&
				sc->data[SC_TKREST].timer != -1 && rand()%100 > sc->data[SC_TKREST].val2) // [marquis007]
				; //Do not consume item.
			else
				pc_delitem(sd,i,1,0);
		}
		if (type&1) //Casting finished
			sd->skillitem = sd->skillitemlv = -1;
		return 1;
	}
	// for the guild skills [celest]
	if (skill >= GD_SKILLBASE)
		j = GD_SKILLRANGEMIN + skill - GD_SKILLBASE;
	else if (skill >= HM_SKILLBASE)	//[orn]
		j = HM_SKILLRANGEMIN + skill - HM_SKILLBASE;
	else
		j = skill;
	if (j < 0 || j >= MAX_SKILL_DB)
  		return 0;
	//Code speedup, rather than using skill_get_* over and over again.
	if (lv < 1 || lv > MAX_SKILL_LEVEL)
		return 0;
	hp = skill_db[j].hp[lv-1];
	sp = skill_db[j].sp[lv-1];
	if((sd->skillid_old == BD_ENCORE) && skill == sd->skillid_dance)
		sp=sp/2;
	hp_rate = skill_db[j].hp_rate[lv-1];
	sp_rate = skill_db[j].sp_rate[lv-1];
	zeny = skill_db[j].zeny[lv-1];
	weapon = skill_db[j].weapon;
	ammo = skill_db[j].ammo;
	ammo_qty = skill_db[j].ammo_qty[lv-1];
	state = skill_db[j].state;
	spiritball = skill_db[j].spiritball[lv-1];
	mhp = skill_db[j].mhp[lv-1];
	for(i = 0; i < 10; i++) {
		itemid[i] = skill_db[j].itemid[i];
		amount[i] = skill_db[j].amount[i];
	}
	if(mhp > 0)
		hp += (status->max_hp * mhp)/100;
	if(hp_rate > 0)
		hp += (status->hp * hp_rate)/100;
	else
		hp += (status->max_hp * (-hp_rate))/100;
	if(sp_rate > 0)
		sp += (status->sp * sp_rate)/100;
	else
		sp += (status->max_sp * (-sp_rate))/100;

	if (!ammo && skill && skill_isammotype(sd, skill))
	{	//Assume this skill is using the weapon, therefore it requires arrows.
		ammo = 2;  //1<<1 <- look 1 (arrows) moved right 1 times.
		ammo_qty = skill_get_num(skill, lv);
		if (ammo_qty < 0) ammo_qty *= -1;
	}

	switch(skill) { // Check for cost reductions due to skills & SCs
		case MC_MAMMONITE:
			if(pc_checkskill(sd,BS_UNFAIRLYTRICK)>0)
				zeny -= zeny*10/100;
			break;
		case AL_HOLYLIGHT:
			if(sc && sc->data[SC_SPIRIT].timer!=-1 && sc->data[SC_SPIRIT].val2 == SL_PRIEST)
				sp *= 5;
			break;
		case SL_SMA:
		case SL_STUN:
		case SL_STIN:
		{
			int kaina_lv = pc_checkskill(sd,SL_KAINA);

			if(kaina_lv==0 || sd->status.base_level<70)
				break;
			if(sd->status.base_level>=90)
				sp -= sp*7*kaina_lv/100;
			else if(sd->status.base_level>=80)
				sp -= sp*5*kaina_lv/100;
			else if(sd->status.base_level>=70)
				sp -= sp*3*kaina_lv/100;
		}
			break;
		case MO_TRIPLEATTACK:
		case MO_CHAINCOMBO:
		case MO_COMBOFINISH:
		case CH_TIGERFIST:
		case CH_CHAINCRUSH:
			if(sc && sc->data[SC_SPIRIT].timer!=-1 && sc->data[SC_SPIRIT].val2 == SL_MONK)
				sp -= sp*25/100; //FIXME: Need real data. this is a custom value.
			break;
	}

	if(sd->dsprate!=100)
		sp=sp*sd->dsprate/100;

	switch(skill) {
	case SA_CASTCANCEL:
		if(sd->ud.skilltimer == -1) {
			clif_skill_fail(sd,skill,0,0);
			return 0;
		}
		break;
	case BS_MAXIMIZE:
	case NV_TRICKDEAD:
	case TF_HIDING:
	case AS_CLOAKING:
	case CR_AUTOGUARD:
	case CR_DEFENDER:
	case ST_CHASEWALK:
	case PA_GOSPEL:
	case CR_SHRINK:
	case TK_RUN:
		if(sc && sc->data[SkillStatusChangeTable(skill)].timer!=-1)
			return 1; //Allow turning off.
		break;

	case AL_WARP:
		if(!(type&2)) //Delete the item when the portal has been selected (type&2). [Skotlex]
			delitem_flag = 0;
		if(!battle_config.duel_allow_teleport && sd->duel_group) { // duel restriction [LuzZza]
			clif_displaymessage(sd->fd, "Duel: Can't use warp in duel.");
			return 0;
		}				
		break;
	case MO_CALLSPIRITS:
		if(sd->spiritball >= lv) {
			clif_skill_fail(sd,skill,0,0);
			return 0;
		}
		break;
	case CH_SOULCOLLECT:
		if(sd->spiritball >= 5) {
			clif_skill_fail(sd,skill,0,0);
			return 0;
		}
		break;
	case MO_FINGEROFFENSIVE:				//指弾
	case GS_FLING:
		if (sd->spiritball > 0 && sd->spiritball < spiritball) {
			spiritball = sd->spiritball;
			sd->spiritball_old = sd->spiritball;
		}
		else sd->spiritball_old = spiritball;
		break;
	case MO_BODYRELOCATION:
		if (sc && sc->data[SC_EXPLOSIONSPIRITS].timer!=-1)
			spiritball = 0;
		break;
	case MO_CHAINCOMBO:
		if(!sc)
			return 0;
		if(sc->data[SC_BLADESTOP].timer!=-1)
			break;
		if(sc->data[SC_COMBO].timer != -1 && sc->data[SC_COMBO].val1 == MO_TRIPLEATTACK)
			break;
		return 0;
	case MO_COMBOFINISH:					//猛龍拳
		if(!sc || sc->data[SC_COMBO].timer == -1 || sc->data[SC_COMBO].val1 != MO_CHAINCOMBO)
			return 0;
		break;
	case CH_TIGERFIST:						//伏虎拳
		if(!sc || sc->data[SC_COMBO].timer == -1 || sc->data[SC_COMBO].val1 != MO_COMBOFINISH)
			return 0;
		break;
	case CH_CHAINCRUSH:						//連柱崩撃
		if(!sc || sc->data[SC_COMBO].timer == -1)
			return 0;
		if(sc->data[SC_COMBO].val1 != MO_COMBOFINISH && sc->data[SC_COMBO].val1 != CH_TIGERFIST)
			return 0;
		break;
	case MO_EXTREMITYFIST:
//		if(sc && sc->data[SC_EXTREMITYFIST].timer != -1) //To disable Asura during the 5 min skill block uncomment this...
//			return 0;
		if(sc && sc->data[SC_BLADESTOP].timer!=-1)
			spiritball--;
		else if (sc && sc->data[SC_COMBO].timer != -1) {
			switch(sc->data[SC_COMBO].val1) {
				case MO_COMBOFINISH:
					spiritball = 4;
					break;
				case CH_TIGERFIST:
					spiritball = 3;
					break;
				case CH_CHAINCRUSH:	//It should consume whatever is left as long as it's at least 1.
					spiritball = sd->spiritball?sd->spiritball:1;
					break;
				default:
					return 0;
			}
		} else if(!type && !unit_can_move(&sd->bl)) //Check only on begin casting.
	  	{	//Placed here as ST_MOVE_ENABLE should not apply if rooted or on a combo. [Skotlex]
			clif_skill_fail(sd,skill,0,0);
			return 0;
		}
		break;

	case TK_MISSION: //Does not works on Non-Taekwon
		if ((sd->class_&MAPID_UPPERMASK) != MAPID_TAEKWON) {
			clif_skill_fail(sd,skill,0,0);
			return 0;
		}
		break;
		
	case TK_READYCOUNTER:
	case TK_READYDOWN:
	case TK_READYSTORM:
	case TK_READYTURN:
		if(sc && sc->data[SkillStatusChangeTable(skill)].timer!=-1)
			return 1; //Enable disabling them regardless of who you are.
	case TK_JUMPKICK:
		if ((sd->class_&MAPID_UPPERMASK) == MAPID_SOUL_LINKER) {
			//They do not work on Soul Linkers.
			clif_skill_fail(sd,skill,0,0);
			return 0;
		}
		break;

	case TK_TURNKICK:
	case TK_STORMKICK:
	case TK_DOWNKICK:
	case TK_COUNTER:
		if ((sd->class_&MAPID_UPPERMASK) == MAPID_SOUL_LINKER)
			return 0; //Anti-Soul Linker check in case you job-changed with Stances active.
		if(!sc || sc->data[SC_COMBO].timer == -1)
			return 0; //Combo needs to be ready
		if (pc_famerank(sd->char_id,MAPID_TAEKWON))
		{	//Unlimited Combo
			if (skill == sd->skillid_old) {
				status_change_end(&sd->bl, SC_COMBO, -1);
				sd->skillid_old = sd->skilllv_old = 0;
				return 0; //Can't repeat previous combo skill.				
			}
			break;
		} else 
		if(sc->data[SC_COMBO].val1 == skill)
			break; //Combo ready.
		return 0;
	case BD_ADAPTATION:				/* アドリブ */
		{
			struct skill_unit_group *group=NULL;
			int time;
			if(!sc || sc->data[SC_DANCING].timer==-1)
			{
				clif_skill_fail(sd,skill,0,0);
				return 0;
			}
			group=(struct skill_unit_group*)sc->data[SC_DANCING].val2;
			time = 1000*(sc->data[SC_DANCING].val3>>16);
			if (!group || (skill_get_time(sc->data[SC_DANCING].val1,group->skill_lv) - time <= skill_get_time2(skill,lv)))
			{
				clif_skill_fail(sd,skill,0,0);
				return 0;
			}
		}
		break;
	case PR_BENEDICTIO:
		{
			if (!battle_config.player_skill_partner_check ||
				(battle_config.gm_skilluncond && pc_isGM(sd) >= battle_config.gm_skilluncond)
			)
				break; //No need to do any partner checking [Skotlex]
			if (!(type&1))
			{	//Started casting.
				if (skill_check_pc_partner(sd, skill, &lv, 1, 0) < 2)
				{
					clif_skill_fail(sd,skill,0,0);
					return 0;
				}
			}
			else
			{	//Done casting
				//Should I repeat the check? If so, it would be best to only do this on cast-ending. [Skotlex]
				skill_check_pc_partner(sd, skill, &lv, 1, 1);
			}
		}
		break;
	case AM_CANNIBALIZE:
	case AM_SPHEREMINE:
		if(type&1){
			int c=0;
			int summons[5] = { 1020, 1068, 1118, 1500, 1368 };
			int maxcount = (skill==AM_CANNIBALIZE)? 6-lv : skill_get_maxcount(skill);
			int mob_class = (skill==AM_CANNIBALIZE)? summons[lv-1] :1142;
			if(battle_config.land_skill_limit && maxcount>0 && (battle_config.land_skill_limit&BL_PC)) {
				i = map_foreachinmap(skill_check_condition_mob_master_sub ,sd->bl.m, BL_MOB, sd->bl.id, mob_class, skill, &c);
				if(c >= maxcount || (skill==AM_CANNIBALIZE && c != i))
				{	//Fails when: exceed max limit. There are other plant types already out.
					clif_skill_fail(sd,skill,0,0);
					return 0;
				}
			}
		}
		break;
	case WZ_FIREPILLAR: // celest
		if (lv <= 5)	// no gems required at level 1-5
			itemid[0] = 0;
		break;
	case SL_SMA:
		if(type) break; //Only do the combo check when the target is selected (type == 0)
		if(!sc || sc->data[SC_SMA].timer == -1)
			return 0;
		break;	

	case HT_POWER:
		if(!sc || sc->data[SC_COMBO].timer == -1 || sc->data[SC_COMBO].val1 != skill)
			return 0;
		break;
	case AM_BERSERKPITCHER:
	case AM_POTIONPITCHER:
	case CR_SLIMPITCHER:
	case MG_STONECURSE:
	case CR_CULTIVATION:
	case SA_FLAMELAUNCHER:
	case SA_FROSTWEAPON:
	case SA_LIGHTNINGLOADER:
	case SA_SEISMICWEAPON:
		delitem_flag = 0;
		break;
	case SA_DELUGE:
	case SA_VOLCANO:
	case SA_VIOLENTGALE:
	case SA_LANDPROTECTOR:
	{	//Does not consumes if the skill is already active. [Skotlex]
		struct skill_unit_group *sg;
		if ((sg= skill_locate_element_field(&sd->bl)) != NULL && sg->skill_id == skill)
		{
			if (sg->limit - DIFF_TICK(gettick(), sg->tick) > 0)
				checkitem_flag = delitem_flag = 0;
			else sg->limit = 0; //Disable it.
		}
		break;
	}
	case CG_HERMODE:
		if (map_foreachinrange (skill_check_condition_hermod_sub, &sd->bl,
			skill_get_splash(skill, lv), BL_NPC) < 1)
		{
			clif_skill_fail(sd,skill,0,0);
			return 0;
		}
		break;
	case CG_MOONLIT: //Check there's no wall in the range+1 area around the caster. [Skotlex]
		{
			int i,x,y,range = skill_get_splash(skill, lv)+1;
			int size = range*2+1;
			for (i=0;i<size*size;i++) {
				x = sd->bl.x+(i%size-range);
				y = sd->bl.y+(i/size-range);
				if (map_getcell(sd->bl.m,x,y,CELL_CHKWALL)) {
					clif_skill_fail(sd,skill,0,0);
					return 0;
				}	
			}
		}
		break;
	case PR_REDEMPTIO:
		{
			int exp;
			if(((exp = pc_nextbaseexp(sd)) > 0 && sd->status.base_exp*100/exp < 1) ||
				((exp = pc_nextjobexp(sd)) > 0 && sd->status.job_exp*100/exp < 1)) {
				clif_skill_fail(sd,skill,0,0); //Not enough exp.
				return 0;
			}
			break;
		}
	case AM_TWILIGHT2:
	case AM_TWILIGHT3:
		if (!party_skill_check(sd, sd->status.party_id, skill, lv))
		{
			clif_skill_fail(sd,skill,0,0);
			return 0;
		}
		break;
	//SHOULD BE OPTIMALIZED [Komurka]
	//Optimized #1. optimize comfort later. [Vicious]
	case SG_SUN_WARM:
	case SG_MOON_WARM:
	case SG_STAR_WARM:
		if ((sd->bl.m == sd->feel_map[skill-SG_SUN_WARM].m) || (sc && sc->data[SC_MIRACLE].timer!=-1))
			break;
		clif_skill_fail(sd,skill,0,0);
		return 0;
		break;
	case SG_SUN_COMFORT:
		if ((sd->bl.m == sd->feel_map[0].m && (battle_config.allow_skill_without_day || is_day_of_sun())) || (sc && sc->data[SC_MIRACLE].timer!=-1))
			break;
		clif_skill_fail(sd,skill,0,0);
		return 0;
	case SG_MOON_COMFORT:
		if ((sd->bl.m == sd->feel_map[1].m && (battle_config.allow_skill_without_day || is_day_of_moon())) || (sc && sc->data[SC_MIRACLE].timer!=-1))
			break;
		clif_skill_fail(sd,skill,0,0);
		return 0;
	case SG_STAR_COMFORT:
		if ((sd->bl.m == sd->feel_map[2].m && (battle_config.allow_skill_without_day || is_day_of_star())) || (sc && sc->data[SC_MIRACLE].timer!=-1))
			break;
		clif_skill_fail(sd,skill,0,0);
		return 0;
	case SG_FUSION:
		if (!sc || sc->data[SC_FUSION].timer!=-1)
			return 1;
		if (sc && sc->data[SC_SPIRIT].timer != -1 && sc->data[SC_SPIRIT].val2 == SL_STAR)
			break;
		return 0;
	case GD_BATTLEORDER:
	case GD_REGENERATION:
	case GD_RESTORE:
		//Emergency Recall is handled on skill_notok
		if (skill != GD_EMERGENCYCALL && !agit_flag) {
			clif_skill_fail(sd,skill,0,0);
			return 0;
		}
	case GD_EMERGENCYCALL:
		if (!sd->status.guild_id || !sd->state.gmaster_flag)
			return 0;
		if (lv <= 0)
			return 0;
		break;

	case GS_GLITTERING:
		if(sd->spiritball >= 10) {
			clif_skill_fail(sd,skill,0,0);
			return 0;
		}
		break;
	
	case NJ_ISSEN:
	case NJ_BUNSINJYUTSU:
		if (!sc || sc->data[SC_NEN].timer==-1) {
			clif_skill_fail(sd,skill,0,0);
			return 0;
	  	}
		break;
	
	case NJ_ZENYNAGE:
		if(sd->status.zeny < zeny) {
			clif_skill_fail(sd,skill,5,0);
			return 0;
		}
		zeny = 0; //Zeny is reduced on skill_attack.
		break;
	case AM_CALLHOMUN: //Can't summon if a hom is already out
		if (sd->status.hom_id && !sd->homunculus.vaporize) {
			clif_skill_fail(sd,skill,0,0);
			return 0;
		}
		if (sd->status.hom_id) //Don't delete items when hom is already out.
			checkitem_flag = delitem_flag = 0;
		break;
	}

	if(!(type&2)){
		if( hp>0 && status->hp <= (unsigned int)hp) {				/* HPチェック */
			clif_skill_fail(sd,skill,2,0);		/* HP不足：失敗通知 */
			return 0;
		}
		if( sp>0 && status->sp < (unsigned int)sp) {				/* SPチェック */
			clif_skill_fail(sd,skill,1,0);		/* SP不足：失敗通知 */
			return 0;
		}
		if( zeny>0 && sd->status.zeny < zeny) {
			clif_skill_fail(sd,skill,5,0);
			return 0;
		}
	
		if(!pc_check_weapontype(sd,weapon)) {
			clif_skill_fail(sd,skill,6,0);
			return 0;
		}
		if(ammo) { //Skill requires stuff equipped in the arrow slot.
			if((i=sd->equip_index[10]) < 0 ||
				!sd->inventory_data[i] ||
				sd->status.inventory[i].amount < ammo_qty
			) {
				clif_arrow_fail(sd,0);
				return 0;
			}
			if (!(ammo&1<<sd->inventory_data[i]->look))
			{	//Ammo type check. Send the "wrong weapon type" message
				//which is the closest we have to wrong ammo type. [Skotlex]
				clif_arrow_fail(sd,0); //Haplo suggested we just send the equip-arrows message instead. [Skotlex]
				//clif_skill_fail(sd,skill,6,0);
				return 0;
			}
		}
		if( spiritball > 0 && sd->spiritball < spiritball) {
			clif_skill_fail(sd,skill,0,0);
			return 0;
		}
	}

	switch(state) {
	case ST_HIDING:
		if(!(sc && sc->option&OPTION_HIDE)) {
			clif_skill_fail(sd,skill,0,0);
			return 0;
		}
		break;
	case ST_CLOAKING:
		if(!pc_iscloaking(sd)) {
			clif_skill_fail(sd,skill,0,0);
			return 0;
		}
		break;
	case ST_HIDDEN:
		if(!pc_ishiding(sd)) {
			clif_skill_fail(sd,skill,0,0);
			return 0;
		}
		break;
	case ST_RIDING:
		if(!pc_isriding(sd)) {
			clif_skill_fail(sd,skill,0,0);
			return 0;
		}
		break;
	case ST_FALCON:
		if(!pc_isfalcon(sd)) {
			clif_skill_fail(sd,skill,0,0);
			return 0;
		}
		break;
	case ST_CART:
		if(!pc_iscarton(sd)) {
			clif_skill_fail(sd,skill,0,0);
			return 0;
		}
		break;
	case ST_SHIELD:
		if(sd->status.shield <= 0) {
			clif_skill_fail(sd,skill,0,0);
			return 0;
		}
		break;
	case ST_SIGHT:
		if((!sc || sc->data[SC_SIGHT].timer == -1) && type&1) {
			clif_skill_fail(sd,skill,0,0);
			return 0;
		}
		break;
	case ST_EXPLOSIONSPIRITS:
		if(!sc || sc->data[SC_EXPLOSIONSPIRITS].timer == -1) {
			clif_skill_fail(sd,skill,0,0);
			return 0;
		}
		break;
	case ST_CARTBOOST:
		if(!pc_iscarton(sd) || !sc || sc->data[SC_CARTBOOST].timer == -1) {
			clif_skill_fail(sd,skill,0,0);
			return 0;
		}
		break;
	case ST_RECOV_WEIGHT_RATE:
		if(battle_config.natural_heal_weight_rate <= 100 && sd->weight*100/sd->max_weight >= battle_config.natural_heal_weight_rate) {
			clif_skill_fail(sd,skill,0,0);
			return 0;
		}
		break;
	case ST_MOVE_ENABLE:
		if(type)//Check only on begin casting. [Skotlex]
			break;
		
		if (sc && sc->data[SC_COMBO].timer != -1 && sc->data[SC_COMBO].val1 == skill)
			sd->ud.canmove_tick = gettick(); //When using a combo, cancel the can't move delay to enable the skill. [Skotlex]
			
		if (!unit_can_move(&sd->bl)) {
			clif_skill_fail(sd,skill,0,0);
			return 0;
		}
		break;
	case ST_WATER:
		if (sc && (sc->data[SC_DELUGE].timer != -1 || sc->data[SC_SUITON].timer != -1))
			break;
		if (map_getcell(sd->bl.m,sd->bl.x,sd->bl.y,CELL_CHKWATER))
			break;
		clif_skill_fail(sd,skill,0,0);
		return 0;
	}

	if (checkitem_flag) {
		for(i=0;i<10;i++) {
			int x = lv%11 - 1;
			index[i] = -1;
			if(itemid[i] <= 0)
				continue;
			if(itemid[i] >= 715 && itemid[i] <= 717 && skill != HW_GANBANTEIN)
			{
				if (sd->special_state.no_gemstone)
				{	//Make it substract 1 gem rather than skipping the cost.
					if (--amount[i] < 1)
						continue;
				}
				if(sc && sc->data[SC_INTOABYSS].timer != -1)
					continue;
			} else
			if(itemid[i] == 1065 && sc && sc->data[SC_INTOABYSS].timer != -1)
				continue;

			if((skill == AM_POTIONPITCHER ||
				skill == CR_SLIMPITCHER ||
				skill == CR_CULTIVATION) && i != x)
				continue;

			index[i] = pc_search_inventory(sd,itemid[i]);
			if(index[i] < 0 || sd->status.inventory[index[i]].amount < amount[i]) {
				if(itemid[i] == 716 || itemid[i] == 717)
					clif_skill_fail(sd,skill,(7+(itemid[i]-716)),0);
				else
					clif_skill_fail(sd,skill,0,0);
				return 0;
			}
			if(itemid[i] >= 715 && itemid[i] <= 717 && sc && sc->data[SC_SPIRIT].timer != -1 && sc->data[SC_SPIRIT].val2 == SL_WIZARD)
				index[i] = -1; //Gemstones are checked, but not substracted from inventory.
		}
	}

	if(!(type&1))
		return 1;

	sd->state.arrow_atk = ammo?1:0; //Update arrow-atk state on cast-end.

	if(delitem_flag) {
		for(i=0;i<10;i++) {
			if(index[i] >= 0)
				pc_delitem(sd,index[i],amount[i],0);
		}
//	Ammo is now reduced in battle_calc_weapon_attack. [Skotlex]		
//		if (ammo && battle_config.arrow_decrement)
//			pc_delitem(sd,sd->equip_index[10],ammo_qty,0);
	}

	if(type&2)
		return 1;

	if(sp || hp)
		status_zap(&sd->bl, hp, sp);
	if(zeny > 0)					// Zeny消費
		pc_payzeny(sd,zeny);
	if(spiritball > 0)
		pc_delspiritball(sd,spiritball,0);

	return 1;
}

/*==========================================
 * 
 *------------------------------------------
 */
int skill_castfix (struct block_list *bl, int skill_id, int skill_lv)
{
	int castnodex = skill_get_castnodex(skill_id, skill_lv);
	int time = skill_get_cast(skill_id, skill_lv);	
	struct map_session_data *sd;

	nullpo_retr(0, bl);
	BL_CAST(BL_PC, bl, sd);
	
	// calculate base cast time (reduced by dex)
	if (!(castnodex&1)) {			// castnodex&~1? wtf. [blackhole89]
		int scale = battle_config.castrate_dex_scale - status_get_dex(bl);
		if (scale > 0)	// not instant cast
			time = time * scale / battle_config.castrate_dex_scale;
		else return 0;	// instant cast
	}

	// calculate cast time reduced by card bonuses
	if (sd && sd->castrate != 100)
		time = time * sd->castrate / 100;

	// config cast time multiplier
	if (battle_config.cast_rate != 100)
		time = time * battle_config.cast_rate / 100;

  	// calculate cast time reduced by skill bonuses
	if (!(castnodex&2))
		time = skill_castfix_sc(bl, time);

	// return final cast time
	return (time > 0) ? time : 0;
}

/*==========================================
 * Does cast-time reductions based on sc data.
 *------------------------------------------
 */
int skill_castfix_sc (struct block_list *bl, int time)
{
	struct status_change *sc = status_get_sc(bl);

	if (sc && sc->count) {
		if (sc->data[SC_SUFFRAGIUM].timer != -1) {
			time -= time * (sc->data[SC_SUFFRAGIUM].val1 * 15) / 100;
			status_change_end(bl, SC_SUFFRAGIUM, -1);
		}

		if (time <= 0)
			return 0; //Only Suffragium gets consumed even if time is 0
	
		if (sc->data[SC_MEMORIZE].timer != -1 && time > 0) {
			time>>=1;
			if ((--sc->data[SC_MEMORIZE].val2) <= 0)
				status_change_end(bl, SC_MEMORIZE, -1);
		}
		if (sc->data[SC_POEMBRAGI].timer != -1)
			time -= time * sc->data[SC_POEMBRAGI].val2 / 100;
	}
	return (time > 0) ? time : 0;
}

/*==========================================
 *
 *------------------------------------------
 */
int skill_delayfix (struct block_list *bl, int skill_id, int skill_lv)
{
	int delaynodex = skill_get_delaynodex(skill_id, skill_lv);
	int time = skill_get_delay(skill_id, skill_lv);
	
	nullpo_retr(0, bl);

	if (bl->type&battle_config.no_skill_delay)
		return battle_config.min_skill_delay_limit; 
	
	// instant cast attack skills depend on aspd as delay [celest]
	if (time == 0) {
		if (skill_get_type(skill_id) == BF_WEAPON && !(skill_get_nk(skill_id)&NK_NO_DAMAGE))
			time = status_get_adelay(bl); //Use attack delay as default delay.
		else
			time = battle_config.default_skill_delay;
	} else if (time < 0)
		time = -time + status_get_amotion(bl);	// if set to <0, the attack motion is added.

	if (battle_config.delay_dependon_dex && !(delaynodex&1))
	{	// if skill casttime is allowed to be reduced by dex
		int scale = battle_config.castrate_dex_scale - status_get_dex(bl);
		if (scale > 0)
			time = time * scale / battle_config.castrate_dex_scale;
		else //To be capped later to minimum.
			time = 0;
	}

	if (bl->type == BL_PC && ((TBL_PC*)bl)->delayrate != 100)
		time = time * ((TBL_PC*)bl)->delayrate / 100;

	if (battle_config.delay_rate != 100)
		time = time * battle_config.delay_rate / 100;

	if (!(delaynodex&2))
	{
		struct status_change *sc;
		sc= status_get_sc(bl);
		if (sc && sc->count) {
			if (sc->data[SC_POEMBRAGI].timer != -1)
				time -= time * sc->data[SC_POEMBRAGI].val3 / 100;
			if (sc->data[SC_SPIRIT].timer != -1)
			switch (skill_id) {
				case CR_SHIELDBOOMERANG:
					if (sc->data[SC_SPIRIT].val2 == SL_CRUSADER)
						time /=2;
					break;
				case AS_SONICBLOW:
					if (!map_flag_gvg(bl->m) && sc->data[SC_SPIRIT].val2 == SL_ASSASIN)
						time /= 2;
					break;
			}
		}
	}

	return (time < battle_config.min_skill_delay_limit)?
		battle_config.min_skill_delay_limit:time;
}

/*=========================================
 * 
 *----------------------------------------
 */
void skill_brandishspear_first (struct square *tc, int dir, int x, int y)
{
	nullpo_retv(tc);

	if(dir == 0){
		tc->val1[0]=x-2;
		tc->val1[1]=x-1;
		tc->val1[2]=x;
		tc->val1[3]=x+1;
		tc->val1[4]=x+2;
		tc->val2[0]=
		tc->val2[1]=
		tc->val2[2]=
		tc->val2[3]=
		tc->val2[4]=y-1;
	}
	else if(dir==2){
		tc->val1[0]=
		tc->val1[1]=
		tc->val1[2]=
		tc->val1[3]=
		tc->val1[4]=x+1;
		tc->val2[0]=y+2;
		tc->val2[1]=y+1;
		tc->val2[2]=y;
		tc->val2[3]=y-1;
		tc->val2[4]=y-2;
	}
	else if(dir==4){
		tc->val1[0]=x-2;
		tc->val1[1]=x-1;
		tc->val1[2]=x;
		tc->val1[3]=x+1;
		tc->val1[4]=x+2;
		tc->val2[0]=
		tc->val2[1]=
		tc->val2[2]=
		tc->val2[3]=
		tc->val2[4]=y+1;
	}
	else if(dir==6){
		tc->val1[0]=
		tc->val1[1]=
		tc->val1[2]=
		tc->val1[3]=
		tc->val1[4]=x-1;
		tc->val2[0]=y+2;
		tc->val2[1]=y+1;
		tc->val2[2]=y;
		tc->val2[3]=y-1;
		tc->val2[4]=y-2;
	}
	else if(dir==1){
		tc->val1[0]=x-1;
		tc->val1[1]=x;
		tc->val1[2]=x+1;
		tc->val1[3]=x+2;
		tc->val1[4]=x+3;
		tc->val2[0]=y-4;
		tc->val2[1]=y-3;
		tc->val2[2]=y-1;
		tc->val2[3]=y;
		tc->val2[4]=y+1;
	}
	else if(dir==3){
		tc->val1[0]=x+3;
		tc->val1[1]=x+2;
		tc->val1[2]=x+1;
		tc->val1[3]=x;
		tc->val1[4]=x-1;
		tc->val2[0]=y-1;
		tc->val2[1]=y;
		tc->val2[2]=y+1;
		tc->val2[3]=y+2;
		tc->val2[4]=y+3;
	}
	else if(dir==5){
		tc->val1[0]=x+1;
		tc->val1[1]=x;
		tc->val1[2]=x-1;
		tc->val1[3]=x-2;
		tc->val1[4]=x-3;
		tc->val2[0]=y+3;
		tc->val2[1]=y+2;
		tc->val2[2]=y+1;
		tc->val2[3]=y;
		tc->val2[4]=y-1;
	}
	else if(dir==7){
		tc->val1[0]=x-3;
		tc->val1[1]=x-2;
		tc->val1[2]=x-1;
		tc->val1[3]=x;
		tc->val1[4]=x+1;
		tc->val2[1]=y;
		tc->val2[0]=y+1;
		tc->val2[2]=y-1;
		tc->val2[3]=y-2;
		tc->val2[4]=y-3;
	}

}

/*=========================================
 *
 *-----------------------------------------
 */
void skill_brandishspear_dir (struct square *tc, int dir, int are)
{
	int c;

	nullpo_retv(tc);

	for(c=0;c<5;c++){
		if(dir==0){
			tc->val2[c]+=are;
		}else if(dir==1){
			tc->val1[c]-=are; tc->val2[c]+=are;
		}else if(dir==2){
			tc->val1[c]-=are;
		}else if(dir==3){
			tc->val1[c]-=are; tc->val2[c]-=are;
		}else if(dir==4){
			tc->val2[c]-=are;
		}else if(dir==5){
			tc->val1[c]+=are; tc->val2[c]-=are;
		}else if(dir==6){
			tc->val1[c]+=are;
		}else if(dir==7){
			tc->val1[c]+=are; tc->val2[c]+=are;
		}
	}
}

/*==========================================
 * Weapon Repair [Celest/DracoRPG]
 *------------------------------------------
 */
void skill_repairweapon (struct map_session_data *sd, int idx)
{
	int material;
	int materials[4] = { 1002, 998, 999, 756 };
	struct item *item;
	struct map_session_data *target_sd;

	nullpo_retv(sd);
	target_sd = map_id2sd(sd->menuskill_lv);
	if (!target_sd) //Failed....
		return;
	if(idx==0xFFFF) // No item selected ('Cancel' clicked)
		return;
	if(idx < 0 || idx >= MAX_INVENTORY)
		return; //Invalid index??

   item = &target_sd->status.inventory[idx];
	if(item->nameid <= 0 || item->attribute == 0)
		return; //Again invalid item....

	if(sd!=target_sd && !battle_check_range(&sd->bl,&target_sd->bl,skill_get_range2(&sd->bl, sd->menuskill_id,pc_checkskill(sd, sd->menuskill_id)))){
		clif_item_repaireffect(sd,item->nameid,1);
		return;
	}

	if (itemdb_type(item->nameid)==4)
		material = materials [itemdb_wlv(item->nameid)-1]; // Lv1/2/3/4 weapons consume 1 Iron Ore/Iron/Steel/Rough Oridecon
	else
		material = materials [2]; // Armors consume 1 Steel
	if (pc_search_inventory(sd,material) < 0 ) {
		clif_skill_fail(sd,sd->menuskill_id,0,0);
		return;
	}
	item->attribute=0;
	clif_equiplist(target_sd);
	pc_delitem(sd,pc_search_inventory(sd,material),1,0);
	clif_item_repaireffect(sd,item->nameid,0);
	if(sd!=target_sd)
		clif_item_repaireffect(target_sd,item->nameid,0);
}

/*==========================================
 * Item Appraisal
 *------------------------------------------
 */
void skill_identify (struct map_session_data *sd, int idx)
{
	int flag=1;

	nullpo_retv(sd);

	if(idx >= 0 && idx < MAX_INVENTORY) {
		if(sd->status.inventory[idx].nameid > 0 && sd->status.inventory[idx].identify == 0 ){
			flag=0;
			sd->status.inventory[idx].identify=1;
		}
	}
	clif_item_identified(sd,idx,flag);
}

/*==========================================
 * Weapon Refine [Celest]
 *------------------------------------------
 */
void skill_weaponrefine (struct map_session_data *sd, int idx)
{
	int i = 0, ep = 0, per;
	int material[5] = { 0, 1010, 1011, 984, 984 };
	struct item *item;

	nullpo_retv(sd);

	if (idx >= 0 && idx < MAX_INVENTORY) {
		struct item_data *ditem = sd->inventory_data[idx];
		item = &sd->status.inventory[idx];

		if(item->nameid > 0 && ditem->type == 4) {
			if (item->refine >= sd->menuskill_lv ||
				item->refine >= MAX_REFINE ||		// if it's no longer refineable
				ditem->flag.no_refine ||	// if the item isn't refinable
				(i = pc_search_inventory(sd, material [ditem->wlv])) < 0 ) { //fixed by Lupus (item pos can be = 0!)
				clif_skill_fail(sd,sd->menuskill_id,0,0);
				return;
			}

			per = percentrefinery [ditem->wlv][(int)item->refine];
			per += (sd->status.job_level-50)/2; //Updated per the new kro descriptions. [Skotlex]

			if (per > rand() % 100) {
				item->refine++;
				pc_delitem(sd, i, 1, 0);
				if(item->equip) {
					ep = item->equip;
					pc_unequipitem(sd,idx,3);
				}
				clif_refine(sd->fd,sd,0,idx,item->refine);
				clif_delitem(sd,idx,1);
				clif_additem(sd,idx,1,0);
				if (ep)
					pc_equipitem(sd,idx,ep);
				clif_misceffect(&sd->bl,3);
				if(item->refine == MAX_REFINE && item->card[0] == CARD0_FORGE &&
					MakeDWord(item->card[2],item->card[3]) == sd->char_id){ // Fame point system [DracoRPG]
					switch(ditem->wlv){
						case 1:
							pc_addfame(sd,1); // Success to refine to +10 a lv1 weapon you forged = +1 fame point
							break;
						case 2:
							pc_addfame(sd,25); // Success to refine to +10 a lv2 weapon you forged = +25 fame point
							break;
						case 3:
							pc_addfame(sd,1000); // Success to refine to +10 a lv3 weapon you forged = +1000 fame point
							break;
					}
				}
			} else {
				pc_delitem(sd, i, 1, 0);
				item->refine = 0;
				if(item->equip)
					pc_unequipitem(sd,idx,3);
				clif_refine(sd->fd,sd,1,idx,item->refine);
				pc_delitem(sd,idx,1,0);
				clif_misceffect(&sd->bl,2);
				clif_emotion(&sd->bl, 23);
			}
		}
	}
}

/*==========================================
 *
 *------------------------------------------
 */
int skill_autospell (struct map_session_data *sd, int skillid)
{
	int skilllv;
	int maxlv=1,lv;

	nullpo_retr(0, sd);

	skilllv = sd->menuskill_lv;
	lv=pc_checkskill(sd,skillid);

	if(skilllv <= 0 || !lv) return 0; // Player must learn the skill before doing auto-spell [Lance]

	if(skillid==MG_NAPALMBEAT)	maxlv=3;
	else if(skillid==MG_COLDBOLT || skillid==MG_FIREBOLT || skillid==MG_LIGHTNINGBOLT){
		if (sd->sc.data[SC_SPIRIT].timer != -1 && sd->sc.data[SC_SPIRIT].val2 == SL_SAGE)
			maxlv =10; //Soul Linker bonus. [Skotlex]
		else if(skilllv==2) maxlv=1;
		else if(skilllv==3) maxlv=2;
		else if(skilllv>=4) maxlv=3;
	}
	else if(skillid==MG_SOULSTRIKE){
		if(skilllv==5) maxlv=1;
		else if(skilllv==6) maxlv=2;
		else if(skilllv>=7) maxlv=3;
	}
	else if(skillid==MG_FIREBALL){
		if(skilllv==8) maxlv=1;
		else if(skilllv>=9) maxlv=2;
	}
	else if(skillid==MG_FROSTDIVER) maxlv=1;
	else return 0;

	if(maxlv > lv)
		maxlv = lv;

	sc_start4(&sd->bl,SC_AUTOSPELL,100,skilllv,skillid,maxlv,0,
		skill_get_time(SA_AUTOSPELL,skilllv));
	return 0;
}

/*==========================================
 *
 *------------------------------------------
 */

static int skill_gangster_count (struct block_list *bl, va_list ap)
{
	struct map_session_data *sd;
	sd=(struct map_session_data*)bl;

	if(sd && pc_issit(sd) && pc_checkskill(sd,RG_GANGSTER) > 0)
		return 1;
	return 0;
}

static int skill_gangster_in (struct block_list *bl, va_list ap)
{
	struct map_session_data *sd;
	sd=(struct map_session_data*)bl;
	if(sd && pc_issit(sd) && pc_checkskill(sd,RG_GANGSTER) > 0)
		sd->state.gangsterparadise=1;
	return 0;
}

static int skill_gangster_out (struct block_list *bl, va_list ap)
{
	struct map_session_data *sd;
	sd=(struct map_session_data*)bl;
	if(sd && sd->state.gangsterparadise)
		sd->state.gangsterparadise=0;
	return 0;
}

int skill_gangsterparadise (struct map_session_data *sd, int type)
{
	int range;
	nullpo_retr(0, sd);

	if((range = pc_checkskill(sd,RG_GANGSTER)) <= 0)
		return 0;
	range = skill_get_splash(RG_GANGSTER, range);

	if(type==1) {
		if (map_foreachinrange(skill_gangster_count,&sd->bl, range, BL_PC) > 1)
	  	{
			map_foreachinrange(skill_gangster_in,&sd->bl, range, BL_PC);
			sd->state.gangsterparadise = 1;
		}
		return 0;
	}
	else if(type==0) {
		if (map_foreachinrange(skill_gangster_count,&sd->bl, range, BL_PC) < 2)
			map_foreachinrange(skill_gangster_out,&sd->bl, range, BL_PC);
		sd->state.gangsterparadise = 0;
		return 0;
	}
	return 0;
}
/*==========================================
 * Taekwon TK_HPTIME and TK_SPTIME skills [Dralnu]
 *------------------------------------------
 */
static int skill_rest_count (struct block_list *bl, va_list ap)
{
	struct map_session_data *sd;
	sd=(struct map_session_data*)bl;

	if(sd && pc_issit(sd) && (pc_checkskill(sd,TK_HPTIME) > 0 || pc_checkskill(sd,TK_SPTIME) > 0  ))
		return 1;
	return 0;
}

static int skill_rest_in(struct block_list *bl, va_list ap)
{
	struct map_session_data *sd;
	nullpo_retr(0, bl);
	nullpo_retr(0, ap);

	sd=(struct map_session_data*)bl;
	if(sd && pc_issit(sd) && (pc_checkskill(sd,TK_HPTIME) > 0 || pc_checkskill(sd,TK_SPTIME) > 0 )){
		sd->state.rest=1;
		status_calc_pc(sd,0);
	}		
	return 0;
}

static int skill_rest_out(struct block_list *bl, va_list ap)
{
	struct map_session_data *sd;
	sd=(struct map_session_data*)bl;
	if(sd && sd->state.rest != 0)
		sd->state.rest=0;
	return 0;
}

int skill_rest(struct map_session_data *sd, int type)
{
	int range;
	nullpo_retr(0, sd);

	if((range = pc_checkskill(sd,TK_HPTIME)) > 0)
		range = skill_get_splash(TK_HPTIME, range);
	else if ((range =	pc_checkskill(sd,TK_SPTIME)) > 0)
		range = skill_get_splash(TK_SPTIME, range);
	else
		return 0;

	
	if(type==1) {	//When you sit down
		if (map_foreachinrange(skill_rest_count,&sd->bl, range, BL_PC) > 1)
		{
			map_foreachinrange(skill_rest_in,&sd->bl, range, BL_PC);
			sd->state.rest = 1;
			status_calc_pc(sd,0);
		}
		return 0;
	}
	else if(type==0) {	//When you stand up
		if (map_foreachinrange(skill_rest_count,&sd->bl, range, BL_PC) < 2)
			map_foreachinrange(skill_rest_out,&sd->bl, range, BL_PC);
		sd->state.rest = 0;
		status_calc_pc(sd,0);
		return 0;
	}
	return 0;
}
/*==========================================
 *
 *------------------------------------------
 */
int skill_frostjoke_scream (struct block_list *bl, va_list ap)
{
	struct block_list *src;
	int skillnum,skilllv;
	unsigned int tick;

	nullpo_retr(0, bl);
	nullpo_retr(0, ap);
	nullpo_retr(0, src=va_arg(ap,struct block_list*));

	skillnum=va_arg(ap,int);
	skilllv=va_arg(ap,int);
	if(skilllv <= 0) return 0;
	tick=va_arg(ap,unsigned int);

	if (src == bl || status_isdead(bl))
		return 0;
	if (bl->type == BL_PC) {
		struct map_session_data *sd = (struct map_session_data *)bl;
		if (sd && sd->sc.option&OPTION_INVISIBLE)
			return 0;
	}
	//It has been reported that Scream/Joke works the same regardless of woe-setting. [Skotlex]
	if(battle_check_target(src,bl,BCT_ENEMY) > 0)
		skill_additional_effect(src,bl,skillnum,skilllv,BF_MISC,tick);
	else if(battle_check_target(src,bl,BCT_PARTY) > 0 && rand()%100 < 10)
		skill_additional_effect(src,bl,skillnum,skilllv,BF_MISC,tick);

	return 0;
}

/*==========================================
 *
 *------------------------------------------
 */
void skill_unitsetmapcell (struct skill_unit *src, int skill_num, int skill_lv, int flag)
{
	int i,x,y,range = skill_get_unit_range(skill_num,skill_lv);
	int size = range*2+1;

	for (i=0;i<size*size;i++) {
		x = src->bl.x+(i%size-range);
		y = src->bl.y+(i/size-range);
		map_setcell(src->bl.m,x,y,flag);
	}
}

/*==========================================
 * Sets a map cell around the caster, according to the skill's splash range.
 *------------------------------------------
 */
void skill_setmapcell (struct block_list *src, int skill_num, int skill_lv, int flag)
{
	int i,x,y,range = skill_get_splash(skill_num, skill_lv);
	int size = range*2+1;

	for (i=0;i<size*size;i++) {
		x = src->x+(i%size-range);
		y = src->y+(i/size-range);
		map_setcell(src->m,x,y,flag);
	}
}
	
/*==========================================
 *
 *------------------------------------------
 */
int skill_attack_area (struct block_list *bl, va_list ap)
{
	struct block_list *src,*dsrc;
	int atk_type,skillid,skilllv,flag,type;
	unsigned int tick;

	if(status_isdead(bl))
		return 0;

	atk_type = va_arg(ap,int);
	src=va_arg(ap,struct block_list*);
	dsrc=va_arg(ap,struct block_list*);
	skillid=va_arg(ap,int);
	skilllv=va_arg(ap,int);
	tick=va_arg(ap,unsigned int);
	flag=va_arg(ap,int);
	type=va_arg(ap,int);

	if(battle_check_target(dsrc,bl,type) <= 0 ||
		!status_check_skilluse(NULL, bl, skillid, 2))
		return 0;
		
	return skill_attack(atk_type,src,dsrc,bl,skillid,skilllv,tick,flag);
}
/*==========================================
 *
 *------------------------------------------
 */
int skill_clear_group (struct block_list *bl, int flag)
{
	struct unit_data *ud = unit_bl2ud(bl);
	struct skill_unit_group *group[MAX_SKILLUNITGROUP];
	int i, count=0;

	nullpo_retr(0, bl);
	if (!ud) return 0;

	//All groups to be deleted are first stored on an array since the array elements shift around when you delete them. [Skotlex]
	for (i=0;i<MAX_SKILLUNITGROUP && ud->skillunit[i];i++)
	{
		switch (ud->skillunit[i]->skill_id) {
			case SA_DELUGE:
			case SA_VOLCANO:
			case SA_VIOLENTGALE:
			case SA_LANDPROTECTOR:
			case NJ_SUITON:
			case NJ_KAENSIN:
				if (flag&1)
					group[count++]= ud->skillunit[i];
				break;
			default:
				if (flag&2 && skill_get_inf2(ud->skillunit[i]->skill_id)&INF2_TRAP)
					group[count++]= ud->skillunit[i];
				break;
		}

	}
	for (i=0;i<count;i++)
		skill_delunitgroup(bl, group[i]);
	return count;
}
	
/*==========================================
 * Returns the first element field found [Skotlex]
 *------------------------------------------
 */
struct skill_unit_group *skill_locate_element_field(struct block_list *bl)
{
	struct unit_data *ud = unit_bl2ud(bl);
	int i;
	nullpo_retr(0, bl);
	if (!ud) return NULL;

	for (i=0;i<MAX_SKILLUNITGROUP && ud->skillunit[i];i++) {
		switch (ud->skillunit[i]->skill_id) {
			case SA_DELUGE:
			case SA_VOLCANO:
			case SA_VIOLENTGALE:
			case SA_LANDPROTECTOR:
			case NJ_SUITON:
				return ud->skillunit[i];
		}
	}
	return NULL;
}

// for graffiti cleaner [Valaris]
int skill_graffitiremover (struct block_list *bl, va_list ap)
{
	struct skill_unit *unit=NULL;

	nullpo_retr(0, bl);
	nullpo_retr(0, ap);

	if(bl->type!=BL_SKILL || (unit=(struct skill_unit *)bl) == NULL)
		return 0;

	if((unit->group) && (unit->group->unit_id == UNT_GRAFFITI))
		skill_delunit(unit);

	return 0;
}

int skill_greed (struct block_list *bl, va_list ap)
{
	struct block_list *src;
	struct map_session_data *sd=NULL;
	struct flooritem_data *fitem=NULL;

	nullpo_retr(0, bl);
	nullpo_retr(0, ap);
	nullpo_retr(0, src = va_arg(ap, struct block_list *));

	if(src->type == BL_PC && (sd=(struct map_session_data *)src) && bl->type==BL_ITEM && (fitem=(struct flooritem_data *)bl))
		pc_takeitem(sd, fitem);

	return 0;
}

/*==========================================
 *
 *------------------------------------------
 */
int skill_landprotector (struct block_list *bl, va_list ap)
{
	int skillid;
	int *alive;
	struct skill_unit *unit;
	struct block_list *src;

	skillid = va_arg(ap,int);
	alive = va_arg(ap,int *);
	src = va_arg(ap,struct block_list *);
	unit = (struct skill_unit *)bl;
	if (unit == NULL || unit->group == NULL)
		return 0;

	switch (skillid)
	{
		case SA_LANDPROTECTOR:
			if (unit->group->skill_id == SA_LANDPROTECTOR &&
				battle_check_target(bl, src, BCT_ENEMY) > 0)
			{	//Check for offensive Land Protector to delete both. [Skotlex]
				(*alive) = 0;
				skill_delunit(unit);
				return 1;
			}
			//Delete the rest of types.
		case HW_GANBANTEIN:
			if(skill_get_type(unit->group->skill_id) == BF_MAGIC)
			{	//Delete Magical effects
				skill_delunit(unit);
				return 1;
			}
			break;
		case SA_VOLCANO:
		case SA_DELUGE:
		case SA_VIOLENTGALE:
		case NJ_SUITON:
		case NJ_KAENSIN:
			switch (unit->group->skill_id)
			{	//These cannot override each other.
				case SA_VOLCANO:
				case SA_DELUGE:
				case SA_VIOLENTGALE:
				case NJ_SUITON:
				case NJ_KAENSIN:
					(*alive) = 0;
					return 1;
			}
			break;
		case HP_BASILICA:
			if (unit->group->skill_id == HP_BASILICA)
			{	//Basilica can't be placed on top of itself to avoid map-cell stacking problems. [Skotlex]
				(*alive) = 0;
				return 1;
			}
			break;
	}
	if (unit->group->skill_id == SA_LANDPROTECTOR &&
		skill_get_type(skillid) == BF_MAGIC)
	{	//Magic tile won't be activated
		(*alive) = 0;
		return 1;
	}

	return 0;
}

/*==========================================
 * variation of skill_landprotector
 *------------------------------------------
 */
int skill_ganbatein (struct block_list *bl, va_list ap)
{
	struct skill_unit *unit;

	nullpo_retr(0, bl);
	nullpo_retr(0, ap);
	if ((unit = (struct skill_unit *)bl) == NULL || unit->group == NULL)
		return 0;

// Apparently, it REMOVES traps.
//	if (skill_get_inf2(unit->group->skill_id)&INF2_TRAP)
//		return 0; //Do not remove traps.
	
	if (unit->group->skill_id == SA_LANDPROTECTOR)
		skill_delunit(unit);
	else skill_delunitgroup(NULL, unit->group);

	return 1;
}

/*==========================================
 * 
 *------------------------------------------
 */
int skill_chastle_mob_changetarget(struct block_list *bl,va_list ap)
{
	struct mob_data* md;
	struct unit_data*ud = unit_bl2ud(bl);
	struct block_list *from_bl;
	struct block_list *to_bl;
	md = (struct mob_data*)bl;
	from_bl = va_arg(ap,struct block_list *);
	to_bl = va_arg(ap,struct block_list *);

	if(ud && ud->target == from_bl->id)
		ud->target = to_bl->id;

	if(md->bl.type == BL_MOB && md->target_id == from_bl->id)
		md->target_id = to_bl->id;
	return 0;
}

/*==========================================
 * 指定範囲内でsrcに対して有効なターゲットのblの数を数える(foreachinarea)
 *------------------------------------------
 */
int skill_count_target (struct block_list *bl, va_list ap)
{
	struct block_list *src = va_arg(ap,struct block_list *);
	if (battle_check_target(src,bl,BCT_ENEMY) > 0)
		return 1;
	return 0;
}
/*==========================================
 *
 *------------------------------------------
 */
int skill_trap_splash (struct block_list *bl, va_list ap)
{
	struct block_list *src;
	int tick;
	struct skill_unit *unit;
	struct skill_unit_group *sg;
	struct block_list *ss;
	int i,count;
	src = va_arg(ap,struct block_list *);
	unit = (struct skill_unit *)src;
	tick = va_arg(ap,int);
	count = va_arg(ap,int);
	
	nullpo_retr(0, sg = unit->group);
	nullpo_retr(0, ss = map_id2bl(sg->src_id));

	if(battle_check_target(src,bl,BCT_ENEMY) > 0){
		switch(sg->unit_id){
			case UNT_SHOCKWAVE:
			case UNT_SANDMAN:
			case UNT_FLASHER:        
				skill_additional_effect(ss,bl,sg->skill_id,sg->skill_lv,BF_MISC,tick);
				break;
			case UNT_BLASTMINE:
			case UNT_CLAYMORETRAP:
				//Special property: Each target is hit N times (N = number of targets on splash area)
				if (!count) count = 1;
				for(i=0;i<count;i++)
					skill_attack(BF_MISC,ss,src,bl,sg->skill_id,sg->skill_lv,tick,0);
				break;
			case UNT_FREEZINGTRAP:
				skill_attack(BF_WEAPON,ss,src,bl,sg->skill_id,sg->skill_lv,tick,0);
				break;
		}
	}

	return 0;
}

/*==========================================
 *
 *------------------------------------------
 */
int skill_enchant_elemental_end (struct block_list *bl, int type)
{
	struct status_change *sc;

	nullpo_retr(0, bl);
	nullpo_retr(0, sc= status_get_sc(bl));

	if (!sc->count) return 0;
	
	if (type != SC_ENCPOISON && sc->data[SC_ENCPOISON].timer != -1)
		status_change_end(bl, SC_ENCPOISON, -1);
	if (type != SC_ASPERSIO && sc->data[SC_ASPERSIO].timer != -1)
		status_change_end(bl, SC_ASPERSIO, -1);
	if (type != SC_FIREWEAPON && sc->data[SC_FIREWEAPON].timer != -1)
		status_change_end(bl, SC_FIREWEAPON, -1);
	if (type != SC_WATERWEAPON && sc->data[SC_WATERWEAPON].timer != -1)
		status_change_end(bl, SC_WATERWEAPON, -1);
	if (type != SC_WINDWEAPON && sc->data[SC_WINDWEAPON].timer != -1)
		status_change_end(bl, SC_WINDWEAPON, -1);
	if (type != SC_EARTHWEAPON && sc->data[SC_EARTHWEAPON].timer != -1)
		status_change_end(bl, SC_EARTHWEAPON, -1);
	if (type != SC_SHADOWWEAPON && sc->data[SC_SHADOWWEAPON].timer != -1)
		status_change_end(bl, SC_SHADOWWEAPON, -1);
	if (type != SC_GHOSTWEAPON && sc->data[SC_GHOSTWEAPON].timer != -1)
		status_change_end(bl, SC_GHOSTWEAPON, -1);
	return 0;
}

int skill_check_cloaking(struct block_list *bl, struct status_change *sc)
{
	static int dx[] = { 0, 1, 0, -1, -1,  1, 1, -1}; //optimized by Lupus
	static int dy[] = {-1, 0, 1,  0, -1, -1, 1,  1};
	int end = 1,i;

	if ((bl->type == BL_PC && battle_config.pc_cloak_check_type&1) ||
		(bl->type != BL_PC && battle_config.monster_cloak_check_type&1))
		{	//Check for walls.
			for (i = 0; i < 8; i++)
			if (map_getcell(bl->m, bl->x+dx[i], bl->y+dy[i], CELL_CHKNOPASS))
			{
				end = 0;
				break;
			}
		} else
			end = 0; //No wall check.
			
	if(end){
		if (sc->data[SC_CLOAKING].timer != -1) {
			if (sc->data[SC_CLOAKING].val1 < 3) //End cloaking.
				status_change_end(bl, SC_CLOAKING, -1);
			else if(sc->data[SC_CLOAKING].val4&1)
			{	//Remove wall bonus
				sc->data[SC_CLOAKING].val4&=~1;
				status_calc_bl(bl,SCB_SPEED);
			}
		} 
	}
	else if(sc->data[SC_CLOAKING].timer != -1 && !(sc->data[SC_CLOAKING].val4&1))
	{	//Add wall speed bonus
		sc->data[SC_CLOAKING].val4|=1;
		status_calc_bl(bl,SCB_SPEED);
	}

	return end;
}

/*
 *----------------------------------------------------------------------------
 *
 *----------------------------------------------------------------------------
 */

/*==========================================
 *
 *
 *------------------------------------------
 */
void skill_stop_dancing (struct block_list *src)
{
	struct status_change* sc;
	struct skill_unit_group* group;
	struct map_session_data* dsd = NULL;

	nullpo_retv(src);
	nullpo_retv(sc = status_get_sc(src));

	if(!sc->count || sc->data[SC_DANCING].timer == -1)
		return;
	
	group = (struct skill_unit_group *)sc->data[SC_DANCING].val2;
	sc->data[SC_DANCING].val2 = 0;
	
	if (sc->data[SC_DANCING].val4)
	{
		if (sc->data[SC_DANCING].val4 != BCT_SELF)
			dsd = map_id2sd(sc->data[SC_DANCING].val4);
		sc->data[SC_DANCING].val4 = 0;
	}

	if (group)
		skill_delunitgroup(NULL, group);
		
	if (dsd)
	{
		dsd->sc.data[SC_DANCING].val4 = dsd->sc.data[SC_DANCING].val2 = 0;
		status_change_end(&dsd->bl, SC_DANCING, -1);
	}
	status_change_end(src, SC_DANCING, -1);
}

/*==========================================
 *
 *------------------------------------------
 */
struct skill_unit *skill_initunit (struct skill_unit_group *group, int idx, int x, int y, int val1, int val2)
{
	struct skill_unit *unit;

	nullpo_retr(NULL, group);
	nullpo_retr(NULL, unit=&group->unit[idx]);

	if(!unit->alive)
		group->alive_count++;

	unit->bl.id=map_addobject(&unit->bl);
	unit->bl.type=BL_SKILL;
	unit->bl.m=group->map;
	unit->bl.x=x;
	unit->bl.y=y;
	unit->group=group;
	unit->alive=1;
	unit->val1=val1;
	unit->val2=val2;

	map_addblock(&unit->bl);

	switch (group->skill_id) {
	case AL_PNEUMA:
		skill_unitsetmapcell(unit,AL_PNEUMA,group->skill_lv,CELL_SETPNEUMA);
		break;
	case MG_SAFETYWALL:
		skill_unitsetmapcell(unit,MG_SAFETYWALL,group->skill_lv,CELL_SETSAFETYWALL);
		break;
	case SA_LANDPROTECTOR:
		skill_unitsetmapcell(unit,SA_LANDPROTECTOR,group->skill_lv,CELL_SETLANDPROTECTOR);
		break;
	case HP_BASILICA:
		skill_unitsetmapcell(unit,HP_BASILICA,group->skill_lv,CELL_SETBASILICA);
		break;
	case WZ_ICEWALL:
		skill_unitsetmapcell(unit,WZ_ICEWALL,group->skill_lv,CELL_SETICEWALL);
		break;
	default:
		if (group->state.song_dance&0x1) //Check for dissonance.
			skill_dance_overlap(unit, 1);
		break;
	}
	clif_skill_setunit(unit);
	return unit;
}

/*==========================================
 *
 *------------------------------------------
 */
int skill_delunit (struct skill_unit *unit)
{
	struct skill_unit_group *group;

	nullpo_retr(0, unit);
	if(!unit->alive)
		return 0;
	nullpo_retr(0, group=unit->group);

	skill_unit_onlimit( unit,gettick() );

	if (group->state.song_dance&0x1) //Restore dissonance effect.
		skill_dance_overlap(unit, 0);

	if (!unit->range) {
		map_foreachincell(skill_unit_effect,unit->bl.m,
			unit->bl.x,unit->bl.y,group->bl_flag,&unit->bl,gettick(),4);
	}

	switch (group->skill_id) {
	case AL_PNEUMA:
		skill_unitsetmapcell(unit,AL_PNEUMA,group->skill_lv,CELL_CLRPNEUMA);
		break;
	case MG_SAFETYWALL:
		skill_unitsetmapcell(unit,MG_SAFETYWALL,group->skill_lv,CELL_CLRSAFETYWALL);
		break;
	case SA_LANDPROTECTOR:
		skill_unitsetmapcell(unit,SA_LANDPROTECTOR,group->skill_lv,CELL_CLRLANDPROTECTOR);
		break;
	case HP_BASILICA:
		skill_unitsetmapcell(unit,HP_BASILICA,group->skill_lv,CELL_CLRBASILICA);
		break;
	case WZ_ICEWALL:
		skill_unitsetmapcell(unit,WZ_ICEWALL,group->skill_lv,CELL_CLRICEWALL);
		break;
	}

	clif_skill_delunit(unit);

	unit->group=NULL;
	unit->alive=0;
	map_delobjectnofree(unit->bl.id);
	if(--group->alive_count==0)
		skill_delunitgroup(NULL, group);

	return 0;
}
/*==========================================
 *
 *------------------------------------------
 */
static int skill_unit_group_newid = MAX_SKILL_DB;
struct skill_unit_group *skill_initunitgroup (struct block_list *src, int count, int skillid, int skilllv, int unit_id, int limit, int interval)
{
	struct unit_data *ud = unit_bl2ud(src);
	struct skill_unit_group *group=NULL;
	int i;

	if(skilllv <= 0) return 0;

	nullpo_retr(NULL, src);
	nullpo_retr(NULL, ud);
	
	for(i=0;i<MAX_SKILLUNITGROUP && ud->skillunit[i]; i++);
	
	if(i == MAX_SKILLUNITGROUP) {
		int j=0;
		unsigned maxdiff=0,x,tick=gettick();
		for(i=0;i<MAX_SKILLUNITGROUP && ud->skillunit[i];i++)
			if((x=DIFF_TICK(tick,ud->skillunit[i]->tick))>maxdiff){
				maxdiff=x;
				j=i;
			}
		skill_delunitgroup(src, ud->skillunit[j]);
		//Since elements must have shifted, we use the last slot.
		i = MAX_SKILLUNITGROUP-1;
	}
	if (!ud->skillunit[i])
		ud->skillunit[i] = ers_alloc(skill_unit_ers, struct skill_unit_group);
	group=ud->skillunit[i];

	group->src_id=src->id;
	group->party_id=status_get_party_id(src);
	group->guild_id=status_get_guild_id(src);
	group->group_id=skill_unit_group_newid++;
	if(skill_unit_group_newid<=0)
		skill_unit_group_newid = MAX_SKILL_DB;
	group->unit=(struct skill_unit *)aCalloc(count,sizeof(struct skill_unit));
	group->unit_count=count;
	group->alive_count=0;
	group->val1=group->val2=group->val3=0;
	group->skill_id=skillid;
	group->skill_lv=skilllv;
	group->unit_id=unit_id;
	group->map=src->m;
	group->limit=limit;
	group->interval=interval;
	group->tick=gettick();
	if (skillid == PR_SANCTUARY) //Sanctuary starts healing +1500ms after casted. [Skotlex]
		group->tick += 1500;
	else if (skillid == PA_GOSPEL) //Prevent Gospel from triggering bonuses right away. [Skotlex]
		group->tick += interval;
	group->valstr=NULL;

	i = skill_get_unit_flag(skillid); //Reuse for faster access from here on. [Skotlex]
	if (i&(UF_DANCE|UF_SONG|UF_ENSEMBLE)) {
		struct map_session_data *sd = NULL;
		if(src->type==BL_PC && (sd=(struct map_session_data *)src) ){
			sd->skillid_dance=skillid;
			sd->skilllv_dance=skilllv;
		}
		sc_start4(src,SC_DANCING,100,skillid,(int)group,0,(i&UF_ENSEMBLE?BCT_SELF:0),skill_get_time(skillid,skilllv)+1000);
		if (sd && i&UF_ENSEMBLE &&
			battle_config.player_skill_partner_check &&
			(!battle_config.gm_skilluncond || pc_isGM(sd) < battle_config.gm_skilluncond)
			) {
				skill_check_pc_partner(sd, skillid, &skilllv, 1, 1);
		}
	}
	return group;
}

/*==========================================
 *
 *------------------------------------------
 */
int skill_delunitgroup (struct block_list *src, struct skill_unit_group *group)
{
	struct unit_data *ud;
	int i,j;

	nullpo_retr(0, group);

	if (!src) src=map_id2bl(group->src_id);
	ud = unit_bl2ud(src);	
	if(!src || !ud) {
		ShowError("skill_delunitgroup: Group's source not found! (src_id: %d skill_id: %d)\n", group->src_id, group->skill_id);
		return 0;	
	}
	if (skill_get_unit_flag(group->skill_id)&(UF_DANCE|UF_SONG|UF_ENSEMBLE))
	{
		struct status_change* sc = status_get_sc(src);
		if (sc && sc->data[SC_DANCING].timer != -1)
		{
			sc->data[SC_DANCING].val2 = 0 ; //This prevents status_change_end attempting to redelete the group. [Skotlex]
			status_change_end(src,SC_DANCING,-1);
		}
	}

	if (group->unit_id == UNT_GOSPEL) { //Clear Gospel [Skotlex]
		struct status_change *sc = status_get_sc(src);
		if(sc && sc->data[SC_GOSPEL].timer != -1) {
			sc->data[SC_GOSPEL].val3 = 0; //Remove reference to this group. [Skotlex]
			status_change_end(src,SC_GOSPEL,-1);
		}
	}
	if (group->skill_id == SG_SUN_WARM ||
		group->skill_id == SG_MOON_WARM ||
		group->skill_id == SG_STAR_WARM) {
		struct status_change *sc = status_get_sc(src);
		if(sc && sc->data[SC_WARM].timer != -1) {
			sc->data[SC_WARM].val4 = 0;
			status_change_end(src,SC_WARM,-1);
		}
	}

	if (src->type==BL_PC && group->state.ammo_consume)
		battle_consume_ammo((TBL_PC*)src, group->skill_id, group->skill_lv);

	group->alive_count=0;
	if(group->unit!=NULL){
		for(i=0;i<group->unit_count;i++)
			if(group->unit[i].alive)
				skill_delunit(&group->unit[i]);
	}
	if(group->valstr!=NULL){
		aFree(group->valstr);
		group->valstr=NULL;
	}

	map_freeblock((struct block_list*)group->unit);
	group->unit=NULL;
	group->group_id=0;
	group->unit_count=0;

	//Locate and clear this unit from the array.
	for (i=0; i<MAX_SKILLUNITGROUP && ud->skillunit[i]!=group; i++);
	for (j=i; j<MAX_SKILLUNITGROUP && ud->skillunit[j]; j++);
	j--;
	if (i<MAX_SKILLUNITGROUP) {
		ud->skillunit[i] = ud->skillunit[j];
		ud->skillunit[j] = NULL;
		ers_free(skill_unit_ers, group);
	} else
		ShowError("skill_delunitgroup: Group not found! (src_id: %d skill_id: %d)\n", group->src_id, group->skill_id);
	return 1;
}

/*==========================================
 *
 *------------------------------------------
 */
int skill_clear_unitgroup (struct block_list *src)
{
	struct unit_data *ud = unit_bl2ud(src);

	nullpo_retr(0, ud);

	while (ud->skillunit[0])
		skill_delunitgroup(src, ud->skillunit[0]);
	return 1;
}

/*==========================================
 *
 *------------------------------------------
 */
struct skill_unit_group_tickset *skill_unitgrouptickset_search (struct block_list *bl, struct skill_unit_group *group, int tick)
{
	int i,j=-1,k,s,id;
	struct unit_data *ud;
	struct skill_unit_group_tickset *set;

	nullpo_retr(0, bl);
	if (group->interval==-1)
		return NULL;
	
	ud = unit_bl2ud(bl);
	if (!ud) return NULL;

	set = ud->skillunittick;

	if (skill_get_unit_flag(group->skill_id)&UF_NOOVERLAP)
		id = s = group->skill_id;
	else
		id = s = group->group_id;

	for (i=0; i<MAX_SKILLUNITGROUPTICKSET; i++) {
		k = (i+s) % MAX_SKILLUNITGROUPTICKSET;
		if (set[k].id == id)
			return &set[k];
		else if (j==-1 && (DIFF_TICK(tick,set[k].tick)>0 || set[k].id==0))
			j=k;
	}

	if (j == -1) {
		if(battle_config.error_log) {
			ShowWarning ("skill_unitgrouptickset_search: tickset is full\n");
		}
		j = id % MAX_SKILLUNITGROUPTICKSET;
	}

	set[j].id = id;
	set[j].tick = tick;
	return &set[j];
}

/*==========================================
 *
 *------------------------------------------
 */
int skill_unit_timer_sub_onplace (struct block_list *bl, va_list ap)
{
	struct skill_unit *unit;
	struct skill_unit_group *group;
	unsigned int tick;

	unit = va_arg(ap,struct skill_unit *);
	tick = va_arg(ap,unsigned int);

	if (!unit->alive || bl->prev==NULL)
		return 0;

	nullpo_retr(0, group=unit->group);

	if (skill_get_type(group->skill_id)==BF_MAGIC
		&& map_getcell(bl->m, bl->x, bl->y, CELL_CHKLANDPROTECTOR))
		return 0; //AoE skills are ineffective. [Skotlex]

	if (battle_check_target(&unit->bl,bl,group->target_flag)<=0)
		return 0;

	if (group->state.song_dance&0x1 && unit->val2&UF_ENSEMBLE)
  	{	//Treat this group as if it were BA_DISSONANCE/DC_UGLYDANCE.
		//Values will be restored on proper switch case.
		int id = group->unit_id;
		unit->val1 = group->skill_id;
		if (unit->val2&UF_SONG) {
			group->unit_id = UNT_DISSONANCE;
			group->skill_id = BA_DISSONANCE;
		} else {
			group->unit_id = UNT_UGLYDANCE;
			group->skill_id = DC_UGLYDANCE;
		}
		skill_unit_onplace_timer(unit,bl,tick);
		group->unit_id = id;
		group->skill_id = unit->val1;
	} else
		skill_unit_onplace_timer(unit,bl,tick);

	return 0;
}

/*==========================================
 *
 *------------------------------------------
 */
int skill_unit_timer_sub (struct block_list *bl, va_list ap)
{
	struct skill_unit *unit;
	struct skill_unit_group *group;
	unsigned int tick;

	unit=(struct skill_unit *)bl;
	tick=va_arg(ap,unsigned int);

	if(!unit->alive)
		return 0;
	group=unit->group;

	nullpo_retr(0, group);

	if (unit->range>=0 && group->interval!=-1) {
		if (battle_config.skill_wall_check)
			map_foreachinshootrange(skill_unit_timer_sub_onplace, bl, unit->range,
				group->bl_flag,bl,tick);
		else
			map_foreachinrange(skill_unit_timer_sub_onplace, bl, unit->range,
				group->bl_flag,bl,tick);
		if (!unit->alive)
			return 0;
	}

	if((DIFF_TICK(tick,group->tick)>=group->limit || DIFF_TICK(tick,group->tick)>=unit->limit)){
		switch(group->unit_id){
			case UNT_BLASTMINE:
				group->unit_id = UNT_USED_TRAPS;
				clif_changetraplook(bl, UNT_USED_TRAPS);
				group->limit=DIFF_TICK(tick+1500,group->tick);
				unit->limit=DIFF_TICK(tick+1500,group->tick);
				break;
			case UNT_SKIDTRAP:
			case UNT_ANKLESNARE:
			case UNT_LANDMINE:
			case UNT_SHOCKWAVE:
			case UNT_SANDMAN:
			case UNT_FLASHER:
			case UNT_FREEZINGTRAP:
			case UNT_CLAYMORETRAP:
			case UNT_TALKIEBOX:
				{
					struct block_list *src=map_id2bl(group->src_id);
					if(group->unit_id == UNT_ANKLESNARE && group->val2);
					else{
						if(src && src->type==BL_PC && !group->state.into_abyss)
						{	//Avoid generating trap items when it did not cost to create them. [Skotlex]
							struct item item_tmp;
							memset(&item_tmp,0,sizeof(item_tmp));
							item_tmp.nameid=1065;
							item_tmp.identify=1;
							map_addflooritem(&item_tmp,1,bl->m,bl->x,bl->y,NULL,NULL,NULL,0);
						}
					}
					skill_delunit(unit);
				}
				break;

			case 0xc1:
			case 0xc2:
			case 0xc3:
			case 0xc4:
				{
					struct block_list *src=map_id2bl(group->src_id);
					if (src)
						group->tick = tick;
				}
				break;

			default:
				skill_delunit(unit);
		}
	}

	if(group->unit_id == UNT_ICEWALL) {
		unit->val1 -= 5;
		if(unit->val1 <= 0 && unit->limit + group->tick > tick + 700)
			unit->limit = DIFF_TICK(tick+700,group->tick);
	}

	return 0;
}
/*==========================================
 *
 *------------------------------------------
 */
int skill_unit_timer (int tid, unsigned int tick, int id, int data)
{
	map_freeblock_lock();

	map_foreachobject( skill_unit_timer_sub, BL_SKILL, tick );

	map_freeblock_unlock();

	return 0;
}

/*==========================================
 *
 *------------------------------------------
 */
int skill_unit_move_sub (struct block_list *bl, va_list ap)
{
	struct skill_unit *unit = (struct skill_unit *)bl;
	struct skill_unit_group *group;
	struct block_list *target;
	unsigned int tick,flag,result;
	int skill_id,unit_id=0; //Set to 0 to shut-up compiler warnings.

	target=va_arg(ap,struct block_list*);
	tick = va_arg(ap,unsigned int);
	flag = va_arg(ap,int);
	
	nullpo_retr(0, group=unit->group);
	
	if (!unit->alive || target->prev==NULL)
		return 0;

  	//Necessary in case the group is deleted after calling on_place/on_out [Skotlex]
	skill_id = unit->group->skill_id;
	
	if (unit->group->interval!=-1 && 
		!(skill_get_unit_flag(skill_id)&UF_DUALMODE)) //Skills in dual mode have to trigger both. [Skotlex]
		return 0;

	//Target-type check.
	if(!(group->bl_flag&target->type && battle_check_target(&unit->bl,target,group->target_flag)>0))
	{
		if(group->src_id == target->id && group->state.song_dance&0x2)
		{	//Ensemble check to see if they went out/in of the area [Skotlex]
			if (flag&1)
			{
				if (flag&2)
				{	//Clear skill ids we have stored in onout.
					int i;
					for(i=0; i<8 && skill_unit_temp[i]!=skill_id; i++);
					if (i<8)
						skill_unit_temp[i] = 0;
				}
			}
			else
			{
				if (flag&2 && skill_unit_index < 7) //Store this unit id.
					skill_unit_temp[skill_unit_index++] = skill_id;
			}
			if (flag&4)
				skill_unit_onleft(skill_id,target,tick);
		}
		return 0;
	}
	
	if (group->state.song_dance&0x1 && unit->val2&UF_ENSEMBLE)
  	{	//Treat this group as if it were BA_DISSONANCE/DC_UGLYDANCE.
		//Values will be restored on proper switch case.
		flag|=64; //Signal to remember to restore it.
		unit_id = group->unit_id;
		unit->val1 = group->skill_id;
		if (unit->val2&UF_SONG) {
			group->unit_id = UNT_DISSONANCE;
			group->skill_id = BA_DISSONANCE;
		} else {
			group->unit_id = UNT_UGLYDANCE;
			group->skill_id = DC_UGLYDANCE;
		}
	}

	if (flag&1)
	{
		result = skill_unit_onplace(unit,target,tick);
		if (flag&2 && result)
		{	//Clear skill ids we have stored in onout.
			int i;
			for(i=0; i<8 && skill_unit_temp[i]!=result; i++);
			if (i<8)
				skill_unit_temp[i] = 0;
		}
	}
	else
	{
		result = skill_unit_onout(unit,target,tick);
		if (flag&2 && skill_unit_index < 7 && result) //Store this unit id.
			skill_unit_temp[skill_unit_index++] = result;
	}

	if (flag&64)
	{	//TODO: Normally, this is dangerous since the unit and group could be freed
		//inside the onout/onplace functions. Currently it is safe because we know song/dance
		//cells do not get deleted within them. [Skotlex]
		skill_id = group->skill_id;
		group->unit_id = unit_id;
		group->skill_id = unit->val1;
	}
		
	if (flag&4)
		skill_unit_onleft(skill_id,target,tick);
	return 1;
}

/*==========================================
 * Invoked when a char has moved and unit cells must be invoked (onplace, onout, onleft)
 * Flag values:
 * flag&1: invoke skill_unit_onplace (otherwise invoke skill_unit_onout)
 * flag&2: this function is being invoked twice as a bl moves, store in memory the affected
 * units to figure out when they have left a group.
 * flag&4: Force a onleft event (triggered when the bl is killed, for example)
 *------------------------------------------
 */
int skill_unit_move (struct block_list *bl, unsigned int tick, int flag)
{
	nullpo_retr(0, bl);

	if(bl->prev==NULL )
		return 0;

	if (flag&2 && !(flag&1))
	{	//Onout, clear data
		memset (&skill_unit_temp,0,sizeof(skill_unit_temp));
		skill_unit_index=0;
	}
		
	map_foreachincell(skill_unit_move_sub,
			bl->m,bl->x,bl->y,BL_SKILL,bl,tick,flag);

	if (flag&2 && flag&1)
	{ //Onplace, check any skill units you have left.
		int i;
		for (i=0; i< 8 && skill_unit_temp[i]>0; i++)
			skill_unit_onleft(skill_unit_temp[i], bl, tick);
	}

	return 0;
}

/*==========================================
 *
 *------------------------------------------
 */
int skill_unit_move_unit_group (struct skill_unit_group *group, int m, int dx, int dy)
{
	int i,j;
	unsigned int tick = gettick();
	int *m_flag;
	struct skill_unit *unit1;
	struct skill_unit *unit2;

	nullpo_retr(0, group);
	if (group->unit_count<=0)
		return 0;
	if (group->unit==NULL)
		return 0;

	if (skill_get_unit_flag(group->skill_id)&UF_ENSEMBLE) //Ensembles may not be moved around.
		return 0;

	m_flag = (int *) aCalloc(group->unit_count, sizeof(int));
	//    m_flag
	//		0: Neither of the following (skill_unit_onplace & skill_unit_onout are needed)
	//		1: Unit will move to a slot that had another unit of the same group (skill_unit_onplace not needed)
	//		2: Another unit from same group will end up positioned on this unit (skill_unit_onout not needed)
	//		3: Both 1+2.
	for(i=0;i<group->unit_count;i++){
		unit1=&group->unit[i];
		if (!unit1->alive || unit1->bl.m!=m)
			continue;
		for(j=0;j<group->unit_count;j++){
			unit2=&group->unit[j];
			if (!unit2->alive)
				continue;
			if (unit1->bl.x+dx==unit2->bl.x && unit1->bl.y+dy==unit2->bl.y){
				m_flag[i] |= 0x1;
			}
			if (unit1->bl.x-dx==unit2->bl.x && unit1->bl.y-dy==unit2->bl.y){
				m_flag[i] |= 0x2;
			}
		}
	}
	j = 0;
	for (i=0;i<group->unit_count;i++) {
		unit1=&group->unit[i];
		if (!unit1->alive)
			continue;
		if (!(m_flag[i]&0x2)) {
			if (group->state.song_dance&0x1) //Restore dissonance effect.
				skill_dance_overlap(unit1, 0);
			map_foreachincell(skill_unit_effect,unit1->bl.m,
				unit1->bl.x,unit1->bl.y,group->bl_flag,&unit1->bl,tick,4);
		}
		//Move Cell using "smart" criteria (avoid useless moving around)
		switch(m_flag[i])
		{
			case 0:
			//Cell moves independently, safely move it.
				map_moveblock(&unit1->bl, unit1->bl.x+dx, unit1->bl.y+dy, tick);
				break;
			case 1:
			//Cell moves unto another cell, look for a replacement cell that won't collide
			//and has no cell moving into it (flag == 2)
				for(;j<group->unit_count;j++)
				{
					if(m_flag[j]!=2 || !group->unit[j].alive)
						continue;
					//Move to where this cell would had moved.
					unit2 = &group->unit[j];
					map_moveblock(&unit1->bl, unit2->bl.x+dx, unit2->bl.y+dy, tick);
					j++; //Skip this cell as we have used it.
					break;
				}
				break;
			case 2:
			case 3:
				break; //Don't move the cell as a cell will end on this tile anyway.
		}
		if (!(m_flag[i]&0x2)) { //We only moved the cell in 0-1
			if (group->state.song_dance&0x1) //Check for dissonance effect.
				skill_dance_overlap(unit1, 1);
			clif_skill_setunit(unit1);
			map_foreachincell(skill_unit_effect,unit1->bl.m,
				unit1->bl.x,unit1->bl.y,group->bl_flag,&unit1->bl,tick,1);
		}
	}
	aFree(m_flag);
	return 0;
}

/*----------------------------------------------------------------------------
 *
 *----------------------------------------------------------------------------
 */

/*==========================================
 *
 *------------------------------------------
 */
int skill_can_produce_mix (struct map_session_data *sd, int nameid, int trigger, int qty)
{
	int i,j;

	nullpo_retr(0, sd);

	if(nameid<=0)
		return 0;

	for(i=0;i<MAX_SKILL_PRODUCE_DB;i++){
		if(skill_produce_db[i].nameid == nameid )
			break;
	}
	if( i >= MAX_SKILL_PRODUCE_DB )
		return 0;

	if(trigger>=0){
		if(trigger>20) { // Non-weapon, non-food item (itemlv must match)
			if(skill_produce_db[i].itemlv!=trigger)
				return 0;
		} else if(trigger>10) { // Food (any item level between 10 and 20 will do)
			if(skill_produce_db[i].itemlv<=10 || skill_produce_db[i].itemlv>20)
				return 0;
		} else { // Weapon (itemlv must be higher or equal)
			if(skill_produce_db[i].itemlv>trigger)
				return 0;
		}
	}
	if( (j=skill_produce_db[i].req_skill)>0 && pc_checkskill(sd,j)<=0 )
		return 0;

	for(j=0;j<MAX_PRODUCE_RESOURCE;j++){
		int id,x,y;
		if( (id=skill_produce_db[i].mat_id[j]) <= 0 )
			continue;
		if(skill_produce_db[i].mat_amount[j] <= 0) {
			if(pc_search_inventory(sd,id) < 0)
				return 0;
		}
		else {
			for(y=0,x=0;y<MAX_INVENTORY;y++)
				if( sd->status.inventory[y].nameid == id )
					x+=sd->status.inventory[y].amount;
			if(x<qty*skill_produce_db[i].mat_amount[j])
				return 0;
		}
	}
	return i+1;
}

/*==========================================
 *
 *------------------------------------------
 */
int skill_produce_mix (struct map_session_data *sd, int skill_id, int nameid, int slot1, int slot2, int slot3, int qty)
{
	int slot[3];
	int i,sc,ele,idx,equip,wlv,make_per,flag;
	struct status_data *status;

	nullpo_retr(0, sd);
	status = status_get_status_data(&sd->bl);

	if( !(idx=skill_can_produce_mix(sd,nameid,-1, qty)) )
		return 0;
	idx--;

	if (qty < 1)
		qty = 1;
	
	if (!skill_id) //A skill can be specified for some override cases.
		skill_id = skill_produce_db[idx].req_skill;
	
	slot[0]=slot1;
	slot[1]=slot2;
	slot[2]=slot3;

	for(i=0,sc=0,ele=0;i<3;i++){ //Note that qty should always be one if you are using these!
		int j;
		if( slot[i]<=0 )
			continue;
		j = pc_search_inventory(sd,slot[i]);
		if(j < 0)
			continue;
		if(slot[i]==1000){	/* Star Crumb */
			pc_delitem(sd,j,1,1);
			sc++;
		}
		if(slot[i]>=994 && slot[i]<=997 && ele==0){	/* Flame Heart . . . Great Nature */
			static const int ele_table[4]={3,1,4,2};
			pc_delitem(sd,j,1,1);
			ele=ele_table[slot[i]-994];
		}
	}

	for(i=0;i<MAX_PRODUCE_RESOURCE;i++){
		int j,id,x;
		if( (id=skill_produce_db[idx].mat_id[i]) <= 0 )
			continue;
		x=qty*skill_produce_db[idx].mat_amount[i];
		do{
			int y=0;
			j = pc_search_inventory(sd,id);

			if(j >= 0){
				y = sd->status.inventory[j].amount;
				if(y>x)y=x;
				pc_delitem(sd,j,y,0);
			}else {
				if(battle_config.error_log)
					ShowError("skill_produce_mix: material item error\n");
			}

			x-=y;
		}while( j>=0 && x>0 );
	}

	if((equip=itemdb_isequip(nameid)))
		wlv = itemdb_wlv(nameid);
	if(!equip) {
		switch(skill_id){
			case BS_IRON:
			case BS_STEEL:
			case BS_ENCHANTEDSTONE:
				// Ores & Metals Refining - skill bonuses are straight from kRO website [DracoRPG]
				i = pc_checkskill(sd,skill_id);
				make_per = sd->status.job_level*20 + status->dex*10 + status->luk*10; //Base chance
				switch(nameid){
					case 998: // Iron
						make_per += 4000+i*500; // Temper Iron bonus: +26/+32/+38/+44/+50
						break;
					case 999: // Steel
						make_per += 3000+i*500; // Temper Steel bonus: +35/+40/+45/+50/+55
						break;
					case 1000: //Star Crumb
						make_per = 100000; // Star Crumbs are 100% success crafting rate? (made 1000% so it succeeds even after penalties) [Skotlex]
						break;
					default: // Enchanted Stones
						make_per += 1000+i*500; // Enchantedstone Craft bonus: +15/+20/+25/+30/+35
					break;
				}
				break;
			case ASC_CDP:
				make_per = (2000 + 40*status->dex + 20*status->luk);
				break;
			case AL_HOLYWATER:
				make_per = 100000; //100% success
				break;
			case AM_PHARMACY: // Potion Preparation - reviewed with the help of various Ragnainfo sources [DracoRPG]
			case AM_TWILIGHT1:
			case AM_TWILIGHT2:
			case AM_TWILIGHT3:
				make_per = pc_checkskill(sd,AM_LEARNINGPOTION)*100
					+ pc_checkskill(sd,AM_PHARMACY)*300 + sd->status.job_level*20
					+ status->int_*5 + status->dex*10+status->luk*10;
				switch(nameid){
					case 501: // Red Potion
					case 503: // Yellow Potion
					case 504: // White Potion
					case 605: // Anodyne
					case 606: // Aloevera
						make_per += 2000;
						break;
					case 505: // Blue Potion
						make_per -= 500;
						break;
					case 545: // Condensed Red Potion
					case 546: // Condensed Yellow Potion
					case 547: // Condensed White Potion
						make_per -= 1000;
					    break;
				 	case 970: // Alcohol
						make_per += 1000;
						break;
					case 7139: // Glistening Coat
						make_per -= 1000;
						break;
					case 7135: // Bottle Grenade
					case 7136: // Acid Bottle
					case 7137: // Plant Bottle
					case 7138: // Marine Sphere Bottle
					default:
						break;
				}
				if(battle_config.pp_rate != 100)
					make_per = make_per * battle_config.pp_rate / 100;
				break;
			case SA_CREATECON: // Elemental Converter Creation - skill bonuses are from kRO [DracoRPG]
				make_per = pc_checkskill(sd, SA_ADVANCEDBOOK)*100 + //TODO: Advanced Book bonus is custom! [Skotlex]
					sd->status.job_level*20 + status->int_*10 + status->dex*10;
				switch(nameid){
					case 12114:
						flag = pc_checkskill(sd,SA_FLAMELAUNCHER);
						if (flag > 0)
							make_per += 1000*flag-500;
						break;
					case 12115:
						flag = pc_checkskill(sd,SA_FROSTWEAPON);
						if (flag > 0)
							make_per += 1000*flag-500;
						break;
					case 12116:
						flag = pc_checkskill(sd,SA_SEISMICWEAPON);
						if (flag > 0)
							make_per += 1000*flag-500;
						break;
					case 12117:
						flag = pc_checkskill(sd,SA_LIGHTNINGLOADER);
						if (flag > 0)
							make_per += 1000*flag-500;
						break;
				}
				break;
			default:
				if (sd->menuskill_id ==	AM_PHARMACY &&
					sd->menuskill_lv > 10 && sd->menuskill_lv <= 20)
				{	//Assume Cooking Dish
					if (sd->menuskill_lv >= 15) //Legendary Cooking Set.
						make_per = 10000; //100% Success
					else
						make_per = 1200*(sd->menuskill_lv-10) //12% chance per set level.
							+ 7000 - 700*(skill_produce_db[idx].itemlv-10); //70% - 7% per dish level
					break;
				}
				make_per = 5000;
				break;
		}
	} else { // Weapon Forging - skill bonuses are straight from kRO website, other things from a jRO calculator [DracoRPG]
		make_per = 5000 + sd->status.job_level*20 + status->dex*10 + status->luk*10; // Base
		make_per += pc_checkskill(sd,skill_id)*500; // Smithing skills bonus: +5/+10/+15
		make_per += pc_checkskill(sd,BS_WEAPONRESEARCH)*100 +((wlv >= 3)? pc_checkskill(sd,BS_ORIDEOCON)*100:0); // Weaponry Research bonus: +1/+2/+3/+4/+5/+6/+7/+8/+9/+10, Oridecon Research bonus (custom): +1/+2/+3/+4/+5
		make_per -= (ele?2000:0) + sc*1500 + (wlv>1?wlv*1000:0); // Element Stone: -20%, Star Crumb: -15% each, Weapon level malus: -0/-20/-30
		if(pc_search_inventory(sd,989) > 0) make_per+= 1000; // Emperium Anvil: +10
		else if(pc_search_inventory(sd,988) > 0) make_per+= 500; // Golden Anvil: +5
		else if(pc_search_inventory(sd,987) > 0) make_per+= 300; // Oridecon Anvil: +3
		else if(pc_search_inventory(sd,986) > 0) make_per+= 0; // Anvil: +0?
		if(battle_config.wp_rate != 100)
			make_per = make_per * battle_config.wp_rate / 100;
	}
// - Baby Class Penalty = 80% (from adult's chance) ----//
	if (sd->class_&JOBL_BABY) //if it's a Baby Class
		make_per = (make_per * 80) / 100; //Lupus

	if(make_per < 1) make_per = 1;

	
	if(rand()%10000 < make_per || qty > 1){ //Success, or crafting multiple items.
		struct item tmp_item;
		memset(&tmp_item,0,sizeof(tmp_item));
		tmp_item.nameid=nameid;
		tmp_item.amount=1;
		tmp_item.identify=1;
		if(equip){
			tmp_item.card[0]=CARD0_FORGE;
			tmp_item.card[1]=((sc*5)<<8)+ele;
			tmp_item.card[2]=GetWord(sd->char_id,0); // CharId
			tmp_item.card[3]=GetWord(sd->char_id,1);
		} else {
			//Flag is only used on the end, so it can be used here. [Skotlex]
			switch (skill_id) {
				case AM_PHARMACY:
				case AM_TWILIGHT1:
				case AM_TWILIGHT2:
				case AM_TWILIGHT3:
					flag = battle_config.produce_potion_name_input;
					break;
				case AL_HOLYWATER:
					flag = battle_config.holywater_name_input;
					break;
				case ASC_CDP:
					flag = battle_config.cdp_name_input;
					break;
				default:
					flag = battle_config.produce_item_name_input;
					break;
			}
			if (flag) {
				tmp_item.card[0]=CARD0_CREATE;
				tmp_item.card[1]=0;
				tmp_item.card[2]=GetWord(sd->char_id,0); // CharId
				tmp_item.card[3]=GetWord(sd->char_id,1);
			}
		}

//		if(log_config.produce > 0)
//			log_produce(sd,nameid,slot1,slot2,slot3,1);
//TODO update PICKLOG

		if(equip){
			clif_produceeffect(sd,0,nameid);
			clif_misceffect(&sd->bl,3);
			if(itemdb_wlv(nameid) >= 3 && ((ele? 1 : 0) + sc) >= 3) // Fame point system [DracoRPG]
				pc_addfame(sd,10); // Success to forge a lv3 weapon with 3 additional ingredients = +10 fame point
		} else {
			int fame = 0;
			tmp_item.amount = 0;
			for (i=0; i< qty; i++)
			{	//Apply quantity modifiers.
				if (rand()%10000 < make_per || qty == 1)
				{ //Success
					tmp_item.amount++;
					if(nameid < 545 || nameid > 547)
						continue;
					if(skill_id != AM_PHARMACY &&
						skill_id != AM_TWILIGHT1 &&
						skill_id != AM_TWILIGHT2 &&
						skill_id != AM_TWILIGHT3)
						continue;						
					//Add fame as needed.
					switch(++sd->potion_success_counter) {
						case 3:
							fame+=1; // Success to prepare 3 Condensed Potions in a row
							break;
						case 5:
							fame+=3; // Success to prepare 5 Condensed Potions in a row
							break;
						case 7:
							fame+=10; // Success to prepare 7 Condensed Potions in a row
							break;
						case 10:
							fame+=50; // Success to prepare 10 Condensed Potions in a row
							sd->potion_success_counter = 0;
							break;
					}
				} else //Failure
					sd->potion_success_counter = 0;
			}
			if (fame)
				pc_addfame(sd,fame);
			//Visual effects and the like.
			switch (skill_id) {
				case AM_PHARMACY:
				case AM_TWILIGHT1:
				case AM_TWILIGHT2:
				case AM_TWILIGHT3:
				case ASC_CDP:
					clif_produceeffect(sd,2,nameid);
					clif_misceffect(&sd->bl,5);
					break;
				case BS_IRON:
				case BS_STEEL:
				case BS_ENCHANTEDSTONE:
					clif_produceeffect(sd,0,nameid);
					clif_misceffect(&sd->bl,3);
					break;
				default: //Those that don't require a skill?
					if (skill_produce_db[idx].itemlv>10 &&
						skill_produce_db[idx].itemlv<= 20) //Cooking items.
						clif_specialeffect(&sd->bl, 608, AREA);
					break;
			}
		}
		if (tmp_item.amount) { //Success
			if((flag = pc_additem(sd,&tmp_item,tmp_item.amount))) {
				clif_additem(sd,0,0,flag);
				map_addflooritem(&tmp_item,tmp_item.amount,sd->bl.m,sd->bl.x,sd->bl.y,NULL,NULL,NULL,0);
			}
			return 1;
		}
	}
	//Failure	
//	if(log_config.produce)
//		log_produce(sd,nameid,slot1,slot2,slot3,0);
//TODO update PICKLOG

	if(equip){
		clif_produceeffect(sd,1,nameid);
		clif_misceffect(&sd->bl,2);
	} else {
		switch (skill_id) {
			case ASC_CDP: //25% Damage yourself, and display same effect as failed potion.
				status_percent_damage(NULL, &sd->bl, -25, 0);
			case AM_PHARMACY:
			case AM_TWILIGHT1:
			case AM_TWILIGHT2:
			case AM_TWILIGHT3:
				clif_produceeffect(sd,3,nameid);
				clif_misceffect(&sd->bl,6);
				sd->potion_success_counter = 0; // Fame point system [DracoRPG]
				break;
			case BS_IRON:
			case BS_STEEL:
			case BS_ENCHANTEDSTONE:
				clif_produceeffect(sd,1,nameid);
				clif_misceffect(&sd->bl,2);
				break;
			default:
				if (skill_produce_db[idx].itemlv>10 &&
					skill_produce_db[idx].itemlv<= 20) //Cooking items.
					clif_specialeffect(&sd->bl, 609, AREA);
		}
	}
	return 0;
}

int skill_arrow_create (struct map_session_data *sd, int nameid)
{
	int i,j,flag,index=-1;
	struct item tmp_item;

	nullpo_retr(0, sd);

	if(nameid <= 0)
		return 1;

	for(i=0;i<MAX_SKILL_ARROW_DB;i++)
		if(nameid == skill_arrow_db[i].nameid) {
			index = i;
			break;
		}

	if(index < 0 || (j = pc_search_inventory(sd,nameid)) < 0)
		return 1;

	pc_delitem(sd,j,1,0);
	for(i=0;i<5;i++) {
		memset(&tmp_item,0,sizeof(tmp_item));
		tmp_item.identify = 1;
		tmp_item.nameid = skill_arrow_db[index].cre_id[i];
		tmp_item.amount = skill_arrow_db[index].cre_amount[i];
		if(battle_config.making_arrow_name_input) {
			tmp_item.card[0]=CARD0_CREATE;
			tmp_item.card[1]=0;
			tmp_item.card[2]=GetWord(sd->char_id,0); // CharId
			tmp_item.card[3]=GetWord(sd->char_id,1);
		}
		if(tmp_item.nameid <= 0 || tmp_item.amount <= 0)
			continue;
		if((flag = pc_additem(sd,&tmp_item,tmp_item.amount))) {
			clif_additem(sd,0,0,flag);
			map_addflooritem(&tmp_item,tmp_item.amount,sd->bl.m,sd->bl.x,sd->bl.y,NULL,NULL,NULL,0);
		}
	}

	return 0;
}

/*==========================================
 *
 *------------------------------------------
 */
int skill_blockpc_end (int tid, unsigned int tick, int id, int data)
{
	struct map_session_data *sd = map_id2sd(id);
	if (data <= 0 || data >= MAX_SKILL)
		return 0;
	if (sd) sd->blockskill[data] = 0;
	
	return 1;
}

int skill_blockpc_start(struct map_session_data *sd, int skillid, int tick)
{
	nullpo_retr (-1, sd);

	if (skillid >= GD_SKILLBASE)
		skillid = GD_SKILLRANGEMIN + skillid - GD_SKILLBASE;
	if (skillid >= HM_SKILLBASE)	//[orn]
		skillid = HM_SKILLRANGEMIN + skillid - HM_SKILLBASE;
	if (skillid < 1 || skillid > MAX_SKILL)
		return -1;

	sd->blockskill[skillid] = 1;
	return add_timer(gettick()+tick,skill_blockpc_end,sd->bl.id,skillid);
}

int skill_blockmerc_end (int tid, unsigned int tick, int id, int data)	//[orn]
{
	struct homun_data *hd = (TBL_HOM*) map_id2bl(id);
	if (data <= 0 || data >= MAX_SKILL)
		return 0;
	if (hd) hd->blockskill[data] = 0;
	
	return 1;
}

int skill_blockmerc_start(struct homun_data *hd, int skillid, int tick)	//[orn]
{
	nullpo_retr (-1, hd);

	if (skillid >= GD_SKILLBASE)
		skillid = GD_SKILLRANGEMIN + skillid - GD_SKILLBASE;
	if (skillid >= HM_SKILLBASE)	//[orn]
		skillid = HM_SKILLRANGEMIN + skillid - HM_SKILLBASE;
	if (skillid < 1 || skillid > MAX_SKILL)
		return -1;

	hd->blockskill[skillid] = 1;
	return add_timer(gettick()+tick,skill_blockmerc_end,hd->bl.id,skillid);
}


/*----------------------------------------------------------------------------
 *
 */

/*
 *
 */
int skill_split_str (char *str, char **val, int num)
{
	int i;

	for (i=0; i<num && str; i++){
		val[i] = str;
		str = strchr(str,',');
		if (str)
			*str++=0;
	}
	return i;
}
/*
 *
 */
int skill_split_atoi (char *str, int *val)
{
	int i, j, diff, step = 1;

	for (i=0; i<MAX_SKILL_LEVEL; i++) {
		if (!str) break;
		val[i] = atoi(str);
		str = strchr(str,':');
		if (str)
			*str++=0;
	}
	if(i==0) //No data found.
		return 0;
	if(i==1)
	{	//Single value, have the whole range have the same value.
		for (; i < MAX_SKILL_LEVEL; i++)
			val[i] = val[i-1];
		return i;
	}
	//Check for linear change with increasing steps until we reach half of the data acquired.
	for (step = 1; step <= i/2; step++)
	{
		diff = val[i-1] - val[i-step-1];
		for(j = i-1; j >= step; j--)
			if ((val[j]-val[j-step]) != diff)
				break;
	
		if (j>=step) //No match, try next step.
			continue;
		
		for(; i < MAX_SKILL_LEVEL; i++)
		{	//Apply linear increase
			val[i] = val[i-step]+diff;
			if (val[i] < 1 && val[i-1] >=0) //Check if we have switched from + to -, cap the decrease to 0 in said cases.
			{ val[i] = 1; diff = 0; step = 1; }
		}
		return i;
	}
	//Okay.. we can't figure this one out, just fill out the stuff with the previous value.
	for (;i<MAX_SKILL_LEVEL; i++)
		val[i] = val[i-1];
	return i;
}

/*
 *
 */
void skill_init_unit_layout (void)
{
	int i,j,size,pos = 0;

	memset(skill_unit_layout,0,sizeof(skill_unit_layout));
	for (i=0; i<=MAX_SQUARE_LAYOUT; i++) {
		size = i*2+1;
		skill_unit_layout[i].count = size*size;
		for (j=0; j<size*size; j++) {
			skill_unit_layout[i].dx[j] = (j%size-i);
			skill_unit_layout[i].dy[j] = (j/size-i);
		}
	}
	pos = i;
	for (i=0;i<MAX_SKILL_DB;i++) {
		if (!skill_db[i].unit_id[0] || skill_db[i].unit_layout_type[0] != -1)
			continue;
		switch (i) {
			case MG_FIREWALL:
			case WZ_ICEWALL:
				break;
			case PR_SANCTUARY:
			{
				static const int dx[] = {
					-1, 0, 1,-2,-1, 0, 1, 2,-2,-1,
					 0, 1, 2,-2,-1, 0, 1, 2,-1, 0, 1};
				static const int dy[]={
					-2,-2,-2,-1,-1,-1,-1,-1, 0, 0,
					 0, 0, 0, 1, 1, 1, 1, 1, 2, 2, 2};
				skill_unit_layout[pos].count = 21;
				memcpy(skill_unit_layout[pos].dx,dx,sizeof(dx));
				memcpy(skill_unit_layout[pos].dy,dy,sizeof(dy));
				break;
			}
			case PR_MAGNUS:
			{
				static const int dx[] = {
					-1, 0, 1,-1, 0, 1,-3,-2,-1, 0,
					 1, 2, 3,-3,-2,-1, 0, 1, 2, 3,
					-3,-2,-1, 0, 1, 2, 3,-1, 0, 1,-1, 0, 1};
				static const int dy[] = {
					-3,-3,-3,-2,-2,-2,-1,-1,-1,-1,
					-1,-1,-1, 0, 0, 0, 0, 0, 0, 0,
					 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 3, 3, 3};
				skill_unit_layout[pos].count = 33;
				memcpy(skill_unit_layout[pos].dx,dx,sizeof(dx));
				memcpy(skill_unit_layout[pos].dy,dy,sizeof(dy));
				break;
			}
			case AS_VENOMDUST:
			{
				static const int dx[] = {-1, 0, 0, 0, 1};
				static const int dy[] = { 0,-1, 0, 1, 0};
				skill_unit_layout[pos].count = 5;
				memcpy(skill_unit_layout[pos].dx,dx,sizeof(dx));
				memcpy(skill_unit_layout[pos].dy,dy,sizeof(dy));
				break;
			}
			case CR_GRANDCROSS:
			case NPC_GRANDDARKNESS:
			{
				static const int dx[] = {
					 0, 0,-1, 0, 1,-2,-1, 0, 1, 2,
					-4,-3,-2,-1, 0, 1, 2, 3, 4,-2,
					-1, 0, 1, 2,-1, 0, 1, 0, 0};
				static const int dy[] = {
					-4,-3,-2,-2,-2,-1,-1,-1,-1,-1,
					 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
					 1, 1, 1, 1, 2, 2, 2, 3, 4};
				skill_unit_layout[pos].count = 29;
				memcpy(skill_unit_layout[pos].dx,dx,sizeof(dx));
				memcpy(skill_unit_layout[pos].dy,dy,sizeof(dy));
				break;
			}
			case PF_FOGWALL:
			{
				static const int dx[] = {
					-2,-1, 0, 1, 2,-2,-1, 0, 1, 2,-2,-1, 0, 1, 2};
				static const int dy[] = {
					-1,-1,-1,-1,-1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1};
				skill_unit_layout[pos].count = 15;
				memcpy(skill_unit_layout[pos].dx,dx,sizeof(dx));
				memcpy(skill_unit_layout[pos].dy,dy,sizeof(dy));
				break;
			}
			case PA_GOSPEL:
			{
				static const int dx[] = {
					-1, 0, 1,-1, 0, 1,-3,-2,-1, 0,
					 1, 2, 3,-3,-2,-1, 0, 1, 2, 3,
					-3,-2,-1, 0, 1, 2, 3,-1, 0, 1,
					-1, 0, 1};
				static const int dy[] = {
					-3,-3,-3,-2,-2,-2,-1,-1,-1,-1,
					-1,-1,-1, 0, 0, 0, 0, 0, 0, 0,
					 1, 1, 1, 1, 1, 1, 1, 2, 2, 2,
					 3, 3, 3};
				skill_unit_layout[pos].count = 33;
				memcpy(skill_unit_layout[pos].dx,dx,sizeof(dx));
				memcpy(skill_unit_layout[pos].dy,dy,sizeof(dy));
				break;
			}
			case NJ_KAENSIN:
			{
				static const int dx[] = {-2,-1, 0, 1, 2,-2,-1, 0, 1, 2,-2,-1, 1, 2,-2,-1, 0, 1, 2,-2,-1, 0, 1, 2};
				static const int dy[] = { 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 0, 0, 0, 0,-1,-1,-1,-1,-1,-2,-2,-2,-2,-2};
				skill_unit_layout[pos].count = 24;
				memcpy(skill_unit_layout[pos].dx,dx,sizeof(dx));
				memcpy(skill_unit_layout[pos].dy,dy,sizeof(dy));
				break;
			}
			case NJ_TATAMIGAESHI:
			{
				//Level 1 (count 4, cross of 3x3)
				static const int dx1[] = {-1, 1, 0, 0};
				static const int dy1[] = { 0, 0,-1, 1};
				//Level 2-3 (count 8, cross of 5x5)
				static const int dx2[] = {-2,-1, 1, 2, 0, 0, 0, 0};
				static const int dy2[] = { 0, 0, 0, 0,-2,-1, 1, 2};
				//Level 4-5 (count 12, cross of 7x7
				static const int dx3[] = {-3,-2,-1, 1, 2, 3, 0, 0, 0, 0, 0, 0};
				static const int dy3[] = { 0, 0, 0, 0, 0, 0,-3,-2,-1, 1, 2, 3};
				//lv1
				j = 0;
				skill_unit_layout[pos].count = 4;
				memcpy(skill_unit_layout[pos].dx,dx1,sizeof(dx1));
				memcpy(skill_unit_layout[pos].dy,dy1,sizeof(dy1));
				skill_db[i].unit_layout_type[j] = pos;
				//lv2/3
				j++;
				pos++;
				skill_unit_layout[pos].count = 8;
				memcpy(skill_unit_layout[pos].dx,dx2,sizeof(dx2));
				memcpy(skill_unit_layout[pos].dy,dy2,sizeof(dy2));
				skill_db[i].unit_layout_type[j] = pos;
				skill_db[i].unit_layout_type[++j] = pos;
				//lv4/5
				j++;
				pos++;
				skill_unit_layout[pos].count = 12;
				memcpy(skill_unit_layout[pos].dx,dx3,sizeof(dx3));
				memcpy(skill_unit_layout[pos].dy,dy3,sizeof(dy3));
				skill_db[i].unit_layout_type[j] = pos;
				skill_db[i].unit_layout_type[++j] = pos;
				//Fill in the rest using lv 5.
				for (;j<MAX_SKILL_LEVEL;j++)
					skill_db[i].unit_layout_type[j] = pos;
				//Skip, this way the check below will fail and continue to the next skill.
				pos++;
				break;
			}
			default:
				ShowError("unknown unit layout at skill %d\n",i);
				break;
		}
		if (!skill_unit_layout[pos].count)
			continue;
		for (j=0;j<MAX_SKILL_LEVEL;j++)
			skill_db[i].unit_layout_type[j] = pos;
		pos++;
	}
	firewall_unit_pos = pos;
	for (i=0;i<8;i++) {
		if (i&1) {
			skill_unit_layout[pos].count = 5;
			if (i&0x2) {
				int dx[] = {-1,-1, 0, 0, 1};
				int dy[] = { 1, 0, 0,-1,-1};
				memcpy(skill_unit_layout[pos].dx,dx,sizeof(dx));
				memcpy(skill_unit_layout[pos].dy,dy,sizeof(dy));
			} else {
				int dx[] = { 1, 1 ,0, 0,-1}; 
				int dy[] = { 1, 0, 0,-1,-1}; 
				memcpy(skill_unit_layout[pos].dx,dx,sizeof(dx));
				memcpy(skill_unit_layout[pos].dy,dy,sizeof(dy));
			}
		} else {
			skill_unit_layout[pos].count = 3;
			if (i%4==0) {
				int dx[] = {-1, 0, 1};
				int dy[] = { 0, 0, 0};
				memcpy(skill_unit_layout[pos].dx,dx,sizeof(dx));
				memcpy(skill_unit_layout[pos].dy,dy,sizeof(dy));
			} else {
				int dx[] = { 0, 0, 0};
				int dy[] = {-1, 0, 1};
				memcpy(skill_unit_layout[pos].dx,dx,sizeof(dx));
				memcpy(skill_unit_layout[pos].dy,dy,sizeof(dy));
			}
		}
		pos++;
	}
	icewall_unit_pos = pos;
	for (i=0;i<8;i++) {
		skill_unit_layout[pos].count = 5;
		if (i&1) {
			if (i&0x2) {
				int dx[] = {-2,-1, 0, 1, 2};
				int dy[] = { 2, 1, 0,-1,-2};
				memcpy(skill_unit_layout[pos].dx,dx,sizeof(dx));
				memcpy(skill_unit_layout[pos].dy,dy,sizeof(dy));
			} else {
				int dx[] = { 2, 1 ,0,-1,-2}; 
				int dy[] = { 2, 1, 0,-1,-2}; 
				memcpy(skill_unit_layout[pos].dx,dx,sizeof(dx));
				memcpy(skill_unit_layout[pos].dy,dy,sizeof(dy));
			}
		} else {
			if (i%4==0) {
				int dx[] = {-2,-1, 0, 1, 2};
				int dy[] = { 0, 0, 0, 0, 0};
				memcpy(skill_unit_layout[pos].dx,dx,sizeof(dx));
				memcpy(skill_unit_layout[pos].dy,dy,sizeof(dy));
			} else {
				int dx[] = { 0, 0, 0, 0, 0};
				int dy[] = {-2,-1, 0, 1, 2};
				memcpy(skill_unit_layout[pos].dx,dx,sizeof(dx));
				memcpy(skill_unit_layout[pos].dy,dy,sizeof(dy));
			}
		}
		pos++;
	}
}

/*==========================================
 * DB reading.
 * skill_db.txt
 * skill_cast_db.txt
 * produce_db.txt 
 * create_arrow_db.txt
 * abra_db.txt
 *------------------------------------------
 */
int skill_readdb (void)
{
	int i,j,k,l,m;
	FILE *fp;
	char line[1024],path[1024],*p;
	char *filename[]={"produce_db.txt","produce_db2.txt"};

	memset(skill_db,0,sizeof(skill_db));
	sprintf(path, "%s/skill_db.txt", db_path);
	fp=fopen(path,"r");
	if(fp==NULL){
		ShowError("can't read %s\n", path);
		return 1;
	}
	while(fgets(line,1020,fp)){
		char *split[50];
		if(line[0]=='/' && line[1]=='/')
			continue;
		j = skill_split_str(line,split,15);
		if(j < 15 || split[14]==NULL)
			continue;

		i=atoi(split[0]);
		if (i >= GD_SKILLRANGEMIN && i <= GD_SKILLRANGEMAX) {
			ShowWarning("read skill_db: Can't use skill id %d as guild skills are placed there!\n");
			continue;
		}
		if (i >= GD_SKILLBASE)
			i = GD_SKILLRANGEMIN + i - GD_SKILLBASE;
		if (i >= HM_SKILLBASE)	//[orn]
			i = HM_SKILLRANGEMIN + i - HM_SKILLBASE;
		if(i<=0 || i>MAX_SKILL_DB)
			continue;
		
		skill_split_atoi(split[1],skill_db[i].range);
		skill_db[i].hit=atoi(split[2]);
		skill_db[i].inf=atoi(split[3]);
		skill_db[i].pl=atoi(split[4]);
		skill_db[i].nk=atoi(split[5]);
		skill_split_atoi(split[6],skill_db[i].splash);
		skill_db[i].max=atoi(split[7]);
		skill_split_atoi(split[8],skill_db[i].num);

		if(strcmpi(split[9],"yes") == 0)
			skill_db[i].castcancel=1;
		else
			skill_db[i].castcancel=0;
		skill_db[i].cast_def_rate=atoi(split[10]);
		skill_db[i].inf2=atoi(split[11]);
		skill_db[i].maxcount=atoi(split[12]);
		if(strcmpi(split[13],"weapon") == 0)
			skill_db[i].skill_type=BF_WEAPON;
		else if(strcmpi(split[13],"magic") == 0)
			skill_db[i].skill_type=BF_MAGIC;
		else if(strcmpi(split[13],"misc") == 0)
			skill_db[i].skill_type=BF_MISC;
		else
			skill_db[i].skill_type=0;
		skill_split_atoi(split[14],skill_db[i].blewcount);

		for (j = 0; skill_names[j].id != 0; j++)
			if (skill_names[j].id == i) {
				skill_db[i].name = skill_names[j].name;
				skill_db[i].desc = skill_names[j].desc;
				break;
			}
	}
	fclose(fp);
	ShowStatus("Done reading '"CL_WHITE"%s"CL_RESET"'.\n",path);

	sprintf(path, "%s/skill_require_db.txt", db_path);
	fp=fopen(path,"r");
	if(fp==NULL){
		ShowError("can't read %s\n", path);
		return 1;
	}
	while(fgets(line,1020,fp)){
		char *split[50];
		if(line[0]=='/' && line[1]=='/')
			continue;
		j = skill_split_str(line,split,32);
		if(j < 32 || split[31]==NULL)
			continue;

		i=atoi(split[0]);
		if (i >= GD_SKILLBASE)
			i = GD_SKILLRANGEMIN + i - GD_SKILLBASE;
		if (i >= HM_SKILLBASE)	//[orn]
			i = HM_SKILLRANGEMIN + i - HM_SKILLBASE;
		if(i<=0 || i>MAX_SKILL_DB)
			continue;

		skill_split_atoi(split[1],skill_db[i].hp);
		skill_split_atoi(split[2],skill_db[i].mhp);
		skill_split_atoi(split[3],skill_db[i].sp);
		skill_split_atoi(split[4],skill_db[i].hp_rate);
		skill_split_atoi(split[5],skill_db[i].sp_rate);
		skill_split_atoi(split[6],skill_db[i].zeny);
		
		p = split[7];
		for(j=0;j<32;j++){
			l = atoi(p);
			if (l==99) {
				skill_db[i].weapon = 0xffffffff;
				break;
			}
			else
				skill_db[i].weapon |= 1<<l;
			p=strchr(p,':');
			if(!p)
				break;
			p++;
		}

		p = split[8];
		for(j=0;j<32;j++){
			l = atoi(p);
			if (l)
				skill_db[i].ammo |= 1<<l;
			p=strchr(p,':');
			if(!p)
				break;
			p++;
		}
		skill_split_atoi(split[9],skill_db[i].ammo_qty);

		if( strcmpi(split[10],"hiding")==0 ) skill_db[i].state=ST_HIDING;
		else if( strcmpi(split[10],"cloaking")==0 ) skill_db[i].state=ST_CLOAKING;
		else if( strcmpi(split[10],"hidden")==0 ) skill_db[i].state=ST_HIDDEN;
		else if( strcmpi(split[10],"riding")==0 ) skill_db[i].state=ST_RIDING;
		else if( strcmpi(split[10],"falcon")==0 ) skill_db[i].state=ST_FALCON;
		else if( strcmpi(split[10],"cart")==0 ) skill_db[i].state=ST_CART;
		else if( strcmpi(split[10],"shield")==0 ) skill_db[i].state=ST_SHIELD;
		else if( strcmpi(split[10],"sight")==0 ) skill_db[i].state=ST_SIGHT;
		else if( strcmpi(split[10],"explosionspirits")==0 ) skill_db[i].state=ST_EXPLOSIONSPIRITS;
		else if( strcmpi(split[10],"cartboost")==0 ) skill_db[i].state=ST_CARTBOOST;
		else if( strcmpi(split[10],"recover_weight_rate")==0 ) skill_db[i].state=ST_RECOV_WEIGHT_RATE;
		else if( strcmpi(split[10],"move_enable")==0 ) skill_db[i].state=ST_MOVE_ENABLE;
		else if( strcmpi(split[10],"water")==0 ) skill_db[i].state=ST_WATER;
		else skill_db[i].state=ST_NONE;

		skill_split_atoi(split[11],skill_db[i].spiritball);
		for (j = 0; j < 10; j++) {
			skill_db[i].itemid[j]=atoi(split[12+ 2*j]);
			skill_db[i].amount[j]=atoi(split[13+ 2*j]);
		}
	}
	fclose(fp);
	ShowStatus("Done reading '"CL_WHITE"%s"CL_RESET"'.\n",path);


	sprintf(path, "%s/skill_cast_db.txt", db_path);
	fp=fopen(path,"r");
	if(fp==NULL){
		ShowError("can't read %s\n", path);
		return 1;
	}

	l=0;
	while(fgets(line,1020,fp)){
		char *split[50];
		l++;
		memset(split,0,sizeof(split));	// [Valaris] thanks to fov
		if(line[0]=='/' && line[1]=='/')
			continue;
		j = skill_split_str(line,split,6);
		if(split[0]==NULL || j<2)
			continue; //Blank line.
		if(split[5]==NULL || j<6) {
			ShowWarning("skill_cast_db.txt: Insufficient number of fields at line %d\n", l);
			continue;
		}
		i=atoi(split[0]);
		if (i >= GD_SKILLBASE)
			i = GD_SKILLRANGEMIN + i - GD_SKILLBASE;
		if (i >= HM_SKILLBASE)	//[orn]
			i = HM_SKILLRANGEMIN + i - HM_SKILLBASE;
		if(i<=0 || i>MAX_SKILL_DB)
			continue;

		skill_split_atoi(split[1],skill_db[i].cast);
		skill_split_atoi(split[2],skill_db[i].delay);
		skill_split_atoi(split[3],skill_db[i].walkdelay);
		skill_split_atoi(split[4],skill_db[i].upkeep_time);
		skill_split_atoi(split[5],skill_db[i].upkeep_time2);
	}
	fclose(fp);
	ShowStatus("Done reading '"CL_WHITE"%s"CL_RESET"'.\n",path);


	sprintf(path, "%s/skill_unit_db.txt", db_path);
	fp=fopen(path,"r");
	if (fp==NULL) {
		ShowError("can't read %s\n", path);
		return 1;
	}
        k = 0;
	while (fgets(line,1020,fp)) {
		char *split[50];
		if (line[0]=='/' && line[1]=='/')
			continue;
		j = skill_split_str(line,split,8);
		if (split[7]==NULL || j<8)
			continue;

		i=atoi(split[0]);
		if (i >= GD_SKILLBASE)
			i = GD_SKILLRANGEMIN + i - GD_SKILLBASE;
		if (i >= HM_SKILLBASE)	//[orn]
			i = HM_SKILLRANGEMIN + i - HM_SKILLBASE;
		if(i<=0 || i>MAX_SKILL_DB)
			continue;
		skill_db[i].unit_id[0] = strtol(split[1],NULL,16);
		skill_db[i].unit_id[1] = strtol(split[2],NULL,16);
		skill_split_atoi(split[3],skill_db[i].unit_layout_type);
		skill_split_atoi(split[4],skill_db[i].unit_range);
		skill_db[i].unit_interval = atoi(split[5]);

		if( strcmpi(split[6],"noenemy")==0 ) skill_db[i].unit_target=BCT_NOENEMY;
		else if( strcmpi(split[6],"friend")==0 ) skill_db[i].unit_target=BCT_NOENEMY;
		else if( strcmpi(split[6],"party")==0 ) skill_db[i].unit_target=BCT_PARTY;
		else if( strcmpi(split[6],"ally")==0 ) skill_db[i].unit_target=BCT_PARTY|BCT_GUILD;
		else if( strcmpi(split[6],"all")==0 ) skill_db[i].unit_target=BCT_ALL;
		else if( strcmpi(split[6],"enemy")==0 ) skill_db[i].unit_target=BCT_ENEMY;
		else if( strcmpi(split[6],"self")==0 ) skill_db[i].unit_target=BCT_SELF;
		else if( strcmpi(split[6],"noone")==0 ) skill_db[i].unit_target=BCT_NOONE;
		else skill_db[i].unit_target = strtol(split[6],NULL,16);

		skill_db[i].unit_flag = strtol(split[7],NULL,16);

		if (skill_db[i].unit_flag&UF_DEFNOTENEMY && battle_config.defnotenemy)
			skill_db[i].unit_target=BCT_NOENEMY;

		//By default, target just characters.
		skill_db[i].unit_target |= BL_CHAR;
		if (skill_db[i].unit_flag&UF_NOPC)
			skill_db[i].unit_target &= ~BL_PC;
		if (skill_db[i].unit_flag&UF_NOMOB)
			skill_db[i].unit_target &= ~BL_MOB;
		if (skill_db[i].unit_flag&UF_SKILL)
			skill_db[i].unit_target |= BL_SKILL;
		k++;
	}
	fclose(fp);
	ShowStatus("Done reading '"CL_WHITE"%s"CL_RESET"'.\n",path);
	skill_init_unit_layout();

	memset(skill_produce_db,0,sizeof(skill_produce_db));
	for(m=0;m<2;m++){
		sprintf(path, "%s/%s", db_path, filename[m]);
		fp=fopen(path,"r");
		if(fp==NULL){
			if(m>0)
				continue;
			ShowError("can't read %s\n",path);
			return 1;
		}
		k=0;
		while(fgets(line,1020,fp)){
			char *split[6 + MAX_PRODUCE_RESOURCE * 2];
			int x,y;
			if(line[0]=='/' && line[1]=='/')
				continue;
			memset(split,0,sizeof(split));
			j = skill_split_str(line,split,(3 + MAX_PRODUCE_RESOURCE * 2));
			if(split[0]==0) //fixed by Lupus
				continue;
			i=atoi(split[0]);
			if(i<=0) continue;

			skill_produce_db[k].nameid=i;
			skill_produce_db[k].itemlv=atoi(split[1]);
			skill_produce_db[k].req_skill=atoi(split[2]);

			for(x=3,y=0; split[x] && split[x+1] && y<MAX_PRODUCE_RESOURCE; x+=2,y++){
				skill_produce_db[k].mat_id[y]=atoi(split[x]);
				skill_produce_db[k].mat_amount[y]=atoi(split[x+1]);
			}
			k++;
			if(k >= MAX_SKILL_PRODUCE_DB)
				break;
		}
		fclose(fp);
		ShowStatus("Done reading '"CL_WHITE"%d"CL_RESET"' entries in '"CL_WHITE"%s"CL_RESET"'.\n",k,path);
	}

	memset(skill_arrow_db,0,sizeof(skill_arrow_db));

	sprintf(path, "%s/create_arrow_db.txt", db_path);
	fp=fopen(path,"r");
	if(fp==NULL){
		ShowError("can't read %s\n", path);
		return 1;
	}
	k=0;
	while(fgets(line,1020,fp)){
		char *split[16];
		int x,y;
		if(line[0]=='/' && line[1]=='/')
			continue;
		memset(split,0,sizeof(split));
		j = skill_split_str(line,split,13);
		if(split[0]==0) //fixed by Lupus
			continue;
		i=atoi(split[0]);
		if(i<=0)
			continue;

		skill_arrow_db[k].nameid=i;

		for(x=1,y=0;split[x] && split[x+1] && y<5;x+=2,y++){
			skill_arrow_db[k].cre_id[y]=atoi(split[x]);
			skill_arrow_db[k].cre_amount[y]=atoi(split[x+1]);
		}
		k++;
		if(k >= MAX_SKILL_ARROW_DB)
			break;
	}
	fclose(fp);
	ShowStatus("Done reading '"CL_WHITE"%d"CL_RESET"' entries in '"CL_WHITE"%s"CL_RESET"'.\n",k,path);

	memset(skill_abra_db,0,sizeof(skill_abra_db));
	sprintf(path, "%s/abra_db.txt", db_path);
	fp=fopen(path,"r");
	if(fp==NULL){
		ShowError("can't read %s\n", path);
		return 1;
	}
	k=0;
	while(fgets(line,1020,fp)){
		char *split[16];
		if(line[0]=='/' && line[1]=='/')
			continue;
		memset(split,0,sizeof(split));
		j = skill_split_str(line,split,13);
		if(split[0]==0) //fixed by Lupus
			continue;
		i=atoi(split[0]);
		if(i<=0)
			continue;

		skill_abra_db[i].req_lv=atoi(split[2]);
		skill_abra_db[i].per=atoi(split[3]);

		k++;
		if(k >= MAX_SKILL_ABRA_DB)
			break;
	}
	fclose(fp);
	ShowStatus("Done reading '"CL_WHITE"%d"CL_RESET"' entries in '"CL_WHITE"%s"CL_RESET"'.\n",k,path);

	sprintf(path, "%s/skill_castnodex_db.txt", db_path);
	fp=fopen(path,"r");
	if(fp==NULL){
		ShowError("can't read %s\n", path);
		return 1;
	}
	while(fgets(line,1020,fp)){
		char *split[50];
		if(line[0]=='/' && line[1]=='/')
			continue;
		memset(split,0,sizeof(split));
		j = skill_split_str(line,split,3);
		if(split[0]==0) //fixed by Lupus
			continue;
		i=atoi(split[0]);
		if (i >= GD_SKILLBASE)
			i = GD_SKILLRANGEMIN + i - GD_SKILLBASE;
		if(i<=0 || i>MAX_SKILL_DB)
			continue;

		skill_split_atoi(split[1],skill_db[i].castnodex);
		if (!split[2])
			continue;
		skill_split_atoi(split[2],skill_db[i].delaynodex);
	}
	fclose(fp);
	ShowStatus("Done reading '"CL_WHITE"%s"CL_RESET"'.\n",path);

	sprintf(path, "%s/skill_nocast_db.txt", db_path);
	fp=fopen(path,"r");
	if(fp==NULL){
		ShowError("can't read %s\n", path);
		return 1;
	}
	k=0;
	while(fgets(line,1020,fp)){
		char *split[16];
		if(line[0]=='/' && line[1]=='/')
			continue;
		memset(split,0,sizeof(split));
		j = skill_split_str(line,split,2);
		if(split[0]==0) //fixed by Lupus
			continue;
		i=atoi(split[0]);
		if (i >= GD_SKILLBASE)
			i = GD_SKILLRANGEMIN + i - GD_SKILLBASE;
		if(i<=0 || i>MAX_SKILL_DB)
			continue;
		skill_db[i].nocast|=atoi(split[1]);
		k++;
	}
	fclose(fp);
	ShowStatus("Done reading '"CL_WHITE"%s"CL_RESET"'.\n",path);

	return 0;
}

/*===============================================
 * For reading leveluseskillspamount.txt [Celest]
 *-----------------------------------------------
 */
static int skill_read_skillspamount (void)
{
	char *buf,*p;
	struct skill_db *skill = NULL;
	int s, idx, new_flag=1, level=1, sp=0;

	buf=(char *) grfio_reads("data\\leveluseskillspamount.txt",&s);

	if(buf==NULL)
		return -1;

	buf[s]=0;
	for(p=buf;p-buf<s;){
		char buf2[64];

		if (sscanf(p,"%[@]",buf2) == 1) {
			level = 1;
			new_flag = 1;
		} else if (new_flag && sscanf(p,"%[^#]#",buf2) == 1) {
			for (idx=0; skill_names[idx].id != 0; idx++) {
				if (strstr(buf2, skill_names[idx].name) != NULL) {
					skill = &skill_db[ skill_names[idx].id ];
					new_flag = 0;
					break;
				}
			}
		} else if (!new_flag && sscanf(p,"%d#",&sp) == 1) {
			skill->sp[level-1]=sp;
			level++;
		}

		p=strchr(p,10);
		if(!p) break;
		p++;
	}
	aFree(buf);
	ShowStatus("Done reading '"CL_WHITE"%s"CL_RESET"'.\n","data\\leveluseskillspamount.txt");

	return 0;
}

void skill_reload (void)
{
	skill_readdb();
	if (battle_config.skill_sp_override_grffile)
		skill_read_skillspamount();
}

/*==========================================
 *
 *------------------------------------------
 */
int do_init_skill (void)
{
	skill_readdb();
	
	skill_unit_ers = ers_new((uint32)sizeof(struct skill_unit_group));
	skill_timer_ers  = ers_new((uint32)sizeof(struct skill_timerskill));
	
	if (battle_config.skill_sp_override_grffile)
		skill_read_skillspamount();

	add_timer_func_list(skill_unit_timer,"skill_unit_timer");
	add_timer_func_list(skill_castend_id,"skill_castend_id");
	add_timer_func_list(skill_castend_pos,"skill_castend_pos");
	add_timer_func_list(skill_timerskill,"skill_timerskill");
	add_timer_func_list(skill_blockpc_end, "skill_blockpc_end");
	
	add_timer_interval(gettick()+SKILLUNITTIMER_INVERVAL,skill_unit_timer,0,0,SKILLUNITTIMER_INVERVAL);

	return 0;
}

int do_final_skill(void)
{
	ers_destroy(skill_unit_ers);
	ers_destroy(skill_timer_ers);
	return 0;
}

#include "config.h"
#include "debug.h"
#include "types.h"

#include "constants/ability.h"
#include "constants/file.h"
#include "constants/game.h"
#include "constants/generated/learnsets.h"
#include "constants/hold_item_effects.h"
#include "constants/item.h"
#include "constants/moves.h"
#include "constants/species.h"
#include "constants/weather_numbers.h"

#include "bag.h"
#include "battle.h"
#include "message.h"
#include "party_menu.h"
#include "pokemon.h"
#include "rtc.h"
#include "save.h"
#include "script.h"
#include "window.h"

#define MOVE_TUTOR_NPC_FRONTIER_TOP_LEFT     0
#define MOVE_TUTOR_NPC_FRONTIER_TOP_RIGHT    1
#define MOVE_TUTOR_NPC_FRONTIER_BOTTOM_RIGHT 2
#define MOVE_TUTOR_NPC_HEADBUTT              3

typedef struct TutorMove {
    u16 move;
    u8 cost;
    u8 tutorNpc;
} TutorMove;

TutorMove sTutorMoves[] = {
    { MOVE_DIVE, 1, MOVE_TUTOR_NPC_FRONTIER_TOP_LEFT },
    { MOVE_MUD_SLAP, 1, MOVE_TUTOR_NPC_FRONTIER_TOP_RIGHT },
    { MOVE_FURY_CUTTER, 1, MOVE_TUTOR_NPC_FRONTIER_TOP_LEFT },
    { MOVE_ICY_WIND, 1, MOVE_TUTOR_NPC_FRONTIER_TOP_LEFT },
    { MOVE_ROLLOUT, 1, MOVE_TUTOR_NPC_FRONTIER_TOP_RIGHT },
    { MOVE_THUNDER_PUNCH, 1, MOVE_TUTOR_NPC_FRONTIER_TOP_LEFT },
    { MOVE_FIRE_PUNCH, 1, MOVE_TUTOR_NPC_FRONTIER_TOP_LEFT },
    { MOVE_SUPERPOWER, 1, MOVE_TUTOR_NPC_FRONTIER_TOP_RIGHT },
    { MOVE_ICE_PUNCH, 1, MOVE_TUTOR_NPC_FRONTIER_TOP_LEFT },
    { MOVE_IRON_HEAD, 1, MOVE_TUTOR_NPC_FRONTIER_TOP_RIGHT },
    { MOVE_AQUA_TAIL, 1, MOVE_TUTOR_NPC_FRONTIER_TOP_RIGHT },
    { MOVE_OMINOUS_WIND, 1, MOVE_TUTOR_NPC_FRONTIER_TOP_LEFT },
    { MOVE_GASTRO_ACID, 1, MOVE_TUTOR_NPC_FRONTIER_TOP_RIGHT },
    { MOVE_SNORE, 1, MOVE_TUTOR_NPC_FRONTIER_BOTTOM_RIGHT },
    { MOVE_SPITE, 1, MOVE_TUTOR_NPC_FRONTIER_BOTTOM_RIGHT },
    { MOVE_AIR_CUTTER, 1, MOVE_TUTOR_NPC_FRONTIER_TOP_LEFT },
    { MOVE_HELPING_HAND, 1, MOVE_TUTOR_NPC_FRONTIER_BOTTOM_RIGHT },
    { MOVE_ENDEAVOR, 1, MOVE_TUTOR_NPC_FRONTIER_TOP_RIGHT },
    { MOVE_OUTRAGE, 1, MOVE_TUTOR_NPC_FRONTIER_TOP_RIGHT },
    { MOVE_ANCIENT_POWER, 1, MOVE_TUTOR_NPC_FRONTIER_TOP_RIGHT },
    { MOVE_SYNTHESIS, 1, MOVE_TUTOR_NPC_FRONTIER_BOTTOM_RIGHT },
    { MOVE_SIGNAL_BEAM, 1, MOVE_TUTOR_NPC_FRONTIER_TOP_RIGHT },
    { MOVE_ZEN_HEADBUTT, 1, MOVE_TUTOR_NPC_FRONTIER_TOP_LEFT },
    { MOVE_VACUUM_WAVE, 1, MOVE_TUTOR_NPC_FRONTIER_TOP_LEFT },
    { MOVE_EARTH_POWER, 1, MOVE_TUTOR_NPC_FRONTIER_TOP_RIGHT },
    { MOVE_GUNK_SHOT, 1, MOVE_TUTOR_NPC_FRONTIER_TOP_RIGHT },
    { MOVE_TWISTER, 1, MOVE_TUTOR_NPC_FRONTIER_TOP_RIGHT },
    { MOVE_SEED_BOMB, 1, MOVE_TUTOR_NPC_FRONTIER_TOP_RIGHT },
    { MOVE_IRON_DEFENSE, 1, MOVE_TUTOR_NPC_FRONTIER_TOP_RIGHT },
    { MOVE_MAGNET_RISE, 1, MOVE_TUTOR_NPC_FRONTIER_BOTTOM_RIGHT },
    { MOVE_LAST_RESORT, 1, MOVE_TUTOR_NPC_FRONTIER_BOTTOM_RIGHT },
    { MOVE_BOUNCE, 1, MOVE_TUTOR_NPC_FRONTIER_TOP_RIGHT },
    { MOVE_TRICK, 1, MOVE_TUTOR_NPC_FRONTIER_TOP_LEFT },
    { MOVE_HEAT_WAVE, 1, MOVE_TUTOR_NPC_FRONTIER_TOP_RIGHT },
    { MOVE_KNOCK_OFF, 1, MOVE_TUTOR_NPC_FRONTIER_TOP_LEFT },
    { MOVE_SUCKER_PUNCH, 1, MOVE_TUTOR_NPC_FRONTIER_TOP_LEFT },
    { MOVE_SWIFT, 1, MOVE_TUTOR_NPC_FRONTIER_BOTTOM_RIGHT },
    { MOVE_UPROAR, 1, MOVE_TUTOR_NPC_FRONTIER_BOTTOM_RIGHT },
    { MOVE_SUPER_FANG, 1, MOVE_TUTOR_NPC_FRONTIER_TOP_RIGHT },
    { MOVE_PAIN_SPLIT, 1, MOVE_TUTOR_NPC_FRONTIER_TOP_RIGHT },
    { MOVE_STRING_SHOT, 1, MOVE_TUTOR_NPC_FRONTIER_BOTTOM_RIGHT },
    { MOVE_TAILWIND, 1, MOVE_TUTOR_NPC_FRONTIER_BOTTOM_RIGHT },
    { MOVE_GRAVITY, 1, MOVE_TUTOR_NPC_FRONTIER_BOTTOM_RIGHT },
    { MOVE_WORRY_SEED, 1, MOVE_TUTOR_NPC_FRONTIER_BOTTOM_RIGHT },
    { MOVE_MAGIC_COAT, 1, MOVE_TUTOR_NPC_FRONTIER_BOTTOM_RIGHT },
    { MOVE_ROLE_PLAY, 1, MOVE_TUTOR_NPC_FRONTIER_BOTTOM_RIGHT },
    { MOVE_HEAL_BELL, 1, MOVE_TUTOR_NPC_FRONTIER_BOTTOM_RIGHT },
    { MOVE_LOW_KICK, 1, MOVE_TUTOR_NPC_FRONTIER_TOP_RIGHT },
    { MOVE_SKY_ATTACK, 1, MOVE_TUTOR_NPC_FRONTIER_TOP_RIGHT },
    { MOVE_BLOCK, 1, MOVE_TUTOR_NPC_FRONTIER_BOTTOM_RIGHT },
    { MOVE_BUG_BITE, 1, MOVE_TUTOR_NPC_FRONTIER_TOP_LEFT },
    { MOVE_HEADBUTT, 0, MOVE_TUTOR_NPC_HEADBUTT },
};

u16 LONG_CALL GetLearnableTutorMoves(struct PartyPokemon *mon, u32 moveTutorNpc, u8 dest[])
{
    int i;
    u16 currentMoves[MAX_MON_MOVES];
    for (i = 0; i < MAX_MON_MOVES; i++) {
        currentMoves[i] = GetMonData(mon, MON_DATA_MOVE1 + i, NULL);
    }

    u32 species = GetMonData(mon, MON_DATA_SPECIES, NULL);
    u32 form = GetMonData(mon, MON_DATA_FORM, NULL);
    u32 tutorLearnset[TUTOR_LEARNSETS_BITFIELD_COUNT];

    ReadFromNarcMemberByIdPair(tutorLearnset, ARC_CODE_ADDONS, CODE_ADDON_TUTOR_LEARNSETS, PokeOtherFormMonsNoGet(species, form) * TUTOR_LEARNSETS_BITFIELD_COUNT * sizeof(u32), TUTOR_LEARNSETS_BITFIELD_COUNT * sizeof(u32));

    u16 numLearnableMoves = 0;
    for (u32 j = 0; j < NELEMS(sTutorMoves); j++) {
        BOOL canLearnMove = (tutorLearnset[j / 32] >> (j % 32)) & 1;
        if (canLearnMove && moveTutorNpc == sTutorMoves[j].tutorNpc) {
            for (i = 0; i < MAX_MON_MOVES; i++) {
                if (currentMoves[i] == sTutorMoves[j].move) {
                    break;
                }
            }
            if (i < MAX_MON_MOVES) {
                continue;
            }
            if (dest != NULL) {
                dest[numLearnableMoves] = j;
            }
            numLearnableMoves++;
        }
    }

    return numLearnableMoves;
}

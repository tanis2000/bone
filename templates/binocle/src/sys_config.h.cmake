//
//  Binocle
//  Copyright(C)2015-2017 Valerio Santinelli
//

#pragma once

#define CDOGS_SDL_VERSION "v@VERSION@"

/* where to look for the game data files */
#define BINOCLE_DATA_DIR "@BINOCLE_DATA_DIR@"

#define CDOGS_CFG_DIR "@CDOGS_CFG_DIR@"

#define CDOGS_MUSIC_DIR "@BINOCLE_DATA_DIR@music"
#define CDOGS_GAME_MUSIC_DIR CDOGS_MUSIC_DIR "/game"
#define CDOGS_MENU_MUSIC_DIR CDOGS_MUSIC_DIR "/menu"

#define CDOGS_CAMPAIGN_DIR "missions"
#define CDOGS_DOGFIGHT_DIR "dogfights"

#define CDOGS_FILENAME_MAX 256
#define CDOGS_PATH_MAX 4096

#define FPS_FRAMELIMIT 70
#define PICKUP_LIMIT (FPS_FRAMELIMIT * 5)

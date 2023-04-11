#include <math.h>
#include <pthread.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#ifdef GCC
#include "HEX.h"
#include "KEY.h"
#include "LEDR.h"
#include "SW.H"
#include "audio.h"
#include "video.h"
#else
#include <intelfpgaup/HEX.h>
#include <intelfpgaup/KEY.h>
#include <intelfpgaup/LEDR.h>
#include <intelfpgaup/SW.h>
#include <intelfpgaup/audio.h>
#include <intelfpgaup/video.h>
#endif

#include "hps_0.h"
#include "hwlib.h"  // has uintX_t defines

// Communications
#include "commands.h"
#include "esp.h"
#include "regs.h"

// Audio / Frontend
#include "fonts.h"
#include "sampledata.h"
#include "song.h"
#include "state.h"

#define FPS 40
#define PIXELS_PER_SAMPLE 0.0034375
#define POSE_LIFETIME 64000
#define SAMPLES_PER_FRAME \
  800  // Recalculate if you change the audio rate or frame rate

// Global State

int screen = 0;  // 0 for init, 1 for lobby, 2 for game, 3 for results
int newScreen = 0;
// int switchScreenLock = 0;
// int graphicsUpdateLock = 0;
pthread_mutex_t graphicsLock;
int buffer = 0;  // 0 or 1

// Song

int numSongs = 3;
char* song_displaynames[] = {"tetris 99 theme", "groovy gray",
                             "super treadmill"};
double song_spbs[] = {AUDIO_RATE / (140.0 / 60)};
int song_offsets[] = {0};
char* menu_filename = "shop.txt";
char* song_filenames[] = {"tetris.txt", "groovy.txt", "treadmill.txt"};

int numSamples;
int sampleNo;
int samplesL[MAX_SAMPLES];
int samplesR[MAX_SAMPLES];

char* sampleLine = NULL;
size_t sampleLen = 0;
ssize_t sampleRead;

void loadSong(char* filename) {
  FILE* fp;
  char path[100] = "songs/";
  strcat(path, filename);

  fp = fopen(path, "r");
  if (fp == NULL) {
    printf("Song file not found\n");
    exit(0);
  }

  int count = 0;
  int left = true;
  while (getline(&sampleLine, &sampleLen, fp) != -1) {
    if (left) {
      samplesL[count] = (int)(VOLUME * atof(sampleLine));
    } else {
      samplesR[count] = (int)(VOLUME * atof(sampleLine));
      count++;
    }
    left = !left;
  }

  sampleNo = 0;
  numSamples = count;

#ifdef DEBUG
  printf("Loaded Song %s | %d samples\n", path, numSamples);
#endif
  fclose(fp);
}

int writeAudio(int l, int r) {
  int space = audio_check_write();
  int left_space = ((space >> 16) & 0xff00) >> 8;
  int right_space = ((space >> 16) & 0xff);

  if (left_space > 0 && right_space > 0) {
    // printf("Space available\n");
    LEDR_set(0);
  } else {
    // printf("writeAudio: %x %d %d\n", space, left_space, right_space);
    // printf("Channels are full\n");
    LEDR_set(1);
    // audio_wait_write();
    return 0;
  }

  audio_write_left(l);
  audio_write_right(r);

  return 1;
}

// State data

const char* init_names[] = {"alex", "bell", "kery", "leon"};
int init_name_speeds[] = {4, 4, -4, -4};
int init_name_offsets[] = {0, 0, 40, 40};
short color_init_names[] = {0xe4a, 0xd19, 0xc641, 0x7859};

struct InitState initState;
struct LobbyState lobbyState;
struct GameState gameState;
struct ResultsState resultsState;

void resetInitState(void) {
  initState.titleSlideDone = 0;
  initState.statusSlideDone = 0;

  for (int i = 0; i < 2; i++) {
    struct InitScreenState state = {240.0,
                                    -100.0,
                                    240.0,
                                    {{{0}, {8}, {16}, {24}},
                                     {{160}, {168}, {176}, {184}},
                                     {{80}, {88}, {96}, {104}},
                                     {{240}, {248}, {256}, {264}}}};
    initState.initScreenStates[i] = state;
  }
}

void resetLobbyState(void) {
  lobbyState.mode = 0;
  lobbyState.songId = 0;

  for (int i = 0; i < 2; i++) {
    struct LobbyScreenState state = {1, 1, 1, 1};
    lobbyState.lobbyScreenStates[i] = state;
  }
}

void resetGameState(void) {
  gameState.pressedStart = 0;
  gameState.threeDone = 0;
  gameState.twoDone = 0;
  gameState.oneDone = 0;  // Analogous to game started
  gameState.goDone = 0;
  gameState.score = 0;

  gameState.countdown = 0;

  gameState.earlyPoseA = -1;
  gameState.earlyPoseD = -1;
  gameState.latePoseA = -1;
  gameState.latePoseD = -1;

  for (int i = 0; i < 2; i++) {
    struct GameScreenState state = {0,  0,  0,  0,  0,   0,  0,
                                    -1, -1, -1, -1, {0}, {0}};
    gameState.gameScreenStates[i] = state;
  }
}

// Poses

int numPoses;
struct Pose defenderPoses[MAX_POSES] = {};
struct Pose attackerPoses[MAX_POSES] = {};

char* poseLine = NULL;
size_t poseLen = 0;
ssize_t poseRead;

void loadPoses(char* filename) {
  FILE* fp;
  char path[50] = "poses/";
  strcat(path, filename);

  fp = fopen(path, "r");
  if (fp == NULL) {
    printf("Pose file not found\n");
    exit(0);
  }

  char* delim = " ";
  int maxPoses = 0;

  while (getline(&poseLine, &poseLen, fp) != -1) {
    int lineSize = strlen(poseLine);
    char* ptr = strtok(poseLine, delim);
    int token = 0;
    int id;
    int isDefender;
    int sample;

    while (ptr != NULL) {
      if (token == 0) {
        id = atoi(ptr);
      } else if (token == 1) {
        isDefender = atoi(ptr);
      } else if (token == 2) {
        sample = (int)(atof(ptr) * song_spbs[lobbyState.songId] +
                       song_offsets[lobbyState.songId]);
        if (isDefender == 1) {
          defenderPoses[id].sample = sample;
          defenderPoses[id].isDefender = 1;
        } else {
          attackerPoses[id].sample = sample;
        }
      }
      token++;
      ptr = strtok(NULL, delim);
    }

    if (id > maxPoses) maxPoses = id;
  }

  numPoses = maxPoses + 1;

#ifdef DEBUG
  printf("Loaded Pose %s | %d samples\n", path, numPoses);
#endif

  fclose(fp);
}

void insertDefenderPose(int id, double ha, double ma) {
  struct Pose pose;
  pose.hx = POSE_HOUR_LENGTH * cos(ha);
  pose.hy = POSE_HOUR_LENGTH * sin(ha);
  pose.mx = POSE_MINUTE_LENGTH * cos(ma);
  pose.my = POSE_MINUTE_LENGTH * sin(ma);
  pose.sample = defenderPoses[id].sample;
  pose.isDefender = 1;

  defenderPoses[id] = pose;
}

void addScore(int score) { gameState.score += score; }

int getPoseX(struct Pose p) {
  int x = (int)(GAME_VLINE_MARGIN +
                PIXELS_PER_SAMPLE *
                    (p.sample - sampleNo + song_offsets[lobbyState.songId]));
  if (x >= GAME_VLINE_MARGIN && x <= WIDTH - GAME_VLINE_MARGIN) {
    return x;
  }
  return -1;
}

void switchBuffer(void) {
  video_show();
  buffer = !buffer;
}

void initGraphics(int s) {
  if (pthread_mutex_trylock(&graphicsLock)) return;
  // while (graphicsUpdateLock)
  //   ;
  // switchScreenLock = 1;
  screen = s;
  for (int i = 0; i < 2; i++) {  // Put the same thing on both buffers
    if (s == 0) {                // Init
      resetInitState();
      drawBackground();
      video_box(BORDER + 1, 110, WIDTH - BORDER - 1, 180, COLOR_INIT_TUBE);
    } else if (s == 1) {  // Lobby
      resetLobbyState();
      drawBackground();
      video_box(BORDER + 1, 50, WIDTH - BORDER - 1, 100, COLOR_LOBBY_BG2);
      video_box(BORDER + 1, 140, WIDTH - BORDER - 1, 180, COLOR_LOBBY_BG2);
      drawString(italicFont, "lobby", LOBBY_TITLE_X, LOBBY_TITLE_Y,
                 COLOR_LOBBY_TITLE, LOBBY_TITLE_SCALE, LOBBY_TITLE_KERNING);
      video_box(LOBBY_PLAYER_X1, LOBBY_PLAYER_Y,
                LOBBY_PLAYER_X1 + LOBBY_PLAYER_W,
                LOBBY_PLAYER_Y + LOBBY_PLAYER_H, COLOR_LOBBY_PLAYER_BORDER);
      video_box(LOBBY_PLAYER_X2, LOBBY_PLAYER_Y,
                LOBBY_PLAYER_X2 + LOBBY_PLAYER_W,
                LOBBY_PLAYER_Y + LOBBY_PLAYER_H, COLOR_LOBBY_PLAYER_BORDER);
      video_box(LOBBY_PLAYER_X1 + LOBBY_PLAYER_BORDER,
                LOBBY_PLAYER_Y + LOBBY_PLAYER_BORDER,
                LOBBY_PLAYER_X1 + LOBBY_PLAYER_W - LOBBY_PLAYER_BORDER,
                LOBBY_PLAYER_Y + LOBBY_PLAYER_H - LOBBY_PLAYER_BORDER,
                COLOR_LOBBY_PLAYER_FILL);
      video_box(LOBBY_PLAYER_X2 + LOBBY_PLAYER_BORDER,
                LOBBY_PLAYER_Y + LOBBY_PLAYER_BORDER,
                LOBBY_PLAYER_X2 + LOBBY_PLAYER_W - LOBBY_PLAYER_BORDER,
                LOBBY_PLAYER_Y + LOBBY_PLAYER_H - LOBBY_PLAYER_BORDER,
                COLOR_LOBBY_PLAYER_FILL);
      video_box(LOBBY_MODE_X, LOBBY_MODE_Y, LOBBY_MODE_X + LOBBY_MODE_W,
                LOBBY_MODE_Y + LOBBY_MODE_H, COLOR_LOBBY_MODE_BORDER);
      video_box(LOBBY_SONG_X, LOBBY_SONG_Y, LOBBY_SONG_X + LOBBY_SONG_W,
                LOBBY_SONG_Y + LOBBY_SONG_H, COLOR_LOBBY_SONG_FILL);
      drawChar(arrowFont, 'a', LOBBY_SONG_X - FONT_WIDTH - 5, 155,
               COLOR_LOBBY_SONG_ARROW, 1);
      drawChar(arrowFont, 'b', LOBBY_SONG_X + LOBBY_SONG_W + 5, 155,
               COLOR_LOBBY_SONG_ARROW, 1);

      drawStringCenter(basicFont, "sw0", LOBBY_MODE_SW_X, LOBBY_MODE_Y + 10,
                       COLOR_LOBBY_MODE_BORDER, 1, 1);
      drawStringCenter(basicFont, "key1", 50, LOBBY_SONG_Y + 10,
                       COLOR_LOBBY_MODE_BORDER, 1, 1);
      drawStringCenter(basicFont, "key2", LOBBY_SONG_KEY_X, LOBBY_SONG_Y + 10,
                       COLOR_LOBBY_MODE_BORDER, 1, 1);

      if (lobbyState.p1Connected) {
        drawStringCenter(basicFont, "p1 ready", 107, 75,
                         COLOR_LOBBY_PLAYER_READY, LOBBY_PLAYER_SCALE,
                         LOBBY_PLAYER_KERNING);
      } else {
        drawStringCenter(basicFont, "p1 missing", 107, 75,
                         COLOR_LOBBY_PLAYER_MISSING, LOBBY_PLAYER_SCALE,
                         LOBBY_PLAYER_KERNING);
      }
      if (lobbyState.p2Connected) {
        drawStringCenter(basicFont, "p2 ready", 214, 75,
                         COLOR_LOBBY_PLAYER_READY, LOBBY_PLAYER_SCALE,
                         LOBBY_PLAYER_KERNING);
      } else {
        drawStringCenter(basicFont, "p2 missing", 214, 75,
                         COLOR_LOBBY_PLAYER_MISSING, LOBBY_PLAYER_SCALE,
                         LOBBY_PLAYER_KERNING);
      }

    } else if (s == 2) {  // Game
      resetGameState();
      drawBackground();

      // Straight lines
      video_line(BORDER + 1, GAME_VLINE_MARGIN, WIDTH - BORDER - 1,
                 GAME_VLINE_MARGIN, COLOR_GAME_HLINE);
      video_line(BORDER + 1, HEIGHT - GAME_VLINE_MARGIN, WIDTH - BORDER - 1,
                 HEIGHT - GAME_VLINE_MARGIN, COLOR_GAME_HLINE);
      drawGameVLines();

      drawPBarOutline();

      drawStringCenter(basicFont, "press key1 to start", GAME_TOP_X, GAME_TOP_Y,
                       COLOR_GAME_PBAR_FILL, 1, 1);
    }
    switchBuffer();
  }
  // switchScreenLock = 0;
  pthread_mutex_unlock(&graphicsLock);
}

double lerp(double current, double target, double factor) {
  return current + (target - current) * factor;
}

void updateGraphics(void) {
  if (pthread_mutex_trylock(&graphicsLock)) return;
  // pthread_mutex_lock(&graphicsLock);
  // graphicsUpdateLock = 1;

  char sendBuffer[ESP_BUFFER_SIZE];
  if (newScreen == 0) {  // Init
    struct InitScreenState* prevState = &(initState.initScreenStates[buffer]);
    struct InitScreenState* otherState =
        &(initState.initScreenStates[(buffer + 1) % 2]);

    if (!initState.titleSlideDone) {
      int dx = (int)round(prevState->danceX);
      int bx = (int)round(prevState->battleX);

      drawString(italicFont, "dance", dx, INIT_DANCE_Y, COLOR_BG,
                 INIT_DANCE_SCALE, INIT_DANCE_KERNING);
      drawString(italicFont, "battle", bx, INIT_BATTLE_Y, COLOR_BG,
                 INIT_BATTLE_SCALE, INIT_BATTLE_KERNING);

      drawHLine(INIT_DANCE_Y + 35, dx + 147, dx + 230, COLOR_BG);
      drawHLine(INIT_DANCE_Y + 30, dx + 147, dx + 180, COLOR_BG);

      drawHLine(INIT_BATTLE_Y, bx - 140, bx - 4, COLOR_BG);
      drawHLine(INIT_BATTLE_Y + 5, bx - 55, bx - 4, COLOR_BG);
      drawHLine(INIT_BATTLE_Y + 10, bx - 25, bx - 4, COLOR_BG);

      prevState->danceX =
          lerp(otherState->danceX, INIT_DANCE_X, INIT_TITLE_LERP);
      prevState->battleX =
          lerp(otherState->battleX, INIT_BATTLE_X, INIT_TITLE_LERP);
      dx = (int)round(prevState->danceX);
      bx = (int)round(prevState->battleX);

      drawString(italicFont, "dance", dx, INIT_DANCE_Y, COLOR_INIT_DANCE,
                 INIT_DANCE_SCALE, INIT_DANCE_KERNING);
      drawString(italicFont, "battle", bx, INIT_BATTLE_Y, COLOR_INIT_BATTLE,
                 INIT_BATTLE_SCALE, INIT_BATTLE_KERNING);

      drawHLine(INIT_DANCE_Y + 35, dx + 147, dx + 230, COLOR_INIT_HLINE);
      drawHLine(INIT_DANCE_Y + 30, dx + 147, dx + 180, COLOR_INIT_HLINE);

      drawHLine(INIT_BATTLE_Y, bx - 140, bx - 4, COLOR_INIT_HLINE);
      drawHLine(INIT_BATTLE_Y + 5, bx - 55, bx - 4, COLOR_INIT_HLINE);
      drawHLine(INIT_BATTLE_Y + 10, bx - 25, bx - 4, COLOR_INIT_HLINE);

      if (dx == INIT_DANCE_X && bx == INIT_BATTLE_X &&
          (int)round(otherState->battleX) == INIT_BATTLE_X) {
        initState.titleSlideDone = 1;
      }
    } else if (!initState.statusSlideDone) {
      int sy = (int)round(prevState->statusY);
      drawString(basicFont, "waiting for server connection", INIT_STATUS_X, sy,
                 COLOR_BG, INIT_STATUS_SCALE, INIT_STATUS_KERNING);
      prevState->statusY =
          lerp(otherState->statusY, INIT_STATUS_Y, INIT_TITLE_LERP);
      sy = (int)round(prevState->statusY);
      drawString(basicFont, "waiting for server connection", INIT_STATUS_X, sy,
                 COLOR_INIT_STATUS, INIT_STATUS_SCALE, INIT_STATUS_KERNING);

      if (sy == INIT_STATUS_Y &&
          (int)round(otherState->statusY) == INIT_STATUS_Y) {
        initState.statusSlideDone = 1;
      }
    } else {
      for (int n = 0; n < 4; n++) {
        for (int c = 0; c < 4; c++) {
          if ((prevState->namePositions)[n][c][1] != 0) {
            drawChar(italicFont, init_names[n][c],
                     (prevState->namePositions)[n][c][0],
                     (prevState->namePositions)[n][c][1], COLOR_INIT_TUBE, 1);
          }
        }
      }
      for (int n = 0; n < 4; n++) {
        for (int c = 0; c < 4; c++) {
          int newX = (otherState->namePositions)[n][c][0] + init_name_speeds[n];
          if (newX >= WIDTH) newX -= WIDTH;
          if (newX < 0) newX += WIDTH;
          int newY = sin(INIT_NAME_FREQUENCY * (newX + init_name_offsets[n])) *
                         INIT_NAME_AMPLITUDE +
                     INIT_NAME_Y;
          drawChar(italicFont, init_names[n][c], newX, newY,
                   color_init_names[n], 1);
          (prevState->namePositions)[n][c][0] = newX;
          (prevState->namePositions)[n][c][1] = newY;
        }
      }
    }
  } else if (newScreen == 1) {  // Lobby
    struct LobbyScreenState* prevState =
        &(lobbyState.lobbyScreenStates[buffer]);
    // struct LobbyScreenState * otherState =
    // &(lobbyScreenStates[(buffer+1)%2]);

    if (lobbyState.p1Connected != prevState->p1Connected) {
      if (lobbyState.p1Connected) {
        drawStringCenter(basicFont, "p1 missing", 107, 75,
                         COLOR_LOBBY_PLAYER_FILL, LOBBY_PLAYER_SCALE,
                         LOBBY_PLAYER_KERNING);
        drawStringCenter(basicFont, "p1 ready", 107, 75,
                         COLOR_LOBBY_PLAYER_READY, LOBBY_PLAYER_SCALE,
                         LOBBY_PLAYER_KERNING);
      } else {
        drawStringCenter(basicFont, "p1 ready", 107, 75,
                         COLOR_LOBBY_PLAYER_FILL, LOBBY_PLAYER_SCALE,
                         LOBBY_PLAYER_KERNING);
        drawStringCenter(basicFont, "p1 missing", 107, 75,
                         COLOR_LOBBY_PLAYER_MISSING, LOBBY_PLAYER_SCALE,
                         LOBBY_PLAYER_KERNING);
      }
      prevState->p1Connected = lobbyState.p1Connected;
    }
    if (lobbyState.p2Connected != prevState->p2Connected) {
      if (lobbyState.p2Connected) {
        drawStringCenter(basicFont, "p2 missing", 214, 75,
                         COLOR_LOBBY_PLAYER_FILL, LOBBY_PLAYER_SCALE,
                         LOBBY_PLAYER_KERNING);
        drawStringCenter(basicFont, "p2 ready", 214, 75,
                         COLOR_LOBBY_PLAYER_READY, LOBBY_PLAYER_SCALE,
                         LOBBY_PLAYER_KERNING);
      } else {
        drawStringCenter(basicFont, "p2 ready", 214, 75,
                         COLOR_LOBBY_PLAYER_FILL, LOBBY_PLAYER_SCALE,
                         LOBBY_PLAYER_KERNING);
        drawStringCenter(basicFont, "p2 missing", 214, 75,
                         COLOR_LOBBY_PLAYER_MISSING, LOBBY_PLAYER_SCALE,
                         LOBBY_PLAYER_KERNING);
      }
      prevState->p2Connected = lobbyState.p2Connected;
    }
    if (lobbyState.mode != prevState->mode) {
      if (lobbyState.mode) {
        video_box(LOBBY_MODE_X + LOBBY_MODE_BORDER,
                  LOBBY_MODE_Y + LOBBY_MODE_BORDER,
                  LOBBY_MODE_X + LOBBY_MODE_BORDER + 100,
                  LOBBY_MODE_Y + LOBBY_MODE_H - LOBBY_MODE_BORDER,
                  COLOR_LOBBY_MODE_FILL1);
        video_box(LOBBY_MODE_X + LOBBY_MODE_BORDER + 100,
                  LOBBY_MODE_Y + LOBBY_MODE_BORDER,
                  LOBBY_MODE_X + LOBBY_MODE_BORDER + 200,
                  LOBBY_MODE_Y + LOBBY_MODE_H - LOBBY_MODE_BORDER,
                  COLOR_LOBBY_MODE_FILL2);
        drawStringCenter(basicFont, "p1 attacks",
                         LOBBY_MODE_X + LOBBY_MODE_BORDER + 50, 120,
                         COLOR_LOBBY_MODE_TEXT1, 1, 1);
        drawStringCenter(basicFont, "both defend",
                         LOBBY_MODE_X + LOBBY_MODE_BORDER + 150, 120,
                         COLOR_LOBBY_MODE_TEXT2, 1, 1);
      } else {
        video_box(LOBBY_MODE_X + LOBBY_MODE_BORDER,
                  LOBBY_MODE_Y + LOBBY_MODE_BORDER,
                  LOBBY_MODE_X + LOBBY_MODE_BORDER + 100,
                  LOBBY_MODE_Y + LOBBY_MODE_H - LOBBY_MODE_BORDER,
                  COLOR_LOBBY_MODE_FILL2);
        video_box(LOBBY_MODE_X + LOBBY_MODE_BORDER + 100,
                  LOBBY_MODE_Y + LOBBY_MODE_BORDER,
                  LOBBY_MODE_X + LOBBY_MODE_BORDER + 200,
                  LOBBY_MODE_Y + LOBBY_MODE_H - LOBBY_MODE_BORDER,
                  COLOR_LOBBY_MODE_FILL1);
        drawStringCenter(basicFont, "p1 attacks",
                         LOBBY_MODE_X + LOBBY_MODE_BORDER + 50, 120,
                         COLOR_LOBBY_MODE_TEXT2, 1, 1);
        drawStringCenter(basicFont, "both defend",
                         LOBBY_MODE_X + LOBBY_MODE_BORDER + 150, 120,
                         COLOR_LOBBY_MODE_TEXT1, 1, 1);
      }
      prevState->mode = lobbyState.mode;
    }
    if (lobbyState.songId != prevState->songId) {
      drawStringCenter(basicFont, (char*)song_displaynames[prevState->songId],
                       160, 160, COLOR_LOBBY_SONG_FILL, 1, 1);
      drawStringCenter(basicFont, (char*)song_displaynames[lobbyState.songId],
                       160, 160, COLOR_LOBBY_SONG_TEXT, 1, 1);
      prevState->songId = lobbyState.songId;
    }

  } else if (newScreen == 2) {  // Game
    struct GameScreenState* prevState = &(gameState.gameScreenStates[buffer]);

    // Update progress bar
    int newPBarWidth = (int)(157 * (double)sampleNo / NUM_SAMPLES);
    drawPBarFill(prevState->pBarWidth, newPBarWidth);
    prevState->pBarWidth = newPBarWidth;

    // Poses
    int prevEarlyA = prevState->earlyPoseA;
    int prevEarlyD = prevState->earlyPoseD;
    int prevLateA = prevState->latePoseA;
    int prevLateD = prevState->latePoseD;

    // Update top text
    if (prevState->pressedStart != gameState.pressedStart) {
      drawStringCenter(basicFont, "press key1 to start", GAME_TOP_X, GAME_TOP_Y,
                       COLOR_BG, 1, 1);
      drawStringCenter(basicFont, "3", GAME_TOP_X, GAME_TOP_Y,
                       COLOR_GAME_PBAR_FILL, 1, 1);
      prevState->pressedStart = gameState.pressedStart;
    } else if (prevState->threeDone != gameState.threeDone) {
      drawStringCenter(basicFont, "3", GAME_TOP_X, GAME_TOP_Y, COLOR_BG, 1, 1);
      drawStringCenter(basicFont, "2", GAME_TOP_X, GAME_TOP_Y,
                       COLOR_GAME_PBAR_FILL, 1, 1);
      prevState->threeDone = gameState.threeDone;
    } else if (prevState->twoDone != gameState.twoDone) {
      drawStringCenter(basicFont, "2", GAME_TOP_X, GAME_TOP_Y, COLOR_BG, 1, 1);
      drawStringCenter(basicFont, "1", GAME_TOP_X, GAME_TOP_Y,
                       COLOR_GAME_PBAR_FILL, 1, 1);
      prevState->twoDone = gameState.twoDone;
    } else if (prevState->oneDone != gameState.oneDone) {
      drawStringCenter(basicFont, "1", GAME_TOP_X, GAME_TOP_Y, COLOR_BG, 1, 1);
      drawStringCenter(italicFont, "go!", GAME_TOP_X, GAME_TOP_Y,
                       COLOR_GAME_PBAR_FILL, 1, 1);
      prevState->oneDone = gameState.oneDone;
    } else if (prevState->goDone != gameState.goDone) {
      drawStringCenter(italicFont, "go!", GAME_TOP_X, GAME_TOP_Y, COLOR_BG, 1,
                       1);
      drawStringCenter(italicFont, "p2 score: 0", GAME_TOP_X, GAME_TOP_Y,
                       COLOR_GAME_PBAR_FILL, 1, 1);
      prevState->goDone = gameState.goDone;
    } else if (prevState->score != gameState.score) {
      char oldScore[50];
      char newScore[50];
      sprintf(oldScore, "p2 score: %d", prevState->score);
      sprintf(newScore, "p2 score: %d", gameState.score);
      drawStringCenter(italicFont, oldScore, GAME_TOP_X, GAME_TOP_Y, COLOR_BG,
                       1, 1);
      drawStringCenter(italicFont, newScore, GAME_TOP_X, GAME_TOP_Y,
                       COLOR_GAME_PBAR_FILL, 1, 1);
      prevState->score = gameState.score;
    }

    // Erase previous poses
    if (prevEarlyA != -1) {
      for (int p = prevEarlyA; p <= prevLateA; p++) {
        int prevX = prevState->poseAXs[p];
        if (prevX != -1) drawPose(attackerPoses[p], prevX, 1);
      }
    }
    if (prevEarlyD != -1) {
      for (int p = prevEarlyD; p <= prevLateD; p++) {
        int prevX = prevState->poseDXs[p];
        if (prevX != -1) drawPose(defenderPoses[p], prevX, 1);
      }
    }

    // Advance pose pointers if necessary
    if (gameState.earlyPoseA == -1) {
      if (sampleNo >= attackerPoses[0].sample - POSE_LIFETIME) {
        gameState.earlyPoseA = 0;
        gameState.latePoseA = 0;
      }
    } else {
      if (gameState.latePoseA < numPoses &&
          sampleNo >=
              attackerPoses[gameState.latePoseA + 1].sample - POSE_LIFETIME) {
        gameState.latePoseA++;
      }
      if (gameState.earlyPoseA < numPoses &&
          sampleNo >= attackerPoses[gameState.earlyPoseA].sample) {
        // HERE (pass earlyPoseA)
        sprintf(sendBuffer, "{\"command\":\"captureAttacker\",\"poseID\":%u}",
                gameState.earlyPoseA);
        esp_write(sendBuffer);
        gameState.earlyPoseA++;
      }
    }
    prevState->earlyPoseA = gameState.earlyPoseA;
    prevState->latePoseA = gameState.latePoseA;

    if (gameState.earlyPoseD == -1) {
      if (sampleNo >= defenderPoses[0].sample - POSE_LIFETIME) {
        gameState.earlyPoseD = 0;
        gameState.latePoseD = 0;
      }
    } else {
      if (gameState.latePoseD < numPoses &&
          sampleNo >=
              defenderPoses[gameState.latePoseD + 1].sample - POSE_LIFETIME) {
        gameState.latePoseD++;
      }
      if (gameState.earlyPoseD < numPoses &&
          sampleNo >= defenderPoses[gameState.earlyPoseD].sample) {
        // HERE (pass earlyPoseD)
        sprintf(sendBuffer, "{\"command\":\"captureDefender\",\"poseID\":%u}",
                gameState.earlyPoseD);
        esp_write(sendBuffer);
        gameState.earlyPoseD++;
      }
    }
    prevState->earlyPoseD = gameState.earlyPoseD;
    prevState->latePoseD = gameState.latePoseD;

    // Draw new poses
    drawGameVLines();

    if (gameState.earlyPoseA != -1) {
      for (int p = gameState.earlyPoseA; p <= gameState.latePoseA; p++) {
        int newX = getPoseX(attackerPoses[p]);
        prevState->poseAXs[p] = newX;
        if (newX != -1) drawPose(attackerPoses[p], newX, 0);
      }
    }
    if (gameState.earlyPoseD != -1) {
      for (int p = gameState.earlyPoseD; p <= gameState.latePoseD; p++) {
        int newX = getPoseX(defenderPoses[p]);
        prevState->poseDXs[p] = newX;
        if (newX != -1) drawPose(defenderPoses[p], newX, 0);
      }
    }
  }

  switchBuffer();
  pthread_mutex_unlock(&graphicsLock);
  // graphicsUpdateLock = 0;
}

void* outputThread(void* vargp) {
  // Initialization

  audio_init();
  audio_rate(AUDIO_RATE);
  initGraphics(0);

  // Loop

  int s = 0;

  while (1) {
    if ((screen == 2) && gameState.pressedStart) {
      sampleNo =
          (sampleNo + writeAudio(samplesL[sampleNo], samplesR[sampleNo])) %
          numSamples;
      //   sampleNo++;
    } else {
      // writeAudio(0, 0);
    }
    // s = (s + 1) % SAMPLES_PER_FRAME;
    // if (s == 0) {
    // while (switchScreenLock)
    // ;
    // updateGraphics();
    // }
  }
}

void* graphicsThread(void* vargp) {
  while (1) {
    // while (switchScreenLock)
    // ;
    updateGraphics();
  }
}

int main(int argc, char** argv) {
  void* virtual_base;
  char sendBuffer[ESP_BUFFER_SIZE];
  unsigned int recvLen = 0;
  unsigned int poseID = 0;

  if (!audio_open()) return 1;
  if (!video_open()) return 1;
  if (!SW_open()) return 1;
  if (!KEY_open()) return 1;
  if (!LEDR_open()) return 1;
  if (!HEX_open()) return 1;
  if (!regs_init(&virtual_base)) return 1;

  audio_init();
  audio_rate(AUDIO_RATE);
  initGraphics(newScreen);
  // pthread_t outputThread_id;
  // pthread_create(&outputThread_id, NULL, outputThread, NULL);

  pthread_mutex_init(&graphicsLock, NULL);

  loadSong("shop.txt");

  pthread_t graphicsThread_id;
  pthread_create(&graphicsThread_id, NULL, graphicsThread, NULL);

  // Comms
  audio_init_reg(virtual_base);
  uart_init(virtual_base);
  esp_reset();

  // if (!esp_init(argc > 1 ? argv[1] : NULL)) return 1;

  // pthread_t espThread_id;
  // pthread_create(&espThread_id, NULL, (void*)&esp_init,
  //  argc > 1 ? argv[1] : NULL);

  int swState = 0;
  int keyState = 0;
  int s = 0;

  while (keyState != 8) {
    SW_read(&swState);
    KEY_read(&keyState);

    if (esp_failed) {
      printf("ESP Failed to connect to backend. Quitting ...\n");
      break;
    } else if (!esp_ready) {
      // Attempt to connect to backend
      esp_init(argc > 1 ? argv[1] : NULL);

      // Tell host what device I am
      if (esp_ready) {
        esp_write("{\"command\":\"setType\",\"identifier\":\"host\"}");
      }
    } else {
      // Read from ESP
      char* recvBuffer = esp_read(&recvLen);

      // If we received something, parse it
      if (recvBuffer) {
#ifdef DEBUG
        printf("[%d] %s\n", recvLen, recvBuffer);
#endif

        unsigned int p1Connected = lobbyState.p1Connected;
        unsigned int p2Connected = lobbyState.p2Connected;

        unsigned int recvID = 0;
        unsigned int score = 0;
        double angles[3];

        switch (recvBuffer[0]) {
          case ESP_LOBBY_COMMAND:
            sscanf(recvBuffer + 1, "%u %u", &p1Connected, &p2Connected);

            lobbyState.p1Connected = p1Connected > 0;
            lobbyState.p2Connected = p2Connected > 0;

#ifdef DEBUG
            printf("P1: %d, P2: %d\n", lobbyState.p1Connected,
                   lobbyState.p2Connected);
#endif
            break;
          case ESP_POSE_COMMAND:
            sscanf(recvBuffer + 1, "%u %lf %lf %lf", &recvID, angles,
                   angles + 1, angles + 2);

            printf("Pose %u: %lf %lf %lf\n", recvID, angles[0], angles[1],
                   angles[2]);

            insertDefenderPose(recvID, angles[0], angles[1]);
            break;
          case ESP_SCORE_COMMAND:
            sscanf(recvBuffer + 1, "%u %u", &recvID, &score);

            printf("Score %u: %u\n", recvID, score);

            addScore(score);
            break;
          case ESP_CLOSE_COMMAND:
            newScreen = 0;
            // initGraphics(0);
            esp_close(argc > 1 ? argv[1] : NULL);
            break;
          default:
#ifdef DEBUG
            printf("[%d] %s\n", recvLen, recvBuffer);
#endif
            break;
        }

        free(recvBuffer);
      }

      if (screen == 0) {
        if (keyState == 1) {
          // initGraphics(1);
          newScreen = 1;
          esp_write("{\"command\":\"lobbyInit\"}");
        }
      } else if (screen == 1) {
        // while (graphicsUpdateLock)
        // ;
        if (keyState == 4)
          lobbyState.songId =
              (lobbyState.songId == 0) ? numSongs - 1 : lobbyState.songId - 1;
        else if (keyState == 8)
          lobbyState.songId = (lobbyState.songId + 1) % numSongs;
        lobbyState.mode = swState & 1;
        // lobbyState.p1Connected = (swState >> 1) & 1;
        // lobbyState.p2Connected = (swState >> 2) & 1;

        if (keyState == 1) {
          loadSong(song_filenames[lobbyState.songId]);
          loadPoses(song_filenames[lobbyState.songId]);
          // initGraphics(2);
          newScreen = 2;
        }
      } else if (screen == 2) {
        if (keyState == 1)
          // initGraphics(0);
          newScreen = 0;
        else if (keyState == 2) {
          gameState.pressedStart = 1;
        } else if (keyState == 4) {
          sprintf(sendBuffer, "{\"command\":\"captureAttacker\",\"poseID\":%u}",
                  poseID);
          esp_write(sendBuffer);
          poseID++;
          insertDefenderPose(poseID, 3.14 * poseID, 3.14);
        }
      }
    }

    // Playing a song
    if ((screen == 0) || (screen == 1) || (screen == 2 && gameState.oneDone)) {
      sampleNo =
          (sampleNo + writeAudio(samplesL[sampleNo], samplesR[sampleNo])) %
          numSamples;
    }

    // Countdown
    if (screen == 2 && !gameState.oneDone && gameState.pressedStart) {
      gameState.countdown += writeAudio(0, 0);
      if (gameState.countdown == AUDIO_RATE) {
        gameState.countdown = 0;
        if (!gameState.threeDone) {
          gameState.threeDone = 1;
        } else if (!gameState.twoDone) {
          gameState.twoDone = 1;
        } else if (!gameState.oneDone) {
          gameState.oneDone = 1;
        } else if (!gameState.goDone) {
          gameState.goDone = 1;
        }
      }
    } else if (screen == 2 && !gameState.goDone && gameState.oneDone) {
      gameState.countdown += 1;
      if (gameState.countdown == AUDIO_RATE) {
        gameState.countdown = 0;
        if (!gameState.goDone) {
          gameState.goDone = 1;
        }
      }
    }

    // else if (keyState == 4) initGraphics(2);
    // if (esp_connected) initGraphics(2);

    if (newScreen != screen) {
      initGraphics(newScreen);
    }
  }

  audio_close();
  video_close();
  SW_close();
  KEY_close();

  LEDR_close();
  HEX_close();

  regs_close(virtual_base);

  return 0;
}

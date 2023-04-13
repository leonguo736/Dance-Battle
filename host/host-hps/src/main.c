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
#define PIXELS_PER_SAMPLE 0.006875
#define POSE_LIFETIME 32000 // In samples
#define SAMPLES_PER_FRAME 800 // Recalculate if you change the audio rate or frame rate
#define CAPTURE_SIGNAL_OFFSET 12000 // In samples

// Global State

int screen = 0;  // 0 for init, 1 for lobby, 2 for game, 3 for results
int newScreen = 0;
// int switchScreenLock = 0;
// int graphicsUpdateLock = 0;
pthread_mutex_t graphicsLock;
int buffer = 0;  // 0 or 1

// Song

int numSongs = 3;
char* song_displaynames[] = {"tetris 99 theme", "ymca",
                             "super treadmill"};
double song_spbs[] = {AUDIO_RATE / (70.0 / 60), AUDIO_RATE / (127.0 / 60), AUDIO_RATE / (77.0 / 60)};
int song_offsets[] = {0, 0, 4000};
char* menu_filename = "shop.txt";
char* song_filenames[] = {"tetris.txt", "ymca.txt", "treadmill.txt"};

int numMainSamples;
int numGameSamples;
int numResultsSamples;
int sampleNo;

int samplesMainL[MAX_SAMPLES];
int samplesMainR[MAX_SAMPLES];
int samplesGameL[MAX_SAMPLES];
int samplesGameR[MAX_SAMPLES];
int samplesResultsL[MAX_SAMPLES];
int samplesResultsR[MAX_SAMPLES];

char* sampleLine = NULL;
size_t sampleLen = 0;
ssize_t sampleRead;

void loadSong(char* filename, int samplesL[], int samplesR[], int* numSamples) {
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
  while (count < MAX_SAMPLES && getline(&sampleLine, &sampleLen, fp) != -1) {
    if (left) {
      samplesL[count] = (int)(VOLUME * atof(sampleLine));
    } else {
      samplesR[count] = (int)(VOLUME * atof(sampleLine));
      count++;
    }
    left = !left;
  }

  *numSamples = count;

#ifdef DEBUG
  printf("Loaded Song %s | %d samples\n", path, numGameSamples);
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
int init_name_speeds[] = {2, 2, -2, -2};
int init_name_offsets[] = {0, 0, 40, 40};
short color_init_names[] = {0xe4a, 0xd19, 0xc641, 0x7859};
short game_flash_colors[] = {0x8033, 0x9035, 0xa838, 0xd8ff, 0xe35f, 0xf67f};

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
  gameState.score2 = 0;
  gameState.goodVibes = 0;
  gameState.goodVibes2 = 0;

  gameState.countdown = 0;
  gameState.flashTimer = 0;
  gameState.messageTimer = 0;
  gameState.messageTimer2 = 0;

  gameState.earlyPoseA = -1;
  gameState.earlyPoseD = -1;
  gameState.latePoseA = -1;
  gameState.latePoseD = -1;
  gameState.nextPoseSendA = 0;
  gameState.nextPoseSendD = 0;

  for (int i = 0; i < 2; i++) {
    struct GameScreenState state = {0,  0,  0,  0,  0, 0,  0, 0, 0, 0, 0, 0,
                                    -1, -1, -1, -1, {0}, {0}};
    gameState.gameScreenStates[i] = state;
  }
}

void resetResultsState(void) {
  resultsState.timer = 0;
  resultsState.winMessage = "...            ...";
  resultsState.winMessageComplete = "... winner: p2 ...";
  for (int i = 0; i < 2; i++) {
    struct ResultsScreenState state = {0, -40, 260, {0}};
    resultsState.resultsScreenStates[i] = state;
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
  char modeApath[50] = "modeAposes/";
  char modeBpath[50] = "modeBposes/";
  strcat((lobbyState.mode == 0) ? modeApath : modeBpath, filename);

  fp = fopen((lobbyState.mode == 0) ? modeApath : modeBpath, "r");
  if (fp == NULL) {
    printf("Pose file not found\n");
    exit(0);
  }

  char* delim = " ";
  int maxPoses = 0;

  while (getline(&poseLine, &poseLen, fp) != -1) {
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
          if (lobbyState.mode == 0) {
            defenderPoses[id].larmx = -14;
            defenderPoses[id].larmy = 14;
            defenderPoses[id].rarmx = 14;
            defenderPoses[id].rarmy = 14;
          }
          defenderPoses[id].llegx = -12;
          defenderPoses[id].llegy = 22;
          defenderPoses[id].rlegx = 12;
          defenderPoses[id].rlegy = 22;
          defenderPoses[id].sample = sample;
          defenderPoses[id].isDefender = 1;
        } else {
          attackerPoses[id].sample = sample;
        }
      } else if (token == 3 && lobbyState.mode == 1) {
        double larm = atof(ptr) * 3.14159 / 180;
        defenderPoses[id].larmx = POSE_ARM_LENGTH * cos(larm);
        defenderPoses[id].larmy = -POSE_ARM_LENGTH * sin(larm);
      } else if (token == 4 && lobbyState.mode == 1) {
        double rarm = atof(ptr) * 3.14159 / 180;
        defenderPoses[id].rarmx = POSE_ARM_LENGTH * cos(rarm);
        defenderPoses[id].rarmy = -POSE_ARM_LENGTH * sin(rarm);
      }
      token++;
      ptr = strtok(NULL, delim);
    }

    if (id > maxPoses) maxPoses = id;
  }

  numPoses = maxPoses + 1;

#ifdef DEBUG
  printf("Loaded Pose %s | %d samples\n", (lobbyState.mode == 0) ? modeApath : modeBpath, numPoses);
#endif

  fclose(fp);
}

void insertDefenderPose(int id, double larm, double rarm, double lleg,
                        double rleg) {
  printf("Inserting pose %d\n", id);

  struct Pose pose;
  pose.larmx = POSE_ARM_LENGTH * cos(larm);
  pose.rarmx = POSE_ARM_LENGTH * cos(rarm);
  pose.larmy = POSE_ARM_LENGTH * sin(larm);
  pose.rarmy = POSE_ARM_LENGTH * sin(rarm);
  if (lleg != 0) {
    pose.llegx = POSE_LEG_LENGTH * cos(lleg);
    pose.llegy = POSE_LEG_LENGTH * sin(lleg);
  }
  if (rleg != 0) {
    pose.rlegx = POSE_LEG_LENGTH * cos(rleg);
    pose.rlegy = POSE_LEG_LENGTH * sin(rleg);
  }
  pose.sample = defenderPoses[id].sample;
  pose.isDefender = 1;

  defenderPoses[id] = pose;
}

void addScore(int score, int player) {
  if (player == 0) { 
    gameState.goodVibes = (score > 50);

    gameState.score += score;
  } else if (player == 1) {
    gameState.goodVibes2 = (score > 50);

    gameState.score2 += score;
  }
}

int getPoseX(struct Pose p) {
  int x = (int)(GAME_VLINE_MARGIN +
                PIXELS_PER_SAMPLE *
                    (p.sample - sampleNo));
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
      drawStringCenter(basicFont, "key1", 40, LOBBY_SONG_Y + 10,
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

      short color = (lobbyState.p1Connected && lobbyState.p2Connected) ? COLOR_LOBBY_GO_ON : COLOR_LOBBY_GO_OFF;
      drawStringCenter(italicFont, "go!", 160, 200, color, 2, 2);
      drawStringCenter(arrowFont, "b b b    a a a", 160, 200, color, 2, 2);
      drawStringCenter(basicFont, "key0", 160, 220, (lobbyState.p1Connected && lobbyState.p2Connected) ? COLOR_LOBBY_MODE_BORDER : COLOR_BG, 1, 1);

    } else if (s == 2) {  // Game
      drawBackground();

      // Straight lines
      video_line(BORDER + 1, GAME_VLINE_MARGIN, WIDTH - BORDER - 1,
                 GAME_VLINE_MARGIN, COLOR_GAME_HLINE);
      video_line(BORDER + 1, HEIGHT - GAME_VLINE_MARGIN, WIDTH - BORDER - 1,
                 HEIGHT - GAME_VLINE_MARGIN, COLOR_GAME_HLINE);
      drawGameVLines();

      drawPBarOutline();

      drawStringCenter(basicFont, "press key0 to start", GAME_TOP_X, GAME_TOP_Y,
                       COLOR_GAME_PBAR_FILL, 1, 1);
    } else if (s == 3) { // Results
      resetResultsState();
      drawBackground();
      video_box(BORDER + 1, RESULTS_WAVEY - 22, WIDTH - BORDER - 1, RESULTS_WAVEY + 40, COLOR_INIT_TUBE);

      printf("A\n");
      if (lobbyState.mode == 0) {
        if (gameState.score < numPoses * 50) resultsState.winMessageComplete = "... winner: p2 ...";
      } else {
        if (gameState.score2 > gameState.score) resultsState.winMessageComplete = "... winner: p2 ...";
      }
      printf("B\n");

      // for (int x = BORDER + 1; x <= WIDTH - BORDER - 1; x++) {
      //   for (int y = BORDER + 1; y <= HEIGHT - BORDER - 1; y++) {
      //     drawResultsPixel(x, y, 0);
      //   }
      // }
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
  if (screen == 0) {  // Init
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
      drawStringCenter(basicFont, "waiting for server connection", 160, sy,
                 COLOR_BG, INIT_STATUS_SCALE, INIT_STATUS_KERNING);
      prevState->statusY =
          lerp(otherState->statusY, INIT_STATUS_Y, INIT_TITLE_LERP);
      sy = (int)round(prevState->statusY);
      drawStringCenter(basicFont, "waiting for server connection", 160, sy,
                 COLOR_INIT_STATUS, INIT_STATUS_SCALE, INIT_STATUS_KERNING);

      if (sy == INIT_STATUS_Y &&
          (int)round(otherState->statusY) == INIT_STATUS_Y) {
        initState.statusSlideDone = 1;
      }
    } else {
      if (esp_ready) {
        int sy = (int)round(prevState->statusY);
        drawStringCenter(basicFont, "waiting for server connection", 160, sy,
                 COLOR_BG, INIT_STATUS_SCALE, INIT_STATUS_KERNING);
        drawStringCenter(basicFont, "press key0", 160, sy,
                 COLOR_INIT_STATUS, INIT_STATUS_SCALE, INIT_STATUS_KERNING);
      }

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
  } else if (screen == 1) {  // Lobby
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
    if ((lobbyState.p1Connected && lobbyState.p2Connected) != (prevState->p1Connected && prevState->p2Connected)) {
      short color = (lobbyState.p1Connected && lobbyState.p2Connected) ? COLOR_LOBBY_GO_ON : COLOR_LOBBY_GO_OFF;
      drawStringCenter(italicFont, "go!", 160, 200, color, 2, 2);
      drawStringCenter(arrowFont, "b b b    a a a", 160, 200, color, 2, 2);
      drawStringCenter(basicFont, "key0", 160, 220, (lobbyState.p1Connected && lobbyState.p2Connected) ? COLOR_LOBBY_MODE_BORDER : COLOR_BG, 1, 1);
    }

  } else if (screen == 2) {  // Game
    struct GameScreenState* prevState = &(gameState.gameScreenStates[buffer]);
    struct GameScreenState* otherState = &(gameState.gameScreenStates[(buffer+1)%2]);

    // Update progress bar
    if (sampleNo <= numGameSamples) {
      int newPBarWidth = (int)(157 * (double)sampleNo / numGameSamples);
      drawPBarFill(prevState->pBarWidth, newPBarWidth);
      prevState->pBarWidth = newPBarWidth;
    }

    // Poses
    int prevEarlyA = prevState->earlyPoseA;
    int prevEarlyD = prevState->earlyPoseD;
    int prevLateA = prevState->latePoseA;
    int prevLateD = prevState->latePoseD;

    // Update top text
    if (prevState->pressedStart != gameState.pressedStart) {
      drawStringCenter(basicFont, "press key0 to start", GAME_TOP_X, GAME_TOP_Y,
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
      drawStringCenter(italicFont, "p2: 0", (lobbyState.mode == 0) ? GAME_TOP_X : (WIDTH - GAME_MODEB_TOP_X), GAME_TOP_Y,
                       COLOR_GAME_PBAR_FILL, 1, 1);
      if (lobbyState.mode == 1) {
        drawStringCenter(italicFont, "p1: 0", GAME_MODEB_TOP_X, GAME_TOP_Y, COLOR_GAME_PBAR_FILL, 1, 1);
      }
      prevState->goDone = gameState.goDone;
    } else if (prevState->score != gameState.score) {
      char oldScore[50];
      char newScore[50];
      sprintf(oldScore, "p2: %d", prevState->score);
      sprintf(newScore, "p2: %d", gameState.score);
      drawStringCenter(italicFont, oldScore, (lobbyState.mode == 0) ? GAME_TOP_X : (WIDTH - GAME_MODEB_TOP_X), GAME_TOP_Y, COLOR_BG,
                       1, 1);
      drawStringCenter(italicFont, newScore, (lobbyState.mode == 0) ? GAME_TOP_X : (WIDTH - GAME_MODEB_TOP_X), GAME_TOP_Y,
                       COLOR_GAME_PBAR_FILL, 1, 1);
      if (lobbyState.mode == 1) {
        char oldScore2[50];
        char newScore2[50];
        sprintf(oldScore2, "p1: %d", prevState->score2);
        sprintf(newScore2, "p1: %d", gameState.score2);
        drawStringCenter(italicFont, oldScore2, GAME_MODEB_TOP_X, GAME_TOP_Y, COLOR_BG, 1, 1);
        drawStringCenter(italicFont, newScore2, GAME_MODEB_TOP_X, GAME_TOP_Y, COLOR_GAME_PBAR_FILL, 1, 1);
        prevState->score2 = gameState.score2;
      }
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
    if (lobbyState.mode == 0) {
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
          gameState.flashTimer = 6 * GAME_FLASH_DURATION;
          
          gameState.earlyPoseA++;
        }
      }
      prevState->earlyPoseA = gameState.earlyPoseA;
      prevState->latePoseA = gameState.latePoseA;
    }

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
        gameState.flashTimer = 6 * GAME_FLASH_DURATION;
        gameState.messageTimer = 20;
        gameState.gameScreenStates[0].messageKerning = 10;
        gameState.gameScreenStates[1].messageKerning = 10;
        if (lobbyState.mode == 1) {
          gameState.messageTimer2 = 20;
          gameState.gameScreenStates[0].messageKerning2 = 10;
          gameState.gameScreenStates[1].messageKerning2 = 10;
        }

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

    // Sending signals
    if (lobbyState.mode == 0) {
      if (gameState.nextPoseSendA < numPoses && attackerPoses[gameState.nextPoseSendA].sample - sampleNo < CAPTURE_SIGNAL_OFFSET) {
        sprintf(sendBuffer, "{\"command\":\"captureAttacker\",\"poseID\":%u}",
                  gameState.nextPoseSendA);
        esp_write(sendBuffer);
        gameState.nextPoseSendA++;
        printf("%s\n", sendBuffer);
      }
    }
    if (gameState.nextPoseSendD < numPoses && defenderPoses[gameState.nextPoseSendD].sample - sampleNo < CAPTURE_SIGNAL_OFFSET) {
      sprintf(sendBuffer, "{\"command\":\"captureDefender\",\"poseID\":%u}",
                gameState.nextPoseSendD);
      esp_write(sendBuffer);
      gameState.nextPoseSendD++;
      printf("%s\n", sendBuffer);
    }

    // Border flash
    gameState.flashTimer = gameState.flashTimer == 0 ? 0 : gameState.flashTimer - 1;
    int borderColorIndex = (int)(gameState.flashTimer / 4);
    drawBorder(game_flash_colors[borderColorIndex]);

    // Messages
    gameState.messageTimer = gameState.messageTimer == 0 ? 0 : gameState.messageTimer - 1;
    double newMessageKerning = lerp(otherState->messageKerning, 1, 0.1);
    drawStringCenter(italicFont, prevState->goodVibes ? "sick!" : "yikes!", (lobbyState.mode == 0) ? 160 : (WIDTH - GAME_MODEB_TOP_X), GAME_TOP_Y + 12, COLOR_BG, 1, (int)(prevState->messageKerning));
    drawStringCenter(italicFont, gameState.goodVibes ? "sick!" : "yikes!", (lobbyState.mode == 0) ? 160 : (WIDTH - GAME_MODEB_TOP_X), GAME_TOP_Y + 12, gameState.messageTimer > 0 ? (gameState.goodVibes ? COLOR_GAME_NICE : COLOR_GAME_OUCH) : COLOR_BG, 1, (int)newMessageKerning);
    prevState->messageKerning = newMessageKerning;
    prevState->goodVibes = gameState.goodVibes;
    if (lobbyState.mode == 1) {
      gameState.messageTimer2 = gameState.messageTimer2 == 0 ? 0 : gameState.messageTimer2 - 1;
      double newMessageKerning2 = lerp(otherState->messageKerning2, 1, 0.1);
      drawStringCenter(italicFont, prevState->goodVibes2 ? "sick!" : "yikes!", GAME_MODEB_TOP_X, GAME_TOP_Y + 12, COLOR_BG, 1, (int)(prevState->messageKerning2));
      drawStringCenter(italicFont, gameState.goodVibes2 ? "sick!" : "yikes!", GAME_MODEB_TOP_X, GAME_TOP_Y + 12, gameState.messageTimer2 > 0 ? (gameState.goodVibes2 ? COLOR_GAME_NICE : COLOR_GAME_OUCH) : COLOR_BG, 1, (int)newMessageKerning2);
      prevState->messageKerning2 = newMessageKerning2;
      prevState->goodVibes2 = gameState.goodVibes2;
    }

  } else if (screen == 3) { // Results
    struct ResultsScreenState *prevState = &(resultsState.resultsScreenStates[buffer]);
    struct ResultsScreenState *otherState = &(resultsState.resultsScreenStates[(buffer+1)%2]);

    // Timers
    //int oldTimer = prevState->timer;
    int newTimer = sampleNo;
    prevState->timer = newTimer;


    // Draw diagonal lines

    // Update Gameover
    double newGameoverY = lerp(otherState->oldGameoverY, 80, 0.2);
    drawStringCenter(italicFont, "game over", 160, prevState->oldGameoverY, COLOR_BG, 4, 2);
    drawStringCenter(italicFont, "game over", 160, newGameoverY, COLOR_RESULTS_GAMEOVER, 4, 2);
    prevState->oldGameoverY = newGameoverY;

    // Update Continue
    if (newTimer >= 5 * AUDIO_RATE) {
      double newContinueY = lerp(otherState->oldContinueY, RESULTS_CONTINUE_ENDY, 0.2);
      drawStringCenter(basicFont, "press key0 to go again", 160, prevState->oldContinueY, COLOR_BG, 1, 1);
      drawStringCenter(basicFont, "press key0 to go again", 160, newContinueY, COLOR_RESULTS_CONTINUE, 1, 1);
      prevState->oldContinueY = newContinueY;
    }

    // Update Wave
    int newWinnerLength = (int)((newTimer - AUDIO_RATE * 1) / (AUDIO_RATE * 0.25));
    int x = 16;
    for (int c = 0; c < 18; c++) {
      drawChar(italicFont, resultsState.winMessageComplete[c], x, prevState->waveYs[c] + RESULTS_WAVEY, COLOR_INIT_TUBE, 2);
      int newY = (int)(RESULTS_WAVE_AMPLITUDE * sin((double)x / RESULTS_WAVE_WAVELENGTH_X - (double)newTimer / RESULTS_WAVE_WAVELENGTH_TIME));
      if (newWinnerLength + 4 > (c - 4) || c >= 14) {
        drawChar(italicFont, resultsState.winMessageComplete[c], x, newY + RESULTS_WAVEY, (c >= 4 && c < 14) ? COLOR_RESULTS_WAVE_WINNER : COLOR_RESULTS_WAVE_DOT, 2);
      }
      prevState->waveYs[c] = newY;
      x += 16;
    }
  }

  switchBuffer();
  pthread_mutex_unlock(&graphicsLock);
  // graphicsUpdateLock = 0;
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

  pthread_mutex_init(&graphicsLock, NULL);

  loadSong("shop.txt", samplesMainL, samplesMainR, &numMainSamples);
  loadSong("victory.txt", samplesResultsL, samplesResultsR, &numResultsSamples);

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
        double angles[4];

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
            sscanf(recvBuffer + 1, "%u %lf %lf %lf %lf", &recvID, angles,
                   angles + 1, angles + 2, angles + 3);

            printf("Pose %u: %lf %lf %lf %lf\n", recvID, angles[0], angles[1],
                   angles[2], angles[3]);
            printf("Delay: %d\n", sampleNo - attackerPoses[recvID].sample);

            if (recvID > gameState.latePoseD) {
              insertDefenderPose(recvID, angles[0], angles[1], angles[2],
                               angles[3]);
            }
            break;
          case ESP_SCORE_COMMAND:
            sscanf(recvBuffer + 1, "%u %u", &recvID, &score);

            printf("Score %u: %u\n", recvID, score);

            addScore(score, 0);
            break;
          case ESP_P1_SCORE_COMMAND:
            sscanf(recvBuffer + 1, "%u %u", &recvID, &score);

            printf("P1 Score %u: %u\n", recvID, score);

            addScore(score, 0);
            break;
          case ESP_P2_SCORE_COMMAND:
            sscanf(recvBuffer + 1, "%u %u", &recvID, &score);

            printf("P2 Score %u: %u\n", recvID, score);

            addScore(score, 1);
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
          sprintf(sendBuffer,
                  "{\"command\":\"setMode\",\"song\":\"%s\",\"type\":%d}",
                  song_filenames[lobbyState.songId],
                  lobbyState.mode);
          esp_write(sendBuffer);
          printf("%s\n", sendBuffer);

          loadSong(song_filenames[lobbyState.songId], samplesGameL, samplesGameR, &numGameSamples);
          loadPoses(song_filenames[lobbyState.songId]);
          // initGraphics(2);
          resetGameState();
          newScreen = 2;
        }
      } else if (screen == 2) {
        if (keyState == 4)
          // initGraphics(0);
          newScreen = 1;
        else if (keyState == 1) {
          gameState.pressedStart = 1;
        } else if (keyState == 2) {
          sprintf(sendBuffer, "{\"command\":\"captureAttacker\",\"poseID\":%u}",
                  poseID);
          esp_write(sendBuffer);
          poseID++;
          // insertDefenderPose(poseID, 3.14 * poseID, 3.14, 3.14 * poseID, 3.14);
        }
      } else if (screen == 3) {
        if (keyState == 1) {
          newScreen = 1;
        }
      }
    }

    // Playing samples
    if ((newScreen == 0) || (newScreen == 1)) {
      sampleNo = (sampleNo + writeAudio(samplesMainL[sampleNo], samplesMainR[sampleNo])) % numMainSamples;

    } else if (newScreen == 2 && gameState.oneDone) {

      // Stopping the song, switching screens
      if (sampleNo < numGameSamples) {
        sampleNo = (sampleNo + writeAudio(samplesGameL[sampleNo], samplesGameR[sampleNo]));
      } else {
        sampleNo = (sampleNo + writeAudio(0, 0));
        if (sampleNo - numGameSamples == AUDIO_RATE * 2) {
          newScreen = 3;
        }
      }

    } else if (newScreen == 3) {
      if (sampleNo < numResultsSamples) {
        sampleNo = (sampleNo + writeAudio(samplesResultsL[sampleNo], samplesResultsR[sampleNo]));
      } else {
        sampleNo = (sampleNo + writeAudio(0, 0));
      }
    }

    // Countdown
    if (newScreen == 2 && !gameState.oneDone && gameState.pressedStart) {
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
    } else if (newScreen == 2 && !gameState.goDone && gameState.oneDone) {
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
      if (!(newScreen == 1 && screen == 0)) sampleNo = 0;
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

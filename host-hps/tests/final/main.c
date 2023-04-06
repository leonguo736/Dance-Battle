#include <math.h>
#include <pthread.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#ifdef GCC
#include "HEX.h"
#include "KEY.h"
#include "LEDR.h"
#include "SW.H"
#include "audio.h"
#include "video.h"
#else
#include <intelfpgaup/KEY.h>
#include <intelfpgaup/SW.h>
#include <intelfpgaup/audio.h>
#include <intelfpgaup/video.h>
#include <intelfpgaup/HEX.h>
#include <intelfpgaup/LEDR.h>
#endif

#include "hps_0.h"

// Communications
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
#define SAMPLES_PER_FRAME 800 // Recalculate if you change the audio rate or frame rate

// Global State

int screen = 0;  // 0 for init, 1 for lobby, 2 for game, 3 for results
int switchScreenLock = 0;
int graphicsUpdateLock = 0;
int buffer = 0;  // 0 or 1

// Song

int numSongs = 3;
char * song_names[] = {"tetris 99 theme", "groovy gray", "super treadmill"};
int song_sampleNos[] = { 1000000, 0, 0 };
double song_spbs[] = { AUDIO_RATE / (140.0 / 60) };
int song_offsets[] = { 0 };
int song_numPoses[] = { 8 };
char * song_samplefiles[] = {
    "tetris32.txt", "groovy.txt", "treadmill.txt"
};

int samplesL[MAX_SAMPLES];
int samplesR[MAX_SAMPLES];

FILE* fp;
char* sampleLine = NULL;
size_t sampleLen = 0;
ssize_t sampleRead;

void loadSong(char * filename, int numSamples) {
    fp = fopen(filename, "r");
    if (fp == NULL) {
        printf("Song file not found\n");
        exit(0);
    }

    for (int i = 0; i < numSamples; i++) {
        samplesL[i] = (getline(&sampleLine, &sampleLen, fp) != -1)
                        ? (int)(VOLUME * atof(sampleLine))
                        : 0;
    }
    for (int i = 0; i < numSamples; i++) {
        samplesR[i] = (getline(&sampleLine, &sampleLen, fp) != -1)
                        ? (int)(VOLUME * atof(sampleLine))
                        : 0;
    }
}

int writeAudio(int l, int r) {
    int space = audio_check_write() ;

    if (space) {
        // printf("Channels are free\n");
        LEDR_set(0);
    } else {
        // printf("writeAudio: %x %d %d\n", space, left_space, right_space);
        // printf("Channels are full\n");
        LEDR_set(1);
        audio_wait_write();
        // return 0;
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

const struct Pose song_poses[][MAX_POSES] = {
    {
        {5, 0.1, 0.2},
        {7, 0.3, 0.4},
        {9, 0.5, 0.6},
        {11, 0.7, 0.8},
        {13, 0.9, 1.0},
        {17, 1.1, 1.2},
        {21, 1.3, 1.4},
        {23, 1.5, 1.6}
    }
};

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
    lobbyState.p1Connected = 0;
    lobbyState.p2Connected = 0;
    lobbyState.mode = 0;
    lobbyState.songId = 0;

    for (int i = 0; i < 2; i++) {
        struct LobbyScreenState state = {0, 0, 0, 0};
        lobbyState.lobbyScreenStates[i] = state;
    }
}

void resetGamescreenState(void) {
    gameState.sampleNo = 0;
    gameState.earlyPose = -1;
    gameState.latePose = -1;
    gameState.gameStarted = 0;

    for (int p = 0; p < song_numPoses[lobbyState.songId]; p++) {
        gameState.screenPoses[p] = convertPose(song_poses[lobbyState.songId][p]);
    }

    for (int i = 0; i < 2; i++) {
        struct GameScreenState state = {0, -1, -1, {0}};
        gameState.gameScreenStates[i] = state;
    }
}

struct ScreenPose convertPose(struct Pose pose) {
    struct ScreenPose sp = {
        (int)(pose.beat * song_spbs[lobbyState.songId] + song_offsets[lobbyState.songId]),
        -1,
        POSE_HOUR_LENGTH * cos(pose.hourAngle),
        POSE_HOUR_LENGTH * sin(pose.hourAngle),
        POSE_MINUTE_LENGTH * cos(pose.minuteAngle),
        POSE_MINUTE_LENGTH * sin(pose.minuteAngle)};

    return sp;
}

int getPoseX(struct ScreenPose sp) {
    int x = (int)(GAME_VLINE_MARGIN + PIXELS_PER_SAMPLE * (sp.sample - gameState.sampleNo + song_offsets[lobbyState.songId]));
    if (x >= GAME_VLINE_MARGIN && x <= WIDTH - GAME_VLINE_MARGIN) {
        return x;
    }
    return 0;
}

void switchBuffer(void) {
    video_show();
    buffer = !buffer;
}

void initGraphics(int s) {
  while (graphicsUpdateLock)
    ;
  switchScreenLock = 1;
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

    } else if (s == 2) {  // Game
      resetGamescreenState();
      drawBackground();

      // Straight lines
      video_line(BORDER + 1, GAME_VLINE_MARGIN, WIDTH - BORDER - 1,
                 GAME_VLINE_MARGIN, COLOR_GAME_HLINE);
      video_line(BORDER + 1, HEIGHT - GAME_VLINE_MARGIN, WIDTH - BORDER - 1,
                 HEIGHT - GAME_VLINE_MARGIN, COLOR_GAME_HLINE);
      drawGameVLines();

      drawPBarOutline();
    }
    switchBuffer();
  }
  switchScreenLock = 0;
}

double lerp(double current, double target, double factor) {
  return current + (target - current) * factor;
}

void updateGraphics(void) {
  graphicsUpdateLock = 1;
  if (screen == 0) {  // Init
    struct InitScreenState* prevState = &(initState.initScreenStates[buffer]);
    struct InitScreenState* otherState = &(initState.initScreenStates[(buffer + 1) % 2]);

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
  } else if (screen == 1) {  // Lobby
    struct LobbyScreenState* prevState = &(lobbyState.lobbyScreenStates[buffer]);
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
      drawStringCenter(basicFont, (char*)song_names[prevState->songId], 160,
                       160, COLOR_LOBBY_SONG_FILL, 1, 1);
      drawStringCenter(basicFont, (char*)song_names[lobbyState.songId], 160, 160,
                       COLOR_LOBBY_SONG_TEXT, 1, 1);
      prevState->songId = lobbyState.songId;
    }

  } else if (screen == 2) {  // Game
    struct GameScreenState* prevState = &(gameState.gameScreenStates[buffer]);
    // struct GameScreenState * otherState = &(gameScreenStates[(buffer+1)%2]);

    // Update progress bar
    int newPBarWidth = (int)(157 * (double)gameState.sampleNo / NUM_SAMPLES);
    drawPBarFill(prevState->pBarWidth, newPBarWidth);
    prevState->pBarWidth = newPBarWidth;

    // Poses
    int prevEarly = prevState->earlyPose;
    int prevLate = prevState->latePose;

    // Erase previous poses
    if (prevEarly != -1) {
      for (int p = prevEarly; p <= prevLate; p++) {
        int prevX = prevState->poseXs[p];
        drawPose(gameState.screenPoses[p], prevX, 1);
      }
    }

    // Advance pose pointers if necessary
    if (gameState.earlyPose == -1) {
      if (gameState.sampleNo >= gameState.screenPoses[0].sample - POSE_LIFETIME) {
        gameState.earlyPose = 0;
        gameState.latePose = 0;
      }
    } else {
      if (gameState.latePose < song_numPoses[lobbyState.songId] &&
          gameState.sampleNo >= gameState.screenPoses[gameState.latePose + 1].sample - POSE_LIFETIME) {
        gameState.latePose++;
      }
      if (gameState.earlyPose < song_numPoses[lobbyState.songId] &&
          gameState.sampleNo >= gameState.screenPoses[gameState.earlyPose].sample) {
        gameState.earlyPose++;
      }
    }
    prevState->earlyPose = gameState.earlyPose;
    prevState->latePose = gameState.latePose;

    // Draw new poses
    drawGameVLines();

    if (gameState.earlyPose != -1) {
      for (int p = gameState.earlyPose; p <= gameState.latePose; p++) {
        int newX = getPoseX(gameState.screenPoses[p]);
        prevState->poseXs[p] = newX;
        drawPose(gameState.screenPoses[p], newX, 0);
      }
    }
  }

  switchBuffer();
  graphicsUpdateLock = 0;
}

void* outputThread(void* vargp) {
  // Initialization

  audio_init();
  audio_rate(AUDIO_RATE);
  initGraphics(0);

  // Loop

  int s = 0;

  while (1) {
    if ((screen == 2) && gameState.gameStarted) {
      gameState.sampleNo = (gameState.sampleNo + writeAudio(samplesL[gameState.sampleNo], samplesR[gameState.sampleNo])) % song_sampleNos[lobbyState.songId];
    //   sampleNo++;
    } else {
      // writeAudio(0, 0);
    }
    s = (s + 1) % SAMPLES_PER_FRAME;
    if (s == 0) {
      while (switchScreenLock)
        ;
      // updateGraphics();
    }
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

  pthread_t outputThread_id;
  pthread_create(&outputThread_id, NULL, outputThread, NULL);

  // Comms
  audio_init_reg(virtual_base);
  uart_init(virtual_base);

  // if (!esp_init(argc, argv))
//   return 1;

  // pthread_t espThread_id;
  // pthread_create(&espThread_id, NULL, (void*) &esp_run, argv);

  int swState = 0;
  int keyState = 0;
  int s = 0;

  while (keyState != 8) {
    char* recvBuffer = esp_read(&recvLen);

    if (recvBuffer) {
      printf("[%d] %s\n", recvLen, recvBuffer);
      free(recvBuffer);
    }

    SW_read(&swState);
    KEY_read(&keyState);

    if (screen == 0) {
      if (keyState == 1) initGraphics(1);
    } else if (screen == 1) {
      while (graphicsUpdateLock)
        ;
      if (keyState == 4)
        lobbyState.songId = (lobbyState.songId == 0) ? numSongs - 1 : lobbyState.songId - 1;
      else if (keyState == 8)
        lobbyState.songId = (lobbyState.songId + 1) % numSongs;
      lobbyState.mode = swState & 1;
      lobbyState.p1Connected = (swState >> 1) & 1;
      lobbyState.p2Connected = (swState >> 2) & 1;

      if (keyState == 1) {
        loadSong(song_samplefiles[lobbyState.songId], song_sampleNos[lobbyState.songId]);
        initGraphics(2);
      }
    } else if (screen == 2) {
      if (keyState == 1)
        initGraphics(0);
      else if (keyState == 2)
        gameState.gameStarted = 1;
      else if (keyState == 4) {
        sprintf(sendBuffer, "{\"command\":\"captureAttacker\",\"poseID\":%u}", poseID);
        esp_write(sendBuffer);
      }
    }

    // if ((screen == 2) && gameStarted) {
    //   sampleNo +=
    //       (writeAudio(samplesL[sampleNo], samplesR[sampleNo]) % 1000000);
    //   //   sampleNo++;
    // } else {
    //   // writeAudio(0, 0);
    // }
    // s = (s + 1) % SAMPLES_PER_FRAME;
    // if (s == 0) {
    //   while (switchScreenLock)
    //     ;
      updateGraphics();
    // }

    // else if (keyState == 4) initGraphics(2);
    // if (esp_connected) initGraphics(2);
  }

  audio_close();
  video_close();
  SW_close();
  KEY_close();

  LEDR_close();
  HEX_close();

  fclose(fp);
  free(sampleLine);

  return 0;
}

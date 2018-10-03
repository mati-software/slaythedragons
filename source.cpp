/*
 *  Slay the Dragons - a simple 2D game played with two analog sticks/nubs
 *  Copyright (C) 2018  Timo Scheit
 *
 *  This program comes with ABSOLUTELY NO WARRANTY.
 *  This project is licensed under the MIT License - see the LICENSE.txt
 *  file for details.
 */



#ifdef _WIN32
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <direct.h>
#else
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <sys/stat.h>
#endif

#include<stdio.h>
#include<cstdlib>


#ifdef _WIN32
FILE * oeffneDatei(const char * const filename, const char * const mode) {
	FILE *stream;
	errno_t err;
	err = fopen_s(&stream, filename, mode);

	if (err == 0) {
		return stream;
	}

	if (stream) {
		err = fclose(stream);
	}
	return NULL;
}
int erstelleOrdner(const char *filename) {
	return _mkdir(filename);
}
#else
FILE * oeffneDatei(const char * const filename, const char * const mode) {
	return fopen(filename, mode);
}
int erstelleOrdner(const char * const filename) {
	return mkdir(filename, 777);
}
#endif


#define DRACHESPRITE_NUMBER_FRAMES 112
#define DRACHESPRITE_WIDTH 236
#define DRACHESPRITE_HEIGHT 94


static const int SCREEN_WIDTH = 1280;
static const int SCREEN_HEIGHT = 720;
static const int GAME_LENGTH = 99500;

static const int BONUSPUNKTE_DOUBLE = 128;
static const int BONUSPUNKTE_TRIPLE = 256;
static const int BONUSPUNKTE_QUADRUPLE = 512;
static const int BONUSPUNKTE_QUINTUPLE = 1024;
static const int BONUSPUNKTE_SEXTUPLE = 2048;



enum EnumState {
	STATE_HAUPTMENUE,
	STATE_UEBERGANG_HAUPTMENUE_GAME,
	STATE_GAME,
	STATE_GAMEOVER,
	STATE_BONUSTABELLE,
	STATE_OPTIONS,
	STATE_UEBERGANG_BONUSTABELLE_HAUPTMENUE,
	STATE_NAMENSEINGABE,
	STATE_BESTENLISTE,
	STATE_UEBERGANG_BONUSTABELLE_NAMENSEINGABE,
	STATE_UEBERGANG_BESTENLISTE_HAUPTMENUE,
	STATE_PAUSE,
	STATE_QUIT_TO_MAIN_MENU,
	STATE_UEBERGANG_GAME_HAUPTMENUE
};


enum EnumDigitalisierteYPositionVonAnalogstick {
	DIGITAL_Y_OBEN,
	DIGITAL_Y_ZENTRIERT,
	DIGITAL_Y_UNTEN
};
enum EnumDigitalisierteXPositionVonAnalogstick {
	DIGITAL_X_LINKS,
	DIGITAL_X_ZENTRIERT,
	DIGITAL_X_RECHTS
};

void hauptmenue_init();
void loopContent_hauptmenue(void);
int getBestenlisteBuchstabenbreite(const char buchstabe, const char folgeBuchstabe);
void maleMenueHintergrund(const Sint16 offsetY);
void schreibeBestenliste(const Sint16 offsetY);
void loopContent_game(void);
void loopContent_uebergangBonustabelleHauptmenue(void);
bool isDracheGetroffen(const int i);
int game_calculateBonuspunkte();
void incrementMultipleKillCount();
void game_maleDrachen(const Uint32 deltaTime);
void game_maleHud(const Uint32 deltaTime, const Sint16 offsetY);
void schreibeBonustabelleZahl(const Uint32 zahl, const Sint16 top, const Sint16 right, const bool hervorgehoben);
void loopContent_uebergangHauptmenueGame(void);
void loopContent_gameover(void);
void game_maleGameOver(const Sint16 offsetX);
void game_maleMastkorb();
void game_init();
void loopContent_bonustabelle(void);
void game_maleBonustabelle(const Sint16 offsetX, const Sint16 offsetY);
bool nichtTransparenterPixel(const int frameNr, const Sint16 x, const Sint16 y, const bool gespiegelt);
void loopContent_options(void);
void loopContent_bestenliste(void);
void maleGrosseBestenliste(const Sint16 offsetX, const Sint16 offsetY, const bool drawCursor, const bool highlighted);
void loopContent_namenseingabe(void);
void loopContent_uebergangBonustabelleNamenseingabe(void);
void loopContent_uebergangBestenlisteHauptmenue(void);
void loopContent_pause(void);
void loopContent_quitToMainMenu(void);
void loopContent_uebergangGameHauptmenue(void);
void malePyrat(const Sint16 offsetY);

void handleGlobalEvents(const SDL_Event* const e);
void toggleFullscreen();
void initGamepads();
void saveUserdata();
void playSound(Mix_Chunk* const sound);
SDL_Texture* ladeBilddateiAlsTexture(const char* str1);
void ladeBilddateiAlsTextureArray(SDL_Texture** const datensenke, const char* const bildpfad, const int anzahlEinzelbilder);

void signBestenlisteneintrag(const Uint32 punkte, const char* const name, char* const signatur);

void calculateAndSetNextSprechenStarttime();





SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;
SDL_Texture* windowTexture = NULL;

//Pictures
SDL_Texture* resource_texture_game_drache[112];
SDL_Texture* resource_texture_game_dracheExplode[15];
SDL_Texture* resource_texture_hauptmenue_hintergrund = NULL;
SDL_Texture* resource_texture_hauptmenue_texte = NULL;
SDL_Texture* resource_texture_options_hintergrund = NULL;
SDL_Texture* resource_texture_options_texte = NULL;
SDL_Texture* resource_texture_game_fadenkreuze = NULL;
SDL_Texture* resource_texture_game_hieb = NULL;
SDL_Texture* resource_texture_game_gameover = NULL;
SDL_Texture* resource_texture_game_bonustabelle = NULL;
SDL_Texture* resource_texture_hud = NULL;
SDL_Texture* resource_texture_bestenlisteBuchstaben = NULL;
SDL_Texture* resource_texture_alphabet = NULL;
SDL_Texture* resource_texture_tastaturersatz = NULL;
SDL_Texture* resource_texture_grosseBestenlisteUeberschrift = NULL;
SDL_Texture* resource_texture_fahne = NULL;
SDL_Texture* resource_texture_quit = NULL;
SDL_Texture* resource_texture_pyratKopf = NULL;
SDL_Texture* resource_texture_pyratTexte = NULL;
SDL_Texture* resource_texture_sprechblase = NULL;
SDL_Texture* resource_texture_pyratArm[30];
SDL_Texture* resource_texture_pyratAuge = NULL;
SDL_Texture* resource_texture_fehlermeldungNoGamepad = NULL;

//Music
Mix_Music *resource_music_hauptmenue = NULL;
Mix_Music *resource_music_game = NULL;
Mix_Music *resource_music_bonustabelle = NULL;

//Sound
Mix_Chunk *resource_sound_menueSelectionChange = NULL;
Mix_Chunk *resource_sound_hieb = NULL;
Mix_Chunk *resource_sound_explosionDrache = NULL;
Mix_Chunk *resource_sound_boom = NULL;
Mix_Chunk *resource_sound_menueConfirm = NULL;
Mix_Chunk *resource_sound_bonuspunkteStep = NULL;
Mix_Chunk *resource_sound_schieb = NULL;
Mix_Chunk *resource_sound_fehleingabe = NULL;
Mix_Chunk *resource_sound_pause = NULL;


bool pixelsTransparenzDrachen[DRACHESPRITE_NUMBER_FRAMES][DRACHESPRITE_HEIGHT][DRACHESPRITE_WIDTH];

bool soundEingeschaltet;
bool musikEingeschaltet;
bool vollbildEingeschaltet;

Uint32 aktuellerStatus_globalStarttime = 0;
Uint32 aktuellerStatus_pause_globalStarttime;
//Main loop flag
bool quit = false;
EnumState state = STATE_HAUPTMENUE;


Uint32 pyrat_sprechen_globalStarttime;
Uint32 pyrat_sprechen_anzahlLaute;
const char* pyrat_sprechen_laute;
int pyrat_sprechen_textindex;
Uint32 pyrat_sprechen_anzahlLaute2;
const char* pyrat_sprechen_laute2;
int pyrat_sprechen_textindex2;

Uint32 pyrat_armanimation_globalStarttime;




struct StructGamepad {
	SDL_GameController *gameController;
	EnumDigitalisierteYPositionVonAnalogstick analogstickLinksDigitaleYPosition;
	EnumDigitalisierteYPositionVonAnalogstick analogstickRechtsDigitaleYPosition;
	EnumDigitalisierteYPositionVonAnalogstick alteWerteAnalogstickLinksDigitaleYPosition;
	EnumDigitalisierteYPositionVonAnalogstick alteWerteAnalogstickRechtsDigitaleYPosition;
	EnumDigitalisierteXPositionVonAnalogstick analogstickLinksDigitaleXPosition;
	EnumDigitalisierteXPositionVonAnalogstick analogstickRechtsDigitaleXPosition;
	EnumDigitalisierteXPositionVonAnalogstick alteWerteAnalogstickLinksDigitaleXPosition;
	EnumDigitalisierteXPositionVonAnalogstick alteWerteAnalogstickRechtsDigitaleXPosition;
};
struct StructGamepad gamepads[10];
int anzahlGamepads;
int aktiverGameController_index;
SDL_GameController *aktiverGameController;


struct StructDrache {
	bool lebend;
	Uint32 localStartzeitpunkt;
	Uint32 localEndzeitpunkt;
	Uint32 localCreationtime;
	Uint32 localSlaytime;
	bool vonRechtsNachLinks;
	SDL_Rect rect;
	SDL_Rect slayRect;
	int animationStartFrame;
	int frameNr;
};
struct StructDrache game_drachen[6];

Uint32 game_hieb_localStarttime;
Uint32 game_hieb_localEndtime;
Sint16 game_hieb_start_x;
Sint16 game_hieb_start_y;
Sint16 game_hieb_ende_x;
Sint16 game_hieb_ende_y;
Sint16 game_hieb_posX;
Sint16 game_hieb_posY;
Sint16 game_hieb_posXAlt;
Sint16 game_hieb_posYAlt;
Uint16 game_hieb_killedDragons;
Uint32 game_points;
Uint32 game_pointsOhneBonuspunkte;
Uint32 game_bonuspunkte;
Uint16 game_anzahlDoubleKills;
Uint16 game_anzahlTripleKills;
Uint16 game_anzahlQuadrupleKills;
Uint16 game_anzahlQuintupleKills;
Uint16 game_anzahlSextupleKills;

int selectedMenuItem = 0;

struct StructBestenlisteneintrag {
	char name[20];
	Uint32 punkte;
	char signatur[22];
};
struct StructBestenlisteneintrag bestenlisteneintraege[5];
int namenseingabe_bestenlisteIndex;



void ladeBestenliste() {
	FILE *stream = oeffneDatei("userdata/ranking.dat", "rb");
	bool fehlerfreiGeladen = false;
	char vergleichsSignatur[22];

	if (stream) {
		fehlerfreiGeladen = true;
		//da durch "packing" Fuell-Bytes eingefuegt wuederen wenn ich die ganze Struct-Liste auf einen Schlag laden/speichern wuerde, lade ich lieber die Werte einzeln
		for (int i = 0; i < 5; i++) {
			fread(&bestenlisteneintraege[i].name, 1, 20, stream);
			fread(&bestenlisteneintraege[i].punkte, 4, 1, stream);
			fread(&bestenlisteneintraege[i].signatur, 1, 22, stream);

			signBestenlisteneintrag(bestenlisteneintraege[i].punkte, bestenlisteneintraege[i].name, vergleichsSignatur);

			for (int signaturByteIndex = 0; signaturByteIndex < 22; signaturByteIndex++) {
				if (bestenlisteneintraege[i].signatur[signaturByteIndex] != vergleichsSignatur[signaturByteIndex]) {
					fehlerfreiGeladen = false;
				}
			}
		}
	}



	if (!fehlerfreiGeladen) {
		bestenlisteneintraege[0].name[0] = '-';
		bestenlisteneintraege[0].name[1] = '-';
		bestenlisteneintraege[0].name[2] = '-';
		bestenlisteneintraege[0].name[3] = NULL;
		bestenlisteneintraege[0].punkte = 30000;
		bestenlisteneintraege[1].name[0] = '-';
		bestenlisteneintraege[1].name[1] = '-';
		bestenlisteneintraege[1].name[2] = '-';
		bestenlisteneintraege[1].name[3] = NULL;
		bestenlisteneintraege[1].punkte = 29000;
		bestenlisteneintraege[2].name[0] = '-';
		bestenlisteneintraege[2].name[1] = '-';
		bestenlisteneintraege[2].name[2] = '-';
		bestenlisteneintraege[2].name[3] = NULL;
		bestenlisteneintraege[2].punkte = 28000;
		bestenlisteneintraege[3].name[0] = '-';
		bestenlisteneintraege[3].name[1] = '-';
		bestenlisteneintraege[3].name[2] = '-';
		bestenlisteneintraege[3].name[3] = NULL;
		bestenlisteneintraege[3].punkte = 27000;
		bestenlisteneintraege[4].name[0] = '-';
		bestenlisteneintraege[4].name[1] = '-';
		bestenlisteneintraege[4].name[2] = '-';
		bestenlisteneintraege[4].name[3] = NULL;
		bestenlisteneintraege[4].punkte = 26000;
	}
	if (stream) {
		fclose(stream);
	}
}

int main(int argc, char* args[])
{
	FILE *stream = oeffneDatei("userdata/options.dat", "rb");
	if (stream) {
		fread(&soundEingeschaltet, 1, 1, stream);
		fread(&musikEingeschaltet, 1, 1, stream);
		fread(&vollbildEingeschaltet, 1, 1, stream);
	}
	else {
		soundEingeschaltet = true;
		musikEingeschaltet = true;
		vollbildEingeschaltet = true;
	}
	if (stream) {
		fclose(stream);
	}




	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER) < 0)
	{
		printf("%s", SDL_GetError());
		return -1;
	}

	int imgFlags = IMG_INIT_PNG;
	if (!(IMG_Init(imgFlags) & imgFlags)) {
		printf("%s\n", IMG_GetError());
	}

	//mit Chunk-Size von aktuell 2048 Bytes ggf. rumspielen
	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
	{
		printf("SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError());
	}

	initGamepads();


	srand(SDL_GetTicks());


	//Create window
	window = SDL_CreateWindow("Slay The Dragons", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, vollbildEingeschaltet ? SDL_WINDOW_FULLSCREEN : 0);

	if (window == NULL) {
		printf("%s", SDL_GetError());
	}
	else
	{
		renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
		
		SDL_ShowCursor(SDL_DISABLE);



		//TODO loeschen? SDL_FillRect(windowSurface, NULL, SDL_MapRGB(windowSurface->format, 0x00, 0xFF, 0xFF));


		ladeBilddateiAlsTextureArray(resource_texture_game_drache, "bilder/drache.png", 112);
		ladeBilddateiAlsTextureArray(resource_texture_game_dracheExplode, "bilder/drache-explode.png", 15);
		ladeBilddateiAlsTextureArray(resource_texture_pyratArm, "bilder/pyrat-arm.png", 30);
		resource_texture_hauptmenue_hintergrund = ladeBilddateiAlsTexture("bilder/menu.png");
		resource_texture_hauptmenue_texte = ladeBilddateiAlsTexture("bilder/menu-items.png");
		resource_texture_options_hintergrund = ladeBilddateiAlsTexture("bilder/options.png");
		resource_texture_options_texte = ladeBilddateiAlsTexture("bilder/options-items.png");
		resource_texture_game_fadenkreuze = ladeBilddateiAlsTexture("bilder/fadenkreuze.png");
		resource_texture_game_hieb = ladeBilddateiAlsTexture("bilder/hieb.png");
		resource_texture_game_gameover = ladeBilddateiAlsTexture("bilder/gameover.png");
		resource_texture_game_bonustabelle = ladeBilddateiAlsTexture("bilder/bonustabelle.png");
		resource_texture_hud = ladeBilddateiAlsTexture("bilder/hud.png");
		resource_texture_bestenlisteBuchstaben = ladeBilddateiAlsTexture("bilder/bestenliste-buchstaben.png");
		resource_texture_alphabet = ladeBilddateiAlsTexture("bilder/alphabet.png");
		resource_texture_tastaturersatz = ladeBilddateiAlsTexture("bilder/tastaturersatz.png");
		resource_texture_grosseBestenlisteUeberschrift = ladeBilddateiAlsTexture("bilder/bestenliste-ueberschrift.png");
		resource_texture_fahne = ladeBilddateiAlsTexture("bilder/fahne.png");
		resource_texture_quit = ladeBilddateiAlsTexture("bilder/quit.png");
		resource_texture_pyratKopf = ladeBilddateiAlsTexture("bilder/pyrat-kopf.png");
		resource_texture_pyratTexte = ladeBilddateiAlsTexture("bilder/pyrat-texte.png");
		resource_texture_sprechblase = ladeBilddateiAlsTexture("bilder/sprechblase.png");
		resource_texture_pyratAuge = ladeBilddateiAlsTexture("bilder/auge.png");
		resource_texture_fehlermeldungNoGamepad = ladeBilddateiAlsTexture("bilder/fehlermeldung-no_gamepad.png");

		resource_music_hauptmenue = Mix_LoadMUS("musik/slaythedragons-theme.ogg");
		resource_music_game = Mix_LoadMUS("musik/Swirl-kurz.ogg");
		resource_music_bonustabelle = Mix_LoadMUS("musik/bonustabelle.ogg");

		resource_sound_hieb = Mix_LoadWAV("sound/schwerthieb-stereo.ogg");
		resource_sound_menueSelectionChange = Mix_LoadWAV("sound/menue.ogg");
		resource_sound_explosionDrache = Mix_LoadWAV("sound/explosion-drache.ogg");
		resource_sound_boom = Mix_LoadWAV("sound/boom.ogg");
		resource_sound_menueConfirm = Mix_LoadWAV("sound/menue-confirm.ogg");
		resource_sound_bonuspunkteStep = Mix_LoadWAV("sound/bonuspunkte-step.ogg");
		resource_sound_schieb = Mix_LoadWAV("sound/schieb.ogg");
		resource_sound_fehleingabe = Mix_LoadWAV("sound/fehleingabe.ogg");
		resource_sound_pause = Mix_LoadWAV("sound/pause.ogg");


		ladeBestenliste();


		SDL_Surface* surfaceDrache = IMG_Load("bilder/drache.png");
		SDL_LockSurface(surfaceDrache);
		Uint32 *pixels = (Uint32 *)surfaceDrache->pixels;
		int pixelsIndex = 0;
		for (int frame = 0; frame < DRACHESPRITE_NUMBER_FRAMES; frame++) {
			for (int y = 0; y < DRACHESPRITE_HEIGHT; y++) {
				for (int x = 0; x < DRACHESPRITE_WIDTH; x++) {
					pixelsTransparenzDrachen[frame][y][x] = pixels[pixelsIndex] != pixels[0];
					pixelsIndex++;
				}
			}
		}
		SDL_FreeSurface(surfaceDrache);



		hauptmenue_init();

		//main loop, aka game loop
		while (!quit) {
			switch (state) {
			case STATE_HAUPTMENUE:
				loopContent_hauptmenue();
				break;
			case STATE_GAME:
				loopContent_game();
				break;
			case STATE_OPTIONS:
				loopContent_options();
				break;
			case STATE_UEBERGANG_HAUPTMENUE_GAME:
				loopContent_uebergangHauptmenueGame();
				break;
			case STATE_GAMEOVER:
				loopContent_gameover();
				break;
			case STATE_BONUSTABELLE:
				loopContent_bonustabelle();
				break;
			case STATE_UEBERGANG_BONUSTABELLE_HAUPTMENUE:
				loopContent_uebergangBonustabelleHauptmenue();
				break;
			case STATE_NAMENSEINGABE:
				loopContent_namenseingabe();
				break;
			case STATE_BESTENLISTE:
				loopContent_bestenliste();
				break;
			case STATE_UEBERGANG_BONUSTABELLE_NAMENSEINGABE:
				loopContent_uebergangBonustabelleNamenseingabe();
				break;
			case STATE_UEBERGANG_BESTENLISTE_HAUPTMENUE:
				loopContent_uebergangBestenlisteHauptmenue();
				break;
			case STATE_PAUSE:
				loopContent_pause();
				break;
			case STATE_QUIT_TO_MAIN_MENU:
				loopContent_quitToMainMenu();
				break;
			case STATE_UEBERGANG_GAME_HAUPTMENUE:
				loopContent_uebergangGameHauptmenue();
				break;
			}
		}
	}

	//Deallocate Resoucen
	for (int i = 0; i < 112; i++) {
		SDL_DestroyTexture(resource_texture_game_drache[i]);
	}
	for (int i = 0; i < 15; i++) {
		SDL_DestroyTexture(resource_texture_game_dracheExplode[i]);
	}
	for (int i = 0; i < 30; i++) {
		SDL_DestroyTexture(resource_texture_pyratArm[i]);
	}
	SDL_DestroyTexture(resource_texture_hauptmenue_hintergrund);
	SDL_DestroyTexture(resource_texture_hauptmenue_texte);
	SDL_DestroyTexture(resource_texture_options_hintergrund);
	SDL_DestroyTexture(resource_texture_options_texte);
	SDL_DestroyTexture(resource_texture_game_fadenkreuze);
	SDL_DestroyTexture(resource_texture_game_hieb);
	SDL_DestroyTexture(resource_texture_game_gameover);
	SDL_DestroyTexture(resource_texture_game_bonustabelle);
	SDL_DestroyTexture(resource_texture_hud);
	SDL_DestroyTexture(resource_texture_bestenlisteBuchstaben);
	SDL_DestroyTexture(resource_texture_alphabet);
	SDL_DestroyTexture(resource_texture_tastaturersatz);
	SDL_DestroyTexture(resource_texture_grosseBestenlisteUeberschrift);
	SDL_DestroyTexture(resource_texture_fahne);
	SDL_DestroyTexture(resource_texture_quit);
	SDL_DestroyTexture(resource_texture_pyratKopf);
	SDL_DestroyTexture(resource_texture_pyratTexte);
	SDL_DestroyTexture(resource_texture_sprechblase);
	SDL_DestroyTexture(resource_texture_pyratAuge);
	SDL_DestroyTexture(resource_texture_fehlermeldungNoGamepad);

	Mix_FreeMusic(resource_music_hauptmenue);
	Mix_FreeMusic(resource_music_game);
	Mix_FreeMusic(resource_music_bonustabelle);

	Mix_FreeChunk(resource_sound_menueSelectionChange);
	Mix_FreeChunk(resource_sound_hieb);
	Mix_FreeChunk(resource_sound_explosionDrache);
	Mix_FreeChunk(resource_sound_boom);
	Mix_FreeChunk(resource_sound_menueConfirm);
	Mix_FreeChunk(resource_sound_bonuspunkteStep);
	Mix_FreeChunk(resource_sound_schieb);
	Mix_FreeChunk(resource_sound_fehleingabe);
	Mix_FreeChunk(resource_sound_pause);

	for (int i = 0; i < anzahlGamepads; i++) {
		SDL_GameControllerClose(gamepads[i].gameController);
	}

	//Destroy window
	SDL_DestroyWindow(window);

	//Quit SDL subsystems
	IMG_Quit();
	SDL_Quit();

	saveUserdata();

	return 0;
}

void saveUserdata()
{
	//Verzeichnis erstellen, falls noch nicht vorhanden
	erstelleOrdner("userdata"); //TODO bei linux berechtigung mit angeben

	FILE *stream = oeffneDatei("userdata/options.dat", "wb");
	if (stream) {
		fwrite(&soundEingeschaltet, 1, 1, stream);
		fwrite(&musikEingeschaltet, 1, 1, stream);
		fwrite(&vollbildEingeschaltet, 1, 1, stream);
		fclose(stream);
	}
}

void saveBestenliste() {
	for (int i = 0; i < 5; i++) {
		signBestenlisteneintrag(bestenlisteneintraege[i].punkte, bestenlisteneintraege[i].name, bestenlisteneintraege[i].signatur);
	}

	//Verzeichnis erstellen, falls noch nicht vorhanden
	erstelleOrdner("userdata"); //TODO bei linux berechtigung mit angeben

	FILE *stream = oeffneDatei("userdata/ranking.dat", "wb");
	if (stream) {
		//da durch "packing" Fuell-Bytes eingefuegt wuederen wenn ich die ganze Struct-Liste auf einen Schlag laden/speichern wuerde, speichere ich lieber die Werte einzeln
		for (int i = 0; i < 5; i++) {
			fwrite(&bestenlisteneintraege[i].name, 1, 20, stream);
			fwrite(&bestenlisteneintraege[i].punkte, 4, 1, stream);
			fwrite(&bestenlisteneintraege[i].signatur, 1, 22, stream);
		}
		fclose(stream);
	}
}

void initGamepads()
{
	int anzahlJoysticks = SDL_NumJoysticks();
	anzahlGamepads = 0;
	for (int i = 0; i < anzahlJoysticks; i++) {
		if (SDL_IsGameController(i)) {
			gamepads[anzahlGamepads].gameController = SDL_GameControllerOpen(i);
			gamepads[anzahlGamepads].analogstickLinksDigitaleYPosition = DIGITAL_Y_ZENTRIERT;
			gamepads[anzahlGamepads].analogstickRechtsDigitaleYPosition = DIGITAL_Y_ZENTRIERT;
			gamepads[anzahlGamepads].alteWerteAnalogstickLinksDigitaleYPosition = DIGITAL_Y_ZENTRIERT;
			gamepads[anzahlGamepads].alteWerteAnalogstickRechtsDigitaleYPosition = DIGITAL_Y_ZENTRIERT;
			gamepads[anzahlGamepads].analogstickLinksDigitaleXPosition = DIGITAL_X_ZENTRIERT;
			gamepads[anzahlGamepads].analogstickRechtsDigitaleXPosition = DIGITAL_X_ZENTRIERT;
			gamepads[anzahlGamepads].alteWerteAnalogstickLinksDigitaleXPosition = DIGITAL_X_ZENTRIERT;
			gamepads[anzahlGamepads].alteWerteAnalogstickRechtsDigitaleXPosition = DIGITAL_X_ZENTRIERT;
			anzahlGamepads++;
			if (anzahlGamepads == 10) {
				break;
			}
		}
	}
}

void handleGlobalEvents(const SDL_Event* const e) {
	//X-Button vom Fenster
	if (e->type == SDL_QUIT) {
		quit = true;
	}
	else if (e->type == SDL_CONTROLLERDEVICEADDED || e->type == SDL_CONTROLLERDEVICEREMOVED) {
		for (int i = 0; i < anzahlGamepads; i++) {
			SDL_GameControllerClose(gamepads[i].gameController);
		}

		initGamepads();
	}
	else if ((e->type == SDL_KEYDOWN && e->key.keysym.sym == SDLK_F11)
		|| (e->type == SDL_KEYDOWN && e->key.keysym.sym == SDLK_RETURN && (e->key.keysym.mod & KMOD_ALT))) {
		toggleFullscreen();
	}
}

void toggleFullscreen()
{
	vollbildEingeschaltet = (SDL_GetWindowFlags(window) & SDL_WINDOW_FULLSCREEN) ? false : true;
	SDL_SetWindowFullscreen(window, vollbildEingeschaltet ? SDL_WINDOW_FULLSCREEN : 0);
}

void updateAnalogstickDigitaleYPositionen() {
	for (int i = 0; i < anzahlGamepads; i++) {
		gamepads[i].alteWerteAnalogstickLinksDigitaleYPosition = gamepads[i].analogstickLinksDigitaleYPosition;
		gamepads[i].alteWerteAnalogstickRechtsDigitaleYPosition = gamepads[i].analogstickRechtsDigitaleYPosition;

		Sint16 leftY = SDL_GameControllerGetAxis(gamepads[i].gameController, SDL_CONTROLLER_AXIS_LEFTY);
		if (leftY < -16000) {
			gamepads[i].analogstickLinksDigitaleYPosition = DIGITAL_Y_OBEN;
		}
		else if (leftY < -10000) {
			//Deadzone
			switch (gamepads[i].analogstickLinksDigitaleYPosition) {
			case DIGITAL_Y_OBEN:
			case DIGITAL_Y_ZENTRIERT:
				//nichts tun, alter Wert beibehalten
				break;
			case DIGITAL_Y_UNTEN:
				gamepads[i].analogstickLinksDigitaleYPosition = DIGITAL_Y_ZENTRIERT;
				break;
			}
		}
		else if (leftY > 16000) {
			gamepads[i].analogstickLinksDigitaleYPosition = DIGITAL_Y_UNTEN;
		}
		else if (leftY > 10000) {
			//Deadzone
			switch (gamepads[i].analogstickLinksDigitaleYPosition) {
			case DIGITAL_Y_OBEN:
				gamepads[i].analogstickLinksDigitaleYPosition = DIGITAL_Y_ZENTRIERT;
				break;
			case DIGITAL_Y_ZENTRIERT:
			case DIGITAL_Y_UNTEN:
				//nichts tun, alter Wert beibehalten
				break;
			}
		}
		else {
			gamepads[i].analogstickLinksDigitaleYPosition = DIGITAL_Y_ZENTRIERT;
		}

		Sint16 rightY = SDL_GameControllerGetAxis(gamepads[i].gameController, SDL_CONTROLLER_AXIS_RIGHTY);
		if (rightY < -16000) {
			gamepads[i].analogstickRechtsDigitaleYPosition = DIGITAL_Y_OBEN;
		}
		else if (rightY < -10000) {
			//Deadzone
			switch (gamepads[i].analogstickRechtsDigitaleYPosition) {
			case DIGITAL_Y_OBEN:
			case DIGITAL_Y_ZENTRIERT:
				//nichts tun, alter Wert beibehalten
				break;
			case DIGITAL_Y_UNTEN:
				gamepads[i].analogstickRechtsDigitaleYPosition = DIGITAL_Y_ZENTRIERT;
				break;
			}
		}
		else if (rightY > 16000) {
			gamepads[i].analogstickRechtsDigitaleYPosition = DIGITAL_Y_UNTEN;
		}
		else if (rightY > 10000) {
			//Deadzone
			switch (gamepads[i].analogstickRechtsDigitaleYPosition) {
			case DIGITAL_Y_OBEN:
				gamepads[i].analogstickRechtsDigitaleYPosition = DIGITAL_Y_ZENTRIERT;
				break;
			case DIGITAL_Y_ZENTRIERT:
			case DIGITAL_Y_UNTEN:
				//nichts tun, alter Wert beibehalten
				break;
			}
		}
		else {
			gamepads[i].analogstickRechtsDigitaleYPosition = DIGITAL_Y_ZENTRIERT;
		}
	}
}

void updateAnalogstickDigitaleXPositionen(int index) {
	gamepads[index].alteWerteAnalogstickLinksDigitaleXPosition = gamepads[index].analogstickLinksDigitaleXPosition;
	gamepads[index].alteWerteAnalogstickRechtsDigitaleXPosition = gamepads[index].analogstickRechtsDigitaleXPosition;

	Sint16 leftX = SDL_GameControllerGetAxis(gamepads[index].gameController, SDL_CONTROLLER_AXIS_LEFTX);
	if (leftX < -16000) {
		gamepads[index].analogstickLinksDigitaleXPosition = DIGITAL_X_LINKS;
	}
	else if (leftX < -10000) {
		//Deadzone
		switch (gamepads[index].analogstickLinksDigitaleXPosition) {
		case DIGITAL_X_LINKS:
		case DIGITAL_X_ZENTRIERT:
			//nichts tun, alter Wert beibehalten
			break;
		case DIGITAL_X_RECHTS:
			gamepads[index].analogstickLinksDigitaleXPosition = DIGITAL_X_ZENTRIERT;
			break;
		}
	}
	else if (leftX > 16000) {
		gamepads[index].analogstickLinksDigitaleXPosition = DIGITAL_X_RECHTS;
	}
	else if (leftX > 10000) {
		//Deadzone
		switch (gamepads[index].analogstickLinksDigitaleXPosition) {
		case DIGITAL_X_LINKS:
			gamepads[index].analogstickLinksDigitaleXPosition = DIGITAL_X_ZENTRIERT;
			break;
		case DIGITAL_X_ZENTRIERT:
		case DIGITAL_X_RECHTS:
			//nichts tun, alter Wert beibehalten
			break;
		}
	}
	else {
		gamepads[index].analogstickLinksDigitaleXPosition = DIGITAL_X_ZENTRIERT;
	}

	Sint16 rightX = SDL_GameControllerGetAxis(gamepads[index].gameController, SDL_CONTROLLER_AXIS_RIGHTX);
	if (rightX < -16000) {
		gamepads[index].analogstickRechtsDigitaleXPosition = DIGITAL_X_LINKS;
	}
	else if (rightX < -10000) {
		//Deadzone
		switch (gamepads[index].analogstickRechtsDigitaleXPosition) {
		case DIGITAL_X_LINKS:
		case DIGITAL_X_ZENTRIERT:
			//nichts tun, alter Wert beibehalten
			break;
		case DIGITAL_X_RECHTS:
			gamepads[index].analogstickRechtsDigitaleXPosition = DIGITAL_X_ZENTRIERT;
			break;
		}
	}
	else if (rightX > 16000) {
		gamepads[index].analogstickRechtsDigitaleXPosition = DIGITAL_X_RECHTS;
	}
	else if (rightX > 10000) {
		//Deadzone
		switch (gamepads[index].analogstickRechtsDigitaleXPosition) {
		case DIGITAL_X_LINKS:
			gamepads[index].analogstickRechtsDigitaleXPosition = DIGITAL_X_ZENTRIERT;
			break;
		case DIGITAL_X_ZENTRIERT:
		case DIGITAL_X_RECHTS:
			//nichts tun, alter Wert beibehalten
			break;
		}
	}
	else {
		gamepads[index].analogstickRechtsDigitaleXPosition = DIGITAL_X_ZENTRIERT;
	}
}

void hauptmenue_init() {
	state = STATE_HAUPTMENUE;
	aktuellerStatus_globalStarttime = SDL_GetTicks();
	if (musikEingeschaltet) {
		Mix_PlayMusic(resource_music_hauptmenue, -1);
	}
	selectedMenuItem = 0;

	pyrat_sprechen_globalStarttime = aktuellerStatus_globalStarttime + 2000;
	pyrat_sprechen_textindex = 2;
	pyrat_sprechen_laute = "AOI PIRAT .....DU IA ALREDI SII TEE DREGENS ...TEN TAKE IER KATLAS AND UP TO TEE KROWS NEST IA GO .....";
	pyrat_sprechen_anzahlLaute = 103;
	pyrat_sprechen_textindex2 = 10;
	pyrat_sprechen_laute2 = "IUS TEE ANALOG STIKS OR NABS TUU POSIKEN TEE START AND END OF IER KAT .....IE KEN KAT WIT TEE KOULDER BATENS .....                   ";
	pyrat_sprechen_anzahlLaute2 = 133;

	pyrat_armanimation_globalStarttime = 0;
}

void loopContent_hauptmenue() {
	//Event handler
	SDL_Event e;

	//event loop (Tastatur, Gamepad, Maus)
	while (SDL_PollEvent(&e) != 0) {
		handleGlobalEvents(&e);

		if (e.type == SDL_KEYDOWN) {
			switch (e.key.keysym.sym) {
			case SDLK_ESCAPE:
				quit = true;
				break;
			}
		}
		else if (e.type == SDL_CONTROLLERBUTTONDOWN) {
			switch (e.cbutton.button) {
			case SDL_CONTROLLER_BUTTON_DPAD_UP:
				selectedMenuItem = (selectedMenuItem + 2) % 3;
				playSound(resource_sound_menueSelectionChange);
				break;
			case SDL_CONTROLLER_BUTTON_DPAD_DOWN:
				selectedMenuItem = (selectedMenuItem + 1) % 3;
				playSound(resource_sound_menueSelectionChange);
				break;
			case SDL_CONTROLLER_BUTTON_START:
			case SDL_CONTROLLER_BUTTON_A:
			case SDL_CONTROLLER_BUTTON_B:
			case SDL_CONTROLLER_BUTTON_X:
			case SDL_CONTROLLER_BUTTON_Y:
				playSound(resource_sound_menueConfirm);
				switch (selectedMenuItem) {
				case 0: //Start slaying
					state = STATE_UEBERGANG_HAUPTMENUE_GAME;
					game_init();
					if (musikEingeschaltet) {
						Mix_HaltMusic();
						Mix_PlayMusic(resource_music_game, 1);
					}
					aktuellerStatus_globalStarttime = SDL_GetTicks();
					for (int i = 0; i < anzahlGamepads; i++) {
						if (e.cbutton.which == SDL_JoystickInstanceID(SDL_GameControllerGetJoystick(gamepads[i].gameController))) {
							aktiverGameController = gamepads[i].gameController;
							aktiverGameController_index = i;
							break;
						}
					}
					break;
				case 1: //Options
					state = STATE_OPTIONS;
					selectedMenuItem = 0;
					loopContent_options();
					return;
				case 2: //Exit
					quit = true;
					break;
				}
				break;
			}
		}
	}

	updateAnalogstickDigitaleYPositionen();
	for (int i = 0; i < anzahlGamepads; i++) {
		if (gamepads[i].analogstickLinksDigitaleYPosition == DIGITAL_Y_OBEN && gamepads[i].alteWerteAnalogstickLinksDigitaleYPosition != DIGITAL_Y_OBEN) {
			selectedMenuItem = (selectedMenuItem + 2) % 3;
			playSound(resource_sound_menueSelectionChange);
		}
		if (gamepads[i].analogstickRechtsDigitaleYPosition == DIGITAL_Y_OBEN && gamepads[i].alteWerteAnalogstickRechtsDigitaleYPosition != DIGITAL_Y_OBEN) {
			selectedMenuItem = (selectedMenuItem + 2) % 3;
			playSound(resource_sound_menueSelectionChange);
		}
		if (gamepads[i].analogstickLinksDigitaleYPosition == DIGITAL_Y_UNTEN && gamepads[i].alteWerteAnalogstickLinksDigitaleYPosition != DIGITAL_Y_UNTEN) {
			selectedMenuItem = (selectedMenuItem + 1) % 3;
			playSound(resource_sound_menueSelectionChange);
		}
		if (gamepads[i].analogstickRechtsDigitaleYPosition == DIGITAL_Y_UNTEN && gamepads[i].alteWerteAnalogstickRechtsDigitaleYPosition != DIGITAL_Y_UNTEN) {
			selectedMenuItem = (selectedMenuItem + 1) % 3;
			playSound(resource_sound_menueSelectionChange);
		}
	}

	Uint32 currentTime = SDL_GetTicks();
	Uint32 deltaTime = currentTime - aktuellerStatus_globalStarttime;

	maleMenueHintergrund(0);

	SDL_Rect menuSrcRect;
	SDL_Rect menuDestRect;

	menuSrcRect.x = deltaTime % 857 > 426 ? 0 : 360;
	menuSrcRect.w = 360;
	menuSrcRect.h = 64;
	menuDestRect.x = 460;
	menuDestRect.w = 360;
	menuDestRect.h = 64;
	switch (selectedMenuItem) {
	case 0:
		menuSrcRect.y = 0;
		menuDestRect.y = 328;
		break;
	case 1:
		menuSrcRect.y = 64;
		menuDestRect.y = 418;
		break;
	case 2:
		menuSrcRect.y = 128;
		menuDestRect.y = 508;
		break;
	}
	
	SDL_RenderCopy(renderer, resource_texture_hauptmenue_texte, &menuSrcRect, &menuDestRect);

	malePyrat(0);

	if (anzahlGamepads == 0 && currentTime % 1500 < 1000) {
		SDL_Rect destRect;
		destRect.x = 985;
		destRect.y = 660;
		destRect.w = 275;
		destRect.h = 40;
		SDL_RenderCopy(renderer, resource_texture_fehlermeldungNoGamepad, NULL,&destRect);
	}

	SDL_RenderPresent(renderer);

}

void malePyrat(const Sint16 offsetY) {
	Uint32 currentTime = SDL_GetTicks();

	int augeOffsetY = 0;

	//Sprech-Animation
	if (pyrat_sprechen_globalStarttime <= currentTime) {
		Uint32 sprechframeIndex = (currentTime - pyrat_sprechen_globalStarttime) / 133;
		if (sprechframeIndex < pyrat_sprechen_anzahlLaute + pyrat_sprechen_anzahlLaute2) {
			//Blase malen
			SDL_Rect blaseDestRect;
			blaseDestRect.x = 932;
			blaseDestRect.y = 249 + offsetY;
			blaseDestRect.w = 346;
			blaseDestRect.h = 228;
			SDL_RenderCopy(renderer, resource_texture_sprechblase, NULL, &blaseDestRect);

			//Text malen
			SDL_Rect textSrcRect;
			textSrcRect.x = 0;
			textSrcRect.y = 103 * (sprechframeIndex < pyrat_sprechen_anzahlLaute ? pyrat_sprechen_textindex : pyrat_sprechen_textindex2);
			textSrcRect.w = 307;
			textSrcRect.h = 103;
			SDL_Rect textDestRect;
			textDestRect.x = 951;
			textDestRect.y = 269 + offsetY;
			textDestRect.w = 307;
			textDestRect.h = 103;
			SDL_RenderCopy(renderer, resource_texture_pyratTexte, &textSrcRect, &textDestRect);

			//Mundebewegungen
			int srcRectIndex = -1;
			switch (sprechframeIndex < pyrat_sprechen_anzahlLaute ? pyrat_sprechen_laute[sprechframeIndex] : pyrat_sprechen_laute2[sprechframeIndex - pyrat_sprechen_anzahlLaute]) {
			case 'C':
			case 'S':
			case 'Y':
			case 'Z':
				srcRectIndex = 0;
				augeOffsetY = 2;
				break;
			case 'F':
			case 'V':
				srcRectIndex = 1;
				augeOffsetY = 2;
				break;
			case 'L':
			case 'N':
			case 'T':
			case 'D':
				srcRectIndex = 2;
				augeOffsetY = 1;
				break;
			case 'M':
			case 'B':
			case 'P':
				srcRectIndex = 3;
				augeOffsetY = 2;
				break;
			case 'O':
			case 'U':
				srcRectIndex = 4;
				break;
			case 'R':
			case 'G':
			case 'K':
				srcRectIndex = 5;
				augeOffsetY = 1;
				break;
			case 'W':
			case 'Q':
				srcRectIndex = 6;
				break;
			case 'A':
			case 'I':
				srcRectIndex = 7;
				break;
			case 'E':
				srcRectIndex = 8;
				augeOffsetY = 1;
				break;
			case '.':
				srcRectIndex = 9;
				augeOffsetY = 2;
				break;
			}

			if (srcRectIndex >= 0) {
				SDL_Rect srcRect;
				srcRect.x = 125 * srcRectIndex;
				srcRect.y = 0;
				srcRect.w = 125;
				srcRect.h = 152;
				SDL_Rect destRect;
				destRect.x = 1140;
				destRect.y = 378 + offsetY;
				destRect.w = 125;
				destRect.h = 152;
				SDL_RenderCopy(renderer, resource_texture_pyratKopf, &srcRect, &destRect);
			}
		}
	}

	//Arm-Animation
	Uint32 pyrat_sprechen_globalEndtime = pyrat_sprechen_globalStarttime + 133 * (pyrat_sprechen_anzahlLaute + pyrat_sprechen_anzahlLaute2);
	//Neuberechnungen der Armanimation finden immer erst statt, nachdem Sprechanimation abgelaufen ist
	if (currentTime >= pyrat_sprechen_globalEndtime) {
		if (pyrat_armanimation_globalStarttime <= pyrat_sprechen_globalStarttime) {
			//Sprechanimation ist gerade erst zuende gelaufen, noch 3 Sekunden warten
			pyrat_armanimation_globalStarttime = pyrat_sprechen_globalEndtime + 3000;
		}
		else if (pyrat_armanimation_globalStarttime + 8000 < currentTime) {
			//alle 8 Sekunden Animation neu starten
			pyrat_armanimation_globalStarttime = currentTime;
		}
	}
	//Animation malen, wenn pyrat_armanimation_starttime mit einem echten Wert befuellt ist und das Zeitfenster passt
	if (pyrat_armanimation_globalStarttime > 0 && pyrat_armanimation_globalStarttime <= currentTime) {
		int armFrameIndex = (currentTime - pyrat_armanimation_globalStarttime) / 33;
		if (armFrameIndex < 60) {
			SDL_Rect destRect;
			destRect.x = 998;
			destRect.y = 349 + offsetY;
			destRect.w = 160;
			destRect.h = 240;
			SDL_RenderCopy(renderer, resource_texture_pyratArm[armFrameIndex < 30 ? armFrameIndex : 59 - armFrameIndex], NULL, &destRect);
		}
	}


	int augeFrameIndex = (currentTime / 33) % 300;
	if (augeFrameIndex < 20) {
		SDL_Rect srcRect;
		srcRect.x = 21 * (augeFrameIndex < 10 ? augeFrameIndex : 19 - augeFrameIndex);
		srcRect.y = 0;
		srcRect.w = 21;
		srcRect.h = 11;
		SDL_Rect destRect;
		destRect.x = 1168;
		destRect.y = 450 + offsetY + augeOffsetY;
		destRect.w = 21;
		destRect.h = 11;
		SDL_RenderCopy(renderer, resource_texture_pyratAuge, &srcRect, &destRect);
	}
}

const Sint16 berechneDeltaYInBestenliste(const Sint16 top, const Sint16 deltaX) {
	const Sint16 steigungBeiTop310 = -21;
	const Sint16 steigungBeiTop504 = -34;

	//gewichtetes Mittel
	const Sint16 steigung = ((504 - top)  * steigungBeiTop310 + (top - 310) * steigungBeiTop504) / (504 - 310);

	return deltaX * steigung / 123;
}

void schreibeTextInBestenliste(const char* text, const Sint16 left, const Sint16 top, const Sint16 offsetY) {
	SDL_Rect srcRect;
	srcRect.y = 2;
	srcRect.w = 11;
	srcRect.h = 16;

	SDL_Rect destRect;
	destRect.w = 11;
	destRect.h = 16;

	Sint16 deltaX = 0;
	Sint16 deltaY = 0;

	char lastChar = 0;

	for (int i = 0; i < 20; i++) {
		if (text[i] == NULL) {
			break;
		}

		int charIndex = -1;

		if (text[i] == '-') {
			charIndex = 26;
		}
		else if (text[i] == '.') {
			charIndex = 27;
		}
		else if (text[i] >= 'A' && text[i] <= 'Z') {
			charIndex = text[i] - 'A';
		}

		if (lastChar) {
			deltaX += getBestenlisteBuchstabenbreite(lastChar, text[i]);
		}

		if (charIndex >= 0) {
			srcRect.x = 1 + charIndex * 12;

			deltaY = berechneDeltaYInBestenliste(top, deltaX);

			destRect.x = left + deltaX;
			destRect.y = top + deltaY + offsetY;

			SDL_RenderCopy(renderer, resource_texture_bestenlisteBuchstaben, &srcRect, &destRect);

		}

		lastChar = text[i];
	}
}

int getBestenlisteBuchstabenbreite(const char buchstabe, const char folgeBuchstabe) {
	switch (buchstabe) {
	case 'I':
	case '.':
		return 4;
	case 'L':
		if (folgeBuchstabe == 'T' || folgeBuchstabe == 'V' || folgeBuchstabe == 'W' || folgeBuchstabe == 'X' || folgeBuchstabe == 'Y' || folgeBuchstabe == 'Z') {
			return 7;
		}
		return 8;
	case 'F':
	case 'J':
		return 8;
	case 'E':
	case 'P':
	case 'T':
	case 'V':
	case 'Y':
		if (folgeBuchstabe == 'A' || folgeBuchstabe == 'J' || folgeBuchstabe == 'S' || folgeBuchstabe == '.') {
			return 8;
		}
		return 9;
	case 'S':
		if (folgeBuchstabe == 'T' || folgeBuchstabe == 'V' || folgeBuchstabe == 'W' || folgeBuchstabe == 'X' || folgeBuchstabe == 'Y' || folgeBuchstabe == 'Z') {
			return 8;
		}
		return 9;
	case 'B':
	case 'K':
	case 'R':
	case '-':
	case ' ':
		return 9;
	case 'C':
		if (folgeBuchstabe == 'A' || folgeBuchstabe == 'J' || folgeBuchstabe == 'S' || folgeBuchstabe == '.') {
			return 9;
		}
		return 10;
	case 'N':
	case 'U':
	case 'X':
		return 10;
	case 'A':
	case 'Q':
	case 'Z':
		if (folgeBuchstabe == 'T' || folgeBuchstabe == 'V' || folgeBuchstabe == 'W' || folgeBuchstabe == 'X' || folgeBuchstabe == 'Y' || folgeBuchstabe == 'Z') {
			return 10;
		}
		return 11;
	case 'D':
	case 'H':
		if (folgeBuchstabe == 'A' || folgeBuchstabe == 'J' || folgeBuchstabe == 'S' || folgeBuchstabe == '.') {
			return 10;
		}
		return 11;
	case 'G':
	case 'M':
	case 'O':
	case 'W':
		return 11;
	}
	return 0;
}

void schreibeZahlInBestenliste(const Uint32 zahl, const Sint16 left, const Sint16 breite, const Sint16 top, const Sint16 offsetY) {
	SDL_Rect zifferSrcRect;
	zifferSrcRect.y = 2;
	zifferSrcRect.w = 10;
	zifferSrcRect.h = 16;

	SDL_Rect zifferDestRect;
	zifferDestRect.w = 10;
	zifferDestRect.h = 16;

	Uint32 zuVerarbeitendeRestzahl = zahl;
	int zifferIndex = 0;
	while (zuVerarbeitendeRestzahl > 0 || zifferIndex == 0) {
		zifferIndex++;
		zifferSrcRect.x = (zuVerarbeitendeRestzahl % 10) * 11 + 339;

		int deltaX = breite - zifferIndex * 10;
		int deltaY = berechneDeltaYInBestenliste(top, deltaX);

		zifferDestRect.x = left + deltaX;
		zifferDestRect.y = top + deltaY + offsetY;

		SDL_RenderCopy(renderer, resource_texture_bestenlisteBuchstaben, &zifferSrcRect, &zifferDestRect);
		zuVerarbeitendeRestzahl = zuVerarbeitendeRestzahl / 10;
	}
}

void maleMenueHintergrund(const Sint16 offsetY)
{
	if (offsetY <= 582) {
		SDL_Rect backgroundRect;
		backgroundRect.x = 0;
		backgroundRect.y = 582 - offsetY;
		backgroundRect.w = 1280;
		backgroundRect.h = 720;
		SDL_RenderCopy(renderer, resource_texture_hauptmenue_hintergrund, &backgroundRect, NULL);
	}
	else {
		SDL_Rect srcRect;
		srcRect.x = 0;
		srcRect.y = 0;
		srcRect.w = 1280;
		srcRect.h = 720 + 582 - offsetY;
		SDL_Rect destRect;
		destRect.x = 0;
		destRect.y = offsetY - 582;
		destRect.w = 1280;
		destRect.h = srcRect.h;
		SDL_RenderCopy(renderer, resource_texture_hauptmenue_hintergrund, &srcRect, &destRect);
	}

	if (offsetY > 0) {
		int frameIndex = (SDL_GetTicks() / 33) % 60;
		SDL_Rect fahneSrcRect;
		fahneSrcRect.x = 0;
		fahneSrcRect.y = 62 * (frameIndex >= 30 ? 60 - frameIndex : frameIndex);
		fahneSrcRect.w = 121;
		fahneSrcRect.h = 62;
		SDL_Rect fahneDestRect;
		fahneDestRect.x = 92;
		fahneDestRect.y = 319 - 582 + offsetY;
		fahneDestRect.w = 121;
		fahneDestRect.h = 62;
		SDL_RenderCopy(renderer, resource_texture_fahne, &fahneSrcRect, &fahneDestRect);
	}

	schreibeBestenliste(offsetY);
}

void schreibeBestenliste(const Sint16 offsetY)
{
	schreibeTextInBestenliste(bestenlisteneintraege[0].name, 291, 364, offsetY);
	schreibeZahlInBestenliste(bestenlisteneintraege[0].punkte, 291, 123, 364, offsetY);
	schreibeTextInBestenliste(bestenlisteneintraege[1].name, 291, 391, offsetY);
	schreibeZahlInBestenliste(bestenlisteneintraege[1].punkte, 291, 123, 391, offsetY);
	schreibeTextInBestenliste(bestenlisteneintraege[2].name, 291, 418, offsetY);
	schreibeZahlInBestenliste(bestenlisteneintraege[2].punkte, 291, 123, 418, offsetY);
	schreibeTextInBestenliste(bestenlisteneintraege[3].name, 291, 445, offsetY);
	schreibeZahlInBestenliste(bestenlisteneintraege[3].punkte, 291, 123, 445, offsetY);
	schreibeTextInBestenliste(bestenlisteneintraege[4].name, 291, 472, offsetY);
	schreibeZahlInBestenliste(bestenlisteneintraege[4].punkte, 291, 123, 472, offsetY);
}

void versucheDracheZuErzeugen(Uint32 deltaTime) {
	for (int i = 0; i < 6; i++) {
		if (game_drachen[i].localEndzeitpunkt < deltaTime) {
			int y = rand() % 586;
			for (int i = 0; i < 6; i++) {
				if (game_drachen[i].lebend && game_drachen[i].localEndzeitpunkt >= deltaTime && abs(game_drachen[i].rect.y - y) < 94) {
					//Kollision, pech gehabt
					return;
				}
			}

			game_drachen[i].lebend = true;
			game_drachen[i].localStartzeitpunkt = deltaTime + rand() % 2000;
			game_drachen[i].localEndzeitpunkt = game_drachen[i].localStartzeitpunkt + 4000 + rand() % 1000;
			game_drachen[i].vonRechtsNachLinks = rand() % 2;
			game_drachen[i].rect.y = y;
			game_drachen[i].animationStartFrame = rand() % 112;
			game_drachen[i].localCreationtime = deltaTime;

			return;
		}
	}
}

int clipValue(int value, int mininmum, int maximum) {
	if (value < mininmum) {
		return mininmum;
	}
	else if (value > maximum) {
		return maximum;
	}
	return value;
}

void drawLine(Sint16 x1, Sint16 y1, Sint16 x2, Sint16 y2) {
	SDL_Rect srcRect;
	srcRect.x = 30;
	srcRect.y = 0;
	srcRect.w = 4;
	srcRect.h = 4;

	SDL_Rect destRect;
	destRect.w = 4;
	destRect.h = 4;



	Sint16 minX = x1 > x2 ? x2 : x1;
	Sint16 minY = y1 > y2 ? y2 : y1;
	Sint16 maxX = x1 > x2 ? x1 : x2;
	Sint16 maxY = y1 > y2 ? y1 : y2;
	Sint16 deltaX = maxX - minX;
	Sint16 deltaY = maxY - minY;
	if (deltaY > deltaX && deltaY > 6 + 15) {
		Sint16 deltaYNetto = deltaY - 6 - 15;
		deltaYNetto -= deltaYNetto % 8;
		Sint16 yLauf = minY + (deltaY - deltaYNetto) / 2;
		while (yLauf < maxY - (6 + 15) / 2) {
			destRect.x = (yLauf - y1) * (x2 - x1) / (y2 - y1) + x1 - 2;
			destRect.y = yLauf - 2;
			SDL_RenderCopy(renderer, resource_texture_game_fadenkreuze, &srcRect, &destRect);
			yLauf += 8;
		}
	}
	else if (deltaX > 6 + 15) {
		Sint16 deltaXNetto = deltaX - 6 - 15;
		deltaXNetto -= deltaXNetto % 8;
		Sint16 xLauf = minX + (deltaX - deltaXNetto) / 2;
		while (xLauf < maxX - (6 + 15) / 2) {
			destRect.y = (xLauf - x1) * (y2 - y1) / (x2 - x1) + y1 - 2;
			destRect.x = xLauf - 2;
			SDL_RenderCopy(renderer, resource_texture_game_fadenkreuze, &srcRect, &destRect);
			xLauf += 8;
		}
	}
}

void loopContent_game() {
	Uint32 currentTime = SDL_GetTicks();
	Uint32 deltaTime = currentTime - aktuellerStatus_globalStarttime;
	if (deltaTime > GAME_LENGTH) {
		//alle lebenden Drachen explodieren lassen
		for (int i = 0; i < 6; i++) {
			if (game_drachen[i].lebend) {
				if (game_drachen[i].localStartzeitpunkt <= deltaTime && game_drachen[i].localEndzeitpunkt >= deltaTime) {
					//aktuelle Position bestimmen
					int flugzeit = game_drachen[i].localEndzeitpunkt - game_drachen[i].localStartzeitpunkt;
					int bereitsGeflogen = game_drachen[i].vonRechtsNachLinks ? deltaTime - game_drachen[i].localStartzeitpunkt : game_drachen[i].localEndzeitpunkt - deltaTime;
					game_drachen[i].rect.x = bereitsGeflogen * (1280 + 236) / flugzeit - 236;
					//Explosion starten
					game_drachen[i].lebend = false;
					game_drachen[i].localSlaytime = deltaTime;
					game_drachen[i].localEndzeitpunkt = deltaTime + 33 * 15;
					game_drachen[i].slayRect.x = game_drachen[i].rect.x - 50;
					game_drachen[i].slayRect.y = game_drachen[i].rect.y - 50;
				}
				else {
					game_drachen[i].localStartzeitpunkt = 0;
					game_drachen[i].localEndzeitpunkt = 0;
				}
			}
		}
		incrementMultipleKillCount();
		game_pointsOhneBonuspunkte = game_points;
		game_bonuspunkte = game_calculateBonuspunkte();
		playSound(resource_sound_boom);
		aktuellerStatus_globalStarttime = currentTime;
		state = STATE_GAMEOVER;
		loopContent_gameover();
		return;
	}

	bool leftShoulderbuttonPressed = false;
	bool rightShoulderbuttonPressed = false;

	SDL_Event e;
	//event loop (Tastatur, Gamepad, Maus)
	while (SDL_PollEvent(&e) != 0) {
		handleGlobalEvents(&e);
		if (e.type == SDL_CONTROLLERBUTTONDOWN && e.cbutton.which == SDL_JoystickInstanceID(SDL_GameControllerGetJoystick(aktiverGameController))) {
			switch (e.cbutton.button) {
			case SDL_CONTROLLER_BUTTON_LEFTSHOULDER:
				leftShoulderbuttonPressed = true;
				break;
			case SDL_CONTROLLER_BUTTON_RIGHTSHOULDER:
				rightShoulderbuttonPressed = true;
				break;
			case SDL_CONTROLLER_BUTTON_START:
				state = STATE_PAUSE;
				aktuellerStatus_pause_globalStarttime = currentTime;
				Mix_PauseMusic();
				playSound(resource_sound_pause);
				loopContent_pause();
				return;
			case SDL_CONTROLLER_BUTTON_BACK:
				state = STATE_QUIT_TO_MAIN_MENU;
				aktuellerStatus_pause_globalStarttime = currentTime;
				Mix_PauseMusic();
				playSound(resource_sound_pause);
				selectedMenuItem = 0;
				loopContent_quitToMainMenu();
				return;
			}
		}
	}

	//Analogsticks auslesen: 32767 * wurzel(2)/2 / halbe Bildschirm-Dimension ergibt abgerundet 36 und 64
	Sint16 leftX = SDL_GameControllerGetAxis(aktiverGameController, SDL_CONTROLLER_AXIS_LEFTX);
	leftX = clipValue(leftX / 36 + 640, 0, 1280 - 1);
	Sint16 leftY = SDL_GameControllerGetAxis(aktiverGameController, SDL_CONTROLLER_AXIS_LEFTY);
	leftY = clipValue(leftY / 64 + 360, 0, 720 - 1);
	Sint16 rightX = SDL_GameControllerGetAxis(aktiverGameController, SDL_CONTROLLER_AXIS_RIGHTX);
	rightX = clipValue(rightX / 36 + 640, 0, 1280 - 1);
	Sint16 rightY = SDL_GameControllerGetAxis(aktiverGameController, SDL_CONTROLLER_AXIS_RIGHTY);
	rightY = clipValue(rightY / 64 + 360, 0, 720 - 1);






	//Berechnungen machen

	int anzahlErzeugterDrachenInDenLetztenZweiSekunden = 0;
	for (int i = 0; i < 6; i++) {
		if (game_drachen[i].localCreationtime + 2000 > deltaTime) {
			anzahlErzeugterDrachenInDenLetztenZweiSekunden++;
		}
	}

	//drachenpos aktualisieren
	for (int i = 0; i < 6; i++) {
		if (game_drachen[i].lebend) {
			if (game_drachen[i].localStartzeitpunkt <= deltaTime && game_drachen[i].localEndzeitpunkt >= deltaTime) {
				int flugzeit = game_drachen[i].localEndzeitpunkt - game_drachen[i].localStartzeitpunkt;
				int bereitsGeflogen = game_drachen[i].vonRechtsNachLinks ? deltaTime - game_drachen[i].localStartzeitpunkt : game_drachen[i].localEndzeitpunkt - deltaTime;
				game_drachen[i].rect.x = bereitsGeflogen * (1280 + 236) / flugzeit - 236;
			}
		}
	}

	//Hieb initialisieren
	if (leftShoulderbuttonPressed || rightShoulderbuttonPressed) {
		if (game_hieb_localEndtime < deltaTime) {
			playSound(resource_sound_hieb);

			Sint16 deltaX = leftX - rightX;
			Sint16 deltaY = leftY - rightY;
			Sint16 abstand = (Sint16)sqrt(deltaX * deltaX + deltaY * deltaY);
			game_hieb_localStarttime = deltaTime;
			game_hieb_localEndtime = deltaTime + 100 + abstand / 5;
			if (rightShoulderbuttonPressed) {
				game_hieb_start_x = rightX;
				game_hieb_start_y = rightY;
				game_hieb_ende_x = leftX;
				game_hieb_ende_y = leftY;
			}
			else {
				game_hieb_start_x = leftX;
				game_hieb_start_y = leftY;
				game_hieb_ende_x = rightX;
				game_hieb_ende_y = rightY;
			}
			game_hieb_posX = game_hieb_start_x;
			game_hieb_posXAlt = game_hieb_start_x;
			game_hieb_posY = game_hieb_start_y;
			game_hieb_posYAlt = game_hieb_start_y;
			//von vorherigem Hieb den Kill-Count abschliessen
			incrementMultipleKillCount();
			game_hieb_killedDragons = 0;
		}
	}

	Sint16 game_hieb_deltaX = 0;
	Sint16 game_hieb_deltaY = 0;
	if (game_hieb_localEndtime >= deltaTime && game_hieb_localEndtime > game_hieb_localStarttime) {
		game_hieb_deltaX = game_hieb_ende_x - game_hieb_start_x;
		game_hieb_deltaY = game_hieb_ende_y - game_hieb_start_y;
		Sint16 game_hieb_deltaTime = game_hieb_localEndtime - game_hieb_localStarttime;
		Sint16 game_hieb_vergangeneZeit = deltaTime - game_hieb_localStarttime;
		game_hieb_posXAlt = game_hieb_posX;
		game_hieb_posYAlt = game_hieb_posY;
		game_hieb_posX = game_hieb_deltaX * game_hieb_vergangeneZeit / game_hieb_deltaTime + game_hieb_start_x;
		game_hieb_posY = game_hieb_deltaY * game_hieb_vergangeneZeit / game_hieb_deltaTime + game_hieb_start_y;


		//nach getroffenen Drachen suchen
		for (int i = 0; i < 6; i++) {
			if (game_drachen[i].lebend
				&& game_drachen[i].localStartzeitpunkt < game_drachen[i].localEndzeitpunkt
				&& deltaTime >= game_drachen[i].localStartzeitpunkt
				&& deltaTime <= game_drachen[i].localEndzeitpunkt) {
				if (isDracheGetroffen(i)) {
					game_points += 256;
					game_hieb_killedDragons++;

					playSound(resource_sound_explosionDrache);
					game_drachen[i].lebend = false;
					game_drachen[i].localSlaytime = deltaTime;
					game_drachen[i].localEndzeitpunkt = deltaTime + 33 * 15;
					game_drachen[i].slayRect.x = game_drachen[i].rect.x - 50;
					game_drachen[i].slayRect.y = game_drachen[i].rect.y - 50;
				}
			}
		}

	}







	//Clear screen
	SDL_SetRenderDrawColor(renderer, 0x74, 0xA8, 0xFC, 255);
	SDL_RenderClear(renderer);

	game_maleMastkorb();

	game_maleDrachen(deltaTime);

	//Aktuelle Linie malen
	drawLine(leftX, leftY, rightX, rightY);

	//Hieb malen
	if (game_hieb_localEndtime >= deltaTime && game_hieb_localEndtime > game_hieb_localStarttime) {
		SDL_Rect hiebSrcRect;
		hiebSrcRect.y = 0;
		hiebSrcRect.w = 55;
		hiebSrcRect.h = 55;

		Sint16 game_hieb_deltaX_betrag = game_hieb_deltaX < 0 ? -game_hieb_deltaX : game_hieb_deltaX;
		Sint16 game_hieb_deltaY_betrag = game_hieb_deltaY < 0 ? -game_hieb_deltaY : game_hieb_deltaY;
		if (game_hieb_deltaX == 0) {
			hiebSrcRect.x = 0;
		}
		else if (game_hieb_deltaY == 0) {
			hiebSrcRect.x = 110;
		}
		else if (game_hieb_deltaX_betrag > game_hieb_deltaY_betrag && game_hieb_deltaX_betrag / game_hieb_deltaY_betrag >= 3) {
			hiebSrcRect.x = 110;
		}
		else if (game_hieb_deltaX_betrag < game_hieb_deltaY_betrag && game_hieb_deltaY_betrag / game_hieb_deltaX_betrag >= 3) {
			hiebSrcRect.x = 0;
		}
		else if ((game_hieb_deltaX > 0 && game_hieb_deltaY > 0) || (game_hieb_deltaX < 0 && game_hieb_deltaY < 0)) {
			hiebSrcRect.x = 165;
		}
		else if ((game_hieb_deltaX > 0 && game_hieb_deltaY < 0) || (game_hieb_deltaX < 0 && game_hieb_deltaY > 0)) {
			hiebSrcRect.x = 55;
		}

		Sint16 game_hieb_zurueckgelegteStrecke_x = game_hieb_posX - game_hieb_start_x;
		Sint16 game_hieb_zurueckgelegteStrecke_y = game_hieb_posY - game_hieb_start_y;
		Sint16 game_hieb_zurueckgelegteStrecke = (Sint16)sqrt(game_hieb_zurueckgelegteStrecke_x * game_hieb_zurueckgelegteStrecke_x + game_hieb_zurueckgelegteStrecke_y * game_hieb_zurueckgelegteStrecke_y);
		Sint16 game_hieb_uebrigeStrecke_x = game_hieb_ende_x - game_hieb_posX;
		Sint16 game_hieb_uebrigeStrecke_y = game_hieb_ende_y - game_hieb_posY;
		Sint16 game_hieb_uebrigeStrecke = (Sint16)sqrt(game_hieb_uebrigeStrecke_x * game_hieb_uebrigeStrecke_x + game_hieb_uebrigeStrecke_y * game_hieb_uebrigeStrecke_y);
		Sint16 game_hieb_abstandZuNaechstemStreckenEnde = game_hieb_uebrigeStrecke < game_hieb_zurueckgelegteStrecke ? game_hieb_uebrigeStrecke : game_hieb_zurueckgelegteStrecke;
		Sint16 game_hieb_radius = game_hieb_abstandZuNaechstemStreckenEnde < 50 ? game_hieb_abstandZuNaechstemStreckenEnde * 17 / 50 + 5 : 22;

		SDL_Rect hiebDestRest;
		hiebDestRest.x = game_hieb_posX - game_hieb_radius;
		hiebDestRest.y = game_hieb_posY - game_hieb_radius;
		hiebDestRest.w = game_hieb_radius * 2 + 1;
		hiebDestRest.h = game_hieb_radius * 2 + 1;
		SDL_RenderCopy(renderer, resource_texture_game_hieb, &hiebSrcRect, &hiebDestRest);
	}

	game_maleHud(deltaTime, 0);

	//Fadenkreuze malen
	SDL_Rect fadenkreuzLeftRect;
	fadenkreuzLeftRect.x = leftX - 7;
	fadenkreuzLeftRect.y = leftY - 7;
	fadenkreuzLeftRect.w = 15;
	fadenkreuzLeftRect.h = 15;
	SDL_Rect fadenkreuzLeftSrcRect;
	fadenkreuzLeftSrcRect.x = (deltaTime / 500 % 2) == 0 ? 0 : 15;
	fadenkreuzLeftSrcRect.y = 15;
	fadenkreuzLeftSrcRect.w = 15;
	fadenkreuzLeftSrcRect.h = 15;
	SDL_RenderCopy(renderer, resource_texture_game_fadenkreuze, &fadenkreuzLeftSrcRect, &fadenkreuzLeftRect);
	SDL_Rect fadenkreuzRightRect;
	fadenkreuzRightRect.x = rightX - 7;
	fadenkreuzRightRect.y = rightY - 7;
	fadenkreuzRightRect.w = 15;
	fadenkreuzRightRect.h = 15;
	SDL_Rect fadenkreuzRightSrcRect;
	fadenkreuzRightSrcRect.x = (deltaTime / 500 % 2) == 0 ? 0 : 15;
	fadenkreuzRightSrcRect.y = 0;
	fadenkreuzRightSrcRect.w = 15;
	fadenkreuzRightSrcRect.h = 15;
	SDL_RenderCopy(renderer, resource_texture_game_fadenkreuze, &fadenkreuzRightSrcRect, &fadenkreuzRightRect);

	SDL_RenderPresent(renderer);

	if (anzahlErzeugterDrachenInDenLetztenZweiSekunden < 2) {
		//versuchen drache zu erzeugen, bei kollision oder fehlendem freien slot einfach nix machen (wird dann beim naechsten frame noch mal probiert)
		versucheDracheZuErzeugen(deltaTime);
	}

}

bool isDracheGetroffen(const int i)
{
	Sint16 x1 = game_hieb_posX - game_drachen[i].rect.x;
	Sint16 x2 = game_hieb_posXAlt - game_drachen[i].rect.x;
	Sint16 y1 = game_hieb_posY - game_drachen[i].rect.y;
	Sint16 y2 = game_hieb_posYAlt - game_drachen[i].rect.y;
	Sint16 minX = x1 > x2 ? x2 : x1;
	Sint16 minY = y1 > y2 ? y2 : y1;
	Sint16 maxX = x1 > x2 ? x1 : x2;
	Sint16 maxY = y1 > y2 ? y1 : y2;
	Sint16 deltaX = maxX - minX;
	Sint16 deltaY = maxY - minY;
	if (deltaX == 0) {
		for (Sint16 yLauf = minY; yLauf <= maxY; yLauf++) {
			if (nichtTransparenterPixel(game_drachen[i].frameNr, x1, yLauf, game_drachen[i].vonRechtsNachLinks)) {
				return true;
			}
		}
	}
	else if (deltaY == 0) {
		for (Sint16 xLauf = minX; xLauf <= maxX; xLauf++) {
			if (nichtTransparenterPixel(game_drachen[i].frameNr, xLauf, y1, game_drachen[i].vonRechtsNachLinks)) {
				return true;
			}
		}
	}
	else if (deltaX > deltaY) {
		for (Sint16 xLauf = minX; xLauf <= maxX; xLauf++) {
			Sint16 yLauf = (xLauf - x1) * (y2 - y1) / (x2 - x1) + y1;
			if (nichtTransparenterPixel(game_drachen[i].frameNr, xLauf, yLauf, game_drachen[i].vonRechtsNachLinks)) {
				return true;
			}
		}
	}
	else {
		for (Sint16 yLauf = minY; yLauf <= maxY; yLauf++) {
			Sint16 xLauf = (yLauf - y1) * (x2 - x1) / (y2 - y1) + x1;
			if (nichtTransparenterPixel(game_drachen[i].frameNr, xLauf, yLauf, game_drachen[i].vonRechtsNachLinks)) {
				return true;
			}
		}
	}

	return false;
}

bool nichtTransparenterPixel(const int frameNr, const Sint16 x, const Sint16 y, const bool gespiegelt) {
	return x >= 0
		&& y >= 0
		&& x < DRACHESPRITE_WIDTH
		&& y < DRACHESPRITE_HEIGHT
		&& pixelsTransparenzDrachen[frameNr][y][gespiegelt ? DRACHESPRITE_WIDTH - 1 - x : x];
}

int game_calculateBonuspunkte()
{
	return game_anzahlDoubleKills * BONUSPUNKTE_DOUBLE
		+ game_anzahlTripleKills * BONUSPUNKTE_TRIPLE
		+ game_anzahlQuadrupleKills * BONUSPUNKTE_QUADRUPLE
		+ game_anzahlQuintupleKills * BONUSPUNKTE_QUINTUPLE
		+ game_anzahlSextupleKills * BONUSPUNKTE_SEXTUPLE;
}

void incrementMultipleKillCount()
{
	switch (game_hieb_killedDragons) {
	case 2:
		game_anzahlDoubleKills++;
		break;
	case 3:
		game_anzahlTripleKills++;
		break;
	case 4:
		game_anzahlQuadrupleKills++;
		break;
	case 5:
		game_anzahlQuintupleKills++;
		break;
	case 6:
		game_anzahlSextupleKills++;
		break;
	}
}

void game_maleDrachen(const Uint32 deltaTime)
{
	//Drachen malen
	for (int i = 0; i < 6; i++) {
		if (game_drachen[i].lebend) {
			if (game_drachen[i].localStartzeitpunkt <= deltaTime && game_drachen[i].localEndzeitpunkt >= deltaTime) {
				game_drachen[i].frameNr = ((deltaTime / 33 + game_drachen[i].animationStartFrame) % 112);
				SDL_RenderCopyEx(renderer, resource_texture_game_drache[game_drachen[i].frameNr], NULL, &game_drachen[i].rect, 0, NULL, game_drachen[i].vonRechtsNachLinks ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE);
			}
		}
		else {
			if (game_drachen[i].localSlaytime < game_drachen[i].localEndzeitpunkt && game_drachen[i].localEndzeitpunkt >= deltaTime) {
				int sterbeFrameNr = (deltaTime - game_drachen[i].localSlaytime) / 33;
				if (sterbeFrameNr < 15) {
					SDL_RenderCopyEx(renderer, resource_texture_game_dracheExplode[sterbeFrameNr], NULL, &game_drachen[i].slayRect, 0, NULL, game_drachen[i].vonRechtsNachLinks ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE);
				}
			}
		}
	}
}

void game_maleHud(const Uint32 deltaTime, const Sint16 offsetY)
{
	//Zeit umrandung malen
	SDL_Rect zeit_umrandung_srcRect;
	zeit_umrandung_srcRect.x = 0;
	zeit_umrandung_srcRect.y = 0;
	zeit_umrandung_srcRect.w = 48;
	zeit_umrandung_srcRect.h = 43;
	SDL_Rect zeit_umrandung_destRect;
	zeit_umrandung_destRect.x = SCREEN_WIDTH / 2 - 24;
	zeit_umrandung_destRect.y = 2 + offsetY;
	zeit_umrandung_destRect.w = 48;
	zeit_umrandung_destRect.h = 43;
	SDL_RenderCopy(renderer, resource_texture_hud, &zeit_umrandung_srcRect, &zeit_umrandung_destRect);

	//Zeit malen
	bool rot = false;
	if (deltaTime < GAME_LENGTH && deltaTime >= GAME_LENGTH - 10000) {
		if ((deltaTime / 260) % 2 == 0) {
			rot = true;
		}
	}
	int restzeit_in_sekunden = (GAME_LENGTH - deltaTime + 999) / 1000;
	if (restzeit_in_sekunden > 99) {
		restzeit_in_sekunden = 99;
	}
	int zeit_ziffer1 = restzeit_in_sekunden / 10;
	int zeit_ziffer2 = restzeit_in_sekunden % 10;
	SDL_Rect zeit_ziffer1_srcRect;
	zeit_ziffer1_srcRect.x = 0 + zeit_ziffer1 * 15;
	zeit_ziffer1_srcRect.y = rot ? 86 : 60;
	zeit_ziffer1_srcRect.w = 14;
	zeit_ziffer1_srcRect.h = 25;
	SDL_Rect zeit_ziffer2_srcRect;
	zeit_ziffer2_srcRect.x = 0 + zeit_ziffer2 * 15;
	zeit_ziffer2_srcRect.y = rot ? 86 : 60;
	zeit_ziffer2_srcRect.w = 14;
	zeit_ziffer2_srcRect.h = 25;
	SDL_Rect zeit_ziffer1_destRect;
	zeit_ziffer1_destRect.x = SCREEN_WIDTH / 2 - 14 - 1;
	zeit_ziffer1_destRect.y = 12 + offsetY;
	zeit_ziffer1_destRect.w = 14;
	zeit_ziffer1_destRect.h = 25;
	SDL_Rect zeit_ziffer2_destRect;
	zeit_ziffer2_destRect.x = SCREEN_WIDTH / 2 + 1;
	zeit_ziffer2_destRect.y = 12 + offsetY;
	zeit_ziffer2_destRect.w = 14;
	zeit_ziffer2_destRect.h = 25;
	SDL_RenderCopy(renderer, resource_texture_hud, &zeit_ziffer1_srcRect, &zeit_ziffer1_destRect);
	SDL_RenderCopy(renderer, resource_texture_hud, &zeit_ziffer2_srcRect, &zeit_ziffer2_destRect);

	//Points-Label malen
	SDL_Rect points_label_srcRect;
	points_label_srcRect.x = 0;
	points_label_srcRect.y = 44;
	points_label_srcRect.w = 59;
	points_label_srcRect.h = 15;
	SDL_Rect points_label_destRect;
	points_label_destRect.x = 4;
	points_label_destRect.y = 4 + offsetY;
	points_label_destRect.w = 59;
	points_label_destRect.h = 15;
	SDL_RenderCopy(renderer, resource_texture_hud, &points_label_srcRect, &points_label_destRect);

	//Points malen
	Uint32 zuVerarbeitendeRestzahl = game_points;
	SDL_Rect points_srcRect;
	points_srcRect.y = 44;
	points_srcRect.w = 10;
	points_srcRect.h = 15;
	SDL_Rect points_destRect;
	points_destRect.y = 4 + offsetY;
	points_destRect.w = 10;
	points_destRect.h = 15;
	for (int pointsZifferIndex = 0; pointsZifferIndex < 6; pointsZifferIndex++) {
		points_srcRect.x = 60 + (zuVerarbeitendeRestzahl % 10) * 11;
		points_destRect.x = 126 - pointsZifferIndex * 11;
		zuVerarbeitendeRestzahl = zuVerarbeitendeRestzahl / 10;
		SDL_RenderCopy(renderer, resource_texture_hud, &points_srcRect, &points_destRect);
	}
}

void game_init() {
	for (int i = 0; i < 6; i++) {
		game_drachen[i].lebend = false;
		game_drachen[i].localSlaytime = 0;
		game_drachen[i].localStartzeitpunkt = 0;
		game_drachen[i].localEndzeitpunkt = 0;
		game_drachen[i].rect.w = 236;
		game_drachen[i].rect.h = 94;
		game_drachen[i].slayRect.w = 336;
		game_drachen[i].slayRect.h = 194;
		game_drachen[i].localCreationtime = 0;
	}
	for (int i = 0; i < 6; i++) {
		versucheDracheZuErzeugen(i * 500);
	}

	game_hieb_localStarttime = 0;
	game_hieb_localEndtime = 0;
	game_hieb_ende_x = 0;
	game_hieb_ende_y = 0;
	game_hieb_start_x = 0;
	game_hieb_start_y = 0;
	game_hieb_killedDragons = 0;

	game_anzahlDoubleKills = 0;
	game_anzahlTripleKills = 0;
	game_anzahlQuadrupleKills = 0;
	game_anzahlQuintupleKills = 0;
	game_anzahlSextupleKills = 0;

	game_points = 0;
}

void loopContent_uebergangHauptmenueGame() {
	const int DAUER_WAIT_BEFORE_SCROLL = 461;
	const int DAUER_SCROLL_UP = 3230; //passend zur Musik, die in 130 BPM ist und ein 3,5 Takte langes Intro hat (3500 * 120 / 130)

	SDL_Event e;

	//event loop (Tastatur, Gamepad, Maus)
	while (SDL_PollEvent(&e) != 0) {
		handleGlobalEvents(&e);
	}

	Uint32 currentTime = SDL_GetTicks();
	Uint32 deltaTime = currentTime - aktuellerStatus_globalStarttime;

	//Clear screen
	SDL_SetRenderDrawColor(renderer, 0x74, 0xA8, 0xFC, 255);
	SDL_RenderClear(renderer);

	Sint16 menueHintergrundOffsetY;
	if (deltaTime <= DAUER_WAIT_BEFORE_SCROLL) {
		menueHintergrundOffsetY = 0;
	}
	else if (deltaTime < DAUER_SCROLL_UP + DAUER_WAIT_BEFORE_SCROLL) {
		menueHintergrundOffsetY = (582 + 720 - 40) * (deltaTime - DAUER_WAIT_BEFORE_SCROLL) / DAUER_SCROLL_UP;
	}
	else {
		menueHintergrundOffsetY = 680 + 582;
	}
	maleMenueHintergrund(menueHintergrundOffsetY);

	//FIXME durch den neuen status und die damit neue status-starttime wird die animation abgebrochen
	//TODO mach einfach die timestamps für die hauptmenue-animationen absolut, also auf gematime basierend statt auf deltatime (deltatime macht nur sinn bei uebergang-scrolls und bei pausierbarem gameplay)
	malePyrat(menueHintergrundOffsetY);

	game_maleHud(0, menueHintergrundOffsetY - 680 - 582);

	SDL_RenderPresent(renderer);

	if (deltaTime >= DAUER_WAIT_BEFORE_SCROLL + DAUER_SCROLL_UP) {
		state = STATE_GAME;
		aktuellerStatus_globalStarttime = SDL_GetTicks();
		//alle Animationen des Pyrat beenden
		pyrat_armanimation_globalStarttime = 0;
		pyrat_sprechen_globalStarttime = 0;
		pyrat_sprechen_anzahlLaute = 0;
		pyrat_sprechen_anzahlLaute2 = 0;
	}
}

SDL_Texture* ladeBilddateiAlsTexture(const char* str1) {
	SDL_Surface* surface = IMG_Load(str1);
	SDL_SetColorKey(surface, SDL_TRUE, SDL_MapRGB(surface->format, 0xFF, 0x00, 0xFF));
	SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
	SDL_FreeSurface(surface);

	return texture;
}

void ladeBilddateiAlsTextureArray(SDL_Texture** const datensenke, const char* const bildpfad, const int anzahlEinzelbilder) {
	SDL_Surface* surface = IMG_Load(bildpfad);
	if (surface->w > surface->h) {
		Sint16 breiteProBild = surface->w / anzahlEinzelbilder;
		SDL_Rect srcRect;
		srcRect.y = 0;
		srcRect.w = breiteProBild;
		srcRect.h = surface->h;
		for (int i = 0; i < anzahlEinzelbilder; i++) {
			SDL_Surface* einzelbildSurface = SDL_CreateRGBSurface(SDL_SWSURFACE, breiteProBild, surface->h, surface->format->BitsPerPixel, surface->format->Rmask, surface->format->Gmask, surface->format->Bmask, 0);
			srcRect.x = breiteProBild * i;
			SDL_BlitSurface(surface, &srcRect, einzelbildSurface, NULL);
			SDL_SetColorKey(einzelbildSurface, SDL_TRUE, SDL_MapRGB(einzelbildSurface->format, 0xFF, 0x00, 0xFF));
			datensenke[i] = SDL_CreateTextureFromSurface(renderer, einzelbildSurface);
			SDL_FreeSurface(einzelbildSurface);
		}
	}
	else {
		Sint16 hoeheProBild = surface->h / anzahlEinzelbilder;
		SDL_Rect srcRect;
		srcRect.x = 0;
		srcRect.w = surface->w;
		srcRect.h = hoeheProBild;
		for (int i = 0; i < anzahlEinzelbilder; i++) {
			SDL_Surface* einzelbildSurface = SDL_CreateRGBSurface(SDL_SWSURFACE, surface->w, hoeheProBild, surface->format->BitsPerPixel, surface->format->Rmask, surface->format->Gmask, surface->format->Bmask, 0);
			srcRect.y = hoeheProBild * i;
			SDL_BlitSurface(surface, &srcRect, einzelbildSurface, NULL);
			SDL_SetColorKey(einzelbildSurface, SDL_TRUE, SDL_MapRGB(einzelbildSurface->format, 0xFF, 0x00, 0xFF));
			datensenke[i] = SDL_CreateTextureFromSurface(renderer, einzelbildSurface);
			SDL_FreeSurface(einzelbildSurface);
		}
	}
	SDL_FreeSurface(surface);
}

void loopContent_gameover() {
	Uint32 currentTime = SDL_GetTicks();
	Uint32 deltaTime = currentTime - aktuellerStatus_globalStarttime;

	bool weiterGedrueckt = false;

	SDL_Event e;

	//event loop (Tastatur, Gamepad, Maus)
	while (SDL_PollEvent(&e) != 0) {
		handleGlobalEvents(&e);

		if (e.type == SDL_KEYDOWN) {
			switch (e.key.keysym.sym) {
			case SDLK_ESCAPE:
				weiterGedrueckt = true;
				break;
			}
		}
		else if (e.type == SDL_CONTROLLERBUTTONDOWN) {
			switch (e.cbutton.button) {
			case SDL_CONTROLLER_BUTTON_START:
			case SDL_CONTROLLER_BUTTON_A:
			case SDL_CONTROLLER_BUTTON_B:
			case SDL_CONTROLLER_BUTTON_X:
			case SDL_CONTROLLER_BUTTON_Y:
				weiterGedrueckt = true;
				break;
			}
		}
	}

	if (weiterGedrueckt) {
		aktuellerStatus_globalStarttime = currentTime;
		state = STATE_BONUSTABELLE;
		playSound(resource_sound_schieb);
		loopContent_bonustabelle();
		return;
	}

	//Clear screen
	SDL_SetRenderDrawColor(renderer, 0x74, 0xA8, 0xFC, 255);
	SDL_RenderClear(renderer);

	game_maleMastkorb();
	game_maleDrachen(deltaTime + GAME_LENGTH);
	game_maleHud(GAME_LENGTH, 0);

	game_maleGameOver(0);

	SDL_RenderPresent(renderer);
}

void game_maleGameOver(Sint16 offsetX)
{
	SDL_Rect gameoverDestRect;
	gameoverDestRect.x = 51 + offsetX;
	gameoverDestRect.y = 85;
	gameoverDestRect.w = 1178;
	gameoverDestRect.h = 550;
	SDL_RenderCopy(renderer, resource_texture_game_gameover, NULL, &gameoverDestRect);
}

void game_maleMastkorb()
{
	SDL_Rect backgroundSrcRect;
	backgroundSrcRect.x = 0;
	backgroundSrcRect.y = 0;
	backgroundSrcRect.w = 1280;
	backgroundSrcRect.h = 40;
	SDL_Rect backgroundDestRect;
	backgroundDestRect.x = 0;
	backgroundDestRect.y = 680;
	backgroundDestRect.w = 1280;
	backgroundDestRect.h = 40;
	SDL_RenderCopy(renderer, resource_texture_hauptmenue_hintergrund, &backgroundSrcRect, &backgroundDestRect);
}

bool inBestenlisteEintragen() {
	int einfuegeIndex = 5;
	while (einfuegeIndex > 0 && bestenlisteneintraege[einfuegeIndex - 1].punkte < game_points) {
		einfuegeIndex--;
		if (einfuegeIndex < 4) {
			bestenlisteneintraege[einfuegeIndex + 1] = bestenlisteneintraege[einfuegeIndex];
		}
	}
	if (einfuegeIndex < 5) {
		bestenlisteneintraege[einfuegeIndex].punkte = game_points;
		bestenlisteneintraege[einfuegeIndex].name[0] = NULL;
		namenseingabe_bestenlisteIndex = einfuegeIndex;
		return true;
	}
	return false;
}

void loopContent_bonustabelle() {
	Uint32 currentTime = SDL_GetTicks();
	Uint32 deltaTime = currentTime - aktuellerStatus_globalStarttime;

	bool weiterGedrueckt = false;

	SDL_Event e;

	//event loop (Tastatur, Gamepad, Maus)
	while (SDL_PollEvent(&e) != 0) {
		handleGlobalEvents(&e);

		if (e.type == SDL_KEYDOWN) {
			switch (e.key.keysym.sym) {
			case SDLK_ESCAPE:
				weiterGedrueckt = true;
				break;
			}
		}
		else if (e.type == SDL_CONTROLLERBUTTONDOWN) {
			switch (e.cbutton.button) {
			case SDL_CONTROLLER_BUTTON_START:
			case SDL_CONTROLLER_BUTTON_A:
			case SDL_CONTROLLER_BUTTON_B:
			case SDL_CONTROLLER_BUTTON_X:
			case SDL_CONTROLLER_BUTTON_Y:
				weiterGedrueckt = true;
				break;
			}
		}
	}

	//Clear screen
	SDL_SetRenderDrawColor(renderer, 0x74, 0xA8, 0xFC, 255);
	SDL_RenderClear(renderer);


	game_maleMastkorb();

	if (deltaTime < 1000) {
		//Bonustabelle reinschieben
		game_maleGameOver(0 - deltaTime * 1280 / 1000);
		game_maleBonustabelle(1280 - deltaTime * 1280 / 1000, 0);
	}
	else {
		if (game_points < game_pointsOhneBonuspunkte + game_bonuspunkte) {
			//Bonuspunkte dazuzaehlen
			if (game_points == game_pointsOhneBonuspunkte) {
				//Zaehlen faengt jetzt an, hintergrundsound abspielen
				if (soundEingeschaltet) {
					Mix_PlayMusic(resource_music_bonustabelle, -1);
				}
			}
			if (deltaTime < 1000 + game_bonuspunkte) {
				//ist noch nicht fertig mit Zaehlen
				game_points = game_pointsOhneBonuspunkte + deltaTime - 1000;
			}
			else {
				//gerade fertig geworden mit Zaehlen
				if (soundEingeschaltet) {
					Mix_HaltMusic();
				}
				game_points = game_pointsOhneBonuspunkte + game_bonuspunkte;
				playSound(resource_sound_boom);
			}
			if (game_anzahlDoubleKills > 0) {
				if (game_calculateBonuspunkte() - BONUSPUNKTE_DOUBLE + game_points >= game_pointsOhneBonuspunkte + game_bonuspunkte) {
					game_anzahlDoubleKills--;
					playSound(resource_sound_bonuspunkteStep);
				}
			}
			else if (game_anzahlTripleKills > 0) {
				if (game_calculateBonuspunkte() - BONUSPUNKTE_TRIPLE + game_points >= game_pointsOhneBonuspunkte + game_bonuspunkte) {
					game_anzahlTripleKills--;
					playSound(resource_sound_bonuspunkteStep);
				}
			}
			else if (game_anzahlQuadrupleKills > 0) {
				if (game_calculateBonuspunkte() - BONUSPUNKTE_QUADRUPLE + game_points >= game_pointsOhneBonuspunkte + game_bonuspunkte) {
					game_anzahlQuadrupleKills--;
					playSound(resource_sound_bonuspunkteStep);
				}
			}
			else if (game_anzahlQuintupleKills > 0) {
				if (game_calculateBonuspunkte() - BONUSPUNKTE_QUINTUPLE + game_points >= game_pointsOhneBonuspunkte + game_bonuspunkte) {
					game_anzahlQuintupleKills--;
					playSound(resource_sound_bonuspunkteStep);
				}
			}
			else if (game_anzahlSextupleKills > 0) {
				if (game_calculateBonuspunkte() - BONUSPUNKTE_SEXTUPLE + game_points >= game_pointsOhneBonuspunkte + game_bonuspunkte) {
					game_anzahlSextupleKills--;
					playSound(resource_sound_bonuspunkteStep);
				}
			}

		}
		game_maleBonustabelle(0, 0);
	}

	game_maleHud(GAME_LENGTH, 0);

	SDL_RenderPresent(renderer);

	if (weiterGedrueckt && deltaTime > 1000 + game_bonuspunkte) {
		aktuellerStatus_globalStarttime = currentTime;
		if (inBestenlisteEintragen()) {
			state = STATE_UEBERGANG_BONUSTABELLE_NAMENSEINGABE;
		}
		else {
			state = STATE_UEBERGANG_BONUSTABELLE_HAUPTMENUE;
		}
		playSound(resource_sound_schieb);
	}
}

void game_maleBonustabelle(const Sint16 offsetX, const Sint16 offsetY)
{
	SDL_Rect bonustabelleSrcRect;
	bonustabelleSrcRect.x = 0;
	bonustabelleSrcRect.y = 0;
	bonustabelleSrcRect.w = 886;
	bonustabelleSrcRect.h = 474;
	SDL_Rect bonustabelleDestRect;
	bonustabelleDestRect.x = (SCREEN_WIDTH - 886) / 2 + offsetX;
	bonustabelleDestRect.y = (SCREEN_HEIGHT - 474) / 2 + offsetY;
	bonustabelleDestRect.w = 886;
	bonustabelleDestRect.h = 474;
	SDL_RenderCopy(renderer, resource_texture_game_bonustabelle, &bonustabelleSrcRect, &bonustabelleDestRect);

	schreibeBonustabelleZahl(BONUSPUNKTE_DOUBLE, 1000 + offsetX, 190 + 55 * 0 + offsetY, false);
	schreibeBonustabelleZahl(BONUSPUNKTE_TRIPLE, 1000 + offsetX, 190 + 55 * 1 + offsetY, false);
	schreibeBonustabelleZahl(BONUSPUNKTE_QUADRUPLE, 1000 + offsetX, 190 + 55 * 2 + offsetY, false);
	schreibeBonustabelleZahl(BONUSPUNKTE_QUINTUPLE, 1000 + offsetX, 190 + 55 * 3 + offsetY, false);
	schreibeBonustabelleZahl(BONUSPUNKTE_SEXTUPLE, 1000 + offsetX, 190 + 55 * 4 + offsetY, false);

	schreibeBonustabelleZahl(game_anzahlDoubleKills, 790 + offsetX, 190 + 55 * 0 + offsetY, false);
	schreibeBonustabelleZahl(game_anzahlTripleKills, 790 + offsetX, 190 + 55 * 1 + offsetY, false);
	schreibeBonustabelleZahl(game_anzahlQuadrupleKills, 790 + offsetX, 190 + 55 * 2 + offsetY, false);
	schreibeBonustabelleZahl(game_anzahlQuintupleKills, 790 + offsetX, 190 + 55 * 3 + offsetY, false);
	schreibeBonustabelleZahl(game_anzahlSextupleKills, 790 + offsetX, 190 + 55 * 4 + offsetY, false);

	if (game_points == game_pointsOhneBonuspunkte + game_bonuspunkte) {
		schreibeBonustabelleZahl(game_points, 1000 + offsetX, 190 + 55 * 4 + 70 + offsetY, true);
	}
	else {
		schreibeBonustabelleZahl(game_points, 1000 + offsetX, 190 + 55 * 4 + 70 + offsetY, false);
	}
}

void schreibeBonustabelleZahl(const Uint32 zahl, const Sint16 right, const Sint16 top, const bool hervorgehoben) {
	SDL_Rect zifferSrcRect;
	zifferSrcRect.y = 478;
	zifferSrcRect.w = 23;
	zifferSrcRect.h = 37;

	SDL_Rect zifferDestRect;
	zifferDestRect.y = top;
	zifferDestRect.w = 23;
	zifferDestRect.h = 37;

	Uint32 zuVerarbeitendeRestzahl = zahl;
	int zifferIndex = 0;
	while (zuVerarbeitendeRestzahl > 0 || zifferIndex == 0) {
		zifferIndex++;
		if (hervorgehoben) {
			zifferSrcRect.x = (zuVerarbeitendeRestzahl % 10) * 23 + (SDL_GetTicks() % 500 < 250 ? 235 : 470);
		}
		else {
			zifferSrcRect.x = (zuVerarbeitendeRestzahl % 10) * 23;
		}
		zifferDestRect.x = right - zifferIndex * 24;

		SDL_RenderCopy(renderer, resource_texture_game_bonustabelle, &zifferSrcRect, &zifferDestRect);
		zuVerarbeitendeRestzahl = zuVerarbeitendeRestzahl / 10;
	}
}

void loopContent_uebergangBonustabelleHauptmenue() {
	Uint32 currentTime = SDL_GetTicks();
	Uint32 deltaTime = currentTime - aktuellerStatus_globalStarttime;

	if (deltaTime > 1000) {
		hauptmenue_init();
		if (game_points >= 30000) {
			pyrat_sprechen_textindex = 4;
			pyrat_sprechen_laute = "SORY LAD ....TET WASNT ENAF FOR TEE LIST .....        ";
			pyrat_sprechen_anzahlLaute = 54;
		}
		else {
			pyrat_sprechen_textindex = 5;
			pyrat_sprechen_laute = "OU MAI ....IE AR A LENDLUBER ..ARENT IE .....        ";
			pyrat_sprechen_anzahlLaute = 53;
		}
		pyrat_sprechen_globalStarttime = currentTime;
		pyrat_sprechen_anzahlLaute2 = 0;
		loopContent_hauptmenue();
		return;
	}

	SDL_Event e;

	//event loop (Tastatur, Gamepad, Maus)
	while (SDL_PollEvent(&e) != 0) {
		handleGlobalEvents(&e);
	}

	//Clear screen
	SDL_SetRenderDrawColor(renderer, 0x74, 0xA8, 0xFC, 255);
	SDL_RenderClear(renderer);

	Sint16 zurueckgelegterWeg = (582 + 720 - 40) * deltaTime / 1000;

	maleMenueHintergrund(680 + 582 - zurueckgelegterWeg);

	game_maleBonustabelle(0, -zurueckgelegterWeg);

	game_maleHud(GAME_LENGTH, -zurueckgelegterWeg);

	SDL_RenderPresent(renderer);

}

void loopContent_options() {
	Uint32 currentTime = SDL_GetTicks();
	Uint32 deltaTime = currentTime - aktuellerStatus_globalStarttime;

	//Event handler
	SDL_Event e;

	//event loop (Tastatur, Gamepad, Maus)
	while (SDL_PollEvent(&e) != 0) {
		handleGlobalEvents(&e);

		if (e.type == SDL_KEYDOWN) {
			switch (e.key.keysym.sym) {
			case SDLK_ESCAPE:
				state = STATE_HAUPTMENUE;
				selectedMenuItem = 0;
				loopContent_hauptmenue();
				return;
			}
		}
		else if (e.type == SDL_CONTROLLERBUTTONDOWN) {
			switch (e.cbutton.button) {
			case SDL_CONTROLLER_BUTTON_DPAD_UP:
				selectedMenuItem = (selectedMenuItem + 3) % 4;
				playSound(resource_sound_menueSelectionChange);
				break;
			case SDL_CONTROLLER_BUTTON_DPAD_DOWN:
				selectedMenuItem = (selectedMenuItem + 1) % 4;
				playSound(resource_sound_menueSelectionChange);
				break;
			case SDL_CONTROLLER_BUTTON_START:
			case SDL_CONTROLLER_BUTTON_A:
			case SDL_CONTROLLER_BUTTON_B:
			case SDL_CONTROLLER_BUTTON_X:
			case SDL_CONTROLLER_BUTTON_Y:
				switch (selectedMenuItem) {
				case 0:
					soundEingeschaltet = !soundEingeschaltet;
					playSound(resource_sound_menueConfirm); //in diesem Fall Sound erst nach der Aenderung spielen
					if (soundEingeschaltet) {
						pyrat_sprechen_textindex = 1;
						pyrat_sprechen_laute = "AII ..LETS HIR IT WEN TOUS FLAIN RATS DAI ....      ";
						pyrat_sprechen_anzahlLaute = 52;
					}
					else {
						pyrat_sprechen_textindex = 6;
						pyrat_sprechen_laute = "NOU SOUND .....DOUNT IE WANT TO HIR TOUS RAPSKILIENS DAI ....      ";
						pyrat_sprechen_anzahlLaute = 67;
					}
					calculateAndSetNextSprechenStarttime();
					pyrat_sprechen_anzahlLaute2 = 0;
					break;
				case 1:
					playSound(resource_sound_menueConfirm);
					musikEingeschaltet = !musikEingeschaltet;
					if (musikEingeschaltet) {
						Mix_PlayMusic(resource_music_hauptmenue, -1);
						pyrat_sprechen_textindex = 8;
						pyrat_sprechen_laute = "IE LAIK TAD FENSI MIUSIK ....      ";
						pyrat_sprechen_anzahlLaute = 35;
					}
					else {
						Mix_HaltMusic();
						pyrat_sprechen_textindex = 0;
						pyrat_sprechen_laute = "AII ..TURN TOUS BLODI KAANTIIS OF ....      ";
						pyrat_sprechen_anzahlLaute = 44;
					}
					calculateAndSetNextSprechenStarttime();
					pyrat_sprechen_anzahlLaute2 = 0;
					break;
				case 2: //Fullscreen
					playSound(resource_sound_menueConfirm);
					toggleFullscreen();
					if (SDL_GetWindowFlags(window) & SDL_WINDOW_FULLSCREEN) {
						pyrat_sprechen_textindex = 3;
						pyrat_sprechen_laute = "...TATS BETER ....NOTING ELS MATERS ....         ";
						pyrat_sprechen_anzahlLaute = 49;
					}
					else {
						pyrat_sprechen_textindex = 9;
						pyrat_sprechen_laute = "...WOOA ..EVERITIN IS MUVING ....         ";
						pyrat_sprechen_anzahlLaute = 42;
					}
					calculateAndSetNextSprechenStarttime();
					pyrat_sprechen_anzahlLaute2 = 0;
					break;
				case 3:
					playSound(resource_sound_menueConfirm);
					state = STATE_HAUPTMENUE;
					selectedMenuItem = 0;
					loopContent_hauptmenue();
					return;
				}
				break;
			}
		}
	}

	updateAnalogstickDigitaleYPositionen();
	for (int i = 0; i < anzahlGamepads; i++) {
		if (gamepads[i].analogstickLinksDigitaleYPosition == DIGITAL_Y_OBEN && gamepads[i].alteWerteAnalogstickLinksDigitaleYPosition != DIGITAL_Y_OBEN) {
			selectedMenuItem = (selectedMenuItem + 3) % 4;
			playSound(resource_sound_menueSelectionChange);
		}
		if (gamepads[i].analogstickRechtsDigitaleYPosition == DIGITAL_Y_OBEN && gamepads[i].alteWerteAnalogstickRechtsDigitaleYPosition != DIGITAL_Y_OBEN) {
			selectedMenuItem = (selectedMenuItem + 3) % 4;
			playSound(resource_sound_menueSelectionChange);
		}
		if (gamepads[i].analogstickLinksDigitaleYPosition == DIGITAL_Y_UNTEN && gamepads[i].alteWerteAnalogstickLinksDigitaleYPosition != DIGITAL_Y_UNTEN) {
			selectedMenuItem = (selectedMenuItem + 1) % 4;
			playSound(resource_sound_menueSelectionChange);
		}
		if (gamepads[i].analogstickRechtsDigitaleYPosition == DIGITAL_Y_UNTEN && gamepads[i].alteWerteAnalogstickRechtsDigitaleYPosition != DIGITAL_Y_UNTEN) {
			selectedMenuItem = (selectedMenuItem + 1) % 4;
			playSound(resource_sound_menueSelectionChange);
		}
	}

	SDL_Rect backgroundRect;
	backgroundRect.x = 0;
	backgroundRect.y = 582;
	backgroundRect.w = 1280;
	backgroundRect.h = 720;
	SDL_RenderCopy(renderer, resource_texture_hauptmenue_hintergrund, &backgroundRect, NULL);

	SDL_Rect optionsBackgroundDestRect;
	optionsBackgroundDestRect.x = 400;
	optionsBackgroundDestRect.y = 238;
	optionsBackgroundDestRect.w = 480;
	optionsBackgroundDestRect.h = 420;
	SDL_RenderCopy(renderer, resource_texture_options_hintergrund, NULL, &optionsBackgroundDestRect);

	schreibeBestenliste(0);

	SDL_Rect menuSrcRect;
	SDL_Rect menuDestRect;
	menuSrcRect.x = deltaTime % 857 > 426 ? 0 : 360;
	menuSrcRect.w = 360;
	menuSrcRect.h = 64;
	menuDestRect.x = 460;
	menuDestRect.w = 360;
	menuDestRect.h = 64;
	switch (selectedMenuItem) {
	case 0:
		menuSrcRect.y = 0;
		menuDestRect.y = 288;
		break;
	case 1:
		menuSrcRect.y = 64;
		menuDestRect.y = 378;
		break;
	case 2:
		menuSrcRect.y = 128;
		menuDestRect.y = 468;
		break;
	case 3:
		menuSrcRect.y = 192;
		menuDestRect.y = 558;
		break;
	}
	SDL_RenderCopy(renderer, resource_texture_options_texte, &menuSrcRect, &menuDestRect);

	if (!soundEingeschaltet) {
		SDL_Rect checkboxSrcRect;
		checkboxSrcRect.x = 720;
		checkboxSrcRect.y = 0;
		checkboxSrcRect.w = 33;
		checkboxSrcRect.h = 33;
		SDL_Rect checkboxDestRect;
		checkboxDestRect.x = 727;
		checkboxDestRect.y = 297;
		checkboxDestRect.w = 33;
		checkboxDestRect.h = 33;
		SDL_RenderCopy(renderer, resource_texture_options_texte, &checkboxSrcRect, &checkboxDestRect);
	}

	if (!musikEingeschaltet) {
		SDL_Rect checkboxSrcRect;
		checkboxSrcRect.x = 720;
		checkboxSrcRect.y = 33;
		checkboxSrcRect.w = 33;
		checkboxSrcRect.h = 33;
		SDL_Rect checkboxDestRect;
		checkboxDestRect.x = 718;
		checkboxDestRect.y = 387;
		checkboxDestRect.w = 33;
		checkboxDestRect.h = 33;
		SDL_RenderCopy(renderer, resource_texture_options_texte, &checkboxSrcRect, &checkboxDestRect);
	}

	if ((SDL_GetWindowFlags(window) & SDL_WINDOW_FULLSCREEN) == 0) {
		SDL_Rect checkboxSrcRect;
		checkboxSrcRect.x = 720;
		checkboxSrcRect.y = 66;
		checkboxSrcRect.w = 33;
		checkboxSrcRect.h = 33;
		SDL_Rect checkboxDestRect;
		checkboxDestRect.x = 782;
		checkboxDestRect.y = 477;
		checkboxDestRect.w = 33;
		checkboxDestRect.h = 33;
		SDL_RenderCopy(renderer, resource_texture_options_texte, &checkboxSrcRect, &checkboxDestRect);
	}

	malePyrat(0);

	SDL_RenderPresent(renderer);

}

void playSound(Mix_Chunk* const sound) {
	if (soundEingeschaltet) {
		Mix_PlayChannel(-1, sound, 0);
	}
}

int calculateBestenlistennameLaenge(const char* text) {
	int laenge = 0;
	int pos = 0;
	while (pos < 19 && text[pos] != NULL) {
		laenge += getBestenlisteBuchstabenbreite(text[pos], text[pos + 1]);
		pos++;
	}
	return laenge;
}

void schreibeTextInGrosseBestenliste(const char* text, const Sint16 left, const Sint16 top, bool mitCursor, bool hervorgehoben) {
	SDL_Rect srcRect;
	srcRect.y = hervorgehoben ? 104 : 1;
	srcRect.w = 40;
	srcRect.h = 50;

	SDL_Rect destRect;
	destRect.y = top;
	destRect.w = 40;
	destRect.h = 50;

	Sint16 deltaX = 0;

	char lastChar = 0;

	for (int i = 0; i < 20; i++) {
		if (text[i] == NULL) {
			break;
		}

		int charIndex = -1;

		if (text[i] == '-') {
			charIndex = 26;
		}
		else if (text[i] == '.') {
			charIndex = 27;
		}
		else if (text[i] >= 'A' && text[i] <= 'Z') {
			charIndex = text[i] - 'A';
		}

		if (lastChar) {
			deltaX += getBestenlisteBuchstabenbreite(lastChar, text[i]) * 7 / 2;
		}

		if (charIndex >= 0) {
			srcRect.x = 1 + charIndex * 41;
			destRect.x = left + deltaX;
			SDL_RenderCopy(renderer, resource_texture_alphabet, &srcRect, &destRect);
		}

		lastChar = text[i];
	}
	if (mitCursor && SDL_GetTicks() % 500 < 250) {
		if (lastChar) {
			deltaX += getBestenlisteBuchstabenbreite(lastChar, 0) * 7 / 2;
		}

		srcRect.x = 331;
		srcRect.y = hervorgehoben ? 103 + 52 : 52;
		destRect.x = left + deltaX;
		SDL_RenderCopy(renderer, resource_texture_alphabet, &srcRect, &destRect);
	}
}

void schreibeZahlInGrosseBestenliste(const Uint32 zahl, const Sint16 right, const Sint16 top, bool hervorgehoben) {
	SDL_Rect zifferSrcRect;
	zifferSrcRect.y = hervorgehoben ? 52 + 103 : 52;
	zifferSrcRect.w = 32;
	zifferSrcRect.h = 50;

	SDL_Rect zifferDestRect;
	zifferDestRect.y = top;
	zifferDestRect.w = 32;
	zifferDestRect.h = 50;

	Uint32 zuVerarbeitendeRestzahl = zahl;
	int zifferIndex = 0;
	while (zuVerarbeitendeRestzahl > 0 || zifferIndex == 0) {
		zifferIndex++;
		zifferSrcRect.x = 1 + (zuVerarbeitendeRestzahl % 10) * 33;
		zifferDestRect.x = right - zifferIndex * 33;

		SDL_RenderCopy(renderer, resource_texture_alphabet, &zifferSrcRect, &zifferDestRect);
		zuVerarbeitendeRestzahl = zuVerarbeitendeRestzahl / 10;
	}
}

void loopContent_bestenliste() {
	bool weiterGedrueckt = false;

	SDL_Event e;

	//event loop (Tastatur, Gamepad, Maus)
	while (SDL_PollEvent(&e) != 0) {
		handleGlobalEvents(&e);

		if (e.type == SDL_KEYDOWN) {
			switch (e.key.keysym.sym) {
			case SDLK_ESCAPE:
				weiterGedrueckt = true;
				break;
			}
		}
		else if (e.type == SDL_CONTROLLERBUTTONDOWN) {
			switch (e.cbutton.button) {
			case SDL_CONTROLLER_BUTTON_START:
			case SDL_CONTROLLER_BUTTON_A:
			case SDL_CONTROLLER_BUTTON_B:
			case SDL_CONTROLLER_BUTTON_X:
			case SDL_CONTROLLER_BUTTON_Y:
				weiterGedrueckt = true;
				break;
			}
		}
	}

	//Clear screen
	SDL_SetRenderDrawColor(renderer, 0x74, 0xA8, 0xFC, 255);
	SDL_RenderClear(renderer);

	game_maleMastkorb();

	maleGrosseBestenliste(0, 0, false, false);

	game_maleHud(GAME_LENGTH, 0);

	SDL_RenderPresent(renderer);

	if (weiterGedrueckt) {
		state = STATE_UEBERGANG_BESTENLISTE_HAUPTMENUE;
		aktuellerStatus_globalStarttime = SDL_GetTicks();
		playSound(resource_sound_schieb);
	}
}

void maleGrosseBestenliste(const Sint16 offsetX, const Sint16 offsetY, const bool drawCursor, const bool highlighted) {
	SDL_Rect destRect;
	destRect.x = 113 + offsetX;
	destRect.y = 40 + offsetY;
	destRect.w = 1054;
	destRect.h = 184;

	SDL_RenderCopy(renderer, resource_texture_grosseBestenlisteUeberschrift, NULL, &destRect);

	schreibeTextInGrosseBestenliste(bestenlisteneintraege[0].name, 425 + offsetX, 240 + offsetY, drawCursor && namenseingabe_bestenlisteIndex == 0, highlighted && namenseingabe_bestenlisteIndex == 0);
	schreibeZahlInGrosseBestenliste(bestenlisteneintraege[0].punkte, 855 + offsetX, 240 + offsetY, highlighted && namenseingabe_bestenlisteIndex == 0);
	schreibeTextInGrosseBestenliste(bestenlisteneintraege[1].name, 425 + offsetX, 310 + offsetY, drawCursor && namenseingabe_bestenlisteIndex == 1, highlighted && namenseingabe_bestenlisteIndex == 1);
	schreibeZahlInGrosseBestenliste(bestenlisteneintraege[1].punkte, 855 + offsetX, 310 + offsetY, highlighted && namenseingabe_bestenlisteIndex == 1);
	schreibeTextInGrosseBestenliste(bestenlisteneintraege[2].name, 425 + offsetX, 380 + offsetY, drawCursor && namenseingabe_bestenlisteIndex == 2, highlighted && namenseingabe_bestenlisteIndex == 2);
	schreibeZahlInGrosseBestenliste(bestenlisteneintraege[2].punkte, 855 + offsetX, 380 + offsetY, highlighted && namenseingabe_bestenlisteIndex == 2);
	schreibeTextInGrosseBestenliste(bestenlisteneintraege[3].name, 425 + offsetX, 450 + offsetY, drawCursor && namenseingabe_bestenlisteIndex == 3, highlighted && namenseingabe_bestenlisteIndex == 3);
	schreibeZahlInGrosseBestenliste(bestenlisteneintraege[3].punkte, 855 + offsetX, 450 + offsetY, highlighted && namenseingabe_bestenlisteIndex == 3);
	schreibeTextInGrosseBestenliste(bestenlisteneintraege[4].name, 425 + offsetX, 520 + offsetY, drawCursor && namenseingabe_bestenlisteIndex == 4, highlighted && namenseingabe_bestenlisteIndex == 4);
	schreibeZahlInGrosseBestenliste(bestenlisteneintraege[4].punkte, 855 + offsetX, 520 + offsetY, highlighted && namenseingabe_bestenlisteIndex == 4);
}

void namenseingabe_addChar(char zeichen) {
	for (int i = 0; i < 19; i++) {
		if (bestenlisteneintraege[namenseingabe_bestenlisteIndex].name[i] == NULL) {
			bestenlisteneintraege[namenseingabe_bestenlisteIndex].name[i] = zeichen;
			bestenlisteneintraege[namenseingabe_bestenlisteIndex].name[i + 1] = NULL;
			if (calculateBestenlistennameLaenge(bestenlisteneintraege[namenseingabe_bestenlisteIndex].name) > 70) {
				bestenlisteneintraege[namenseingabe_bestenlisteIndex].name[i] = NULL;
				playSound(resource_sound_fehleingabe);
			}
			else {
				playSound(resource_sound_menueSelectionChange);
			}
			return;
		}
	}
	//in der Paxis kommt der Code nie hier hin
}

void namenseingabe_removeChar() {
	int laenge = 0;
	while (laenge < 20 && bestenlisteneintraege[namenseingabe_bestenlisteIndex].name[laenge] != NULL) {
		laenge++;
	}
	if (laenge > 0) {
		bestenlisteneintraege[namenseingabe_bestenlisteIndex].name[laenge - 1] = NULL;
		playSound(resource_sound_hieb);
	}
}

void loopContent_namenseingabe() {
	static const double tangensVonPiAchtel = 0.414213562373095;
	bool weiterGedrueckt = false;

	//Fuer den Tastaturersatz kann der Stick in eine von 8 Zonen bewegt werden
	int leftAnalogStickZone = -1;

	double leftX = (double)SDL_GameControllerGetAxis(aktiverGameController, SDL_CONTROLLER_AXIS_LEFTX) / 32768.0;
	double leftY = (double)SDL_GameControllerGetAxis(aktiverGameController, SDL_CONTROLLER_AXIS_LEFTY) / 32768.0;
	double ausschlagQuadriert = leftX * leftX + leftY * leftY;
	if (ausschlagQuadriert > .5) {
		//um die genaue Zone zu berechnen schaue ich mir 16 moegliche Faelle an
		if (leftX >= 0 && leftY >= 0 && leftX >= leftY) {
			leftAnalogStickZone = leftY / leftX > tangensVonPiAchtel ? 3 : 2;
		}
		else if (leftX >= 0 && leftY >= 0 && leftX < leftY) {
			leftAnalogStickZone = leftX / leftY > tangensVonPiAchtel ? 3 : 4;
		}
		else if (leftX < 0 && leftY >= 0 && -leftX < leftY) {
			leftAnalogStickZone = -leftX / leftY > tangensVonPiAchtel ? 5 : 4;
		}
		else if (leftX < 0 && leftY >= 0 && -leftX >= leftY) {
			leftAnalogStickZone = leftY / -leftX > tangensVonPiAchtel ? 5 : 6;
		}
		else if (leftX < 0 && leftY < 0 && -leftX >= -leftY) {
			leftAnalogStickZone = -leftY / -leftX > tangensVonPiAchtel ? 7 : 6;
		}
		else if (leftX < 0 && leftY < 0 && -leftX < -leftY) {
			leftAnalogStickZone = -leftX / -leftY > tangensVonPiAchtel ? 7 : 0;
		}
		else if (leftX >= 0 && leftY < 0 && leftX < -leftY) {
			leftAnalogStickZone = leftX / -leftY > tangensVonPiAchtel ? 1 : 0;
		}
		else if (leftX >= 0 && leftY < 0 && leftX >= -leftY) {
			leftAnalogStickZone = -leftY / leftX > tangensVonPiAchtel ? 1 : 2;
		}
	}


	SDL_Event e;

	//event loop (Tastatur, Gamepad, Maus)
	while (SDL_PollEvent(&e) != 0) {
		handleGlobalEvents(&e);

		if (e.type == SDL_KEYDOWN) {
			if (e.key.keysym.sym == SDLK_ESCAPE || e.key.keysym.sym == SDLK_RETURN) {
				weiterGedrueckt = true;
			}
			else if (e.key.keysym.sym >= SDLK_a && e.key.keysym.sym <= SDLK_z) {
				namenseingabe_addChar(e.key.keysym.sym - SDLK_a + 'A');
			}
			else if (e.key.keysym.sym == SDLK_MINUS) {
				namenseingabe_addChar('-');
			}
			else if (e.key.keysym.sym == SDLK_PERIOD) {
				namenseingabe_addChar('.');
			}
			else if (e.key.keysym.sym == SDLK_SPACE) {
				namenseingabe_addChar(' ');
			}
			else if (e.key.keysym.sym == SDLK_BACKSPACE) {
				namenseingabe_removeChar();
			}
		}

		if (e.type == SDL_CONTROLLERBUTTONDOWN && e.cbutton.which == SDL_JoystickInstanceID(SDL_GameControllerGetJoystick(aktiverGameController))) {
			switch (e.cbutton.button) {
			case SDL_CONTROLLER_BUTTON_START:
				weiterGedrueckt = true;
				break;
			case SDL_CONTROLLER_BUTTON_BACK:
				namenseingabe_removeChar();
				break;
			case SDL_CONTROLLER_BUTTON_A:
				if (leftAnalogStickZone >= 0 && leftAnalogStickZone < 6) {
					namenseingabe_addChar('D' + leftAnalogStickZone * 4);
				}
				else if (leftAnalogStickZone == 6) {
					namenseingabe_addChar('-');
				}
				else if (leftAnalogStickZone == 7) {
					namenseingabe_addChar(' ');
				}
				break;
			case SDL_CONTROLLER_BUTTON_B:
				if (leftAnalogStickZone >= 0 && leftAnalogStickZone < 6) {
					namenseingabe_addChar('C' + leftAnalogStickZone * 4);
				}
				else if (leftAnalogStickZone == 6) {
					namenseingabe_addChar('.');
				}
				else if (leftAnalogStickZone == 7) {
					weiterGedrueckt = true;
				}
				break;
			case SDL_CONTROLLER_BUTTON_X:
				if (leftAnalogStickZone >= 0 && leftAnalogStickZone < 7) {
					namenseingabe_addChar('A' + leftAnalogStickZone * 4);
				}
				else if (leftAnalogStickZone == 7) {
					namenseingabe_addChar(' ');
				}
				break;
			case SDL_CONTROLLER_BUTTON_Y:
				if (leftAnalogStickZone >= 0 && leftAnalogStickZone < 7) {
					namenseingabe_addChar('B' + leftAnalogStickZone * 4);
				}
				else if (leftAnalogStickZone == 7) {
					namenseingabe_removeChar();
				}
				break;
			}
		}
	}


	if (weiterGedrueckt) {
		saveBestenliste();
		state = STATE_BESTENLISTE;
		playSound(resource_sound_menueConfirm);
	}

	//Clear screen
	SDL_SetRenderDrawColor(renderer, 0x74, 0xA8, 0xFC, 255);
	SDL_RenderClear(renderer);

	game_maleMastkorb();

	maleGrosseBestenliste(0, 0, true, true);

	game_maleHud(GAME_LENGTH, 0);

	SDL_Rect srcRect;
	srcRect.x = 0;
	srcRect.y = 0;
	srcRect.w = 400;
	srcRect.h = 400;
	SDL_Rect destRect;
	destRect.x = 0;
	destRect.y = 320;
	destRect.w = 400;
	destRect.h = 400;
	SDL_RenderCopy(renderer, resource_texture_tastaturersatz, &srcRect, &destRect);
	if (leftAnalogStickZone >= 0) {
		srcRect.x = leftAnalogStickZone % 4 * 200 + 400;
		srcRect.y = leftAnalogStickZone / 4 * 200;
		srcRect.w = 200;
		srcRect.h = 200;
		destRect.w = 200;
		destRect.h = 200;
		switch (leftAnalogStickZone) {
		case 0:
			destRect.x = 100;
			destRect.y = 320 + 0;
			break;
		case 1:
			destRect.x = 200;
			destRect.y = 320 + 0;
			break;
		case 2:
			destRect.x = 200;
			destRect.y = 320 + 100;
			break;
		case 3:
			destRect.x = 200;
			destRect.y = 320 + 200;
			break;
		case 4:
			destRect.x = 100;
			destRect.y = 320 + 200;
			break;
		case 5:
			destRect.x = 0;
			destRect.y = 320 + 200;
			break;
		case 6:
			destRect.x = 0;
			destRect.y = 320 + 100;
			break;
		case 7:
			destRect.x = 0;
			destRect.y = 320 + 0;
			break;
		}
		SDL_RenderCopy(renderer, resource_texture_tastaturersatz, &srcRect, &destRect);
	}


	SDL_RenderPresent(renderer);
}

//Simpler, kryptografisch unsischerer Verschluesselungsalgorithmus... wer unbedingt schummeln will kann ja eh im Quelltext nachgucken
char reorderBits(char zeichen) {
	return ((zeichen & 0x01) << 5)
		| ((zeichen & 0x02) << 1)
		| ((zeichen & 0x04) << 5)
		| ((zeichen & 0x08) << 3)
		| ((zeichen & 0x10) >> 4)
		| ((zeichen & 0x20) >> 2)
		| ((zeichen & 0x40) >> 2)
		| ((zeichen & 0x80) >> 6);
}
void signBestenlisteneintrag(const Uint32 punkte, const char* const name, char* const signatur) {
	//die hinteren 3 Bit von Punkte sind immer 0
	char punkte1 = punkte >> 3 & 0xFF;
	char punkte2 = punkte >> 11 & 0xFF;

	signatur[0] = reorderBits(punkte1) ^ punkte2 ^ 0x85;
	for (char i = 0; i < 20; i++) {
		signatur[i + 1] = reorderBits(signatur[i] ^ name[i]) ^ i;
	}
	signatur[21] = reorderBits(punkte2) ^ signatur[20];
}

void loopContent_uebergangBonustabelleNamenseingabe() {
	Uint32 currentTime = SDL_GetTicks();
	Uint32 deltaTime = currentTime - aktuellerStatus_globalStarttime;

	SDL_Event e;

	//event loop (Tastatur, Gamepad, Maus)
	while (SDL_PollEvent(&e) != 0) {
		handleGlobalEvents(&e);
	}

	if (deltaTime < 1000) {
		//Clear screen
		SDL_SetRenderDrawColor(renderer, 0x74, 0xA8, 0xFC, 255);
		SDL_RenderClear(renderer);

		game_maleMastkorb();

		game_maleBonustabelle(0 - deltaTime * 1280 / 1000, 0);
		maleGrosseBestenliste(1280 - deltaTime * 1280 / 1000, 0, false, true);

		game_maleHud(GAME_LENGTH, 0);

		SDL_RenderPresent(renderer);
	}
	else {
		state = STATE_NAMENSEINGABE;
		loopContent_namenseingabe();
	}
}

void loopContent_uebergangBestenlisteHauptmenue() {
	Uint32 currentTime = SDL_GetTicks();
	Uint32 deltaTime = currentTime - aktuellerStatus_globalStarttime;

	if (deltaTime > 1000) {
		hauptmenue_init();
		pyrat_sprechen_textindex = 7;
		pyrat_sprechen_laute = "HUUREI ..IE DID IT ....TEIK A GROG AND SELEBRAIT TE RESULT .....       ";
		pyrat_sprechen_anzahlLaute = 71;
		pyrat_sprechen_globalStarttime = currentTime;
		pyrat_sprechen_anzahlLaute2 = 0;
		loopContent_hauptmenue();
		return;
	}

	SDL_Event e;

	//event loop (Tastatur, Gamepad, Maus)
	while (SDL_PollEvent(&e) != 0) {
		handleGlobalEvents(&e);
	}

	//Clear screen
	SDL_SetRenderDrawColor(renderer, 0x74, 0xA8, 0xFC, 255);
	SDL_RenderClear(renderer);

	Sint16 zurueckgelegterWeg = (582 + 720 - 40) * deltaTime / 1000;

	maleMenueHintergrund(680 + 582 - zurueckgelegterWeg);

	maleGrosseBestenliste(0, -zurueckgelegterWeg, false, false);

	game_maleHud(GAME_LENGTH, -zurueckgelegterWeg);

	SDL_RenderPresent(renderer);

}

void loopContent_pause() {
	SDL_Event e;
	//event loop (Tastatur, Gamepad, Maus)
	while (SDL_PollEvent(&e) != 0) {
		handleGlobalEvents(&e);
		if (e.type == SDL_CONTROLLERBUTTONDOWN && e.cbutton.which == SDL_JoystickInstanceID(SDL_GameControllerGetJoystick(aktiverGameController))) {
			switch (e.cbutton.button) {
			case SDL_CONTROLLER_BUTTON_START:
				state = STATE_GAME;
				aktuellerStatus_globalStarttime += SDL_GetTicks() - aktuellerStatus_pause_globalStarttime;
				Mix_ResumeMusic();
				loopContent_game();
				return;
			case SDL_CONTROLLER_BUTTON_BACK:
				state = STATE_QUIT_TO_MAIN_MENU;
				playSound(resource_sound_pause);
				selectedMenuItem = 0;
				loopContent_quitToMainMenu();
				return;
			}
		}
	}

	Uint32 deltaTime = aktuellerStatus_pause_globalStarttime - aktuellerStatus_globalStarttime;

	//Clear screen
	SDL_SetRenderDrawColor(renderer, 0x74, 0xA8, 0xFC, 255);
	SDL_RenderClear(renderer);

	game_maleMastkorb();

	game_maleDrachen(deltaTime);

	game_maleHud(deltaTime, 0);

	SDL_Rect srcRect;
	srcRect.x = 0;
	srcRect.y = 112;
	srcRect.w = 162;
	srcRect.h = 37;

	SDL_Rect destRect;
	destRect.x = (SCREEN_WIDTH - 162) / 2;
	destRect.y = (SCREEN_HEIGHT - 37) / 2;
	destRect.w = 162;
	destRect.h = 37;

	if (SDL_GetTicks() % 1000 > 500) {
		SDL_RenderCopy(renderer, resource_texture_hud, &srcRect, &destRect);
	}

	SDL_RenderPresent(renderer);
}

void loopContent_quitToMainMenu() {
	bool backToGame = false;

	SDL_Event e;
	//event loop (Tastatur, Gamepad, Maus)
	while (SDL_PollEvent(&e) != 0) {
		handleGlobalEvents(&e);
		if (e.type == SDL_CONTROLLERBUTTONDOWN && e.cbutton.which == SDL_JoystickInstanceID(SDL_GameControllerGetJoystick(aktiverGameController))) {
			switch (e.cbutton.button) {
			case SDL_CONTROLLER_BUTTON_DPAD_LEFT:
			case SDL_CONTROLLER_BUTTON_DPAD_RIGHT:
				selectedMenuItem = (selectedMenuItem + 1) % 2;
				playSound(resource_sound_menueSelectionChange);
				break;
			case SDL_CONTROLLER_BUTTON_A:
			case SDL_CONTROLLER_BUTTON_B:
			case SDL_CONTROLLER_BUTTON_X:
			case SDL_CONTROLLER_BUTTON_Y:
			case SDL_CONTROLLER_BUTTON_START:
				if (selectedMenuItem == 0) {
					backToGame = true;
				}
				else {
					state = STATE_UEBERGANG_GAME_HAUPTMENUE;
					aktuellerStatus_globalStarttime = SDL_GetTicks();
					playSound(resource_sound_schieb);
					loopContent_uebergangGameHauptmenue();
					return;
				}
				break;
			case SDL_CONTROLLER_BUTTON_BACK:
				if (selectedMenuItem == 0) {
					backToGame = true;
				}
				break;
			}
		}
	}

	updateAnalogstickDigitaleXPositionen(aktiverGameController_index);
	if ((gamepads[aktiverGameController_index].analogstickLinksDigitaleXPosition == DIGITAL_X_LINKS && gamepads[aktiverGameController_index].alteWerteAnalogstickLinksDigitaleXPosition != DIGITAL_X_LINKS)
		|| (gamepads[aktiverGameController_index].analogstickRechtsDigitaleXPosition == DIGITAL_X_LINKS && gamepads[aktiverGameController_index].alteWerteAnalogstickRechtsDigitaleXPosition != DIGITAL_X_LINKS)
		|| (gamepads[aktiverGameController_index].analogstickLinksDigitaleXPosition == DIGITAL_X_RECHTS && gamepads[aktiverGameController_index].alteWerteAnalogstickLinksDigitaleXPosition != DIGITAL_X_RECHTS)
		|| (gamepads[aktiverGameController_index].analogstickRechtsDigitaleXPosition == DIGITAL_X_RECHTS && gamepads[aktiverGameController_index].alteWerteAnalogstickRechtsDigitaleXPosition != DIGITAL_X_RECHTS)) {
		selectedMenuItem = (selectedMenuItem + 1) % 2;
		playSound(resource_sound_menueSelectionChange);
	}

	if (backToGame) {
		state = STATE_GAME;
		aktuellerStatus_globalStarttime += SDL_GetTicks() - aktuellerStatus_pause_globalStarttime;
		Mix_ResumeMusic();
		loopContent_game();
		return;
	}

	Uint32 deltaTime = aktuellerStatus_pause_globalStarttime - aktuellerStatus_globalStarttime;

	//Clear screen
	SDL_SetRenderDrawColor(renderer, 0x74, 0xA8, 0xFC, 255);
	SDL_RenderClear(renderer);

	game_maleMastkorb();

	game_maleHud(deltaTime, 0);

	SDL_Rect textSrcRect;
	textSrcRect.x = 0;
	textSrcRect.y = 0;
	textSrcRect.w = 520;
	textSrcRect.h = 220;
	SDL_Rect textDestRect;
	textDestRect.x = 380;
	textDestRect.y = 240;
	textDestRect.w = 520;
	textDestRect.h = 220;
	SDL_RenderCopy(renderer, resource_texture_quit, &textSrcRect, &textDestRect);

	SDL_Rect auswahlSrcRect;
	auswahlSrcRect.x = (SDL_GetTicks() % 500 > 250) ? 6 : 163;
	auswahlSrcRect.y = 230;
	auswahlSrcRect.w = 150;
	auswahlSrcRect.h = 120;
	SDL_Rect auswahlDestRect;
	auswahlDestRect.x = selectedMenuItem == 0 ? 380 + 98 : 380 + 267;
	auswahlDestRect.y = 240 + 128;
	auswahlDestRect.w = 150;
	auswahlDestRect.h = 120;
	SDL_RenderCopy(renderer, resource_texture_quit, &auswahlSrcRect, &auswahlDestRect);

	SDL_RenderPresent(renderer);
}

void loopContent_uebergangGameHauptmenue() {
	Uint32 currentTime = SDL_GetTicks();
	Uint32 deltaTime = currentTime - aktuellerStatus_globalStarttime;

	if (deltaTime > 1000) {
		Mix_ResumeMusic();
		Mix_HaltMusic();
		hauptmenue_init();
		loopContent_hauptmenue();
		return;
	}

	SDL_Event e;

	//event loop (Tastatur, Gamepad, Maus)
	while (SDL_PollEvent(&e) != 0) {
		handleGlobalEvents(&e);
	}

	//Clear screen
	SDL_SetRenderDrawColor(renderer, 0x74, 0xA8, 0xFC, 255);
	SDL_RenderClear(renderer);

	Sint16 zurueckgelegterWeg = (582 + 720 - 40) * deltaTime / 1000;

	maleMenueHintergrund(680 + 582 - zurueckgelegterWeg);

	SDL_Rect textSrcRect;
	textSrcRect.x = 0;
	textSrcRect.y = 0;
	textSrcRect.w = 520;
	textSrcRect.h = 220;
	SDL_Rect textDestRect;
	textDestRect.x = 380;
	textDestRect.y = 240 - zurueckgelegterWeg;
	textDestRect.w = 520;
	textDestRect.h = 220;
	SDL_RenderCopy(renderer, resource_texture_quit, &textSrcRect, &textDestRect);

	game_maleHud(GAME_LENGTH, -zurueckgelegterWeg);

	SDL_RenderPresent(renderer);

}

void calculateAndSetNextSprechenStarttime() {
	const Uint32 currentTime = SDL_GetTicks();

	//aktuell noch laufende Arm-Animation muss zuerst zuende laufen, ansonsten sofort beginnen
	if (pyrat_armanimation_globalStarttime > 0 && pyrat_armanimation_globalStarttime < currentTime && currentTime < pyrat_armanimation_globalStarttime + 33 * 60) {
		pyrat_sprechen_globalStarttime = pyrat_armanimation_globalStarttime + 33 * 60;
	}
	else {
		pyrat_sprechen_globalStarttime = currentTime;
		pyrat_armanimation_globalStarttime = 0;
	}
}
